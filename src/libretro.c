#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include "libretro.h"
#include "chip8.h"

#ifdef USE_RGB565
typedef unsigned short pixel_t;
#if defined(ABGR1555)
#define vRGB(r,g,b) ((((b) & 0xf8) << 7) | (((g) & 0xf8) << 2) | (((r) & 0xf8) >> 3))
#else
#define vRGB(r,g,b) ((((r) & 0xf8) << 8) | (((g) & 0xfc) << 3) | (((b) & 0xf8) >> 3))
#endif
#else
typedef unsigned int pixel_t;
#define vRGB(r,g,b) (((r) << 16) | ((g) << 8) | (b))
#endif

static pixel_t frame[DISPLAY_WIDTH * DISPLAY_HEIGHT];

static retro_environment_t environ_cb;
static retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;

static CHIP8 chip8;
static int cpu_debt = 0;
#define AUDIO_RESAMPLE_RATE 44100
static unsigned int audio_counter_chip8 = 0;
static unsigned int audio_counter_resample = 0;
static int audio_freq_chip8 = 0;
static int snd_buf_pntr = 0;
static uint8_t sram[NUM_USER_FLAGS];

struct theme {
    pixel_t bg, p1, p2, overlap;
    const char *name;
};

bool chip8_handle_user_flags(CHIP8 *chip8p, int num_flags, bool save)
{
    if (num_flags <= NUM_USER_FLAGS)
    {
	if (save)
	{
	    memcpy(sram, chip8p->V, num_flags);
	}
	else
	{
	    memcpy(chip8p->V, sram, num_flags);
	}

	return true;
    }
    else
    {
        return true; // Only return false on file open error.
    }
}


#define BG_COLOR_DEFAULT vRGB(0,0,0)
#define P1_COLOR_DEFAULT vRGB(0xFF,0xFF,0xFF)
#define P2_COLOR_DEFAULT vRGB(0xAA,0xAA,0xAA)
#define OVERLAP_COLOR_DEFAULT vRGB(0x55,0x55,0x55)

// Color/Display
// TODO: Add more themes!
static const struct theme color_themes[] = {
    {BG_COLOR_DEFAULT, P1_COLOR_DEFAULT, P2_COLOR_DEFAULT, OVERLAP_COLOR_DEFAULT, "Default"},
    {vRGB(0,0,0), vRGB(0xFF,0xFF,0xFF), vRGB(0,0,0), vRGB(0,0,0), "Black and white"},
    {vRGB(0xFF,0xFF,0xFF), vRGB(0,0,0), vRGB(0,0,0), vRGB(0,0,0), "Inverted black and white"},
    {vRGB(0,0,0), vRGB(0xFF,0,0), vRGB(0,0,0), vRGB(0,0,0), "Blood"},
    {vRGB(0,0,0), vRGB(0,0xFF,0), vRGB(0,0,0), vRGB(0,0,0), "Hacker"},
    {vRGB(0,0,0), vRGB(0,0,0xFF), vRGB(0,0,0), vRGB(0,0,0), "Space" },
    {vRGB(0xF0,0x90,0x7C), vRGB(0x96,0x29,0x12), vRGB(0xF4,0x69,0), vRGB(0xFA, 0xA4,0), "Crazy Orange"},
    {vRGB(0x10,0,0x19), vRGB(0xFF,0xE1,0xFF), vRGB(0xE7,0,0xEA), vRGB(0xE7,0,0xEA), "Cyberpunk"}
};
static pixel_t bg_color = BG_COLOR_DEFAULT;
static pixel_t p1_color = P1_COLOR_DEFAULT;
static pixel_t p2_color = P2_COLOR_DEFAULT;
static pixel_t overlap_color = OVERLAP_COLOR_DEFAULT;

static void fallback_log(enum retro_log_level level,
			 const char *fmt, ...) {
    va_list args;

    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
}
   
