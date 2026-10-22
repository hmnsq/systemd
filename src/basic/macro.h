/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "../fundamental/macro.h" /* IWYU pragma: export */

#ಆದರೆ !defined(HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER)
#  ಆದರೆ defined(__has_feature)
#    ಆದರೆ __has_feature(ಸ್ಮರಣೆ_sanitizer)
#      define HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER 1
#    endif
#  endif
#  ಆದರೆ !defined(HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER)
#    define HAS_FEATURE_ಸ್ಮರಣೆ_SANITIZER 0
#  endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAS_FEATURE_ವಿಳಾಸ_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_ವಿಳಾಸ_SANITIZER 1
#  elif defined(__has_feature)
#    ಆದರೆ __has_feature(ವಿಳಾಸ_sanitizer)
#      define HAS_FEATURE_ವಿಳಾಸ_SANITIZER 1
#    endif
#  endif
#  ಆದರೆ !defined(HAS_FEATURE_ವಿಳಾಸ_SANITIZER)
#    define HAS_FEATURE_ವಿಳಾಸ_SANITIZER 0
#  endif
#ಆದರೆ_ಅಂತ್ಯ

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#ಆದರೆ HAS_FEATURE_ವಿಳಾಸ_SANITIZER && defined(__clang__)
#ವ್ಯಾಖ್ಯಾನಿಸು variable_no_sanitize_ವಿಳಾಸ __attribute__((__no_sanitize_address__))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು variable_no_sanitize_ವಿಳಾಸ
#ಆದರೆ_ಅಂತ್ಯ

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#ಆದರೆ defined(__clang__)
#ವ್ಯಾಖ್ಯಾನಿಸು function_no_sanitize_float_cast_ಅತಿಪ್ರವಾಹ __attribute__((no_sanitize("float-cast-overflow")))
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು function_no_sanitize_float_cast_ಅತಿಪ್ರವಾಹ
#ಆದರೆ_ಅಂತ್ಯ

/* test harness */
#ವ್ಯಾಖ್ಯಾನಿಸು EXIT_TEST_SKIP 77

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t u64_multiply_ಸುರಕ್ಷಿತ(uint64_t a, uint64_t b) {
        ಆದರೆ (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                ಹಿಂತಿರುಗಿ 0; /*ಅತಿಪ್ರವಾಹ*/

        ಹಿಂತಿರುಗಿ a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ALIGN_POWER2(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ u) {

        /* Avoid subtraction overflow */
        ಆದರೆ (u == 0)
                ಹಿಂತಿರುಗಿ 0;

        /* clz(0) is undefined */
        ಆದರೆ (u == 1)
                ಹಿಂತಿರುಗಿ 1;

        /* left-shift overflow is undefined */
        ಆದರೆ (__builtin_clzl(u - 1UL) < 1)
                ಹಿಂತಿರುಗಿ 0;

        ಹಿಂತಿರುಗಿ 1UL << (ಗಾತ್ರ(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು container_ದ(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#ವ್ಯಾಖ್ಯಾನಿಸು __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ಸ್ಥಿರಾಂಕ typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (ಅಕ್ಷರ *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_INT(p) ((ಪೂರ್ಣಾಂಕ) ((intptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು INT_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((intptr_t) (u)))
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_UINT(p) ((ಚಿಹ್ನೆರಹಿತ) ((uintptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು UINT_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((uintptr_t) (u)))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_LONG(p) ((ದೀರ್ಘ) ((intptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು LONG_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((intptr_t) (u)))
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_ULONG(p) ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) ((uintptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು ULONG_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((uintptr_t) (u)))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು UINT8_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((uintptr_t) (u)))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_INT32(p) ((int32_t) ((intptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು INT32_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((intptr_t) (u)))
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು UINT32_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((uintptr_t) (u)))

#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_INT64(p) ((int64_t) ((intptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು INT64_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((intptr_t) (u)))
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_ಗಮ್ಯ_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#ವ್ಯಾಖ್ಯಾನಿಸು UINT64_ಗಮ್ಯ_PTR(u) ((ಶೂನ್ಯ *) ((uintptr_t) (u)))

#ವ್ಯಾಖ್ಯಾನಿಸು CHAR_ಗಮ್ಯ_STR(x) ((ಅಕ್ಷರ[2]) { x, 0 })

#ವ್ಯಾಖ್ಯಾನಿಸು char_array_0(x) x[ಗಾತ್ರ(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#ವ್ಯಾಖ್ಯಾನಿಸು HEXADECIMAL_STR_MAX(type) (2 + ಗಾತ್ರ(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#ವ್ಯಾಖ್ಯಾನಿಸು DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (ಗಾತ್ರ(type) <= 1 ? 3U :                                   \
             ಗಾತ್ರ(type) <= 2 ? 5U :                                   \
             ಗಾತ್ರ(type) <= 4 ? 10U :                                  \
             ಗಾತ್ರ(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ[-2*(ಗಾತ್ರ(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#ವ್ಯಾಖ್ಯಾನಿಸು DECIMAL_STR_ಅಗಲ(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                ಆದರೆ (_x_ == 0)                                     \
                        ans = 1;                                  \
                ಇಲ್ಲದಿದ್ದರೆ {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        ಆಗ ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು ಅದಲುಬದಲು_TWO(x, y) ಮಾಡು {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } ಆಗ (false)

#ವ್ಯಾಖ್ಯಾನಿಸು STRV_MAKE(...) ((ಅಕ್ಷರ**) ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[]) { __VA_ARGS__, ಖಾಲಿ }))
#ವ್ಯಾಖ್ಯಾನಿಸು STRV_MAKE_CONST(...) ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ*) ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*[]) { __VA_ARGS__, ಖಾಲಿ }))

/* Pointers range from NULL to POINTER_MAX */
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೂಚಕ_MAX ((ಶೂನ್ಯ*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#ವ್ಯಾಖ್ಯಾನಿಸು READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                ಸ್ಮರಣೆ_ನಕಲು(&_copy, &(x), ಗಾತ್ರ(_copy));                    \
                asm ಅಸ್ಥಿರ ("" : : : "memory");                       \
                _copy;                                                  \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು saturate_ಕೂಡಿಸು(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t ಗಾತ್ರ_ಕೂಡಿಸು(size_t x, size_t y) {
        ಹಿಂತಿರುಗಿ saturate_ಕೂಡಿಸು(x, y, SIZE_MAX);
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
#ವ್ಯಾಖ್ಯಾನಿಸು PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : ಖಾಲಿ;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_ವಾದ(ನಮೂದು, ...)                                     \
        FOREACH_ವಾದ_2(ನಮೂದು, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು FOREACH_ವಾದ_2(ನಮೂದು, _entries_, _current_, _va_sentinel_, ...)      \
        ಗಾಗಿ (typeof(ನಮೂದು) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((ದೀರ್ಘ)(_current_ - _entries_) < (ದೀರ್ಘ)(ELEMENTSOF(_entries_) - 1)) && ({ ನಮೂದು = *_current_; true; }); \
             _current_++)
