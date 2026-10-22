/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા શૂન્ય (*hash_func_t)(અચલ શૂન્ય *p, સંરચના siphash *state);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*compare_func_t)(અચલ શૂન્ય *a, અચલ શૂન્ય *b);

સંરચના hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ચાવી;
        free_func_t free_કિંમત;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ચાવી_func, free_કિંમત_func, scope) \
        _unused_ સ્થિર શૂન્ય (* UNIQ_T(static_hash_wrapper, uq))(અચલ type *, સંરચના siphash *) = hash_func; \
        _unused_ સ્થિર પૂર્ણાંક (* UNIQ_T(static_compare_wrapper, uq))(અચલ type *, અચલ type *) = compare_func; \
        scope અચલ સંરચના hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ચાવી = free_ચાવી_func,                              \
                .free_કિંમત = free_કિંમત_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        સ્થિર શૂન્ય UNIQ_T(wrapper_name, uq)(શૂન્ય *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ખાલી, scope)

#define DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_કિંમત, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_કિંમત, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ખાલી, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_ચાવી_func, type_કિંમત, free_કિંમત_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ચાવી_wrapper, free_ચાવી_func); \
        _DEFINE_FREE_FUNC(uq, type_કિંમત, static_free_કિંમત_wrapper, free_કિંમત_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ચાવી_wrapper, uq),           \
                         UNIQ_T(static_free_કિંમત_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ખાલી, ખાલી,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ખાલી, ખાલી, સ્થિર)

#define DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_ચાવી_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ચાવી_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, સ્થિર)

#define DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(name, type, hash_func, compare_func, કિંમત_type, free_func) \
        DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, કિંમત_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_કિંમત_DESTRUCTOR(name, type, hash_func, compare_func, કિંમત_type, free_func) \
        DEFINE_HASH_OPS_WITH_કિંમત_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, કિંમત_type, free_func, સ્થિર)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ચાવી_func, કિંમત_type, free_કિંમત_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ચાવી_func, કિંમત_type, free_કિંમત_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ચાવી_func, કિંમત_type, free_કિંમત_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ચાવી_func, કિંમત_type, free_કિંમત_func, સ્થિર)

શૂન્ય string_hash_func(અચલ અક્ષર *p, સંરચના siphash *state);
#define string_compare_func strcmp
બાહ્ય અચલ સંરચના hash_ops string_hash_ops;
બાહ્ય અચલ સંરચના hash_ops string_hash_ops_free;
બાહ્ય અચલ સંરચના hash_ops string_hash_ops_કિંમત_free;
બાહ્ય અચલ સંરચના hash_ops string_hash_ops_free_free;
બાહ્ય અચલ સંરચના hash_ops string_hash_ops_free_strv_free;

શૂન્ય path_hash_func(અચલ અક્ષર *p, સંરચના siphash *state);
બાહ્ય અચલ સંરચના hash_ops path_hash_ops;
બાહ્ય અચલ સંરચના hash_ops path_hash_ops_free;
બાહ્ય અચલ સંરચના hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
શૂન્ય trivial_hash_func(અચલ શૂન્ય *p, સંરચના siphash *state);
પૂર્ણાંક trivial_compare_func(અચલ શૂન્ય *a, અચલ શૂન્ય *b) _const_;
બાહ્ય અચલ સંરચના hash_ops trivial_hash_ops;
બાહ્ય અચલ સંરચના hash_ops trivial_hash_ops_free;
બાહ્ય અચલ સંરચના hash_ops trivial_hash_ops_કિંમત_free;
બાહ્ય અચલ સંરચના hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
શૂન્ય uint64_hash_func(અચલ uint64_t *p, સંરચના siphash *state);
પૂર્ણાંક uint64_compare_func(અચલ uint64_t *a, અચલ uint64_t *b) _pure_;
બાહ્ય અચલ સંરચના hash_ops uint64_hash_ops;
બાહ્ય અચલ સંરચના hash_ops uint64_hash_ops_કિંમત_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#જો SIZEOF_DEV_T != 8
શૂન્ય devt_hash_func(અચલ dev_t *p, સંરચના siphash *state);
#નહીંતર
#define devt_hash_func uint64_hash_func
#endif

પૂર્ણાંક devt_compare_func(અચલ dev_t *a, અચલ dev_t *b) _pure_;
બાહ્ય અચલ સંરચના hash_ops devt_hash_ops;
