/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#ifndef _MEM_MALLOC_H_
#define _MEM_MALLOC_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#ifndef MEM_MALLOC_HEAP_SIZE_CONFIG
#define MEM_MALLOC_HEAP_SIZE_CONFIG (0x4)
#endif

void* mem_malloc(unsigned int size);
void mem_free(void* p_addr);
unsigned int mem_free_size(void);
#endif
