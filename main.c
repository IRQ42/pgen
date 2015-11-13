/*****************************************************************************
 *                      Pgen, a password generator                        *
 *                      Author: IRQ42                                        *
 * Pagen is a simple program that generates pseudorandom alphanumeric      *
 * strings. It currently supports several modes, allowing the user to        *
 * generate many strings at once, and to select the type of characters used  *
 * to compose the strings (currently lowercase, uppercase, digits, and punct *
 * characters, as well as the ability to specify lists of additional         *
 * characters which will be either excluded, or included to give a high level*
 * of control over the character set used to generate passwords.             *
 * The length can also be specified (max password length is LONG_MAX)        *
 * All options ar specified as command line arguments when invoking pgen.    *
 *                                                                           *
 * When invoked without arguments, default parameters are used.              *
 * without options, a single string of default length and mode is produced.  *
 *                                                                           *
 * This software is currently free open source software, the author wrote    *
 * this program purely out of boredom in his free time, and provides no      *
 * warranty whatsover. You have been warned.                                 *
 ****************************************************************************/

#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>

#include "alloc.h"
#include "info.h"
#include "charset.h"
#include "color.h"

#define DEFAULT_PLEN    6
#define DEFAULT_PCNT    1

#define PLEN_MIN        0
#define PLEN_MAX        LONG_MAX
#define PCNT_MIN        0
#define PCNT_MAX        LONG_MAX

#define FAST_CHAR_OPT_MIN       1
#define FAST_CHAR_OPT_MAX       4
#define DEFAULT_FAST_CHAR_OPT   3

/**
 * This macro checks range of N; N is a member of the set [MIN,MAX] 
 */
#define IN_RANGE(MIN, MAX, N)    \
    ((N) >= (MIN) && (N) <= (MAX))

char *strdup(const char *str)
{
    int n = strlen(str) + 1;
    char *dup = malloc(n);
    if(dup)
    {
        strcpy(dup, str);
    }
    return dup;
}

static char *generate(size_t len, char *table, int fd);
static void die(char *msg, int status);
static char *str_rmdup(const char *s);
static void pgen_exit_cleanup(void);

static struct bst_node *g_alloc_bst     = NULL;      // bst for tracking heap allocation

