/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "../fundamental/macro.h" /* IWYU pragma: export */

#यदि !defined(HAS_FEATURE_स्मृतिः_SANITIZER)
#  यदि defined(__has_feature)
#    यदि __has_feature(स्मृतिः_sanitizer)
#      define HAS_FEATURE_स्मृतिः_SANITIZER 1
#    endif
#  endif
#  यदि !defined(HAS_FEATURE_स्मृतिः_SANITIZER)
#    define HAS_FEATURE_स्मृतिः_SANITIZER 0
#  endif
#यदि_अन्तः

#यदि !defined(HAS_FEATURE_स्थानाङ्कः_SANITIZER)
#  ifdef __SANITIZE_ADDRESS__
#      define HAS_FEATURE_स्थानाङ्कः_SANITIZER 1
#  elif defined(__has_feature)
#    यदि __has_feature(स्थानाङ्कः_sanitizer)
#      define HAS_FEATURE_स्थानाङ्कः_SANITIZER 1
#    endif
#  endif
#  यदि !defined(HAS_FEATURE_स्थानाङ्कः_SANITIZER)
#    define HAS_FEATURE_स्थानाङ्कः_SANITIZER 0
#  endif
#यदि_अन्तः

/* Note: on GCC "no_sanitize_address" is a function attribute only, on llvm it may also be applied to global
 * variables. We define a specific macro which knows this. Note that on GCC we don't need this decorator so much, since
 * our primary use case for this attribute is registration structures placed in named ELF sections which shall not be
 * padded, but GCC doesn't pad those anyway if AddressSanitizer is enabled. */
#यदि HAS_FEATURE_स्थानाङ्कः_SANITIZER && defined(__clang__)
#परिभाषय variable_no_sanitize_स्थानाङ्कः __attribute__((__no_sanitize_address__))
#अन्यथा
#परिभाषय variable_no_sanitize_स्थानाङ्कः
#यदि_अन्तः

/* Apparently there's no has_feature() call defined to check for ubsan, hence let's define this
 * unconditionally on llvm */
#यदि defined(__clang__)
#परिभाषय function_no_sanitize_float_cast_अतिप्रवाहः __attribute__((no_sanitize("float-cast-overflow")))
#अन्यथा
#परिभाषय function_no_sanitize_float_cast_अतिप्रवाहः
#यदि_अन्तः

/* test harness */
#परिभाषय EXIT_TEST_SKIP 77

स्थिर अन्तःस्थ uint64_t u64_multiply_safe(uint64_t a, uint64_t b) {
        यदि (_unlikely_(a != 0 && b > (UINT64_MAX / a)))
                प्रत्यागम 0; /*अतिप्रवाहः*/

        प्रत्यागम a * b;
}

/* align to next higher power-of-2 (except for: 0 => 0, overflow => 0) */
स्थिर अन्तःस्थ अचिह्नित दीर्घ ALIGN_POWER2(अचिह्नित दीर्घ u) {

        /* Avoid subtraction overflow */
        यदि (u == 0)
                प्रत्यागम 0;

        /* clz(0) is undefined */
        यदि (u == 1)
                प्रत्यागम 1;

        /* left-shift overflow is undefined */
        यदि (__builtin_clzl(u - 1UL) < 1)
                प्रत्यागम 0;

        प्रत्यागम 1UL << (परिमाण(u) * 8 - __builtin_clzl(u - 1UL));
}

/*
 * container_of - cast a member of a structure out to the containing structure
 * @ptr: the pointer to the member.
 * @type: the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 */
#परिभाषय container_सम्बन्धि(ptr, type, member) __container_of(UNIQ, (ptr), type, member)
#परिभाषय __container_of(uniq, ptr, type, member)                         \
        ({                                                              \
                ध्रुव typeof( ((type*)0)->member ) *UNIQ_T(A, uniq) = (ptr); \
                (type*)( (वर्ण *)UNIQ_T(A, uniq) - offsetof(type, member) ); \
        })

#परिभाषय PTR_गन्तव्यम्_INT(p) ((पूर्णाङ्क) ((intptr_t) (p)))
#परिभाषय INT_गन्तव्यम्_PTR(u) ((शून्य *) ((intptr_t) (u)))
#परिभाषय PTR_गन्तव्यम्_UINT(p) ((अचिह्नित) ((uintptr_t) (p)))
#परिभाषय UINT_गन्तव्यम्_PTR(u) ((शून्य *) ((uintptr_t) (u)))

#परिभाषय PTR_गन्तव्यम्_LONG(p) ((दीर्घ) ((intptr_t) (p)))
#परिभाषय LONG_गन्तव्यम्_PTR(u) ((शून्य *) ((intptr_t) (u)))
#परिभाषय PTR_गन्तव्यम्_ULONG(p) ((अचिह्नित दीर्घ) ((uintptr_t) (p)))
#परिभाषय ULONG_गन्तव्यम्_PTR(u) ((शून्य *) ((uintptr_t) (u)))

#परिभाषय PTR_गन्तव्यम्_UINT8(p) ((uint8_t) ((uintptr_t) (p)))
#परिभाषय UINT8_गन्तव्यम्_PTR(u) ((शून्य *) ((uintptr_t) (u)))

#परिभाषय PTR_गन्तव्यम्_INT32(p) ((int32_t) ((intptr_t) (p)))
#परिभाषय INT32_गन्तव्यम्_PTR(u) ((शून्य *) ((intptr_t) (u)))
#परिभाषय PTR_गन्तव्यम्_UINT32(p) ((uint32_t) ((uintptr_t) (p)))
#परिभाषय UINT32_गन्तव्यम्_PTR(u) ((शून्य *) ((uintptr_t) (u)))

