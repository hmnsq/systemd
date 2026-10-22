/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع فراغ (*hash_func_t)(ثابت فراغ *p, بنية siphash *state);
تعريفنوع صحيح (*compare_func_t)(ثابت فراغ *a, ثابت فراغ *b);

بنية hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ڪنجي;
        free_func_t free_قدر;
};

#وصف_ڪريو _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ڪنجي_func, free_قدر_func, scope) \
        اڻ_استعمال_ٿيل ساكن فراغ (* UNIQ_T(static_hash_wrapper, uq))(ثابت type *, بنية siphash *) = hash_func; \
        اڻ_استعمال_ٿيل ساكن صحيح (* UNIQ_T(static_compare_wrapper, uq))(ثابت type *, ثابت type *) = compare_func; \
        scope ثابت بنية hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ڪنجي = free_ڪنجي_func,                              \
                .free_قدر = free_قدر_func,                          \
        }

#وصف_ڪريو _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ساكن فراغ UNIQ_T(wrapper_name, uq)(فراغ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#وصف_ڪريو DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), عدم, scope)

#وصف_ڪريو DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_قدر, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_قدر, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         عدم, UNIQ_T(static_free_wrapper, uq), scope)

#وصف_ڪريو DEFINE_HASH_OPS_ڀريل_2(uq, name, type, hash_func, compare_func, free_ڪنجي_func, type_قدر, free_قدر_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ڪنجي_wrapper, free_ڪنجي_func); \
        _DEFINE_FREE_FUNC(uq, type_قدر, static_free_قدر_wrapper, free_قدر_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ڪنجي_wrapper, uq),           \
                         UNIQ_T(static_free_قدر_wrapper, uq), scope)

#وصف_ڪريو DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم,)

#وصف_ڪريو DEFINE_ذاتي_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم, ساكن)

#وصف_ڪريو DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#وصف_ڪريو DEFINE_ذاتي_HASH_OPS_WITH_ڪنجي_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ڪنجي_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ساكن)

#وصف_ڪريو DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR(name, type, hash_func, compare_func, قدر_type, free_func) \
        DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, قدر_type, free_func,)

#وصف_ڪريو DEFINE_ذاتي_HASH_OPS_WITH_قدر_DESTRUCTOR(name, type, hash_func, compare_func, قدر_type, free_func) \
        DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, قدر_type, free_func, ساكن)

#وصف_ڪريو DEFINE_HASH_OPS_ڀريل(name, type, hash_func, compare_func, free_ڪنجي_func, قدر_type, free_قدر_func) \
        DEFINE_HASH_OPS_ڀريل_2(UNIQ, name, type, hash_func, compare_func, free_ڪنجي_func, قدر_type, free_قدر_func,)

#وصف_ڪريو DEFINE_ذاتي_HASH_OPS_ڀريل(name, type, hash_func, compare_func, free_ڪنجي_func, قدر_type, free_قدر_func) \
        DEFINE_HASH_OPS_ڀريل_2(UNIQ, name, type, hash_func, compare_func, free_ڪنجي_func, قدر_type, free_قدر_func, ساكن)

فراغ string_hash_func(ثابت حرف *p, بنية siphash *state);
#وصف_ڪريو string_compare_func strcmp
خارجي ثابت بنية hash_ops string_hash_ops;
خارجي ثابت بنية hash_ops string_hash_ops_free;
خارجي ثابت بنية hash_ops string_hash_ops_قدر_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_strv_free;

فراغ رستو_hash_func(ثابت حرف *p, بنية siphash *state);
خارجي ثابت بنية hash_ops رستو_hash_ops;
خارجي ثابت بنية hash_ops رستو_hash_ops_free;
خارجي ثابت بنية hash_ops رستو_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
فراغ trivial_hash_func(ثابت فراغ *p, بنية siphash *state);
صحيح trivial_compare_func(ثابت فراغ *a, ثابت فراغ *b) _const_;
خارجي ثابت بنية hash_ops trivial_hash_ops;
خارجي ثابت بنية hash_ops trivial_hash_ops_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_قدر_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
فراغ uint64_hash_func(ثابت uint64_t *p, بنية siphash *state);
صحيح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) خالص;
خارجي ثابت بنية hash_ops uint64_hash_ops;
خارجي ثابت بنية hash_ops uint64_hash_ops_قدر_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#إذا SIZEOF_DEV_T != 8
فراغ devt_hash_func(ثابت dev_t *p, بنية siphash *state);
#وإلا
#وصف_ڪريو devt_hash_func uint64_hash_func
#جيڪڏهن_پورو

صحيح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) خالص;
خارجي ثابت بنية hash_ops devt_hash_ops;
