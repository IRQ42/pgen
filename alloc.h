#ifndef ALLOC_H
#define ALLOC_H

struct bst_node {
    void *datap;
    struct bst_node *left;
    struct bst_node *right;
};

void *pgen_alloc(struct bst_node *bst, size_t size);
void pgen_free(struct bst_node **bst, void *ptr);
void pgen_alloc_cleanup(struct bst_node *bst);
void dealloc_bst(struct bst_node *bst);
struct bst_node *pgen_alloc_bst_insert(struct bst_node **p, void *data);

#endif  /* ALLOC_H */
