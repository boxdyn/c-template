//! @file inline_slice.h
//!
//! Owned, stack-allocated, fillable "slices" of memory

#pragma once

#include "../slice.h"

#define InlineSlice(T, cap) \
    /** \
     * @struct InlineSlice(T, cap)                                             \
     *                                                                         \
     * An owning Slice type backed by an inline array with the given capacity. \
     */ \
    struct InlineSlice_##T##$##cap { \
        usize len; \
        T buf[cap]; \
    }