#परिभाषय PTR_गन्तव्यम्_INT64(p) ((int64_t) ((intptr_t) (p)))
#परिभाषय INT64_गन्तव्यम्_PTR(u) ((शून्य *) ((intptr_t) (u)))
#परिभाषय PTR_गन्तव्यम्_UINT64(p) ((uint64_t) ((uintptr_t) (p)))
#परिभाषय UINT64_गन्तव्यम्_PTR(u) ((शून्य *) ((uintptr_t) (u)))

#परिभाषय CHAR_गन्तव्यम्_STR(x) ((वर्ण[2]) { x, 0 })

#परिभाषय char_array_0(x) x[परिमाण(x)-1] = 0;

/* Maximum buffer size needed for formatting an unsigned integer type as hex, including space for '0x'
 * prefix and trailing NUL suffix. */
#परिभाषय HEXADECIMAL_STR_MAX(type) (2 + परिमाण(type) * 2 + 1)

/* Returns the number of chars needed to format variables of the specified type as a decimal string. Adds in
 * extra space for a negative '-' prefix for signed types. Includes space for the trailing NUL. */
#परिभाषय DECIMAL_STR_MAX(type)                                           \
        ((size_t) IS_SIGNED_INTEGER_TYPE(type) + 1U +                   \
            (परिमाण(type) <= 1 ? 3U :                                   \
             परिमाण(type) <= 2 ? 5U :                                   \
             परिमाण(type) <= 4 ? 10U :                                  \
             परिमाण(type) <= 8 ? (IS_SIGNED_INTEGER_TYPE(type) ? 19U : 20U) : परिमाण(पूर्णाङ्क[-2*(परिमाण(type) > 8)])))

/* Returns the number of chars needed to format the specified integer value. It's hence more specific than
 * DECIMAL_STR_MAX() which answers the same question for all possible values of the specified type. Does
 * *not* include space for a trailing NUL. (If you wonder why we special case _x_ == 0 here: it's to trick
 * out gcc's -Wtype-limits, which would complain on comparing an unsigned type with < 0, otherwise. By
 * special-casing == 0 here first, we can use <= 0 instead of < 0 to trick out gcc.) */
#परिभाषय DECIMAL_STR_विस्तारः(x)                                      \
        ({                                                        \
                typeof(x) _x_ = (x);                              \
                size_t ans;                                       \
                यदि (_x_ == 0)                                     \
                        ans = 1;                                  \
                अन्यथा {                                            \
                        ans = _x_ <= 0 ? 2 : 1;                   \
                        यावत् ((_x_ /= 10) != 0)                  \
                                ans++;                            \
                }                                                 \
                ans;                                              \
        })

#परिभाषय SWAP_TWO(x, y) कुरु {                        \
                typeof(x) _t = (x);                \
                (x) = (y);                         \
                (y) = (_t);                        \
        } यावत् (false)

#परिभाषय STRV_MAKE(...) ((वर्ण**) ((ध्रुव वर्ण*[]) { __VA_ARGS__, रिक्त }))
#परिभाषय STRV_MAKE_CONST(...) ((ध्रुव वर्ण* ध्रुव*) ((ध्रुव वर्ण*[]) { __VA_ARGS__, रिक्त }))

/* Pointers range from NULL to POINTER_MAX */
#परिभाषय POINTER_MAX ((शून्य*) UINTPTR_MAX)

/* A macro to force copying of a variable from memory. This is useful whenever we want to read something from
 * memory and want to make sure the compiler won't optimize away the destination variable for us. It's not
 * supposed to be a full CPU memory barrier, i.e. CPU is still allowed to reorder the reads, but it is not
 * allowed to remove our local copies of the variables. We want this to work for unaligned memory, hence
 * memcpy() is great for our purposes. */
#परिभाषय READ_NOW(x)                                                     \
        ({                                                              \
                typeof(x) _copy;                                        \
                स्मृतिं_प्रतिलिख(&_copy, &(x), परिमाण(_copy));                    \
                asm चञ्चल ("" : : : "memory");                       \
                _copy;                                                  \
        })

#परिभाषय saturate_सङ्कलय(x, y, limit)                                       \
        ({                                                              \
                typeof(limit) _x = (x);                                 \
                typeof(limit) _y = (y);                                 \
                _x > (limit) || _y >= (limit) - _x ? (limit) : _x + _y; \
        })

स्थिर अन्तःस्थ size_t आकार_सङ्कलय(size_t x, size_t y) {
        प्रत्यागम saturate_सङ्कलय(x, y, SIZE_MAX);
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
#परिभाषय PTR_SUB1(p, base)                                \
        ({                                               \
                typeof(p) _q = (p);                      \
                _q && _q > (base) ? &_q[-1] : रिक्त;      \
        })

/* Iterate through each argument passed. All must be the same type as 'entry' or must be implicitly
 * convertible. The iteration variable 'entry' must already be defined. */
#परिभाषय FOREACH_तर्कः(प्रविष्टिः, ...)                                     \
        FOREACH_तर्कः_2(प्रविष्टिः, UNIQ_T(_entries_, UNIQ), UNIQ_T(_current_, UNIQ), UNIQ_T(_va_sentinel_, UNIQ), ##__VA_ARGS__)
#परिभाषय FOREACH_तर्कः_2(प्रविष्टिः, _entries_, _current_, _va_sentinel_, ...)      \
        प्रति (typeof(प्रविष्टिः) _va_sentinel_[1] = {}, _entries_[] = { __VA_ARGS__ __VA_OPT__(,) _va_sentinel_[0] }, *_current_ = _entries_; \
             ((दीर्घ)(_current_ - _entries_) < (दीर्घ)(ELEMENTSOF(_entries_) - 1)) && ({ प्रविष्टिः = *_current_; true; }); \
             _current_++)