#define FIRST_QUIRK_VARIABLE 0
static struct retro_variable variables[] =
{
    {
	"jaxe_quirk_0_ram_init",
	"Ram init quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_1_8xy6_8xye",
	"8xy6/8xyE quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_2_fx55_fx65",
	"Fx55/Fx65 quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_3_bnnn",
	"Bnnn quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_4_big_sprite_lores",
	"Big Sprite LORES quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_5_00fe_00ff",
	"00FE/00FF quirk; enabled|disabled",
    },
    {
	"jaxe_quirk_6_sprite_wrapping",
	"Sprite Wrapping; enabled|disabled",
    },
    {
	"jaxe_quirk_7_collision_enumeration",
	"Collision Enumeration; enabled|disabled",
    },
    {
	"jaxe_quirk_8_collision_bottom_of_screen",
	"Collision with Bottom of Screen; enabled|disabled",
    },
    {
	"jaxe_cpu_requency",
	"CPU frequency; 1000|1500|2000|3000|5000|10000|800|750|600|500|400|300",
    },
    {
	"jaxe_theme",
	"Theme; Default|Black and white|Inverted black and white|Blood|Hacker|Space|Crazy Orange|Cyberpunk"
    },
    { NULL, NULL },
};

void retro_set_environment(retro_environment_t fn)
{
    environ_cb = fn;

    fn(RETRO_ENVIRONMENT_SET_VARIABLES, variables);

    struct retro_log_callback log;

    if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
	log_cb = log.log;
    else
	log_cb = fallback_log;
}

static void load_theme(void)
{
    struct retro_variable var;
    int theme_number = 0;
    var.key = "jaxe_theme";
    var.value = NULL;

    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value) {
	for (int i = 0; i < sizeof (color_themes) / sizeof(color_themes[0]); i++) {
	    if (strcmp(var.value, color_themes[i].name) == 0)
		theme_number = i;
	}
    }

    bg_color = color_themes[theme_number].bg;
    p1_color = color_themes[theme_number].p1;
    p2_color = color_themes[theme_number].p2;
    overlap_color = color_themes[theme_number].overlap;
}

static int get_cpu_freq_var(int def)
{
    struct retro_variable var;
    var.key = "jaxe_cpu_requency";
    var.value = NULL;
    if (!environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) || !var.value)
	return def;
    int cpu_freq = strtoul(var.value, 0, 0);
    if (cpu_freq <= 0)
	return def;
    return cpu_freq;
}

static void chip8_init_with_vars(void)
{
    bool quirks[NUM_QUIRKS] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    unsigned long cpu_freq = CPU_FREQ_DEFAULT;
    unsigned long timer_freq = TIMER_FREQ_DEFAULT;
    unsigned long refresh_freq = REFRESH_FREQ_DEFAULT;
    uint16_t pc_start_addr = PC_START_ADDR_DEFAULT;

    load_theme();

    for (int i = 0; i < NUM_QUIRKS; i++) {
	struct retro_variable var;
	var.key = variables[i + FIRST_QUIRK_VARIABLE].key;
	var.value = NULL;

	quirks[i] = (!environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) || !var.value) || strcmp(var.value, "disabled") != 0;
    }

    cpu_freq = get_cpu_freq_var(CPU_FREQ_DEFAULT);

    chip8_init(&chip8, cpu_freq, timer_freq, refresh_freq, pc_start_addr,
	       quirks);
}

// Makes the physical screen match the emulator display.
void draw_display(void)
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
	for (int x = 0; x < DISPLAY_WIDTH; x++)
	{
	    long color;

	    if (!chip8.display[y][x] && !chip8.display2[y][x])
	    {
		color = bg_color;
	    }
	    else if (chip8.display[y][x] && !chip8.display2[y][x])
	    {
		color = p1_color;
	    }
	    else if (!chip8.display[y][x] && chip8.display2[y][x])
	    {
		color = p2_color;
	    }
	    else if (chip8.display[y][x] && chip8.display2[y][x])
	    {
		color = overlap_color;
	    }

	    frame[x + y * DISPLAY_WIDTH] = color;
	}
    }
}

void retro_set_video_refresh(retro_video_refresh_t fn) { video_cb = fn; }
void retro_set_audio_sample(retro_audio_sample_t fn) { audio_cb = fn; }
void retro_set_audio_sample_batch(retro_audio_sample_batch_t fn) { audio_batch_cb = fn; }
void retro_set_input_poll(retro_input_poll_t fn) { input_poll_cb = fn; }
void retro_set_input_state(retro_input_state_t fn) { input_state_cb = fn; }

