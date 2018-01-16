#ifndef RADIXTREE_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H
#define RADIXTREE_CORE_ACCESSSERVER_HTTPPROXY_KUGOU_H

#include <boost/shared_ptr.hpp>

namespace kugou
{
namespace httpproxy
{
namespace accessserver
{
namespace core
{

//#define NGX_RADIX_NO_VALUE   (uintptr_t) -1
#define NGX_RADIX_NO_VALUE   (int) -1

typedef struct ngx_radix_node_s  ngx_radix_node_t;
#ifdef _WIN32
typedef unsigned int uint32_t;
#endif

struct ngx_radix_node_s 
{
    boost::shared_ptr<ngx_radix_node_t>  right;
    boost::shared_ptr<ngx_radix_node_t>  left;
    boost::shared_ptr<ngx_radix_node_t>  parent;
    int               value;
};

class RadixTree
{
private:
    boost::shared_ptr<ngx_radix_node_t>  root;
    boost::shared_ptr<ngx_radix_node_t>  free;
    char              *start;
    size_t             size;

    static boost::shared_ptr<ngx_radix_node_t> nullPtr;
private:
    void intTomask(void);
    void ipToInt(void);
public:
    RadixTree(void);
    ~RadixTree(void);
    bool insertNode(uint32_t key, uint32_t mask, uintptr_t value);
    bool deleteNode(uint32_t key, uint32_t mask);
    int  findNode(uint32_t key);
    /*
    ngx_radix_tree_t *ngx_radix_tree_create(ngx_pool_t *pool, ngx_int_t preallocate);
    ngx_int_t ngx_radix32tree_insert(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask, uintptr_t value);
    ngx_int_t ngx_radix32tree_delete(ngx_radix_tree_t *tree, uint32_t key, uint32_t mask);
    uintptr_t ngx_radix32tree_find(ngx_radix_tree_t *tree, uint32_t key);
    */
};

}
}
}
}

#endif
