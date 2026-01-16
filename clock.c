#include <conio.h>
#include <peekpoke.h>
#include <c64.h>

/* C64 jiffy clock is at $A0-$A2 (TI in BASIC) */
#define TI_HIGH   0xA0
#define TI_MID    0xA1
#define TI_LOW    0xA2

/* Screen dimensions */
#define SCREEN_WIDTH  40
#define SCREEN_HEIGHT 25

/* Digit dimensions */
#define DIGIT_WIDTH  5
#define DIGIT_HEIGHT 7

/* PETSCII block characters (screen codes) */
#define BLOCK_FULL   0xA0  /* Reversed space - full block */
#define BLOCK_EMPTY  0x20  /* Space - empty */

/* Triangle characters for corner clipping (screen codes) */
#define TRI_BR       0x7E  /* Lower-right filled - clips top-left outer corner */
#define TRI_BL       0x7C  /* Lower-left filled - clips top-right outer corner */
#define TRI_TR       0x7B  /* Upper-right filled - clips bottom-left outer corner */
#define TRI_TL       0x6C  /* Upper-left filled - clips bottom-right outer corner */

/* Reverse triangles for inner corner fills */
#define TRI_TL_REV   0x69  /* Upper-left empty - fills top-left inner corner */
#define TRI_TR_REV   0x5F  /* Upper-right empty - fills top-right inner corner */
#define TRI_BL_REV   0x69  /* Lower-left empty - fills bottom-left inner corner */
#define TRI_BR_REV   0x5F  /* Lower-right empty - fills bottom-right inner corner */

/* Pattern values */
#define P_EMPTY      0   /* Empty space */
#define P_FULL       1   /* Full block */
#define P_TL_CLIP    2   /* Top-left outer corner clip */
#define P_TR_CLIP    3   /* Top-right outer corner clip */
#define P_BL_CLIP    4   /* Bottom-left outer corner clip */
#define P_BR_CLIP    5   /* Bottom-right outer corner clip */
#define P_TL_FILL    6   /* Top-left inner corner fill */
#define P_TR_FILL    7   /* Top-right inner corner fill */
#define P_BL_FILL    8   /* Bottom-left inner corner fill */
#define P_BR_FILL    9   /* Bottom-right inner corner fill */

/* Big digit patterns (7 rows x 5 cols each) */
/* Uses P_* values: 0=empty, 1=full, 2-5=outer corner clips, 6-9=inner corner fills */
static const unsigned char digit_patterns[10][7][5] = {
    /* 0 */
    {
        {2,1,1,1,3},
        {1,1,6,1,1},
        {1,1,0,1,1},
        {1,1,0,1,1},
        {1,1,0,1,1},
        {1,1,8,1,1},
        {4,1,1,1,5}
    },
    /* 1 */
    {
        {0,2,1,3,0},
        {0,1,1,1,0},
        {0,0,1,1,0},
        {0,0,1,1,0},
        {0,0,1,1,0},
        {0,0,1,1,0},
        {2,1,1,1,3}
    },
    /* 2 */
    {
        {2,1,1,1,3},
        {6,0,0,1,1},
        {0,0,0,1,1},
        {2,1,1,1,5},
        {1,1,0,0,7},
        {1,1,0,0,0},
        {4,1,1,1,3}
    },
    /* 3 */
    {
        {2,1,1,1,3},
        {6,0,0,1,1},
        {0,0,0,1,1},
        {0,1,1,1,5},
        {0,0,0,1,1},
        {8,0,0,1,1},
        {2,1,1,1,5}
    },
    /* 4 */
    {
        {2,3,0,2,3},
        {1,1,0,1,1},
        {1,1,0,1,1},
        {4,1,1,1,1},
        {8,0,0,1,1},
        {0,0,0,1,1},
        {0,0,0,4,5}
    },
    /* 5 */
    {
        {2,1,1,1,3},
        {1,1,0,0,7},
        {1,1,0,0,0},
        {4,1,1,1,3},
        {6,0,0,1,1},
        {8,0,0,1,1},
        {2,1,1,1,5}
    },
    /* 6 */
    {
        {2,1,1,1,3},
        {1,1,0,0,7},
        {1,1,0,0,0},
        {1,1,1,1,3},
        {1,1,6,1,1},
        {1,1,8,1,1},
        {4,1,1,1,5}
    },
    /* 7 */
    {
        {2,1,1,1,3},
        {6,0,0,1,1},
        {0,0,0,1,1},
        {0,0,0,1,1},
        {0,0,0,1,1},
        {0,0,0,1,1},
        {0,0,0,4,5}
    },
    /* 8 */
    {
        {2,1,1,1,3},
        {1,1,6,1,1},
        {1,1,8,1,1},
        {4,1,1,1,5},
        {1,1,6,1,1},
        {1,1,8,1,1},
        {4,1,1,1,5}
    },
    /* 9 */
    {
        {2,1,1,1,3},
        {1,1,6,1,1},
        {1,1,8,1,1},
        {4,1,1,1,1},
        {8,0,0,1,1},
        {0,0,0,1,1},
        {2,1,1,1,5}
    }
};