// TODO: find a better mapping
static int hexorder[] = {
    RETRO_DEVICE_ID_JOYPAD_B,
    RETRO_DEVICE_ID_JOYPAD_START,
    RETRO_DEVICE_ID_JOYPAD_Y,
    RETRO_DEVICE_ID_JOYPAD_SELECT,
    RETRO_DEVICE_ID_JOYPAD_L,
    RETRO_DEVICE_ID_JOYPAD_UP,
    RETRO_DEVICE_ID_JOYPAD_A,
    RETRO_DEVICE_ID_JOYPAD_LEFT,
    RETRO_DEVICE_ID_JOYPAD_DOWN,
    RETRO_DEVICE_ID_JOYPAD_RIGHT,
    RETRO_DEVICE_ID_JOYPAD_R,
    RETRO_DEVICE_ID_JOYPAD_L2,
    RETRO_DEVICE_ID_JOYPAD_X,
    RETRO_DEVICE_ID_JOYPAD_R2,
    RETRO_DEVICE_ID_JOYPAD_L3,
    RETRO_DEVICE_ID_JOYPAD_R3
};

struct retro_input_descriptor desc[] = {
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "7" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_Y,     "2" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "8" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "9" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "0" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_SELECT,"3" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_X,     "C" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "5" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "6" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "1" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L,     "5" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R,     "A" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L2,    "B" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R2,    "D" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_L3,    "E" },
    { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_R3,    "F" },

    { 0 },
};

void retro_init(void)
{
}

bool retro_load_game(const struct retro_game_info *info)
{
    cpu_debt = 0;
    audio_counter_chip8 = 0;
    audio_counter_resample = 0;
    audio_freq_chip8 = 0;
    snd_buf_pntr = 0;

    chip8_init_with_vars();
    chip8_load_font(&chip8);

    chip8_load_rom_buffer(&chip8, info->data, info->size);

    environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

    return true;
}

void retro_unload_game(void)
{
	
}

int get_audio_sample(void)
{
    // Get the byte of the emulator's buffer that the next sample is in.
    int x = chip8.RAM[AUDIO_BUF_ADDR + (snd_buf_pntr / 8)];

    // Get the actual sample bit.
    x <<= (snd_buf_pntr % 8);
    x &= 0x80;

    // Finally set the buffer to max volume if sample is 1.
    x *= 0xFF;

    /* Keep track of where we are in the emulator's sound buffer and
       wrap back around if necessary. */
    snd_buf_pntr++;
    if (snd_buf_pntr >= (AUDIO_BUF_SIZE * 8))
    {
	snd_buf_pntr = 0;
    }

    return x;
}

static void audio_sample(int16_t sample) {    
    // TODO: Use audio_batch_cb
    while (audio_counter_resample >= ONE_SEC / AUDIO_RESAMPLE_RATE) {
	audio_cb(sample, sample);
	audio_counter_resample -= ONE_SEC / AUDIO_RESAMPLE_RATE;
    }
}

void retro_run(void)
{
    if (chip8.exit) {
	environ_cb(RETRO_ENVIRONMENT_SHUTDOWN, NULL);
	return;
    }

    bool updated = false;
    if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated) {
	load_theme();
	int cpu_freq = get_cpu_freq_var(chip8.cpu_freq);
	if (cpu_freq != chip8.cpu_freq)
	    chip8_set_cpu_freq(&chip8, cpu_freq);
    }

    input_poll_cb();

    for (int i = 0; i < 16; i++)
	if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, hexorder[i]))
	    chip8.keypad[i] = KEY_DOWN;
	else
	    chip8.keypad[i] = chip8.keypad[i] == KEY_DOWN ? KEY_RELEASED : KEY_UP;

    for (int i = 0; i < (chip8.cpu_freq + cpu_debt) / chip8.refresh_freq && !chip8.exit; i++) {
	chip8_cycle(&chip8);
	    
	if (!chip8.beep) {
	    audio_freq_chip8 = 0;
	    audio_counter_chip8 = 0;
	    snd_buf_pntr = 0;
	    audio_counter_resample += ONE_SEC / chip8.cpu_freq;
	    audio_sample(0);
	} else {
	    if (!audio_freq_chip8) {
		audio_freq_chip8 = chip8_get_sound_freq(&chip8);
		snd_buf_pntr = 0;
	    }
	    audio_counter_chip8 += ONE_SEC / chip8.cpu_freq;
	    while (audio_counter_chip8 > ONE_SEC / audio_freq_chip8) {
		audio_counter_chip8 -= ONE_SEC / audio_freq_chip8;
		int sample = get_audio_sample();
		audio_counter_resample += ONE_SEC / audio_freq_chip8;
		audio_sample(sample);
	    }
	}
    }

    cpu_debt = (chip8.cpu_freq + cpu_debt) % chip8.refresh_freq;

    // Output video
    draw_display();
    video_cb(frame, DISPLAY_WIDTH, DISPLAY_HEIGHT, sizeof(pixel_t) * DISPLAY_WIDTH);
}

