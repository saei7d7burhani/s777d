/**
 * =============================================================================
 * KHR Platform Header
 * =============================================================================
 * This header provides platform-specific type definitions used by OpenGL.
 * It ensures consistent type sizes across different platforms (Windows, Linux, Mac).
 * =============================================================================
 */

#ifndef __khrplatform_h_
#define __khrplatform_h_

// Include standard integer types
#include <stdint.h>

// =============================================================================
// Type Definitions
// =============================================================================
// These types ensure OpenGL has consistent integer sizes across platforms.
// The khronos_ prefix is used to avoid conflicts with other libraries.

typedef int8_t   khronos_int8_t;
typedef uint8_t  khronos_uint8_t;
typedef int16_t  khronos_int16_t;
typedef uint16_t khronos_uint16_t;
typedef int32_t  khronos_int32_t;
typedef uint32_t khronos_uint32_t;
typedef int64_t  khronos_int64_t;
typedef uint64_t khronos_uint64_t;

// Floating point type (always 32-bit float)
typedef float    khronos_float_t;

// Platform-specific pointer types
// These vary in size between 32-bit and 64-bit systems
#if defined(_WIN64) || defined(__LP64__) || defined(__x86_64__)
    typedef int64_t  khronos_intptr_t;
    typedef int64_t  khronos_ssize_t;
    typedef uint64_t khronos_uintptr_t;
#else
    typedef int32_t  khronos_intptr_t;
    typedef int32_t  khronos_ssize_t;
    typedef uint32_t khronos_uintptr_t;
#endif

#endif // __khrplatform_h_