int
main(int argc, char **argv)
{
    long            pass_cnt            = DEFAULT_PCNT;
    long            pass_len            = DEFAULT_PLEN;
    charset_opt_t   char_opt            = 0;
    long            fast_char_opt       = DEFAULT_FAST_CHAR_OPT;
    int             fast_char_opt_on    = 1;
    int             color_on            = 0;
    int             prefix_on           = 0;
    int             dump_on             = 0;
    int             no_sub              = 0;

    char *symtab                = NULL;
    char *pass_prefix           = NULL;
    char *exclude_list          = NULL;
    char *include_list          = NULL;
    int fd;

    int bad_args                = 0;

    if (atexit(pgen_exit_cleanup))
        die("E: cannot set exit function\n", EXIT_FAILURE);

    /* parse the command line arguments */
    for (int opt; (opt = getopt(argc, argv, "CLUDPNdnhl:p:f:c:e:i:")) != -1; ) {
        char *endptr; 

        switch (opt) {
        case 'C':       // color mode enable
            color_on = 1; 
            break;
        case 'L':       // character set switches override fast character mode
            char_opt |= LOWER;
            fast_char_opt_on = 0;
            break;
        case 'U':
            char_opt |= UPPER;
            fast_char_opt_on = 0;
            break;
        case 'D':
            char_opt |= DIGIT;
            fast_char_opt_on = 0;
            break;
        case 'P':
            char_opt |= PUNCT;
            fast_char_opt_on = 0;
            break;
        case 'N':
            fast_char_opt_on = 0;
            break;
        case 'd':       // dump symbol table
            dump_on = 1;
            break;
        case 'l':       // length
            errno = 0;
            pass_len = strtol(optarg, &endptr, 0);
            if ((errno == ERANGE && (pass_len == LONG_MAX || pass_len == LONG_MIN))
                       || (errno != 0 && pass_len == 0))
            {
                perror("strtol");
                exit(EXIT_FAILURE);
            } else if (endptr == optarg || *endptr != '\0') {
                fprintf(stderr, "%s: invalid argument '%s'\n", *argv, optarg);
                exit(EXIT_FAILURE);
            }
            break;
        case 'c':       // count
            errno = 0;
            pass_cnt = strtol(optarg, &endptr, 0);
            if ((errno == ERANGE && (pass_cnt == LONG_MAX || pass_cnt == LONG_MIN))
                       || (errno != 0 && pass_cnt == 0))
            {
                perror("strtol");
                exit(EXIT_FAILURE);
            } else if (endptr == optarg || *endptr != '\0') {
                fprintf(stderr, "%s: invalid argument '%s'\n", *argv, optarg);
                exit(EXIT_FAILURE); 
            }
            break;
        case 'f':       // fast character mode
            errno = 0;
            fast_char_opt = strtol(optarg, &endptr, 0);
            if ((errno == ERANGE && (fast_char_opt == LONG_MAX || fast_char_opt == LONG_MIN))
                       || (errno != 0 && fast_char_opt == 0))
            {
                perror("strtol");
                exit(EXIT_FAILURE);
            } else if (endptr == optarg || *endptr != '\0') {
                fprintf(stderr, "%s: invalid argument '%s'\n", *argv, optarg);
                exit(EXIT_FAILURE); 
            }
            break;
        case 'p':       // prefix
            if (!(pass_prefix = strdup(optarg))) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }
            if (!pgen_alloc_bst_insert(&g_alloc_bst, pass_prefix)) {
                die("pgen_alloc_bst_insert: allocation failed\n", EXIT_FAILURE);
            }
            prefix_on = 1;
            break;
        case 'n':       // no substitution
            no_sub = 1;
            break;
        case 'e':
            qsort(optarg, strlen(optarg), sizeof(char), cmpchar);
            if (!(exclude_list = str_rmdup(optarg))) {
                die("str_rmdup: allocation failed\n", EXIT_FAILURE);
            }
            if (!pgen_alloc_bst_insert(&g_alloc_bst, exclude_list)) {
                die("pgen_alloc_bst_insert: allocation failed\n", EXIT_FAILURE);
            }
            break;
        case 'i':
            qsort(optarg, strlen(optarg), sizeof(char), cmpchar);
            if (!(include_list = str_rmdup(optarg))) {
                die("str_rmdup: allocation failed\n", EXIT_FAILURE);
            }
            if (!pgen_alloc_bst_insert(&g_alloc_bst, include_list)) {
                die("pgen_alloc_bst_insert: allocation failed\n", EXIT_FAILURE);
            }
            break;
        case 'h':
            show_info(*argv);
            exit(EXIT_SUCCESS);
        case '?':
            fprintf(stderr, "%s -h for usage\n", *argv);
            // fall through
        default:
            exit(EXIT_FAILURE);
        }
    }

    // print invalid args to stderr
    for (int i = optind; i < argc; ++i) {
        fprintf(stderr, "%s: %s '%s'\n", *argv, "unknown option --", argv[i]);
        bad_args = 1;
    }

    // check options for sanity
    if (!IN_RANGE(FAST_CHAR_OPT_MIN,
                  FAST_CHAR_OPT_MAX,
                  fast_char_opt))
    {
        fprintf(stderr, "%s: Bad character mode option '%li'.\n",
                *argv, fast_char_opt);
        bad_args = 1;
    }
    if (!IN_RANGE(PLEN_MIN, PLEN_MAX, pass_len)) {
        fprintf(stderr, "%s: Bad password length (%li)\n", *argv, pass_len);
        bad_args = 1;
    }
    if (!IN_RANGE(PCNT_MIN, PCNT_MAX, pass_cnt)) {
        fprintf(stderr, "%s: Bad password count (%li)\n", *argv, pass_cnt);
        bad_args = 1;
    }
    if (prefix_on && !no_sub && (int) strlen(pass_prefix) >= pass_len) {
        fprintf(stderr, "%s: Prefix must be shorter than password length\n"
                "Use -n to turn off prefix substition\n",
                *argv);
        bad_args = 1;
    }

    // if arguments are bad, cleanup and exit
    if (bad_args) {
        fprintf(stderr, "%s -h for usage\n", *argv);
        exit(EXIT_FAILURE);
    }

    // set char_opt if using fast option
    if (fast_char_opt_on) {
        switch (fast_char_opt) {
        case 4:
            char_opt |= PUNCT;
            // fall through
        case 3:
            char_opt |= DIGIT;
            // fall through
        case 2:
            char_opt |= UPPER;
            // fall through
        case 1:
            char_opt |= LOWER;
            break;
        default:
            die("E: encountered error setting char_opt\n",
                EXIT_FAILURE);
            break;
        }
    }

    // if using prefix, shorten length to make room for prefix
    if (prefix_on && !no_sub)
        pass_len -= strlen(pass_prefix);

    // generate character set for password symbol table
    symtab = generate_charset(char_opt, exclude_list, include_list);
    if (!symtab) {
        die("generate_charset: allocation failed\n", EXIT_FAILURE);
    }
    if (exclude_list) pgen_free(&g_alloc_bst, exclude_list);
    if (include_list) pgen_free(&g_alloc_bst, include_list);
    if (!pgen_alloc_bst_insert(&g_alloc_bst, symtab)) {
        die("pgen_alloc_bst_insert: allocation failed\n", EXIT_FAILURE);
    }

    // dump symbol table
    if (dump_on)
        printf("symbols: %s\n", symtab);
    
    // check for zero length symbol table
    if (strlen(symtab) == 0) {
        fprintf(stderr, "%s: invalid table length '0'\n", *argv);
        exit(EXIT_FAILURE);
    }

    // open /dev/urandom for use as pseudorandom source
    if ((fd = open("/dev/urandom", O_RDONLY)) == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < pass_cnt; ++i) {
        char *pass;

        /* We needn't add pass to tree since we free ASAP */
        if (!(pass = generate(pass_len, symtab, fd))) {
            fprintf(stderr, "%s: failed to generate string\n", *argv);
            exit(EXIT_FAILURE);
        }
        
        printf((color_on) ? ANSI_COLORTERM_YELLOW("%s%s\n") : "%s%s\n",
               (prefix_on) ? pass_prefix : "", pass);

        free(pass);
    }
    fflush(stdout);
    pgen_free(&g_alloc_bst, pass_prefix);
    close(fd);

    // cleanup after return
    return EXIT_SUCCESS;
}

