/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */


#include <sys/mman.h>

#include "base/awh_shmem.h"
#include "base/awh_config.h"

namespace base{
awh_uint_t awh_shm_alloc(AwhShmt *shm){
	shm->addr = (u_char *)mmap(NULL, shm->size),
							   PROT_READ|PROT_WRITE,
							   MAP_ANON|MAP_SHARED, -1, 0);
	if (shm->addr == MAP_FAILED){
		//TO DO,log
		return AWH_ERROR;
	}

	return AWH_OK;
}

void awh_shm_free(AwhShmt *shm){
	if (munmap((void*)shm->addr, shm->size) == -1){
		//TO DO , log
	}
}
} //namespace base

