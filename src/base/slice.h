//! @file slice.h
//!
//! Non-owning "slices" of contiguous memory

#include "base.h"

#define Slice(T) \
    /** \
     * @struct Slice(T, cap)                                         \
     *                                                               \
     * A simple buffer-length-pair (nominally) non-owning view type. \
     */ \
    struct Slice_##T { \
        T ptr buf; \
        usize len; \
    }

#define slice_of(T, slice) \
    /** \
     * @fn slice_of                                                             \
     *                                                                          \
     * Constructs a non-owning `Slice` with the same `buf` and `len` as `slice` \
     */ \
    (Slice(T)) { \
        .buf = slice.buf, .len = slice.len, \
    }

// args are reversed so it's a compile error
// to use `slice_as` in place of the safer `slice_of`
#define slice_as(slice, T) \
    /** \
     * @fn slice_as                                                         \
     *                                                                      \
     * Casts anything that looks like a Slice to a Slice of the given type. \
     *                                                                      \
     * SAFETY: Users are responsible for ensuring the size and alignment    \
     * requirements for type T are met.                                     \
     */ \
    (Slice(T)) { \
        .buf = (T *)(slice).buf, .len = (slice).len * sizeof(*(slice).buf) / sizeof(T) \
    }

#define foreach(it, slice) \
    /** \
     * @fn foreach                                           \
     *                                                       \
     * Iterates by reference over the elements of a `Slice`. \
     */ \
    for (auto it = (slice).buf; (it) < (slice).buf + (slice).len; ++(it))

#define foreach_index(idx, slice) \
    /** \
     * @fn foreach_index                                 \
     *                                                   \
     * Iterates by index over the elements of a `Slice`. \
     */ \
    for (auto idx = 0; idx < (slice).len; ++idx)

#define slice_pop_last(slice, default) \
    /** \
     * @fn slice_pop_last                                   \
     *                                                      \
     * Pops the last element off the slice, and returns it. \
     * If there is no last element, returns `default`.      \
     */ \
    ((slice).len ? (slice).buf[--(slice).len] : (default))

#define slice_pop_first(slice, default) \
    /** \
     * @fn slice_pop_first                                               \
     *                                                                   \
     * Pops the first element off the slice, and returns it.             \
     * If there is no first element, returns `default`.                  \
     *                                                                   \
     * SAFETY: This modifies the `buf` pointer of `slice`.               \
     * For owning slices whose `buf` must be passed back to an allocator \
     * to be freed, this may cause memory corruption.                    \
     *                                                                   \
     * If you're unsure whether your slice type can be `pop_first`ed,    \
     * please create a non-owning view with `slice_of(T, slice)`.        \
     */ \
    ((slice).len ? (slice).len--, *(slice).buf++ : (default))

#define slice_until(slice, n) \
    /** \
     * @fn slice_until                                                  \
     *                                                                  \
     * Creates a slice of `slice`'s elements up to (but excluding) `n`. \
     */ \
    (typeof(slice)) { \
        .buf = (slice).buf, .len = base_clamp(0, (isize)n, (isize)(slice).len) \
    }

#define slice_after(slice, n) \
    /** \
     * @fn slice_after                                                  \
     *                                                                  \
     * Creates a slice of `slice`'s elements after (and including) `n`. \
     */ \
    ({ \
        auto offset = base_clamp(0, (isize)(n), (isize)(slice).len); \
        (typeof(slice)) {.buf = (slice).buf + offset, .len = (slice).len - offset}; \
    })
