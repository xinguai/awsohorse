/*
 *  Copyright (c) 2014 The awesohorse Authors. All rights reserved.
 *  Use of this source code is governed by a BSD-style license 
 *  that can be found in the LICENSE file.
 */


#ifndef AWH_CONFIG_H_
#define AWH_CONFIG_H_

#include "base/basic_types.h"   

typedef intptr_t     awh_int_t;
typedef uintptr_t    awh_uint_t;
typedef intptr_t     awh_flag_t;

#define AWH_SIGNAL_HELPER(n)      SIG##n
#define AWH_SIGNAL_VALUE(n)       AWH_SIGNAL_HELPER(n)

#define AWH_SHUTDOWN_SIGNAL       QUIT
#define AWH_TERMINATE_SIGNAL      TERM
#define AWH_NOACCEPT_SIGNAL       WINCH
#define AWH_RECONFIGURE_SIGNAL    HUP

#define AWH_REOPEN_SIGNAL         INFO
#define AWH_CHANGEBIN_SIGNAL      XCPU

#define AWH_OK                    0
#define AWH_ERROR                 -1
#define AWH_AGAIN                 -2
#define AWH_BUSY                  -3
#define AWH_DONE                  -4
#define AWH_DECLIEND              -5
#define AWH_ABORT                 -6

#ifndef  AWH_ALIGNMENT
#define  AWH_ALIGNMENT sizeof(unsigned long) /* platform word */
#endif

#define awh_align(d, a)      (((d) + (a - 1)) & ~(a - 1))
#define awh_align_ptr(p, a)    \
	(u_char*) (((uintptr_t) (p) + ((uintptr_t)a - 1)) & ~((uintptr_t) a -1))

#endif /* AWH_CONFIG_H_ */
