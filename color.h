#ifndef COLOR_H
#define COLOR_H

/* SGR escpape sequences for ANSI terminals to set attributes */
#define ANSI_ATTR_RESET         "\x1b[0m"
#define ANSI_SETFG_BLACK        "\x1b[30m"
#define ANSI_SETFG_WHITE        "\x1b[37m"
#define ANSI_SETFG_RED          "\x1b[31m"
#define ANSI_SETFG_GREEN        "\x1b[32m"
#define ANSI_SETFG_YELLOW       "\x1b[33m"
#define ANSI_SETFG_BLUE         "\x1b[34m"
#define ANSI_SETFG_MAGENTA      "\x1b[35m"
#define ANSI_SETFG_CYAN         "\x1b[35m"

/* The following macros prepare a string for color, then resetting term */
#define ANSI_COLORTERM_BLACK(s)     \
    ANSI_SETFG_BLACK    s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_WHITE(s)     \
    ANSI_SETFG_WHITE    s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_RED(s)       \
    ANSI_SETFG_RED      s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_GREEN(s)     \
    ANSI_SETFG_GREEN    s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_YELLOW(s)    \
    ANSI_SETFG_YELLOW   s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_BLUE(s)      \
    ANSI_SETFG_BLUE     s   ANSI_ATTR_RESET

#define ANSI_COLORTERM_MAGENTA(s)   \
    ANSI_SETFG_MAGENTA  s   ANSI_ATTR_RESET

#define ANSI_COLOR_FPRINTF(SGR, STREAM, FORMAT, ...)    \
    fputs(SGR, STREAM);                                 \
    fprintf(STREAM, FORMAT ANSI_ATTR_RESET, __VA_ARGS__)

#endif /* COLOR_H */
