/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*hash_func_t)(ಧ್ರುವ ಶೂನ್ಯ *p, ಸಂರಚನಾ siphash *state);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*compare_func_t)(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b);

ಸಂರಚನಾ hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ಕೀಲಿ;
        free_func_t free_ಮೌಲ್ಯ;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, free_ಮೌಲ್ಯ_func, scope) \
        _unused_ ಸ್ಥಿರ ಶೂನ್ಯ (* UNIQ_T(static_hash_wrapper, uq))(ಧ್ರುವ type *, ಸಂರಚನಾ siphash *) = hash_func; \
        _unused_ ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (* UNIQ_T(static_compare_wrapper, uq))(ಧ್ರುವ type *, ಧ್ರುವ type *) = compare_func; \
        scope ಧ್ರುವ ಸಂರಚನಾ hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ಕೀಲಿ = free_ಕೀಲಿ_func,                              \
                .free_ಮೌಲ್ಯ = free_ಮೌಲ್ಯ_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ಸ್ಥಿರ ಶೂನ್ಯ UNIQ_T(wrapper_name, uq)(ಶೂನ್ಯ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ರಿಕ್ತ, scope)

#define DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_ಮೌಲ್ಯ, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_ಮೌಲ್ಯ, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ರಿಕ್ತ, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, type_ಮೌಲ್ಯ, free_ಮೌಲ್ಯ_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ಕೀಲಿ_wrapper, free_ಕೀಲಿ_func); \
        _DEFINE_FREE_FUNC(uq, type_ಮೌಲ್ಯ, static_free_ಮೌಲ್ಯ_wrapper, free_ಮೌಲ್ಯ_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ಕೀಲಿ_wrapper, uq),           \
                         UNIQ_T(static_free_ಮೌಲ್ಯ_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ರಿಕ್ತ, ರಿಕ್ತ,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ರಿಕ್ತ, ರಿಕ್ತ, ಸ್ಥಿರ)

#define DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ಸ್ಥಿರ)

#define DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func, ಸ್ಥಿರ)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func, ಸ್ಥಿರ)

ಶೂನ್ಯ string_hash_func(ಧ್ರುವ ವರ್ಣ *p, ಸಂರಚನಾ siphash *state);
#define string_compare_func strcmp
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops string_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops string_hash_ops_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops string_hash_ops_ಮೌಲ್ಯ_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops string_hash_ops_free_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops string_hash_ops_free_strv_free;

ಶೂನ್ಯ path_hash_func(ಧ್ರುವ ವರ್ಣ *p, ಸಂರಚನಾ siphash *state);
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops path_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops path_hash_ops_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
ಶೂನ್ಯ trivial_hash_func(ಧ್ರುವ ಶೂನ್ಯ *p, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ trivial_compare_func(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b) _const_;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops trivial_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops trivial_hash_ops_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops trivial_hash_ops_ಮೌಲ್ಯ_free;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
ಶೂನ್ಯ uint64_hash_func(ಧ್ರುವ uint64_t *p, ಸಂರಚನಾ siphash *state);
ಪೂರ್ಣಾಙ್ಕ uint64_compare_func(ಧ್ರುವ uint64_t *a, ಧ್ರುವ uint64_t *b) _pure_;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops uint64_hash_ops;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops uint64_hash_ops_ಮೌಲ್ಯ_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#ಯದಿ SIZEOF_DEV_T != 8
ಶೂನ್ಯ devt_hash_func(ಧ್ರುವ dev_t *p, ಸಂರಚನಾ siphash *state);
#ಅನ್ಯಥಾ
#define devt_hash_func uint64_hash_func
#endif

ಪೂರ್ಣಾಙ್ಕ devt_compare_func(ಧ್ರುವ dev_t *a, ಧ್ರುವ dev_t *b) _pure_;
ಬಾಹ್ಯ ಧ್ರುವ ಸಂರಚನಾ hash_ops devt_hash_ops;
