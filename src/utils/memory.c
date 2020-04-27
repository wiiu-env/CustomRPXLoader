/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <coreinit/memexpheap.h>
#include <coreinit/memdefaultheap.h>
#include <coreinit/memorymap.h>
#include <malloc.h>
#include <string.h>
#include <errno.h>

extern uint32_t * pMEMAllocFromDefaultHeapEx;
extern uint32_t * pMEMAllocFromDefaultHeap;
extern uint32_t * pMEMFreeToDefaultHeap;

//!-------------------------------------------------------------------------------------------
//! reent versions
//!-------------------------------------------------------------------------------------------
void *_malloc_r(struct _reent *r, size_t size) {
    void *ptr = ((void * (*)(size_t))(*pMEMAllocFromDefaultHeap))(size);
    if (!ptr) {
        r->_errno = ENOMEM;
    }
    return ptr;
}

void *_calloc_r(struct _reent *r, size_t num, size_t size) {
    void *ptr = ((void * (*)(size_t))(*pMEMAllocFromDefaultHeap))(size);
    if (ptr) {
        memset(ptr, 0, num * size);
    } else {
        r->_errno = ENOMEM;
    }

    return ptr;
}

void *_memalign_r(struct _reent *r, size_t align, size_t size) {
    return ((void * (*)(size_t, size_t))(*pMEMAllocFromDefaultHeapEx))(size, align);
}

void _free_r(struct _reent *r, void *ptr) {
    if (ptr) {
        ((void (*)(void *))(*pMEMFreeToDefaultHeap))(ptr);
    }
}

void *_realloc_r(struct _reent *r, void *p, size_t size) {
    void *new_ptr = ((void * (*)(size_t))(*pMEMAllocFromDefaultHeap))(size);
    if (!new_ptr) {
        r->_errno = ENOMEM;
        return new_ptr;
    }

    if (p) {
        size_t old_size = MEMGetSizeForMBlockExpHeap(p);
        memcpy(new_ptr, p, old_size <= size ? old_size : size);
        ((void (*)(void *))(*pMEMFreeToDefaultHeap))(p);
    }
    return new_ptr;
}

struct mallinfo _mallinfo_r(struct _reent *r) {
    struct mallinfo info = { 0 };
    return info;
}

void
_malloc_stats_r(struct _reent *r) {
}

int
_mallopt_r(struct _reent *r, int param, int value) {
    return 0;
}

size_t
_malloc_usable_size_r(struct _reent *r, void *ptr) {
    return MEMGetSizeForMBlockExpHeap(ptr);
}

void *
_valloc_r(struct _reent *r, size_t size) {
    return ((void * (*)(size_t, size_t))(*pMEMAllocFromDefaultHeapEx))(size, OS_PAGE_SIZE);
}

void *
_pvalloc_r(struct _reent *r, size_t size) {
    return ((void * (*)(size_t, size_t))(*pMEMAllocFromDefaultHeapEx))((size + (OS_PAGE_SIZE - 1)) & ~(OS_PAGE_SIZE - 1), OS_PAGE_SIZE);
}

int
_malloc_trim_r(struct _reent *r, size_t pad) {
    return 0;
}
