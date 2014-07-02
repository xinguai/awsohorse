/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */

#include <errno.h>

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
	u_char      *m;
	size_t       psize;
	awh_pool_t  *p, *newp,*currentp;

	psize = (size_t) (pool->d.end - (u_char*)pool);

	m = awh_memalign(AWH_POOL_ALIGNMENT, psize);
	if (m == NULL){
		return NULL;
	}

	newp = (awh_pool_t *)m;
	newp->d.end = m + psize;
	newp->d.next = NULL;
	newp->d.failed = 0;

	m += sizeof(awh_pool_data_t);
	m = awh_align_ptr(m, AWH_ALIGNMENT);
	newp->d.last = m + size;

	currentp = pool->current;

	for (p = currentp; p->d.next; p = p->d.next){
		if (p->d.failed++ > 4){
			currentp = p->d.next;
		}
	}
	
	p->d.next = newp;
	pool->current = currentp? currentp:newp;

	return m;
}

static void* awh_palloc_large(awh_pool_t *pool, size_t size){
	void               *p;
	awh_uint_t          n;
	awh_pool_large_t   *large;

	p = awh_alloc(size);
	if (p == NULL){
		return NULL;
	}

	n = 0;

	for (large = pool->large; large; large = large->next){
		if (large->alloc == NULL){
			large->alloc = p;
			return p;
		}

		if (n++ > 3){
			break;
		}
	}

	large = awh_palloc(pool, sizeof(awh_pool_large_t));
	if (large == NULL){
		awh_free(p);
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

void* awh_pmemalign(awh_pool_t *pool, size_t size, size_t alignment){
	void              *p;
	awh_pool_large_t  *large;

	p = awh_memalign(alignment, size);
	if (p == NULL){
		return NULL;
	}

	large = awh_palloc(pool, sizeof(awh_pool_large_t));
	if (large == NULL){
		awh_free(p);
		return NULL;
	}

	large->alloc = p;
	large->next = pool->large;
	pool->large = large;

	return p;
}

awh_int_t awh_pfree(awh_pool_t *pool, void *p){
	awh_pool_large_t   *l;

	for (l = pool->large; l; l = l->next){
		if (p == l->alloc){
			awh_free(l->alloc);
			l->alloc = NULL;

			return AWH_OK;
		}
	}

	return AWH_DECLIEND;
}

void* awh_pcalloc(awh_pool_t *pool, size_t size){
	void *p;

	p = awh_palloc(pool, size);
	if (p){
		memset(p, 0, size);
	}

	return p;
}

awh_pool_cleanup_t* awh_pool_cleanup_add(awh_pool_t * p, size_t size){
	awh_pool_cleanup_t *c;
	 c = awh_palloc(p, sizeof(awh_pool_cleanup_t));
	 if (c == NULL){
		return NULL;
	 }

	 if (size){
		c->data = awh_palloc(p, size);
		if (c->data == NULL){
			return NULL;
		}
	 }else{
		c->data = NULL;
	 }

	 c->handler = NULL;
	 c->next = p->cleanup;
	 p->cleanup = c;

	 return c;
}

void awh_pool_run_cleanup_file(awh_pool_t * p, int fd){
	awh_pool_cleanup_t          *c;
	awh_pool_cleanup_file_t     *cf;

	for (c = p->cleanup; c; c = c->next){
		if (c->handler == awh_pool_cleanup_file){
			cf = c->data;

			if (cf->fd == fd){
				c->handler(cf);
				c->handler = NULL;
				return;
			}
		}
	}
}

void awh_pool_cleanup_file(void * data){
	awh_pool_cleanup_file_t   *c = data;

	if (close(c->fd) == -1){
		//TO DO log
	}
}

void awh_pool_delete_file(void * data){
	awh_pool_cleanup_file_t   *c = data;

	int err;
	if (unlink(c->name) == -1){
		err = errno;

		if (err != ENOENT){
			//TO DO log;
		}
	}

	if (close(c->fd) == -1){
		//TO DO log;
	}
}

}//namespace base

