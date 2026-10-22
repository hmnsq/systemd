/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع تهی (*hash_func_t)(ثابت تهی *p, ساختار siphash *state);
تعریفنوع صحیح (*compare_func_t)(ثابت تهی *a, ثابت تهی *b);

ساختار hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_کلید;
        free_func_t free_مقدار;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_کلید_func, free_مقدار_func, scope) \
        _unused_ ایستا تهی (* UNIQ_T(static_hash_wrapper, uq))(ثابت type *, ساختار siphash *) = hash_func; \
        _unused_ ایستا صحیح (* UNIQ_T(static_compare_wrapper, uq))(ثابت type *, ثابت type *) = compare_func; \
        scope ثابت ساختار hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_کلید = free_کلید_func,                              \
                .free_مقدار = free_مقدار_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ایستا تهی UNIQ_T(wrapper_name, uq)(تهی *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), پوچ, scope)

#define DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_مقدار, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_مقدار, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         پوچ, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_کلید_func, type_مقدار, free_مقدار_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_کلید_wrapper, free_کلید_func); \
        _DEFINE_FREE_FUNC(uq, type_مقدار, static_free_مقدار_wrapper, free_مقدار_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_کلید_wrapper, uq),           \
                         UNIQ_T(static_free_مقدار_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, پوچ, پوچ,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, پوچ, پوچ, ایستا)

#define DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_کلید_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ایستا)

#define DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR(name, type, hash_func, compare_func, مقدار_type, free_func) \
        DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, مقدار_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_مقدار_DESTRUCTOR(name, type, hash_func, compare_func, مقدار_type, free_func) \
        DEFINE_HASH_OPS_WITH_مقدار_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, مقدار_type, free_func, ایستا)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_کلید_func, مقدار_type, free_مقدار_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_کلید_func, مقدار_type, free_مقدار_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_کلید_func, مقدار_type, free_مقدار_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_کلید_func, مقدار_type, free_مقدار_func, ایستا)

تهی string_hash_func(ثابت نویسه *p, ساختار siphash *state);
#define string_compare_func strcmp
بیرونی ثابت ساختار hash_ops string_hash_ops;
بیرونی ثابت ساختار hash_ops string_hash_ops_free;
بیرونی ثابت ساختار hash_ops string_hash_ops_مقدار_free;
بیرونی ثابت ساختار hash_ops string_hash_ops_free_free;
بیرونی ثابت ساختار hash_ops string_hash_ops_free_strv_free;

تهی path_hash_func(ثابت نویسه *p, ساختار siphash *state);
بیرونی ثابت ساختار hash_ops path_hash_ops;
بیرونی ثابت ساختار hash_ops path_hash_ops_free;
بیرونی ثابت ساختار hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
تهی trivial_hash_func(ثابت تهی *p, ساختار siphash *state);
صحیح trivial_compare_func(ثابت تهی *a, ثابت تهی *b) _const_;
بیرونی ثابت ساختار hash_ops trivial_hash_ops;
بیرونی ثابت ساختار hash_ops trivial_hash_ops_free;
بیرونی ثابت ساختار hash_ops trivial_hash_ops_مقدار_free;
بیرونی ثابت ساختار hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
تهی uint64_hash_func(ثابت uint64_t *p, ساختار siphash *state);
صحیح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) _pure_;
بیرونی ثابت ساختار hash_ops uint64_hash_ops;
بیرونی ثابت ساختار hash_ops uint64_hash_ops_مقدار_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#اگر SIZEOF_DEV_T != 8
تهی devt_hash_func(ثابت dev_t *p, ساختار siphash *state);
#وگرنه
#define devt_hash_func uint64_hash_func
#endif

صحیح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) _pure_;
بیرونی ثابت ساختار hash_ops devt_hash_ops;
