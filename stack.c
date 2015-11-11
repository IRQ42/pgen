/**
 * Stack implementation
 */
#include <string.h>
#include <stdlib.h>
#include "stack.h"

static struct stack_node *
new_stack_node(void)
{
    return malloc(sizeof(struct stack_node));
}

/*
 * Initialize stack to a known state that is ready for use
 */
void
init_stack(struct stack *stack)
{
    memset(stack, 0, sizeof(struct stack));
}

/*
 * Destroys all data in stack and frees heap memory
 */
void 
destroy_stack(struct stack *stack)
{
    while (stack->depth) {
        struct stack_node *node;

        node = stack->stackp;
        stack->stackp = node->prev;
        free(node);
        --(stack->depth);    
    }
}

/*
 * Push data onto the stack. Returns the integer value 1 on success. In the
 * case that memory allocation fails this function will return the value 0
 * to indicate the failure to the calling function
 */
int 
push(struct stack *stack, int data)
{
    struct stack_node *node;

    if (!(node = new_stack_node()))
        return 0;

    node->data = data;

    // if not head of list point prev at current location in stack
    if (stack->stackp) 
       node->prev = stack->stackp; 

    // point stack pointer at new node
    stack->stackp = node;
    ++(stack->depth);

    return 1;
}

/*
 * Pop data from the stack. The result is placed in *data. This function in all
 * cases returns the depth of the stack when the function was first called,
 * which will be 1 + the stack depth after this function completes, unless the
 * stack was empty when this function is called, in which case this function
 * returns the value 0, indicating that the stack was empty when called.
 */
int
pop(struct stack *stack, int *data)
{
    struct stack_node *popped;
    int ret;

    ret = stack->depth;

    if (stack->depth == 0)
        return ret;

    *data = stack->stackp->data;
    popped = stack->stackp;
    stack->stackp = popped->prev;
    free(popped); 
    --(stack->depth);

    return ret;
}
