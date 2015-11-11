#ifndef STACK_H
#define STACK_H

struct stack_node {
    int data;
    struct stack_node *prev;
};

struct stack {
    int depth;
    struct stack_node *stackp;
};

void init_stack(struct stack *stack);
void destroy_stack(struct stack *stack);
int push(struct stack *stack, int data);
int pop(struct stack *stack, int *data);

#endif  /* STACK_H */
