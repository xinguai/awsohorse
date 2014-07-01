/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */

#include "base/awh_alloc.h"

namespace base{

awh_uint_t awh_pagesize;
awh_uint_t awh_pagesize_shift;
awh_uint_t awh_cacheline_size;

void* awh_alloc(size_t size){
	void* p;
	p = malloc(size);
	if (p == NULL){
		//TO DO log
	}

	return p;
}

void* awh_calloc(size_t size){
	void* p;
	p = awh_alloc(size);
	if (p){
		memset(p, 0, size);
	}

	return p;
}

void* awh_memalign(size_t alignment, size_t size){
	void* p;
	int err;

	err = posix_memalign(&p, alignment, size);
	if (err){
		//TO DO log
		p = NULL;
	}

	return p;
}

} //namespace base

