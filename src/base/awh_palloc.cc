/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */

#include "base/awh_palloc.h"

namespace base{

static void* awh_palloc_block(awh_pool_t *pool, size_t size);

static void* awh_palloc_large(awh_pool_t *pool, size_t size);

awh_pool_t* awh_create_pool(size_t size){
	awh_pool_t *p;

	p = awh_memalign(AWH_POOL_ALIGNMENT, size);
	if (p == NULL){
		return NULL;
	}

	p->d.last = (u_char*) p + sizeof(awh_pool_t);
	p->d.end = (u_char*) p + size;
	p->d.next = NULL;
	p->d.failed = 0;

	size = size - sizeof(awh_pool_t);
	p->max = (size < AWH_MAX_ALLOC_FROM_POOL) ? size : AWH_MAX_ALLOC_FROM_POOL;
	p->current = p;
	p->chain = NULL;
	p->large = NULL;
	p->cleanup = NULL;

	return p;
}

void awh_destroy_pool(awh_pool_t * pool){
	awh_pool_t           *p, *n;
	awh_pool_large_t     *l;
	awh_pool_cleanup_t   *c;

	for (c = pool->cleanup; c; c = c->next){
		if (c->handler){
			c->handler(c->data);
		}
	}

	for (l = pool->large; l; l = l->next){
		if (l->alloc){
			awh_free(l->alloc);
		}
	}

	for (p = pool, n = pool->d.next; ; p = n, n = n->d.next){
		awh_free(p);

		if (n == NULL){
			break;
		}
	}
}

void awh_reset_pool(awh_pool_t *pool){
	awh_pool_t          *p;
	awh_pool_large_t    *l;

	for (l = pool->large; l; l = l->next){
		if (l->alloc){
			awh_free(l->alloc);
		}
	}

	for (p = pool; p; p = p->d.next){
		p->d.last = (u_char*) p + sizeof(awh_pool_t);
		p->d.failed = 0;
	}

	pool->current = pool;
	pool->chain = NULL;
	pool->large = NULL;
}

void* awh_palloc(awh_pool_t *pool, size_t size){
	u_char       *m;
	awh_pool_t   *p;

	if (size <= pool->max){
		p = pool->current;
		do {
			m = awh_align_ptr(p->d.last, AWH_ALIGNMENT);

			if ((size_t) (p->d.end - m) >= size){
				p->d.last = m + size;

				return m;
			}

			p = p->d.next;
		}while(p);

		return awh_palloc_block(pool, size);
	}
	return awh_palloc_large(pool, size);
}

void* awh_pnalloc(awh_pool_t *pool, size_t size){
	u_char        *m;
	awh_pool_t    *p;

	if (size <= pool->max){
		p = pool->current;
		do {
			m = p->d.last;
			if ((size_t)(p->d.end - m) >= size){
				p->d.last = m + size;
				return m;
			}

			p = p->d.next;
		}while(p);

		return awh_palloc_block(pool, size);
	}
	return awh_palloc_large(pool, size);
}

static void* awh_palloc_block(awh_pool_t *pool, size_t size){
	
}

}//namespace base

