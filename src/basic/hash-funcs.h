/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*hash_func_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, ರಚನೆ siphash *state);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*compare_func_t)(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b);

ರಚನೆ hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ಕೀಲಿ;
        free_func_t free_ಮೌಲ್ಯ;
};

#ವ್ಯಾಖ್ಯಾನಿಸು _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, free_ಮೌಲ್ಯ_func, scope) \
        ಬಳಸದ ಸ್ಥಿರ ಶೂನ್ಯ (* UNIQ_T(static_hash_wrapper, uq))(ಸ್ಥಿರಾಂಕ type *, ರಚನೆ siphash *) = hash_func; \
        ಬಳಸದ ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (* UNIQ_T(static_compare_wrapper, uq))(ಸ್ಥಿರಾಂಕ type *, ಸ್ಥಿರಾಂಕ type *) = compare_func; \
        scope ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ಕೀಲಿ = free_ಕೀಲಿ_func,                              \
                .free_ಮೌಲ್ಯ = free_ಮೌಲ್ಯ_func,                          \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ಸ್ಥಿರ ಶೂನ್ಯ UNIQ_T(wrapper_name, uq)(ಶೂನ್ಯ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ಖಾಲಿ, scope)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_ಮೌಲ್ಯ, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_ಮೌಲ್ಯ, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ಖಾಲಿ, UNIQ_T(static_free_wrapper, uq), scope)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_ತುಂಬಿದ_2(uq, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, type_ಮೌಲ್ಯ, free_ಮೌಲ್ಯ_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ಕೀಲಿ_wrapper, free_ಕೀಲಿ_func); \
        _DEFINE_FREE_FUNC(uq, type_ಮೌಲ್ಯ, static_free_ಮೌಲ್ಯ_wrapper, free_ಮೌಲ್ಯ_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ಕೀಲಿ_wrapper, uq),           \
                         UNIQ_T(static_free_ಮೌಲ್ಯ_wrapper, uq), scope)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ಖಾಲಿ, ಖಾಲಿ,)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ಖಾಲಿ, ಖಾಲಿ, ಸ್ಥಿರ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ಕೀಲಿ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ಸ್ಥಿರ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func,)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR(name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ಮೌಲ್ಯ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ಮೌಲ್ಯ_type, free_func, ಸ್ಥಿರ)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_HASH_OPS_ತುಂಬಿದ(name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func) \
        DEFINE_HASH_OPS_ತುಂಬಿದ_2(UNIQ, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func,)

#ವ್ಯಾಖ್ಯಾನಿಸು DEFINE_ಖಾಸಗಿ_HASH_OPS_ತುಂಬಿದ(name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func) \
        DEFINE_HASH_OPS_ತುಂಬಿದ_2(UNIQ, name, type, hash_func, compare_func, free_ಕೀಲಿ_func, ಮೌಲ್ಯ_type, free_ಮೌಲ್ಯ_func, ಸ್ಥಿರ)

ಶೂನ್ಯ string_hash_func(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ರಚನೆ siphash *state);
#ವ್ಯಾಖ್ಯಾನಿಸು string_compare_func strcmp
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops string_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops string_hash_ops_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops string_hash_ops_ಮೌಲ್ಯ_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops string_hash_ops_free_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops string_hash_ops_free_strv_free;

ಶೂನ್ಯ ಮಾರ್ಗ_hash_func(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, ರಚನೆ siphash *state);
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ಮಾರ್ಗ_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ಮಾರ್ಗ_hash_ops_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops ಮಾರ್ಗ_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
ಶೂನ್ಯ trivial_hash_func(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ trivial_compare_func(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b) _const_;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops trivial_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops trivial_hash_ops_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops trivial_hash_ops_ಮೌಲ್ಯ_free;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
ಶೂನ್ಯ uint64_hash_func(ಸ್ಥಿರಾಂಕ uint64_t *p, ರಚನೆ siphash *state);
ಪೂರ್ಣಾಂಕ uint64_compare_func(ಸ್ಥಿರಾಂಕ uint64_t *a, ಸ್ಥಿರಾಂಕ uint64_t *b) ಶುದ್ಧ;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops uint64_hash_ops;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops uint64_hash_ops_ಮೌಲ್ಯ_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#ಆದರೆ SIZEOF_DEV_T != 8
ಶೂನ್ಯ devt_hash_func(ಸ್ಥಿರಾಂಕ dev_t *p, ರಚನೆ siphash *state);
#ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿಸು devt_hash_func uint64_hash_func
#ಆದರೆ_ಅಂತ್ಯ

ಪೂರ್ಣಾಂಕ devt_compare_func(ಸ್ಥಿರಾಂಕ dev_t *a, ಸ್ಥಿರಾಂಕ dev_t *b) ಶುದ್ಧ;
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ hash_ops devt_hash_ops;
