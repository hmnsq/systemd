/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/macro.h" /* IWYU pragma: export */

#ਜੇ !defined(HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER)
#  ਜੇ defined(__has_feature)
#    ਜੇ __has_feature(ਸਮ੍ਰਿਤੀ_sanitizer)
#      define HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER 1
#    endif
#  endif
#  ਜੇ !defined(HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER)
#    define HAS_FEATURE_ਸਮ੍ਰਿਤੀ_SANITIZER 0
#  endif
#ਜੇ_ਅੰਤ

#ਜੇ !defined(HAS_FEATURE_ਪਤਾ_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_ਪਤਾ_SANITIZER 1
#  elif defined(__has_feature)
#    ਜੇ __has_feature(ਪਤਾ_sanitizer)
#      define HAS_FEATURE_ਪਤਾ_SANITIZER 1
#    endif
#  endif
#  ਜੇ !defined(HAS_FEATURE_ਪਤਾ_SANITIZER)
#    define HAS_FEATURE_ਪਤਾ_SANITIZER 0
#  endif
#ਜੇ_ਅੰਤ

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#ਜੇ HAS_FEATURE_ਪਤਾ_SANITIZER && defined(__clang__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ variable_no_sanitize_ਪਤਾ __attribute__((__no_sanitize_address__))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ variable_no_sanitize_ਪਤਾ
#ਜੇ_ਅੰਤ

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#ਜੇ defined(__clang__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ function_no_sanitize_float_cast_ਉਛਾਲ __attribute__((no_sanitize("float-cast-overflow")))
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ function_no_sanitize_float_cast_ਉਛਾਲ
#ਜੇ_ਅੰਤ

/* test harness */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EXIT_TEST_SKIP 77

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t u64_multiply_ਸੁਰੱਖਿਅਤ(uint64_t a, uint64_t b) {
        ਜੇ (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                ਵਾਪਸ 0; /*ਉਛਾਲ*/

        ਵਾਪਸ a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
ਸਥਿਰ ਸਤਰਅੰਦਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ALIGN_POWER2(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ u) {

        /* Avoid subtraction overflow */
        ਜੇ (u == 0)
                ਵਾਪਸ 0;

        /* clz(0) is undefined */
        ਜੇ (u == 1)
                ਵਾਪਸ 1;

        /* left-shift overflow is undefined */
        ਜੇ (__builtin_clzl(u - 1UL) < 1)
                ਵਾਪਸ 0;

        ਵਾਪਸ 1UL << (ਆਕਾਰ(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ container_ਦਾ(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ਅਚਲ typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (ਅੱਖਰ *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_INT(p) ((ਪੂਰਨ) ((intptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((intptr_t) (u)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_UINT(p) ((ਬਿਨਾਨਿਸ਼ਾਨੀ) ((uintptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((uintptr_t) (u)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_LONG(p) ((ਲੰਮਾ) ((intptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LONG_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((intptr_t) (u)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_ULONG(p) ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) ((uintptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ULONG_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((uintptr_t) (u)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT8_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((uintptr_t) (u)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_INT32(p) ((int32_t) ((intptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT32_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((intptr_t) (u)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT32_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((uintptr_t) (u)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_INT64(p) ((int64_t) ((intptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ INT64_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((intptr_t) (u)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_ਮੰਜ਼ਿਲ_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UINT64_ਮੰਜ਼ਿਲ_PTR(u) ((ਖਾਲੀ *) ((uintptr_t) (u)))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CHAR_ਮੰਜ਼ਿਲ_STR(x) ((ਅੱਖਰ[2]) { x, 0 })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ char_array_0(x) x[ਆਕਾਰ(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HEXADECIMAL_STR_MAX(type) (2 + ਆਕਾਰ(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (ਆਕਾਰ(type) <= 1 ? 3U :                                   \
             ਆਕਾਰ(type) <= 2 ? 5U :                                   \
             ਆਕਾਰ(type) <= 4 ? 10U :                                  \
             ਆਕਾਰ(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : ਆਕਾਰ(ਪੂਰਨ[-2*(ਆਕਾਰ(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DECIMAL_STR_ਚੌੜਾਈ(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                ਜੇ (_x_ == 0)                                     \
                        ans = 1;                                  \
                ਨਹੀਂਤਾਂ {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        ਜਦੋਂ ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਅਦਲਾ_ਬਦਲੀ_TWO(x, y) ਕਰੋ {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } ਜਦੋਂ (false)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_MAKE(...) ((ਅੱਖਰ**) ((ਅਚਲ ਅੱਖਰ*[]) { __VA_ARGS__, ਨਿਰਾ }))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STRV_MAKE_CONST(...) ((ਅਚਲ ਅੱਖਰ* ਅਚਲ*) ((ਅਚਲ ਅੱਖਰ*[]) { __VA_ARGS__, ਨਿਰਾ }))

/* Pointers range from NULL to POINTER_MAX */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸੂਚਕ_MAX ((ਖਾਲੀ*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                ਸਮ੍ਰਿਤੀ_ਨਕਲ(&_copy, &(x), ਆਕਾਰ(_copy));                    \
                asm ਅਸਥਿਰ ("" : : : "memory");                       \
                _copy;                                                  \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ saturate_ਜੋੜੋ(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

ਸਥਿਰ ਸਤਰਅੰਦਰ size_t ਮਾਪ_ਜੋੜੋ(size_t x, size_t y) {
        ਵਾਪਸ saturate_ਜੋੜੋ(x, y, SIZE_MAX);
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : ਨਿਰਾ;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_ਦਲੀਲ(ਇੰਦਰਾਜ, ...)                                     \
        FOREACH_ਦਲੀਲ_2(ਇੰਦਰਾਜ, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREACH_ਦਲੀਲ_2(ਇੰਦਰਾਜ, _entries_, _current_, _va_sentinel_, ...)      \
        ਲਈ (typeof(ਇੰਦਰਾਜ) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((ਲੰਮਾ)(_current_ - _entries_) < (ਲੰਮਾ)(ELEMENTSOF(_entries_) - 1)) && ({ ਇੰਦਰਾਜ = *_current_; true; }); \
             _current_++)
