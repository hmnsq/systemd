/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع فراغ (*hash_func_t)(ثابت فراغ *p, بنية siphash *state);
تعريفنوع صحيح (*compare_func_t)(ثابت فراغ *a, ثابت فراغ *b);

بنية hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_کلیل;
        free_func_t free_بەها;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_کلیل_func, free_بەها_func, scope) \
        _unused_ ساكن فراغ (* UNIQ_T(static_hash_wrapper, uq))(ثابت type *, بنية siphash *) = hash_func; \
        _unused_ ساكن صحيح (* UNIQ_T(static_compare_wrapper, uq))(ثابت type *, ثابت type *) = compare_func; \
        scope ثابت بنية hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_کلیل = free_کلیل_func,                              \
                .free_بەها = free_بەها_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ساكن فراغ UNIQ_T(wrapper_name, uq)(فراغ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_کلیل_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), عدم, scope)

#define DEFINE_HASH_OPS_WITH_بەها_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_بەها, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_بەها, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         عدم, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_کلیل_func, type_بەها, free_بەها_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_کلیل_wrapper, free_کلیل_func); \
        _DEFINE_FREE_FUNC(uq, type_بەها, static_free_بەها_wrapper, free_بەها_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_کلیل_wrapper, uq),           \
                         UNIQ_T(static_free_بەها_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, عدم, عدم, ساكن)

#define DEFINE_HASH_OPS_WITH_کلیل_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلیل_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_کلیل_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلیل_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ساكن)

#define DEFINE_HASH_OPS_WITH_بەها_DESTRUCTOR(name, type, hash_func, compare_func, بەها_type, free_func) \
        DEFINE_HASH_OPS_WITH_بەها_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, بەها_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_بەها_DESTRUCTOR(name, type, hash_func, compare_func, بەها_type, free_func) \
        DEFINE_HASH_OPS_WITH_بەها_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, بەها_type, free_func, ساكن)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_کلیل_func, بەها_type, free_بەها_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_کلیل_func, بەها_type, free_بەها_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_کلیل_func, بەها_type, free_بەها_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_کلیل_func, بەها_type, free_بەها_func, ساكن)

فراغ string_hash_func(ثابت حرف *p, بنية siphash *state);
#define string_compare_func strcmp
خارجي ثابت بنية hash_ops string_hash_ops;
خارجي ثابت بنية hash_ops string_hash_ops_free;
خارجي ثابت بنية hash_ops string_hash_ops_بەها_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_free;
خارجي ثابت بنية hash_ops string_hash_ops_free_strv_free;

فراغ path_hash_func(ثابت حرف *p, بنية siphash *state);
خارجي ثابت بنية hash_ops path_hash_ops;
خارجي ثابت بنية hash_ops path_hash_ops_free;
خارجي ثابت بنية hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
فراغ trivial_hash_func(ثابت فراغ *p, بنية siphash *state);
صحيح trivial_compare_func(ثابت فراغ *a, ثابت فراغ *b) _const_;
خارجي ثابت بنية hash_ops trivial_hash_ops;
خارجي ثابت بنية hash_ops trivial_hash_ops_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_بەها_free;
خارجي ثابت بنية hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
فراغ uint64_hash_func(ثابت uint64_t *p, بنية siphash *state);
صحيح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) _pure_;
خارجي ثابت بنية hash_ops uint64_hash_ops;
خارجي ثابت بنية hash_ops uint64_hash_ops_بەها_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#إذا SIZEOF_DEV_T != 8
فراغ devt_hash_func(ثابت dev_t *p, بنية siphash *state);
#وإلا
#define devt_hash_func uint64_hash_func
#endif

صحيح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) _pure_;
خارجي ثابت بنية hash_ops devt_hash_ops;
