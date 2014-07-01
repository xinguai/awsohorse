/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */

#ifndef AWH_ALLOC_H_
#define AWH_ALLOC_H_

#include <stdlib.h>

#include "base/awh_config.h"

namespace base{

void* awh_alloc(size_t size);
void* awh_calloc(size_t size);

#define awh_free free

void* awh_memalign(size_t alignment, size_t size);

extern awh_uint_t awh_pagesize;
extern awh_uint_t awh_pagesize_shift;
extern awh_uint_t awh_cacheline_size;

} //namespace base

#endif /* AWH_ALLOC_H_ */

