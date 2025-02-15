/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _K_MALLOC_H_
#define _K_MALLOC_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef K_MALLOC_HEAP_SIZE_CONFIG
#define K_MALLOC_HEAP_SIZE_CONFIG (0x4)
#endif

void* k_malloc(unsigned int size);
void k_free(void* p_addr);
unsigned int k_free_size(void);
#endif
