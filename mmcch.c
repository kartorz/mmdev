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

cache_list(size_t size, kmem_cache *cache);

struct kmem_cache *get_cache(char *name, size_t size)
{
        if (cache_list[size] == NULL) {        
                char name[32];
                sprintf(name, "wine-object-%d", size);
                struct kmem_cache *new =  kmem_cache_create(name, xxxx);
                cache_list[size] = new;
        } else {
                return cache_list[size];
        }
}

void* alloc_cache(size_t size)
{
        struct kmem_cache * = get_cache(size);
        return kmem_cache_alloc(kmem_cache, gfp_mask);
}
