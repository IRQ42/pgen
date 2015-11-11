/*****************************************************************************
 * Allocation manager implementation for pgen
 ****************************************************************************/

#include <stdlib.h>
#include <stddef.h>

#include "alloc.h"

static struct bst_node *
new_bst_node(void)
{
    return malloc(sizeof(struct bst_node));
}

/*
 * Returns pointer to in-order successor of bst_node 
 */ 
static struct bst_node *
get_ino_successor(const struct bst_node *p)
{
    struct bst_node *cur = p->right;

    while (cur->left != NULL)
        cur = cur->left;
    
    return cur;
}

/**
 * Insert pointer in BST. Returns memory address of the inserted node
 * containing the inserted key, or, if pointer already exists in 
 * tree, the address of the existing node is returned. A return value
 * of NULL indicates a failure to allocate memory.
 */
struct bst_node * 
pgen_alloc_bst_insert(struct bst_node **p, void *data)
{ 
    struct bst_node *ret = *p;

    if (!*p) {
       *p = new_bst_node(); 
       if (*p) {
           (*p)->datap = data;
           (*p)->left = (*p)->right = NULL;
       }
       ret = *p;
    } else if (data < (*p)->datap) {
       ret = pgen_alloc_bst_insert(&(*p)->left, data);
    } else if (data > (*p)->datap) {
       ret = pgen_alloc_bst_insert(&(*p)->right, data);
    } else {
        ; // entry already exists in tree
    }

    return ret;
}

/**
 * Deletes key from bst, returns pointer to new root
 */
static struct bst_node * 
pgen_alloc_bst_delete(struct bst_node *p, void *data)
{
    if (!p) {
        return p;
    } else if (data < p->datap) {
        p->left = pgen_alloc_bst_delete(p->left, data);
    } else if (data > p->datap) {
        p->right = pgen_alloc_bst_delete(p->right, data);
    } else {
        // found data
        struct bst_node *tmp;

        // node with one or zero children, parent points at node's child
        if (!p->left) {
            tmp = p->right;
            free(p);
            return tmp;
        } else if (!p->right) {
            tmp = p->left;
            free(p);
            return tmp;
        }
        // if both children are NULL, parent points at NULL child

        // node with two children, find in-order successor
        tmp = get_ino_successor(p);
        p->datap = tmp->datap;

        // remove in-order successor (now tmp)
        free(tmp);
    }

    return p;
}

/**
 * Allocates block of memory from the heap by calling malloc, inserts
 * pointer to block in bst. A return value of NULL indicates that malloc
 * failed.
 */
void *
pgen_alloc(struct bst_node *bst, size_t size)
{
    void *p;

    if (!(p = malloc(size)))
        return NULL;

    if (!pgen_alloc_bst_insert(&bst, p))
        return NULL;
    
    return p;
}

/**
 * Free the memory space pointed to by ptr. Removes ptr from bst.
 */
void
pgen_free(struct bst_node **bst, void *ptr)
{
    free(ptr);
    *bst = pgen_alloc_bst_delete(*bst, ptr);     
} 

/**
 * Free all memory allocated by pgen_alloc.
 */
void
pgen_alloc_cleanup(struct bst_node *bst)
{
    if (!bst)
        return;

    pgen_alloc_cleanup(bst->left);
    pgen_alloc_cleanup(bst->right);

    if (bst->datap) free(bst->datap);
}

/**
 * Free memory associated with bst structure
 */
void 
dealloc_bst(struct bst_node *bst)
{
    if (!bst)
        return;

    dealloc_bst(bst->left);
    dealloc_bst(bst->right);

    free(bst);
}
