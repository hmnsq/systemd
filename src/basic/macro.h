/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "../fundamental/macro.h" /* IWYU pragma: export */

#إذا !defined(HAS_FEATURE_حافظہ_SANITIZER)
#  إذا defined(__has_feature)
#    إذا __has_feature(حافظہ_sanitizer)
#      define HAS_FEATURE_حافظہ_SANITIZER 1
#    endif
#  endif
#  إذا !defined(HAS_FEATURE_حافظہ_SANITIZER)
#    define HAS_FEATURE_حافظہ_SANITIZER 0
#  endif
#جے_انت

#إذا !defined(HAS_FEATURE_پتہ_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_پتہ_SANITIZER 1
#  elif defined(__has_feature)
#    إذا __has_feature(پتہ_sanitizer)
#      define HAS_FEATURE_پتہ_SANITIZER 1
#    endif
#  endif
#  إذا !defined(HAS_FEATURE_پتہ_SANITIZER)
#    define HAS_FEATURE_پتہ_SANITIZER 0
#  endif
#جے_انت

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#إذا HAS_FEATURE_پتہ_SANITIZER && defined(__clang__)
#تعریف_کرو variable_no_sanitize_پتہ __attribute__((__no_sanitize_address__))
#وإلا
#تعریف_کرو variable_no_sanitize_پتہ
#جے_انت

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#إذا defined(__clang__)
#تعریف_کرو function_no_sanitize_float_cast_حد_توں_ودھ __attribute__((no_sanitize("float-cast-overflow")))
#وإلا
#تعریف_کرو function_no_sanitize_float_cast_حد_توں_ودھ
#جے_انت

/* test harness */
#تعریف_کرو EXIT_TEST_SKIP 77

ساكن مضمن uint64_t u64_multiply_محفوظ(uint64_t a, uint64_t b) {
        إذا (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                أرجع 0; /*حد_توں_ودھ*/

        أرجع a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
ساكن مضمن بنانشانی لما ALIGN_POWER2(بنانشانی لما u) {

        /* Avoid subtraction overflow */
        إذا (u == 0)
                أرجع 0;

        /* clz(0) is undefined */
        إذا (u == 1)
                أرجع 1;

        /* left-shift overflow is undefined */
        إذا (__builtin_clzl(u - 1UL) < 1)
                أرجع 0;

        أرجع 1UL << (حجم(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#تعریف_کرو container_دا(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#تعریف_کرو __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ثابت typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (حرف *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#تعریف_کرو PTR_منزل_INT(p) ((صحيح) ((intptr_t) (p)))
#تعریف_کرو INT_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کرو PTR_منزل_UINT(p) ((بنانشانی) ((uintptr_t) (p)))
#تعریف_کرو UINT_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کرو PTR_منزل_LONG(p) ((لما) ((intptr_t) (p)))
#تعریف_کرو LONG_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کرو PTR_منزل_ULONG(p) ((بنانشانی لما) ((uintptr_t) (p)))
#تعریف_کرو ULONG_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کرو PTR_منزل_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#تعریف_کرو UINT8_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کرو PTR_منزل_INT32(p) ((int32_t) ((intptr_t) (p)))
#تعریف_کرو INT32_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کرو PTR_منزل_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#تعریف_کرو UINT32_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کرو PTR_منزل_INT64(p) ((int64_t) ((intptr_t) (p)))
#تعریف_کرو INT64_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کرو PTR_منزل_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#تعریف_کرو UINT64_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کرو CHAR_منزل_STR(x) ((حرف[2]) { x, 0 })

#تعریف_کرو char_array_0(x) x[حجم(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#تعریف_کرو HEXADECIMAL_STR_MAX(type) (2 + حجم(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#تعریف_کرو DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (حجم(type) <= 1 ? 3U :                                   \
             حجم(type) <= 2 ? 5U :                                   \
             حجم(type) <= 4 ? 10U :                                  \
             حجم(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : حجم(صحيح[-2*(حجم(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#تعریف_کرو DECIMAL_STR_چوڑائی(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                إذا (_x_ == 0)                                     \
                        ans = 1;                                  \
                وإلا {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        طالما ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#تعریف_کرو وٹاندرا_TWO(x, y) افعل {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } طالما (false)

#تعریف_کرو STRV_MAKE(...) ((حرف**) ((ثابت حرف*[]) { __VA_ARGS__, عدم }))
#تعریف_کرو STRV_MAKE_CONST(...) ((ثابت حرف* ثابت*) ((ثابت حرف*[]) { __VA_ARGS__, عدم }))

/* Pointers range from NULL to POINTER_MAX */
#تعریف_کرو اشارہ_گر_MAX ((فراغ*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#تعریف_کرو READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                حافظہ_نقل(&_copy, &(x), حجم(_copy));                    \
                asm متطاير ("" : : : "memory");                       \
                _copy;                                                  \
        })

#تعریف_کرو saturate_جوڑو(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

ساكن مضمن size_t ناپ_جوڑو(size_t x, size_t y) {
        أرجع saturate_جوڑو(x, y, SIZE_MAX);
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
#تعریف_کرو PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : عدم;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#تعریف_کرو FOREACH_دلیل(اندراج, ...)                                     \
        FOREACH_دلیل_2(اندراج, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#تعریف_کرو FOREACH_دلیل_2(اندراج, _entries_, _current_, _va_sentinel_, ...)      \
        لكل (typeof(اندراج) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((لما)(_current_ - _entries_) < (لما)(ELEMENTSOF(_entries_) - 1)) && ({ اندراج = *_current_; true; }); \
             _current_++)
