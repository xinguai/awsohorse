/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */

#ifndef AWH_PALLOC_H_
#define AWH_PALLOC_H_

#include "base/awh_alloc.h"

namespace base{

typedef struct awh_pool_s              awh_pool_t;
typedef struct awh_pool_large_s        awh_pool_large_t;    

/*
 * AWH_MAX_ALLOC_FROM_POOL should be (awh_pagesize -1),i.e. 4095 on x86
 */
#define AWH_MAX_ALLOC_FROM_POOL  (awh_pagesize - 1)

#define AWH_DEFAULT_POOL_SIZE    (16 * 1024)

#define AWH_POOL_ALIGNMENT       16

#define AWH_MIN_POOL_SIZE                   \
	awh_align((sizeof(awh_pool_t) + 2 * sizeof(awh_pool_large_t)), \
				AWH_POOL_ALIGNMENT)

typedef void (*awh_pool_cleanup_pt)(void *data);

typedef struct awh_pool_cleanup_s awh_pool_cleanup_t;

struct awh_pool_cleanup_s{
	awh_pool_cleanup_pt       handler;
	void                     *data;
	awh_pool_cleanup_t      *next;
};

typedef struct awh_pool_large_s      awh_pool_large_t;

struct awh_pool_large_s{
	awh_pool_large_t    *next;
	void                *alloc;
};

typedef struct{
	u_char          *last;
	u_char          *end;
	awh_pool_t      *next;
	awh_uint_t       failed;
}awh_pool_data_t;

struct awh_pool_s{
	awh_pool_data_t      d;
	size_t               max;
	awh_pool_t          *current;
	awh_chain_t         *chain;
	awh_pool_large_t    *large;
	awh_pool_cleanup_t  *cleanup;
};

typedef struct{
	int                  fd;
	u_char              *name;
}awh_pool_cleanup_file_t;

void* awh_alloc(size_t size);

void* awh_calloc(size_t size);

awh_pool_t* awh_create_pool(size_t size);

void awh_destroy_pool(awh_pool_t *pool);

void awh_reset_pool(awh_pool_t *pool);

void* awh_palloc(awh_pool_t *pool, size_t size);

void* awh_pnalloc(awh_pool_t *pool, size_t size);

void* awh_pcalloc(awh_pool_t *pool, size_t size, size_t alignment);

awh_int_t awh_pfree(awh_pool_t *pool, void *p);

awh_pool_cleanup_t* awh_pool_cleanup_add(awh_pool_t *p, size_t size);

void awh_pool_run_cleanup_file(awh_pool_t *p, int fd);

void awh_pool_cleanup_file(void *data);

void awh_pool_delete_file(void *data);

} //namespace awh

#endif /* AWH_PALLOC_H_ */
