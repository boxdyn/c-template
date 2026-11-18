//! @file str.h
//!
//! Non-owning string-views and UTF-8 decoding.

#pragma once

#include "base.h"
#include "slice.h"
#include "slice/inline_slice.h"

/// @struct str
///
/// A slice of bytes, which gains special properties if it's utf-8 encoded
Slice(u8) typedef str;

#define STR(lit) ((str) {.buf = (u8 *)lit, .len = sizeof(lit) - 1})
#define STRFMT "%.*s"
#define STRARG(sv) ((int)(sv).len), ((char *)(sv).buf)

enum utf8_pop_Error : i32 {
    /// Reached the end of the string (no more characters)
    pop_utf8_END = -1,
    /// Expected an introducer, found an invalid byte
    pop_utf8_OUT_OF_RANGE = -2,
    /// UTF-8 sequence is overlong encoding
    pop_utf8_BAD_CONTINUE = -3,
    /// Expected an introducer, found a continuation byte
    pop_utf8_MISSING_HEAD = -4,
    /// Expected a continuation byte, found an introducer or overlong encoding
    pop_utf8_MISSING_TAIL = -5,
};

/// Removes the last codepoint from `self`, and returns it.
///
/// Returns:
/// - The next unicode codepoint, if one exists
/// - one of [utf8_pop_Error] if one does not
i32 utf8_pop_last(str mut self);

/// Removes the first codepoint from `self`, and returns it.
///
/// # SAFETY: This modifies the `buf` pointer of `self`.
/// For owning slices, whose `buf` must be passed back to an allocator
/// to be freed, this may cause memory corruption.
///
/// Returns:
/// - The next unicode codepoint, if one exists
/// - one of [utf8_pop_Error] if one does not
i32 utf8_pop_first(str mut self);

/// Checks whether the bytes at `self` are valid UTF-8
bool str_is_utf8(str self);

/// Encodes `ch` into an inline slice of u8
InlineSlice(u8, 4) utf8_encode(u32 code);
