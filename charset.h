#ifndef CHARSET_H
#define CHARSET_H

typedef enum {
    LOWER = (1<<0),
    UPPER = (1<<1),
    DIGIT = (1<<2),
    PUNCT = (1<<3),       

} charset_opt_t;

int cmpchar(const void *c1, const void *c2);
char *generate_charset(charset_opt_t opt, const char *exclude, const char *include);

#endif
