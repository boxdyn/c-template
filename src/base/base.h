//! @file base.h
//!
//! The Base Library

#pragma once

#include <stddefer.h> // IWYU pragma: export

// Include signed and unsigned integer types
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

// Integer types

/// The unsigned 8-bit integer type
uint8_t typedef u8;
/// The unsigned 16-bit integer type
uint16_t typedef u16;
/// The unsigned 32-bit integer type
uint32_t typedef u32;
/// The unsigned 64-bit integer type
uint64_t typedef u64;
/// The unsigned 128-bit integer type
unsigned __int128 typedef u128;
/// The unsigned pointer-width integer type
size_t typedef usize;
static_assert(sizeof((void *)0) == sizeof(usize));

/// The signed 8-bit integer type
int8_t typedef i8;
/// The signed 16-bit integer type
int16_t typedef i16;
/// The signed 32-bit integer type
int32_t typedef i32;
/// The signed 64-bit integer type
int64_t typedef i64;
/// The signed 128-bit integer type
__int128 typedef i128;
/// The signed pointer-width integer type
ssize_t typedef isize;
static_assert(sizeof((void *)0) == sizeof(isize));

/// The 32-bit floating-point type
float typedef f32;
static_assert(sizeof(float) == sizeof(u32));
/// The 64-bit floating-point type
double typedef f64;
static_assert(sizeof(double) == sizeof(u64));

/// Defines a type-inferred variable
#define let auto
#ifdef __clang__
    #define nonnull _Nonnull
    #define nullable _Nullable
#else
    #define nonnull
    #define nullable
#endif

#define ref const *nonnull
#define mut *nonnull
#define const_ptr const *nullable
#define ptr *nullable

#define base_min(lhs, rhs) \
    ({ \
        auto a = (lhs); \
        auto b = (rhs); \
        a < b ? a : b; \
    })

#define base_max(lhs, rhs) \
    ({ \
        auto a = (lhs); \
        auto b = (rhs); \
        a < b ? b : a; \
    })

#define base_clamp(low, value, high) \
    ({ \
        auto l = (low); \
        auto v = (value); \
        auto h = (high); \
        base_max(l, base_min(v, h)); \
    })
