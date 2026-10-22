/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع فراغ (*hash_func_t)(ثابت فراغ *p, بنية siphash *state);
تعريفنوع صحيح (*compare_func_t)(ثابت فراغ *a, ثابت فراغ *b);

بنية hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_کلی;
        free_func_t free_ارزښت;
};

#تعریف_کړه _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_کلی_func, free_ارزښت_func, scope) \
        ناکارول_شوی ساكن فراغ (* UNIQ_T(static_hash_wrapper, uq))(ثابت type *, بنية siphash *) = hash_func; \
        ناکارول_شوی ساكن صحيح (* UNIQ_T(static_compare_wrapper, uq))(ثابت type *, ثابت type *) = compare_func; \
        scope ثابت بنية hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_کلی = free_کلی_func,                              \
                .free_ارزښت = free_ارزښت_func,                          \
        }

#تعریف_کړه _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ساكن فراغ UNIQ_T(wrapper_name, uq)(فراغ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#تعریف_کړه DEFINE_HASH_OPS_WITH_کلی_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), عدم, scope)

#تعریف_کړه DEFINE_HASH_OPS_WITH_ارزښت_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_ارزښت, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_ارزښت, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         عدم, UNIQ_T(static_free_wrapper, uq), scope)

#تعریف_کړه DEFINE_HASH_OPS_ډک_2(uq, name, type, hash_func, compare_func, free_کلی_func, type_ارزښت, free_ارزښت_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_کلی_wrapper, free_کلی_func); \
        _DEFINE_FREE_FUNC(uq, type_ارزښت, static_free_ارزښت_wrapper, free_ارزښت_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_کلی_wrapper, uq),           \
                         UNIQ_T(static_free_ارزښت_wrapper, uq), scope)

#تعریف_کړه DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم,)

#تعریف_کړه DEFINE_شخصي_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم, ساكن)

#تعریف_کړه DEFINE_HASH_OPS_WITH_کلی_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلی_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#تعریف_کړه DEFINE_شخصي_HASH_OPS_WITH_کلی_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلی_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ساكن)

#تعریف_کړه DEFINE_HASH_OPS_WITH_ارزښت_DESTRUCTOR(name, type, hash_func, compare_func, ارزښت_type, free_func) \
        DEFINE_HASH_OPS_WITH_ارزښت_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ارزښت_type, free_func,)

#تعریف_کړه DEFINE_شخصي_HASH_OPS_WITH_ارزښت_DESTRUCTOR(name, type, hash_func, compare_func, ارزښت_type, free_func) \
        DEFINE_HASH_OPS_WITH_ارزښت_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ارزښت_type, free_func, ساكن)

#تعریف_کړه DEFINE_HASH_OPS_ډک(name, type, hash_func, compare_func, free_کلی_func, ارزښت_type, free_ارزښت_func) \
        DEFINE_HASH_OPS_ډک_2(UNIQ, name, type, hash_func, compare_func, free_کلی_func, ارزښت_type, free_ارزښت_func,)

#تعریف_کړه DEFINE_شخصي_HASH_OPS_ډک(name, type, hash_func, compare_func, free_کلی_func, ارزښت_type, free_ارزښت_func) \
        DEFINE_HASH_OPS_ډک_2(UNIQ, name, type, hash_func, compare_func, free_کلی_func, ارزښت_type, free_ارزښت_func, ساكن)

فراغ string_hash_func(ثابت حرف *p, بنية siphash *state);
#تعریف_کړه string_compare_func strcmp
خارجي ثابت بنية hash_ops string_hash_ops;
خارجي ثابت بنية hash_ops string_hash_ops_free;
خارجي ثابت بنية hash_ops string_hash_ops_ارزښت_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_strv_free;

فراغ لار_hash_func(ثابت حرف *p, بنية siphash *state);
خارجي ثابت بنية hash_ops لار_hash_ops;
خارجي ثابت بنية hash_ops لار_hash_ops_free;
خارجي ثابت بنية hash_ops لار_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
فراغ trivial_hash_func(ثابت فراغ *p, بنية siphash *state);
صحيح trivial_compare_func(ثابت فراغ *a, ثابت فراغ *b) _const_;
خارجي ثابت بنية hash_ops trivial_hash_ops;
خارجي ثابت بنية hash_ops trivial_hash_ops_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_ارزښت_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
فراغ uint64_hash_func(ثابت uint64_t *p, بنية siphash *state);
صحيح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) خالص;
خارجي ثابت بنية hash_ops uint64_hash_ops;
خارجي ثابت بنية hash_ops uint64_hash_ops_ارزښت_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#إذا SIZEOF_DEV_T != 8
فراغ devt_hash_func(ثابت dev_t *p, بنية siphash *state);
#وإلا
#تعریف_کړه devt_hash_func uint64_hash_func
#د_که_پای

صحيح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) خالص;
خارجي ثابت بنية hash_ops devt_hash_ops;