/**
 * Returns a copy of a string, with duplicate characters removed. s must be
 * in ascending sorted order
 */
static char *
str_rmdup(const char *s)
{
    size_t  s_len;
    char    *s_nodup;
    char    *p;
    char    prev;
    char    *ret;

    s_len = strlen(s);

    // initially allocate memory for string as long as s
    if (!(s_nodup = malloc(sizeof(char) * (s_len + 1))))
        return NULL;

    /*
     * duplicates will be in consecutive positions in sorted list. Copy
     * only if current is greater than previous
     */
    p     = s_nodup;
    prev  = '\0';
    for (size_t i = 0; i < s_len; ++i) {
        if (s[i] > prev)
            *p++ = s[i];
        prev = s[i];
    }
    *p = '\0';
    
    if (!(ret = realloc(s_nodup, sizeof(char) * (p - s_nodup + 1))))
        free(s_nodup);

    return ret;
}

/**
 * Print an error message to stderr and exit
 */
static void
die(char *msg, int status)
{
    if (msg)
        fputs(msg, stderr);

    exit(status);
}

/**
 * Generates a pseudorandom string, composed of symbols selected from table.
 * table must be a pointer to a NULL terminated character array, this is
 * necessary for the function to be able to determine length without taking
 * the length as a parameter.
 * Returns a pointer to the generated string on success, on failure a NULL
 * pointer will be returned.
 */
static char *
generate(size_t len, char *table, int fd)
{
    char            *pstring = NULL;
    size_t          table_len = strlen(table);

    // check for invalid parameters
    if (table_len < 1) {
        fprintf(stderr, "E: invalid table length '%zd'\n", table_len);
        goto fail;
    }

    if (!(pstring = malloc((len + 1) * sizeof(char)))) {
        fprintf(stderr, "E: failed to allocate memory (malloc)\n");
        goto fail;
    }

    // produce a pseudorandom string comprised of characters from table
    for (size_t i = 0; i < len; ++i) {
        unsigned int rand;
        unsigned int max_rand = UINT_MAX - ((long int) UINT_MAX + 1) % table_len;

        if (read(fd, &rand, sizeof rand) == -1) {
            perror("read");
            goto fail;
        }
        
        // ensure against modulo bias
        while (rand > max_rand) {
            if (read(fd, &rand, sizeof rand) == -1) {
                perror("read");
                goto fail;
            }
        }

        pstring[i] = table[rand % table_len];
    }
    pstring[len] = '\0';

    return pstring;

fail:
    if (pstring) free(pstring);
    return NULL;
}

static void
pgen_exit_cleanup(void)
{
    pgen_alloc_cleanup(g_alloc_bst); 
    dealloc_bst(g_alloc_bst);
}
