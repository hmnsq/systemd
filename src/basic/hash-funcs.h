/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை வெறுமை (*hash_func_t)(மாறிலி வெறுமை *p, கட்டமைப்பு siphash *state);
வகைவரை முழு (*compare_func_t)(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b);

கட்டமைப்பு hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_சாவி;
        free_func_t free_மதிப்பு;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_சாவி_func, free_மதிப்பு_func, scope) \
        _unused_ நிலையான வெறுமை (* UNIQ_T(static_hash_wrapper, uq))(மாறிலி type *, கட்டமைப்பு siphash *) = hash_func; \
        _unused_ நிலையான முழு (* UNIQ_T(static_compare_wrapper, uq))(மாறிலி type *, மாறிலி type *) = compare_func; \
        scope மாறிலி கட்டமைப்பு hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_சாவி = free_சாவி_func,                              \
                .free_மதிப்பு = free_மதிப்பு_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        நிலையான வெறுமை UNIQ_T(wrapper_name, uq)(வெறுமை *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), வெற்று, scope)

#define DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_மதிப்பு, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_மதிப்பு, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         வெற்று, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_சாவி_func, type_மதிப்பு, free_மதிப்பு_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_சாவி_wrapper, free_சாவி_func); \
        _DEFINE_FREE_FUNC(uq, type_மதிப்பு, static_free_மதிப்பு_wrapper, free_மதிப்பு_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_சாவி_wrapper, uq),           \
                         UNIQ_T(static_free_மதிப்பு_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, வெற்று, வெற்று,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, வெற்று, வெற்று, நிலையான)

#define DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_சாவி_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_சாவி_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, நிலையான)

#define DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(name, type, hash_func, compare_func, மதிப்பு_type, free_func) \
        DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, மதிப்பு_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR(name, type, hash_func, compare_func, மதிப்பு_type, free_func) \
        DEFINE_HASH_OPS_WITH_மதிப்பு_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, மதிப்பு_type, free_func, நிலையான)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_சாவி_func, மதிப்பு_type, free_மதிப்பு_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_சாவி_func, மதிப்பு_type, free_மதிப்பு_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_சாவி_func, மதிப்பு_type, free_மதிப்பு_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_சாவி_func, மதிப்பு_type, free_மதிப்பு_func, நிலையான)

வெறுமை string_hash_func(மாறிலி எழுத்து *p, கட்டமைப்பு siphash *state);
#define string_compare_func strcmp
வெளி மாறிலி கட்டமைப்பு hash_ops string_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops string_hash_ops_free;
வெளி மாறிலி கட்டமைப்பு hash_ops string_hash_ops_மதிப்பு_free;
வெளி மாறிலி கட்டமைப்பு hash_ops string_hash_ops_free_free;
வெளி மாறிலி கட்டமைப்பு hash_ops string_hash_ops_free_strv_free;

வெறுமை path_hash_func(மாறிலி எழுத்து *p, கட்டமைப்பு siphash *state);
வெளி மாறிலி கட்டமைப்பு hash_ops path_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops path_hash_ops_free;
வெளி மாறிலி கட்டமைப்பு hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
வெறுமை trivial_hash_func(மாறிலி வெறுமை *p, கட்டமைப்பு siphash *state);
முழு trivial_compare_func(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b) _const_;
வெளி மாறிலி கட்டமைப்பு hash_ops trivial_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops trivial_hash_ops_free;
வெளி மாறிலி கட்டமைப்பு hash_ops trivial_hash_ops_மதிப்பு_free;
வெளி மாறிலி கட்டமைப்பு hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
வெறுமை uint64_hash_func(மாறிலி uint64_t *p, கட்டமைப்பு siphash *state);
முழு uint64_compare_func(மாறிலி uint64_t *a, மாறிலி uint64_t *b) _pure_;
வெளி மாறிலி கட்டமைப்பு hash_ops uint64_hash_ops;
வெளி மாறிலி கட்டமைப்பு hash_ops uint64_hash_ops_மதிப்பு_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#எனில் SIZEOF_DEV_T != 8
வெறுமை devt_hash_func(மாறிலி dev_t *p, கட்டமைப்பு siphash *state);
#இல்லையெனில்
#define devt_hash_func uint64_hash_func
#endif

முழு devt_compare_func(மாறிலி dev_t *a, மாறிலி dev_t *b) _pure_;
வெளி மாறிலி கட்டமைப்பு hash_ops devt_hash_ops;
