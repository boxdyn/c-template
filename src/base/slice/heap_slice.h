//! @file heap_slice.h
//!
//! Owned, malloc-heap allocated, growable "slices" of memory

#pragma once

#include <stdlib.h>

#include "../base.h"
#include "../slice.h"

#define HeapSlice(T) \
    /** \
     * @struct HeapSlice(T)                                                         \
     * An owning Slice type backed by a heap-allocated array with dynamic capacity. \
     *                                                                              \
     * Zero-initializable, but must be cleaned up.                                  \
     */ \
    struct HeapSlice_##T { \
        T ptr buf; \
        usize len, cap; \
    }

#define hs_reserve(hs, new_len) \
    /** \
     * @fn hs_reserve                                                      \
     *                                                                     \
     * Allocates enough storage for *at least* `new_len` *total* elements. \
     *                                                                     \
     * Does not shrink the backing allocation.                             \
     *                                                                     \
     * This function may invalidate pointers into the HeapSlice's `buf`.   \
     */ \
    HeapSlice_get_bigger((HeapSlice(void) mut)hs, sizeof((hs)->buf[0]), new_len)

/// Makes a HeapSlice get bigger
void HeapSlice_get_bigger(HeapSlice(void) mut hs, usize element_size, usize new_len) {
    while (new_len > (hs)->cap) {
        let new_cap = base_max(base_max((new_len), 8U), (hs)->cap + ((hs)->cap >> 1));
        (hs)->buf = realloc((hs)->buf, element_size * new_cap);
        (hs)->cap = (hs)->buf ? new_cap : 0;
    }
}

#define hs_free(self) \
    /** \
     * @fn hs_free                                        \
     *                                                    \
     * Frees the allocated `buf` of the HeapSlice `self`, \
     * and reinitializes it to zero.                      \
     */ \
    (free((self)->buf), (self)->buf = nullptr, (self)->len = 0, (self)->cap = 0)

#define hs_push(hs, elem) \
    /** \
     * @fn hs_push                                                       \
     *                                                                   \
     * Appends a new element to the slice, resizing the buffer           \
     * if necessary.                                                     \
     *                                                                   \
     * # SAFETY:                                                         \
     * This function may invalidate pointers into the HeapSlice's `buf`. \
     */ \
    ({ \
        hs_reserve(hs, (hs)->len + 1); \
        (hs)->buf[(hs)->len++] = elem; \
    })

#define hs_pop(hs, default) \
    /** \
     * @fn hs_pop                                               \
     *                                                          \
     * Pops the last element off the HeapSlice, and returns it. \
     * If there is no last element, returns `default`.          \
     */ \
    slice_pop_last(hs, default)
