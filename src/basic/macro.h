/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "../fundamental/macro.h" /* IWYU pragma: export */

#ఒకవేళ !defined(HAS_FEATURE_జ్ఞాపకం_SANITIZER)
#  ఒకవేళ defined(__has_feature)
#    ఒకవేళ __has_feature(జ్ఞాపకం_sanitizer)
#      define HAS_FEATURE_జ్ఞాపకం_SANITIZER 1
#    endif
#  endif
#  ఒకవేళ !defined(HAS_FEATURE_జ్ఞాపకం_SANITIZER)
#    define HAS_FEATURE_జ్ఞాపకం_SANITIZER 0
#  endif
#అయితే_ముగింపు

#ఒకవేళ !defined(HAS_FEATURE_చిరునామా_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_చిరునామా_SANITIZER 1
#  elif defined(__has_feature)
#    ఒకవేళ __has_feature(చిరునామా_sanitizer)
#      define HAS_FEATURE_చిరునామా_SANITIZER 1
#    endif
#  endif
#  ఒకవేళ !defined(HAS_FEATURE_చిరునామా_SANITIZER)
#    define HAS_FEATURE_చిరునామా_SANITIZER 0
#  endif
#అయితే_ముగింపు

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#ఒకవేళ HAS_FEATURE_చిరునామా_SANITIZER && defined(__clang__)
#నిర్వచించు variable_no_sanitize_చిరునామా __attribute__((__no_sanitize_address__))
#లేకపోతే
#నిర్వచించు variable_no_sanitize_చిరునామా
#అయితే_ముగింపు

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#ఒకవేళ defined(__clang__)
#నిర్వచించు function_no_sanitize_float_cast_అధికప్రవాహం __attribute__((no_sanitize("float-cast-overflow")))
#లేకపోతే
#నిర్వచించు function_no_sanitize_float_cast_అధికప్రవాహం
#అయితే_ముగింపు

/* test harness */
#నిర్వచించు EXIT_TEST_SKIP 77

స్థిర అంతఃవరుస uint64_t u64_multiply_సురక్షిత(uint64_t a, uint64_t b) {
        ఒకవేళ (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                తిరిగి 0; /*అధికప్రవాహం*/

        తిరిగి a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
స్థిర అంతఃవరుస గుర్తులేని దీర్ఘ ALIGN_POWER2(గుర్తులేని దీర్ఘ u) {

        /* Avoid subtraction overflow */
        ఒకవేళ (u == 0)
                తిరిగి 0;

        /* clz(0) is undefined */
        ఒకవేళ (u == 1)
                తిరిగి 1;

        /* left-shift overflow is undefined */
        ఒకవేళ (__builtin_clzl(u - 1UL) < 1)
                తిరిగి 0;

        తిరిగి 1UL << (పరిమాణం(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#నిర్వచించు container_యొక్క(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#నిర్వచించు __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                స్థిరరాశి typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (అక్షరం *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#నిర్వచించు PTR_గమ్యం_INT(p) ((పూర్ణాంకం) ((intptr_t) (p)))
#నిర్వచించు INT_గమ్యం_PTR(u) ((శూన్య *) ((intptr_t) (u)))
#నిర్వచించు PTR_గమ్యం_UINT(p) ((గుర్తులేని) ((uintptr_t) (p)))
#నిర్వచించు UINT_గమ్యం_PTR(u) ((శూన్య *) ((uintptr_t) (u)))

#నిర్వచించు PTR_గమ్యం_LONG(p) ((దీర్ఘ) ((intptr_t) (p)))
#నిర్వచించు LONG_గమ్యం_PTR(u) ((శూన్య *) ((intptr_t) (u)))
#నిర్వచించు PTR_గమ్యం_ULONG(p) ((గుర్తులేని దీర్ఘ) ((uintptr_t) (p)))
#నిర్వచించు ULONG_గమ్యం_PTR(u) ((శూన్య *) ((uintptr_t) (u)))

#నిర్వచించు PTR_గమ్యం_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#నిర్వచించు UINT8_గమ్యం_PTR(u) ((శూన్య *) ((uintptr_t) (u)))

#నిర్వచించు PTR_గమ్యం_INT32(p) ((int32_t) ((intptr_t) (p)))
#నిర్వచించు INT32_గమ్యం_PTR(u) ((శూన్య *) ((intptr_t) (u)))
#నిర్వచించు PTR_గమ్యం_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#నిర్వచించు UINT32_గమ్యం_PTR(u) ((శూన్య *) ((uintptr_t) (u)))

#నిర్వచించు PTR_గమ్యం_INT64(p) ((int64_t) ((intptr_t) (p)))
#నిర్వచించు INT64_గమ్యం_PTR(u) ((శూన్య *) ((intptr_t) (u)))
#నిర్వచించు PTR_గమ్యం_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#నిర్వచించు UINT64_గమ్యం_PTR(u) ((శూన్య *) ((uintptr_t) (u)))

#నిర్వచించు CHAR_గమ్యం_STR(x) ((అక్షరం[2]) { x, 0 })

#నిర్వచించు char_array_0(x) x[పరిమాణం(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#నిర్వచించు HEXADECIMAL_STR_MAX(type) (2 + పరిమాణం(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#నిర్వచించు DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (పరిమాణం(type) <= 1 ? 3U :                                   \
             పరిమాణం(type) <= 2 ? 5U :                                   \
             పరిమాణం(type) <= 4 ? 10U :                                  \
             పరిమాణం(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : పరిమాణం(పూర్ణాంకం[-2*(పరిమాణం(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#నిర్వచించు DECIMAL_STR_వెడల్పు(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                ఒకవేళ (_x_ == 0)                                     \
                        ans = 1;                                  \
                లేకపోతే {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        అయితే ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#నిర్వచించు మార్పిడి_TWO(x, y) చేయి {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } అయితే (false)

#నిర్వచించు STRV_MAKE(...) ((అక్షరం**) ((స్థిరరాశి అక్షరం*[]) { __VA_ARGS__, ఖాళీ }))
#నిర్వచించు STRV_MAKE_CONST(...) ((స్థిరరాశి అక్షరం* స్థిరరాశి*) ((స్థిరరాశి అక్షరం*[]) { __VA_ARGS__, ఖాళీ }))

/* Pointers range from NULL to POINTER_MAX */
#నిర్వచించు సూచిక_MAX ((శూన్య*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#నిర్వచించు READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                జ్ఞాపకం_నకలు(&_copy, &(x), పరిమాణం(_copy));                    \
                asm అస్థిర ("" : : : "memory");                       \
                _copy;                                                  \
        })

#నిర్వచించు saturate_కూడు(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

స్థిర అంతఃవరుస size_t కొలత_కూడు(size_t x, size_t y) {
        తిరిగి saturate_కూడు(x, y, SIZE_MAX);
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
#నిర్వచించు PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : ఖాళీ;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#నిర్వచించు FOREACH_వాదం(నమోదు_2, ...)                                     \
        FOREACH_వాదం_2(నమోదు_2, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#నిర్వచించు FOREACH_వాదం_2(నమోదు_2, _entries_, _current_, _va_sentinel_, ...)      \
        కొరకు (typeof(నమోదు_2) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((దీర్ఘ)(_current_ - _entries_) < (దీర్ఘ)(ELEMENTSOF(_entries_) - 1)) && ({ నమోదు_2 = *_current_; true; }); \
             _current_++)
