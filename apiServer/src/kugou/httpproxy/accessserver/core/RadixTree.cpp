#include <iostream>
#include <kugou/httpproxy/accessserver/core/RadixTree.h>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{
boost::shared_ptr<ngx_radix_node_t> RadixTree::nullPtr;

RadixTree::RadixTree(void)
{
    uint32_t           key, mask, inc;
    
    /*
    ngx_radix_tree_t  *tree;
    tree = ngx_palloc(pool, sizeof(ngx_radix_tree_t));
    if (tree == NULL) {
        return NULL;
    }
    */

    //tree->pool = pool;
    this->free = nullPtr;
    this->start = NULL;
    this->size = 0;

    this->root.reset(new ngx_radix_node_t());
    if (this->root == nullPtr) 
    {
        //return NULL;
    }

    this->root->right = nullPtr;
    this->root->left = nullPtr;
    this->root->parent = nullPtr;
    this->root->value = NGX_RADIX_NO_VALUE;
#if 0
    if (preallocate == 0) 
    {
        return tree;
    }

    /*
     * Preallocation of first nodes : 0, 1, 00, 01, 10, 11, 000, 001, etc.
     * increases TLB hits even if for first lookup iterations.
     * On 32-bit platforms the 7 preallocated bits takes continuous 4K,
     * 8 - 8K, 9 - 16K, etc.  On 64-bit platforms the 6 preallocated bits
     * takes continuous 4K, 7 - 8K, 8 - 16K, etc.  There is no sense to
     * to preallocate more than one page, because further preallocation
     * distributes the only bit per page.  Instead, a random insertion
     * may distribute several bits per page.
     *
     * Thus, by default we preallocate maximum
     *     6 bits on amd64 (64-bit platform and 4K pages)
     *     7 bits on i386 (32-bit platform and 4K pages)
     *     7 bits on sparc64 in 64-bit mode (8K pages)
     *     8 bits on sparc64 in 32-bit mode (8K pages)
     */

    if (preallocate == -1) {
        switch (ngx_pagesize / sizeof(ngx_radix_node_t)) {

        /* amd64 */
        case 128:
            preallocate = 6;
            break;

        /* i386, sparc64 */
        case 256:
            preallocate = 7;
            break;

        /* sparc64 in 32-bit mode */
        default:
            preallocate = 8;
        }
    }

    mask = 0;
    inc = 0x80000000;

    while (preallocate--) {

        key = 0;
        mask >>= 1;
        mask |= 0x80000000;

        do {
            if (ngx_radix32tree_insert(tree, key, mask, NGX_RADIX_NO_VALUE)
                != NGX_OK)
            {
                return NULL;
            }

            key += inc;

        } while (key);

        inc >>= 1;
    }

    return tree;
#endif
}

RadixTree::~RadixTree(void)
{
    //TODO 删除所有节点
    return ;
}


bool RadixTree::insertNode(uint32_t key, uint32_t mask, uintptr_t value)
//ngx_int_t ngx_radix32tree_insert(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask,  uintptr_t value)
{
    uint32_t           bit;
    boost::shared_ptr<ngx_radix_node_t>  node, next;

    bit = 0x80000000;

    node = this->root;
    next = this->root;

    while (bit & mask) {
        if (key & bit) {
            next = node->right;

        } else {
            next = node->left;
        }

        if (next == nullPtr) {
            break;
        }

        bit >>= 1;
        node = next;
    }

    if (next) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            return false;
        }

        node->value = value;
        return true;
    }

    while (bit & mask) {
        next.reset(new ngx_radix_node_t());
        if (next == nullPtr) {
            return false;
        }

        next->right = nullPtr;
        next->left = nullPtr;
        next->parent = node;
        next->value = NGX_RADIX_NO_VALUE;

        if (key & bit) {
            node->right = next;

        } else {
            node->left = next;
        }

        bit >>= 1;
        node = next;
    }

    node->value = value;

    return true;
}

bool RadixTree::deleteNode(uint32_t key, uint32_t mask)
//ngx_int_t ngx_radix32tree_delete(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask)
{
    uint32_t           bit;
    boost::shared_ptr<ngx_radix_node_t> node;

    bit = 0x80000000;
    node = this->root;

    while (node && (bit & mask)) {
        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;
    }

    if (node == nullPtr) {
        return false;
    }

    if (node->right || node->left) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            node->value = NGX_RADIX_NO_VALUE;
            return true;
        }

        return false;
    }

    for ( ;; ) {
        if (node->parent->right == node) {
            node->parent->right = nullPtr;

        } else {
            node->parent->left = nullPtr;
        }

        node->right = this->free;
        this->free = node;

        node = node->parent;

        if (node->right || node->left) {
            break;
        }

        if (node->value != NGX_RADIX_NO_VALUE) {
            break;
        }

        if (node->parent == nullPtr) {
            break;
        }
    }

    return true;
}

int  RadixTree::findNode(uint32_t key)
//uintptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, uint32_t key)
{
    uint32_t           bit;
    uintptr_t          value;
    boost::shared_ptr<ngx_radix_node_t>  node;

    bit = 0x80000000;
    value = NGX_RADIX_NO_VALUE;
    node = this->root;

    while (node) {
        if (node->value != NGX_RADIX_NO_VALUE) {
            value = node->value;
        }

        if (key & bit) {
            node = node->right;

        } else {
            node = node->left;
        }

        bit >>= 1;
    }

    return value;
}

#if 0
static ngx_radix_node_t *ngx_radix_alloc(ngx_radix_tree_t *tree)
{
    ngx_radix_node_t  *p;

    if (tree->free) {
        p = tree->free;
        tree->free = tree->free->right;
        return p;
    }

    if (tree->size < sizeof(ngx_radix_node_t)) {
        tree->start = ngx_pmemalign(tree->pool, ngx_pagesize, ngx_pagesize);
        if (tree->start == NULL) {
            return NULL;
        }

        tree->size = ngx_pagesize;
    }

    p = (ngx_radix_node_t *) tree->start;
    tree->start += sizeof(ngx_radix_node_t);
    tree->size -= sizeof(ngx_radix_node_t);

    return p;
}
#endif

}
}
}
}