/* Colon pattern */
static const unsigned char colon_pattern[7] = {
    0,
    1,
    1,
    0,
    1,
    1,
    0
};

/* Screen memory base */
#define SCREEN_MEM 0x0400

/* Map pattern value to screen code */
unsigned char pattern_to_char(unsigned char p) {
    switch (p) {
        case P_EMPTY:   return BLOCK_EMPTY;
        case P_FULL:    return BLOCK_FULL;
        case P_TL_CLIP: return TRI_BR;      /* Outer top-left corner */
        case P_TR_CLIP: return TRI_BL;      /* Outer top-right corner */
        case P_BL_CLIP: return TRI_TR;      /* Outer bottom-left corner */
        case P_BR_CLIP: return TRI_TL;      /* Outer bottom-right corner */
        case P_TL_FILL: return TRI_TL_REV;  /* Inner top-left corner */
        case P_TR_FILL: return TRI_TR_REV;  /* Inner top-right corner */
        case P_BL_FILL: return TRI_BL_REV;  /* Inner bottom-left corner */
        case P_BR_FILL: return TRI_BR_REV;  /* Inner bottom-right corner */
        default:        return BLOCK_EMPTY;
    }
}

/* Draw a big digit at screen position */
void draw_digit(unsigned char digit, unsigned char x, unsigned char y) {
    unsigned char row, col;
    unsigned char *screen;

    for (row = 0; row < DIGIT_HEIGHT; ++row) {
        screen = (unsigned char *)(SCREEN_MEM + (y + row) * 40 + x);
        for (col = 0; col < DIGIT_WIDTH; ++col) {
            screen[col] = pattern_to_char(digit_patterns[digit][row][col]);
        }
    }
}

/* Draw colon separator */
void draw_colon(unsigned char x, unsigned char y) {
    unsigned char row;
    unsigned char *screen;

    for (row = 0; row < DIGIT_HEIGHT; ++row) {
        screen = (unsigned char *)(SCREEN_MEM + (y + row) * 40 + x);
        screen[0] = colon_pattern[row] ? BLOCK_FULL : BLOCK_EMPTY;
    }
}

/* Read the jiffy clock and convert to H:M:S */
void get_time(unsigned char *hours, unsigned char *minutes, unsigned char *seconds) {
    unsigned long jiffies;
    unsigned long total_seconds;

    /* Read 24-bit jiffy clock (counts at 60 Hz on NTSC, ~50 Hz PAL) */
    /* Using 60 Hz for NTSC */
    jiffies = ((unsigned long)PEEK(TI_HIGH) << 16) |
              ((unsigned long)PEEK(TI_MID) << 8) |
              (unsigned long)PEEK(TI_LOW);

    /* Convert to seconds (60 jiffies per second on NTSC) */
    total_seconds = jiffies / 60;

    /* Extract hours, minutes, seconds */
    *hours = (unsigned char)((total_seconds / 3600) % 24);
    *minutes = (unsigned char)((total_seconds / 60) % 60);
    *seconds = (unsigned char)(total_seconds % 60);
}

/* Display the full clock */
void display_clock(unsigned char hours, unsigned char minutes, unsigned char seconds) {
    unsigned char start_x = 0;  /* Starting X position */
    unsigned char start_y = 9;  /* Center vertically */
    unsigned char x = start_x;

    /* Hours */
    draw_digit(hours / 10, x, start_y);
    x += DIGIT_WIDTH + 1;
    draw_digit(hours % 10, x, start_y);
    x += DIGIT_WIDTH + 1;

    /* Colon */
    draw_colon(x, start_y);
    x += 2;

    /* Minutes */
    draw_digit(minutes / 10, x, start_y);
    x += DIGIT_WIDTH + 1;
    draw_digit(minutes % 10, x, start_y);
    x += DIGIT_WIDTH + 1;

    /* Colon */
    draw_colon(x, start_y);
    x += 2;

    /* Seconds */
    draw_digit(seconds / 10, x, start_y);
    x += DIGIT_WIDTH + 1;
    draw_digit(seconds % 10, x, start_y);
}

int main(void) {
    unsigned char hours, minutes, seconds;
    unsigned char last_seconds = 255;

    /* Set background and border colors */
    bgcolor(COLOR_BLUE);
    bordercolor(COLOR_BLUE);

    /* Clear screen */
    clrscr();

    /* Set text color for blocks */
    textcolor(COLOR_CYAN);

    /* Fill color RAM with cyan */
    {
        unsigned int i;
        for (i = 0; i < 1000; ++i) {
            POKE(0xD800 + i, COLOR_CYAN);
        }
    }

    /* Main loop - update display continuously */
    while (1) {
        /* Get current time from jiffy clock */
        get_time(&hours, &minutes, &seconds);

        /* Only redraw if seconds changed */
        if (seconds != last_seconds) {
            display_clock(hours, minutes, seconds);
            last_seconds = seconds;
        }

        /* Check for keypress to exit */
        if (kbhit()) {
            cgetc();  /* consume the key */
            break;
        }
    }

    /* Restore default colors */
    bgcolor(COLOR_BLUE);
    bordercolor(COLOR_LIGHTBLUE);
    textcolor(COLOR_LIGHTBLUE);
    clrscr();

    return 0;
}
