/**
 * Copyright (c) Riven Zheng (zhengheiot@gmail.com).
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 **/
#include "mem_malloc.h"

#define HEAP_SIZE        (MEM_MALLOC_HEAP_SIZE_CONFIG)
#define FREE_HEAD_SIZE   (sizeof(struct free_head))
#define MALLOC_HEAD_SIZE (sizeof(struct malloc_head))

struct free_head {
    struct free_head *pNext;
    unsigned int size;
};

struct malloc_head {
    unsigned int size;
};

static struct free_head g_free_list_head = {.pNext = NULL, .size = 0u};
static unsigned int g_heap_mem[HEAP_SIZE / sizeof(unsigned int)] = {0};
static unsigned int g_heap_idx = 0;

static void *_pack_address(void *p_addr, unsigned int sz)
{
    struct malloc_head *p_malloc_hd = (struct malloc_head *)p_addr;

    memset((unsigned char *)p_addr, 0x0, sz);
    p_malloc_hd->size = sz;
    p_addr = (void *)((unsigned int)p_addr + MALLOC_HEAD_SIZE);

    return p_addr;
}

static void *_unpack_address(void *p_addr)
{
    p_addr = (void *)((unsigned int)p_addr - MALLOC_HEAD_SIZE);

    return p_addr;
}

void *mem_malloc(unsigned int size)
{
    void *p_addr = NULL;
    unsigned char align = sizeof(unsigned int);

    if (!size) {
        return p_addr;
    }

    if (size < FREE_HEAD_SIZE) {
        size = FREE_HEAD_SIZE;
    }
    size += MALLOC_HEAD_SIZE;
    size += (align - 1);
    size &= ~(align - 1);

    struct free_head *p_prev_hd = NULL;
    struct free_head *p_cur_hd = g_free_list_head.pNext;
    while (p_cur_hd && size > p_cur_hd->size) {
        p_prev_hd = p_cur_hd;
        p_cur_hd = p_cur_hd->pNext;
    }

    if (p_cur_hd) {
        unsigned int remain = p_cur_hd->size - size;
        if (remain >= FREE_HEAD_SIZE + align) {
            // Split a new one.
            struct free_head *p_new_head = (struct free_head *)((unsigned int)p_cur_hd + size);

            if (!p_prev_hd) {
                g_free_list_head.pNext = p_new_head;
            } else {
                p_prev_hd->pNext = p_new_head;
            }
            p_new_head->pNext = p_cur_hd->pNext;
            p_new_head->size = remain;

            p_addr = (void *)p_cur_hd;
        } else {
            // Delete it.
            if (!p_prev_hd) {
                g_free_list_head.pNext = p_cur_hd->pNext;
            } else {
                p_prev_hd->pNext = p_cur_hd->pNext;
            }

            p_addr = (void *)p_cur_hd;
            size = p_cur_hd->size;
        }
        return _pack_address(p_addr, size);
    }

    if ((g_heap_idx * align) + size > HEAP_SIZE) {
        return p_addr;
    }

    p_addr = (void *)&g_heap_mem[g_heap_idx];
    g_heap_idx += size / align;

    return _pack_address(p_addr, size);
}

void mem_free(void *p_addr)
{
    if (!p_addr) {
        return;
    }

    if ((unsigned int)p_addr < (unsigned int)(void *)&g_heap_mem[0]) {
        return;
    }

    if ((unsigned int)p_addr >= (unsigned int)(void *)&g_heap_mem[sizeof(g_heap_mem) / sizeof(unsigned int) - 1]) {
        return;
    }

    struct malloc_head *p_malloc_hd = (struct malloc_head *)_unpack_address(p_addr);
    struct free_head *p_new_head = (struct free_head *)p_malloc_hd;

    struct free_head *p_cur_hd = g_free_list_head.pNext;
    while (p_cur_hd) {
        if (p_cur_hd == p_new_head) {
            /* Duplicated free operation */
            return;
        }
        p_cur_hd = p_cur_hd->pNext;
    }

    unsigned int sz = p_malloc_hd->size;
    memset((unsigned char *)p_malloc_hd, 0x0, sz);

    struct free_head *p_prev_hd = NULL;
    p_cur_hd = g_free_list_head.pNext;
    while (p_cur_hd && p_cur_hd < p_new_head) {
        p_prev_hd = p_cur_hd;
        p_cur_hd = p_cur_hd->pNext;
    }

    if (!p_prev_hd) {
        g_free_list_head.pNext = p_new_head;

        if (((unsigned int)p_new_head + sz) != (unsigned int)p_cur_hd) {
            // Split a new one.
            p_new_head->size = sz;

            p_new_head->pNext = p_cur_hd;
            return;
        }

        // Merge the next one.
        p_new_head->size = p_cur_hd->size + sz;
        p_new_head->pNext = p_cur_hd->pNext;
        memset((unsigned char *)p_cur_hd, 0x0, p_cur_hd->size);
        return;
    }

    if ((unsigned int)p_prev_hd + p_prev_hd->size == (unsigned int)p_new_head) {
        // Merge the previous one.
        p_prev_hd->size += sz;
    } else if ((unsigned int)p_new_head + sz == (unsigned int)p_cur_hd) {
        // Merge the next one.
        p_new_head->size = p_cur_hd->size + sz;
        p_new_head->pNext = p_cur_hd->pNext;
        memset((unsigned char *)p_cur_hd, 0x0, p_cur_hd->size);
    } else {
        // Split a new one.
        p_new_head->size = sz;

        p_prev_hd->pNext = p_new_head;
        p_new_head->pNext = p_cur_hd;
    }
}

unsigned int mem_free_size(void)
{
    unsigned int size = 0u;

    struct free_head *p_cur_hd = g_free_list_head.pNext;
    while (p_cur_hd) {
        size += p_cur_hd->size;
        p_cur_hd = p_cur_hd->pNext;
    }

    return size;
}

bool mem_allocated(void *p_addr)
{
    if ((unsigned int)p_addr < (unsigned int)(void *)&g_heap_mem[0]) {
        return false;
    }

    if ((unsigned int)p_addr > (unsigned int)(void *)&g_heap_mem[sizeof(g_heap_mem) / sizeof(unsigned int) - 1]) {
        return false;
    }

    return true;
}
