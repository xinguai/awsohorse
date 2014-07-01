/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */


#ifndef AWH_SHMEM_H_
#define AWH_SHMEM_H_

#include "base/basic_types.h"

namespace base{

typedef struct {
	u_char         *addr;
	size_t         size;
	std::string    name;
	awh_uint_t     exists;
}AwhShmt;


awh_uint_t awh_shm_alloc(AwhShmt *shm);

void awh_shm_free(AwhShmt *shm);


} //namespace base

#endif /* AWH_SHMEMH_ */

