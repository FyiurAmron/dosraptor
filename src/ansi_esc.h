#ifndef ANSI_COLORS_H
#define ANSI_COLORS_H

#define ANSI_ESCAPE      "\x1b["
#define ANSI_END         "m"
#define ANSI_REGULAR     "0"
#define ANSI_BOLD        "1" // it's actually just "bright" in DOS e.g.
// #define ANSI_UNDERLINE   "4" // not widely supported or useful
#define ANSI_BLINK       "5"
// #define ANSI_FASTBLINK   "6" // not widely supported

#define ANSI_BLACK       "0"
#define ANSI_RED         "1"
#define ANSI_GREEN       "2"
#define ANSI_YELLOW      "3"
#define ANSI_BLUE        "4"
#define ANSI_MAGENTA     "5"
#define ANSI_CYAN        "6"
#define ANSI_WHITE       "7"

#define ANSI_FONT        "3"
#define ANSI_BKGD        "4"

// #define ANSI_BRIGHT_FONT "9" // not widely supported
// #define ANSI_BRIGHT_BKGD "10" // not widely supported

#define ANSI_RESET       ANSI_ESCAPE "0" ANSI_END

#endif // ANSI_COLORS_H
