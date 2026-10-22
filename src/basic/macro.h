/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "../fundamental/macro.h" /* IWYU pragma: export */

#إذا !defined(HAS_FEATURE_حافظه_SANITIZER)
#  إذا defined(__has_feature)
#    إذا __has_feature(حافظه_sanitizer)
#      define HAS_FEATURE_حافظه_SANITIZER 1
#    endif
#  endif
#  إذا !defined(HAS_FEATURE_حافظه_SANITIZER)
#    define HAS_FEATURE_حافظه_SANITIZER 0
#  endif
#د_که_پای

#إذا !defined(HAS_FEATURE_پته_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_پته_SANITIZER 1
#  elif defined(__has_feature)
#    إذا __has_feature(پته_sanitizer)
#      define HAS_FEATURE_پته_SANITIZER 1
#    endif
#  endif
#  إذا !defined(HAS_FEATURE_پته_SANITIZER)
#    define HAS_FEATURE_پته_SANITIZER 0
#  endif
#د_که_پای

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#إذا HAS_FEATURE_پته_SANITIZER && defined(__clang__)
#تعریف_کړه variable_no_sanitize_پته __attribute__((__no_sanitize_address__))
#وإلا
#تعریف_کړه variable_no_sanitize_پته
#د_که_پای

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#إذا defined(__clang__)
#تعریف_کړه function_no_sanitize_float_cast_ډکېدنه __attribute__((no_sanitize("float-cast-overflow")))
#وإلا
#تعریف_کړه function_no_sanitize_float_cast_ډکېدنه
#د_که_پای

/* test harness */
#تعریف_کړه EXIT_TEST_SKIP 77

ساكن مضمن uint64_t u64_multiply_خوندي(uint64_t a, uint64_t b) {
        إذا (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                أرجع 0; /*ډکېدنه*/

        أرجع a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
ساكن مضمن بینښه اوږد ALIGN_POWER2(بینښه اوږد u) {

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
#تعریف_کړه container_د(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#تعریف_کړه __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ثابت typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (حرف *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#تعریف_کړه PTR_منزل_INT(p) ((صحيح) ((intptr_t) (p)))
#تعریف_کړه INT_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کړه PTR_منزل_UINT(p) ((بینښه) ((uintptr_t) (p)))
#تعریف_کړه UINT_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کړه PTR_منزل_LONG(p) ((اوږد) ((intptr_t) (p)))
#تعریف_کړه LONG_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کړه PTR_منزل_ULONG(p) ((بینښه اوږد) ((uintptr_t) (p)))
#تعریف_کړه ULONG_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کړه PTR_منزل_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#تعریف_کړه UINT8_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کړه PTR_منزل_INT32(p) ((int32_t) ((intptr_t) (p)))
#تعریف_کړه INT32_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کړه PTR_منزل_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#تعریف_کړه UINT32_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کړه PTR_منزل_INT64(p) ((int64_t) ((intptr_t) (p)))
#تعریف_کړه INT64_منزل_PTR(u) ((فراغ *) ((intptr_t) (u)))
#تعریف_کړه PTR_منزل_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#تعریف_کړه UINT64_منزل_PTR(u) ((فراغ *) ((uintptr_t) (u)))

#تعریف_کړه CHAR_منزل_STR(x) ((حرف[2]) { x, 0 })

#تعریف_کړه char_array_0(x) x[حجم(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#تعریف_کړه HEXADECIMAL_STR_MAX(type) (2 + حجم(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#تعریف_کړه DECIMAL_STR_MAX(type)                                           \
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
#تعریف_کړه DECIMAL_STR_پلنوالی(x)                                      \
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

#تعریف_کړه بدلون_TWO(x, y) افعل {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } طالما (false)

#تعریف_کړه STRV_MAKE(...) ((حرف**) ((ثابت حرف*[]) { __VA_ARGS__, عدم }))
#تعریف_کړه STRV_MAKE_CONST(...) ((ثابت حرف* ثابت*) ((ثابت حرف*[]) { __VA_ARGS__, عدم }))

/* Pointers range from NULL to POINTER_MAX */
#تعریف_کړه اشاره_ګر_MAX ((فراغ*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#تعریف_کړه READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                حافظه_نقل(&_copy, &(x), حجم(_copy));                    \
                asm متطاير ("" : : : "memory");                       \
                _copy;                                                  \
        })

#تعریف_کړه saturate_جمع_کړه(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

ساكن مضمن size_t اندازه_جمع_کړه(size_t x, size_t y) {
        أرجع saturate_جمع_کړه(x, y, SIZE_MAX);
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
#تعریف_کړه PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : عدم;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#تعریف_کړه FOREACH_دلیل(ثبت, ...)                                     \
        FOREACH_دلیل_2(ثبت, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#تعریف_کړه FOREACH_دلیل_2(ثبت, _entries_, _current_, _va_sentinel_, ...)      \
        لكل (typeof(ثبت) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((اوږد)(_current_ - _entries_) < (اوږد)(ELEMENTSOF(_entries_) - 1)) && ({ ثبت = *_current_; true; }); \
             _current_++)
