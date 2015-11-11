#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "charset.h"
#include "stack.h"

/**
 * Returns an integer less than, equal to, or greater than zero if the
 * character pointed to by c1 is found, respectively, to be less than, 
 * to match, or be greater than the character pointed to by c2
 */
int
cmpchar(const void *c1, const void *c2)
{
    return *(char *) c1 - *(char *) c2;
}

/**
 * Returns a null terminated character array to be used as symbol lookup table.
 * include/exclude should be a list of characters in the form of a sorted
 * string in ascending order.
 */
char *
generate_charset(charset_opt_t opt, const char *exclude, const char *include)
{
    struct stack    stack;
    char            *symtab;
    size_t          symtab_size;

    init_stack(&stack);

    // start at end so characters can be popped in order    
    for (int i = '~'; i >= '!'; --i) {
        int     allowed     = 0;
        int     disallowed  = 0;

        // allow according to options/include list
        if ((opt & LOWER && islower(i)) ||
            (opt & UPPER && isupper(i)) ||
            (opt & DIGIT && isdigit(i)) ||
            (opt & PUNCT && ispunct(i)) ||
            (include && bsearch(&i, include, strlen(include),
                                sizeof(char), cmpchar))) 
        {
            allowed = 1;
        }

        // disallow if found in exclude list
        if (exclude && 
            bsearch(&i, exclude, strlen(exclude),
                    sizeof(char), cmpchar))
        {
            disallowed = 1;
        }

        // push allowed characters onto stack
        if (allowed && !disallowed)
            if (!push(&stack, i)) return NULL;
    }

    /* Populate symbol table */
    symtab_size = stack.depth;
    symtab = malloc((symtab_size + 1) * sizeof(char));
    for (int i = 0, c; pop(&stack, &c); ++i)
        symtab[i] = (char) c;
    symtab[symtab_size] = '\0'; 
    
    destroy_stack(&stack);

    return symtab;
}
