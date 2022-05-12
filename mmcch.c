/*
 * The wine memory mananger.
 *
 * 
 *
 * ================================================
 * Copyright (C) 2021 NFSChina. All rights reserved.
 * 
 * Author:
 *     Li Li Qiong <liqiong@nfschina.com>
 *     Tu Ren Yu@nfschina.com <renyu@nfschina.com>
 *
 */

#include <linux/slab.h>
#include <linux/hashtable.h> // hashtable API
#include <linux/module.h> 
#include <linux/types.h> 

DECLARE_HASHTABLE(tbl, 3);

 struct kmem_cache* hash_cache;

struct h_node {
    struct kmem_cache* cache;
    int size;
    struct hlist_node node;
};

struct kmem_cache* cache_table(size_t size)
{
        struct h_node *cur;
        hash_for_each_possible(tbl, cur, node, size) {
                size, cur->data);

                if (cur->size == size)) {
                        return cur->cache;
                }
        }
        return NULL;
}

struct kmem_cache *get_cache(char *name, size_t size)
{
        struct kmem_cache * cache = cache_table(size);
        if (cache == NULL) {        
                char name[32];
                h_node* hash_node = kmem_cache_alloc(hash_cache, GFP_KERNEL);
                sprintf(name, "wine-object-%d", size);
                hash_node->cache = kmem_cache_create(name,size, 0, SLAB_HWCACHE_ALIGN,NULL);
                hash_node->size = size;

                hash_add(tbl,&hash_node->node,size);
                return hash_node->cache;
        } else {
                return cache;
        }
}

void* alloc_cache(size_t size)
{
        struct kmem_cache* cache = get_cache(size);
        return kmem_cache_alloc(cache, GFP_KERNEL);
}

void* cache_free(struct kmem_cache* cache,void* object)
{
        kmem_cache_free(cache,object);
}

void init_cache_for_hash(void)
{
  char name[32];
  sprintf(name, "wine-cache-for-hash-%d",sizeof(h_node) );
  struct kmem_cache *hash_cache =  kmem_cache_create(name, sizeof(struct h_node),0, SLAB_HWCACHE_ALIGN,NULL);
}

void exit(void)
{    
        struct h_node *cur;
        unsigned bkt;

        hash_for_each(tbl, bkt, cur, node) 
        {
                pr_info("myhashtable: element: data = %d",
                cur->size, );
                kmem_cache_free(hash_cache,cur);
        }
        kmem_cache_destroy(hash_cache);

}