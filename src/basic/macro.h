/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "../fundamental/macro.h" /* IWYU pragma: export */

#যদি !defined(HAS_FEATURE_MEMORY_SANITIZER)
#  যদি defined(__has_feature)
#    যদি __has_feature(memory_sanitizer)
#      define HAS_FEATURE_MEMORY_SANITIZER 1
#    endif
#  endif
#  যদি !defined(HAS_FEATURE_MEMORY_SANITIZER)
#    define HAS_FEATURE_MEMORY_SANITIZER 0
#  endif
#endif

#যদি !defined(HAS_FEATURE_ADDRESS_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_ADDRESS_SANITIZER 1
#  elif defined(__has_feature)
#    যদি __has_feature(address_sanitizer)
#      define HAS_FEATURE_ADDRESS_SANITIZER 1
#    endif
#  endif
#  যদি !defined(HAS_FEATURE_ADDRESS_SANITIZER)
#    define HAS_FEATURE_ADDRESS_SANITIZER 0
#  endif
#endif

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#যদি HAS_FEATURE_ADDRESS_SANITIZER && defined(__clang__)
#define _variable_no_sanitize_address_ __attribute__((__no_sanitize_address__))
#অন্যথা
#define _variable_no_sanitize_address_
#endif

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#যদি defined(__clang__)
#define _function_no_sanitize_float_cast_overflow_ __attribute__((no_sanitize("float-cast-overflow")))
#অন্যথা
#define _function_no_sanitize_float_cast_overflow_
#endif

/* test harness */
#define EXIT_TEST_SKIP 77

স্থির অন্তঃস্থ uint64_t u64_multiply_safe(uint64_t a, uint64_t b) {
        যদি (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                ঘূৰাই 0; /* overflow */

        ঘূৰাই a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
স্থির অন্তঃস্থ unsigned long ALIGN_POWER2(unsigned long u) {

        /* Avoid subtraction overflow */
        যদি (u == 0)
                ঘূৰাই 0;

        /* clz(0) is undefined */
        যদি (u == 1)
                ঘূৰাই 1;

        /* left-shift overflow is undefined */
        যদি (__builtin_clzl(u - 1UL) < 1)
                ঘূৰাই 0;

        ঘূৰাই 1UL << (পরিমাণ(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#define container_of(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#define __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ধ্রুৱ typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (ৱর্ণ *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#define PTR_TO_INT(p) ((পূর্ণাঙ্ক) ((intptr_t) (p)))
#define INT_TO_PTR(u) ((শূন্য *) ((intptr_t) (u)))
#define PTR_TO_UINT(p) ((unsigned) ((uintptr_t) (p)))
#define UINT_TO_PTR(u) ((শূন্য *) ((uintptr_t) (u)))

#define PTR_TO_LONG(p) ((long) ((intptr_t) (p)))
#define LONG_TO_PTR(u) ((শূন্য *) ((intptr_t) (u)))
#define PTR_TO_ULONG(p) ((unsigned long) ((uintptr_t) (p)))
#define ULONG_TO_PTR(u) ((শূন্য *) ((uintptr_t) (u)))

#define PTR_TO_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#define UINT8_TO_PTR(u) ((শূন্য *) ((uintptr_t) (u)))

#define PTR_TO_INT32(p) ((int32_t) ((intptr_t) (p)))
#define INT32_TO_PTR(u) ((শূন্য *) ((intptr_t) (u)))
#define PTR_TO_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#define UINT32_TO_PTR(u) ((শূন্য *) ((uintptr_t) (u)))

#define PTR_TO_INT64(p) ((int64_t) ((intptr_t) (p)))
#define INT64_TO_PTR(u) ((শূন্য *) ((intptr_t) (u)))
#define PTR_TO_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#define UINT64_TO_PTR(u) ((শূন্য *) ((uintptr_t) (u)))

#define CHAR_TO_STR(x) ((ৱর্ণ[2]) { x, 0 })

#define char_array_0(x) x[পরিমাণ(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#define HEXADECIMAL_STR_MAX(type) (2 + পরিমাণ(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#define DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (পরিমাণ(type) <= 1 ? 3U :                                   \
             পরিমাণ(type) <= 2 ? 5U :                                   \
             পরিমাণ(type) <= 4 ? 10U :                                  \
             পরিমাণ(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : পরিমাণ(পূর্ণাঙ্ক[-2*(পরিমাণ(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#define DECIMAL_STR_WIDTH(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                যদি (_x_ == 0)                                     \
                        ans = 1;                                  \
                অন্যথা {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        যেতিয়ালৈ ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#define SWAP_TWO(x, y) কৰা {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } যেতিয়ালৈ (false)

#define STRV_MAKE(...) ((ৱর্ণ**) ((ধ্রুৱ ৱর্ণ*[]) { __VA_ARGS__, রিক্ত }))
#define STRV_MAKE_CONST(...) ((ধ্রুৱ ৱর্ণ* ধ্রুৱ*) ((ধ্রুৱ ৱর্ণ*[]) { __VA_ARGS__, রিক্ত }))

/* Pointers range from NULL to POINTER_MAX */
#define POINTER_MAX ((শূন্য*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#define READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                memcpy(&_copy, &(x), পরিমাণ(_copy));                    \
                asm চঞ্চল ("" : : : "memory");                       \
                _copy;                                                  \
        })

#define saturate_add(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

স্থির অন্তঃস্থ size_t size_add(size_t x, size_t y) {
        ঘূৰাই saturate_add(x, y, SIZE_MAX);
}

/* A little helper for subtracting 1 off a pointer in a safe UB-free way. This is intended to be used for
 * loops that count down from a high pointer until some base. A naive loop would implement this like this:
 *
 * for (p = end-1; p >= base; p--) …
 *
 * But this is not safe because p before the base is UB in C. With this macro the loop becomes this instead:
 *
 * for (p = PTR_SUB1(end, base); p; p = PTR_SUB1(p, base)) …
 *
 * And is free from UB! */
#define PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : রিক্ত;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#define FOREACH_ARGUMENT(entry, ...)                                     \
        _FOREACH_ARGUMENT(entry, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#define _FOREACH_ARGUMENT(entry, _entries_, _current_, _va_sentinel_, ...)      \
        বাবে (typeof(entry) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((long)(_current_ - _entries_) < (long)(ELEMENTSOF(_entries_) - 1)) && ({ entry = *_current_; true; }); \
             _current_++)
