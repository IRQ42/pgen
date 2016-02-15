\#include <stdio.h>

#include "info.h"

#define INFO_HEAD                                                                               \
    "Usage: %s [OPTIONS]\n"                                                                     \
    "Generate a pseudorandom password. Uses /dev/urandom as pseudorandom source.\n"             \
 
#define OPTIONS_INFO                                                                            \
    "OPTIONS:\n"                                                                                \
    "   -L      enable lower case characters in output\n"                                       \
    "   -U      enable upper case characters in output\n"                                       \
    "   -D      enable digit characters in output\n"                                            \
    "   -P      enable punctuation characters in output\n"                                      \
    "   -N      enable no characters. dummy option to override default behavior when\n"         \
    "           no other character set options will be specified in the case that it is\n"      \
    "           desired for the output to be composed of characters specified in the\n"         \
    "           include list only using the -e option\n"                                        \
    "\n"                                                                                        \
    "   -f      fast character set mode, used to control which characters\n"                    \
    "           will be included in the password produced, use a numerical option\n"            \
    "           from the table below (see: Fast character set mode options). Fast \n"           \
    "           character set mode options will be override by any of the above character\n"    \
    "           mode switches when used together simultaneously\n"                              \
    "\n"                                                                                        \
    "   -l      password length in characters\n"                                                \
    "   -c      password count to be generated\n"                                               \
    "   -p      optional prefix to be prepended to password produced\n"                         \
    "   -n      no prefix substitution -- when this option is specified\n"                      \
    "           the total length will be equal to prefix length, plus length specified\n"       \
    "           via the -l option\n"                                                            \
    "   -h      display this help and exit\n"                                                   \
    "   -e      excludes any characters in the string given as argument\n"                      \
    "   -i      specifies additional characters to include in character set\n"                  \
    "\n"                                                                                        \
    "   -d      dump symbol table\n"                                                            \
    "   -C      enable colorful text output\n"                                                  \
    "        * Colors are specified by supplying an optional argument to the -C option.\n"  \
    "        * If the -C switch is on w/o an optional arg, a default color will be used\n"  \
    "        * The colors supported are standard on 8-color ANSI terminals. See below:\n"   \
    "           * 0 -- Black\n"                             \
    "           * 1 -- Red\n"                               \
    "           * 2 -- Green\n"                             \
    "           * 3 -- Yellow\n"                            \
    "           * 4 -- Blue\n"                              \
    "           * 5 -- Magenta\n"                           \
    "           * 6 -- Cyan\n"                              \
    "           * 7 -- White\n"                             \
    "\n"                                                                                        \
    "Without specifying any options, default parameters will be used\n"                         \
    "Default parameters are fast character mode 3 and a length of 8, equivalent to\n"           \
    "%s -f3 -l8. Default behavior can be overridden by specifying the desired options\n"        \
 
#define MODE_INFO                                                                               \
    "-f Fast character set mode options: \n"                                                    \
    "   1   only lowercase alphabetic characters will be used, equivalent to -L\n"              \
    "   2   lowercase and uppercase alphabetic characters, equivalent to -LU\n"                 \
    "   3   lowercase/uppercase and numeric characters, equivalent to -LUD\n"                   \
    "   4   alphanumeric plus special characters. Unless an exclude list\n"                     \
    "       is specified, the result will be the use of all printable non-whitespace\n"         \
    "       ASCII characters. equivalent  -LUDP\n"                                              \
 
#define EXAMPLE_INFO                                                                            \
    "Examples:\n"                                                                               \
    "   %s -f1 -l42\tProduce a password of length 42 using only\n"                              \
    "\t\t\tlowercase alphabetic characters\n"                                                   \
    "\t\t\te.g. crtgjazxwckmkyswgfqplojohplmfsdjamgljacwfh\n"                                   \
    "\n"                                                                                        \
    "   %s -l8 -pfoo-\tProduce password of length 8, with prefix \"foo-\".\n"                   \
    "\t\t\te.g. foo-lOdk.\n"                                                                    \
    "\n"                                                                                        \
    "   %s -l8 -pfoo- -n\tProduces password of length 8 with prefix \"foo-\".\n"                \
    "\t\t\tNo prefix substitution\n"                                                            \
    "\t\t\te.g. foo-n2M2zKP4\n"                                                                 \
    "\n"                                                                                        \
    "   %s -f3 -i'!@#$'\tProduces default length (8) character password using\n"                \
    "\t\t\talphanumeric characters plus !@#$ as special chars\n"                                \
    "\t\t\te.g. 7C!#KUHf\n"                                                                     \
    "\n"                                                                                        \
    "   %s -C4 \t\tProduces default length password using defaults, except\n"                   \
    "\t\t\tconsole output is in blue text\n"                                                    \
    "\t\t\te.g. \x1b[34m2FWCZnGL\x1b[0m\n"

void show_info(const char *fname)
{
    printf(INFO_HEAD, fname);
    fputc('\n', stdout);
    printf(OPTIONS_INFO, fname);
    fputc('\n', stdout);
    printf(MODE_INFO);
    fputc('\n', stdout);
    printf(EXAMPLE_INFO, fname
           , fname
           , fname
           , fname
           , fname);
}