unsigned retro_get_region(void)
{
    return RETRO_REGION_NTSC;
}

void retro_get_system_info(struct retro_system_info *info)
{
    memset(info, 0, sizeof(*info));
    info->library_name = "JAXE";
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif
    info->library_version = "1.0" GIT_VERSION;
    info->valid_extensions = "ch8|sc8|xo8|hc8";
    info->need_fullpath = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
#ifdef USE_RGB565
    int pixelformat = RETRO_PIXEL_FORMAT_RGB565;
#else
    int pixelformat = RETRO_PIXEL_FORMAT_XRGB8888;
#endif	

    memset(info, 0, sizeof(*info));
    info->geometry.base_width   = DISPLAY_WIDTH;
    info->geometry.base_height  = DISPLAY_HEIGHT;
    info->geometry.max_width    = DISPLAY_WIDTH;
    info->geometry.max_height   = DISPLAY_HEIGHT;
    info->geometry.aspect_ratio = ((float)DISPLAY_WIDTH) / ((float)DISPLAY_HEIGHT);

    info->timing.fps = chip8.refresh_freq;
    info->timing.sample_rate = AUDIO_RESAMPLE_RATE;

    environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixelformat);
}


void retro_deinit(void) {  }

void retro_reset(void)
{
    // TODO
}

struct serialized_state
{
    CHIP8 chip8;
    unsigned long cpu_debt;
    unsigned int audio_counter_chip8;
    unsigned int audio_counter_resample;
    int audio_freq_chip8;
    int snd_buf_pntr;
    uint8_t sram[NUM_USER_FLAGS];
};

size_t retro_serialize_size(void)
{
    return sizeof (struct serialized_state);
}

bool retro_serialize(void *data, size_t size)
{
    if (size < sizeof (struct serialized_state))
	return false;

    struct serialized_state *st = data;
    memcpy(&st->chip8, &chip8, sizeof(st->chip8));
    st->cpu_debt = cpu_debt;
    st->audio_counter_chip8 = audio_counter_chip8;
    st->audio_counter_resample = audio_counter_resample;
    st->audio_freq_chip8 = audio_freq_chip8;
    st->snd_buf_pntr = snd_buf_pntr;
    memcpy(st->sram, sram, sizeof(st->sram));
    return true;
}

bool retro_unserialize(const void *data, size_t size)
{
    if (size < sizeof (struct serialized_state))
	return false;

    const struct serialized_state *st = data;
    memcpy(&chip8, &st->chip8, sizeof(chip8));
    cpu_debt = st->cpu_debt;
    audio_counter_chip8 = st->audio_counter_chip8;
    audio_counter_resample = st->audio_counter_resample;
    audio_freq_chip8 = st->audio_freq_chip8;
    snd_buf_pntr = st->snd_buf_pntr;
    memcpy(sram, st->sram, sizeof(st->sram));
    return true;
}

size_t retro_get_memory_size(unsigned id)
{
    switch(id)
    {
    case RETRO_MEMORY_SYSTEM_RAM: // System Memory
	return MAX_RAM;

    case RETRO_MEMORY_SAVE_RAM: // SRAM
	return sizeof(sram);

	// TODO: VRAM
    }
    return 0;
}

void *retro_get_memory_data(unsigned id)
{
    switch(id)
    {
    case RETRO_MEMORY_SYSTEM_RAM: // System Memory
	return chip8.RAM;

    case RETRO_MEMORY_SAVE_RAM: // SRAM
	return sram;

	// TODO: VRAM
    }
    return 0;
}

/* Stubs */
unsigned int retro_api_version(void) { return RETRO_API_VERSION; }
void retro_cheat_reset(void) {  }
void retro_cheat_set(unsigned index, bool enabled, const char *code) {  }
bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) { return false; }
void retro_set_controller_port_device(unsigned port, unsigned device) {  }
