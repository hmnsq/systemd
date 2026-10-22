/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*hash_func_t)(ਅਚਲ ਖਾਲੀ *p, ਬਣਤਰ siphash *state);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*compare_func_t)(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b);

ਬਣਤਰ hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ਕੁੰਜੀ;
        free_func_t free_ਮੁੱਲ;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, free_ਮੁੱਲ_func, scope) \
        ਅਣਵਰਤਿਆ ਸਥਿਰ ਖਾਲੀ (* UNIQ_T(static_hash_wrapper, uq))(ਅਚਲ type *, ਬਣਤਰ siphash *) = hash_func; \
        ਅਣਵਰਤਿਆ ਸਥਿਰ ਪੂਰਨ (* UNIQ_T(static_compare_wrapper, uq))(ਅਚਲ type *, ਅਚਲ type *) = compare_func; \
        scope ਅਚਲ ਬਣਤਰ hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ਕੁੰਜੀ = free_ਕੁੰਜੀ_func,                              \
                .free_ਮੁੱਲ = free_ਮੁੱਲ_func,                          \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ਸਥਿਰ ਖਾਲੀ UNIQ_T(wrapper_name, uq)(ਖਾਲੀ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ਨਿਰਾ, scope)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_ਮੁੱਲ, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_ਮੁੱਲ, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ਨਿਰਾ, UNIQ_T(static_free_wrapper, uq), scope)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_ਭਰਿਆ_2(uq, name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, type_ਮੁੱਲ, free_ਮੁੱਲ_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ਕੁੰਜੀ_wrapper, free_ਕੁੰਜੀ_func); \
        _DEFINE_FREE_FUNC(uq, type_ਮੁੱਲ, static_free_ਮੁੱਲ_wrapper, free_ਮੁੱਲ_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ਕੁੰਜੀ_wrapper, uq),           \
                         UNIQ_T(static_free_ਮੁੱਲ_wrapper, uq), scope)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ਨਿਰਾ, ਨਿਰਾ,)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ਨਿਰਾ, ਨਿਰਾ, ਸਥਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ਕੁੰਜੀ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ਸਥਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(name, type, hash_func, compare_func, ਮੁੱਲ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ਮੁੱਲ_type, free_func,)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR(name, type, hash_func, compare_func, ਮੁੱਲ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ਮੁੱਲ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ਮੁੱਲ_type, free_func, ਸਥਿਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_HASH_OPS_ਭਰਿਆ(name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, ਮੁੱਲ_type, free_ਮੁੱਲ_func) \
        DEFINE_HASH_OPS_ਭਰਿਆ_2(UNIQ, name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, ਮੁੱਲ_type, free_ਮੁੱਲ_func,)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_HASH_OPS_ਭਰਿਆ(name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, ਮੁੱਲ_type, free_ਮੁੱਲ_func) \
        DEFINE_HASH_OPS_ਭਰਿਆ_2(UNIQ, name, type, hash_func, compare_func, free_ਕੁੰਜੀ_func, ਮੁੱਲ_type, free_ਮੁੱਲ_func, ਸਥਿਰ)

ਖਾਲੀ string_hash_func(ਅਚਲ ਅੱਖਰ *p, ਬਣਤਰ siphash *state);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ string_compare_func strcmp
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops string_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops string_hash_ops_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops string_hash_ops_ਮੁੱਲ_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops string_hash_ops_free_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops string_hash_ops_free_strv_free;

ਖਾਲੀ ਰਸਤਾ_hash_func(ਅਚਲ ਅੱਖਰ *p, ਬਣਤਰ siphash *state);
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ਰਸਤਾ_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ਰਸਤਾ_hash_ops_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops ਰਸਤਾ_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
ਖਾਲੀ trivial_hash_func(ਅਚਲ ਖਾਲੀ *p, ਬਣਤਰ siphash *state);
ਪੂਰਨ trivial_compare_func(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b) _const_;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops trivial_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops trivial_hash_ops_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops trivial_hash_ops_ਮੁੱਲ_free;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
ਖਾਲੀ uint64_hash_func(ਅਚਲ uint64_t *p, ਬਣਤਰ siphash *state);
ਪੂਰਨ uint64_compare_func(ਅਚਲ uint64_t *a, ਅਚਲ uint64_t *b) ਸ਼ੁੱਧ;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops uint64_hash_ops;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops uint64_hash_ops_ਮੁੱਲ_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#ਜੇ SIZEOF_DEV_T != 8
ਖਾਲੀ devt_hash_func(ਅਚਲ dev_t *p, ਬਣਤਰ siphash *state);
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ devt_hash_func uint64_hash_func
#ਜੇ_ਅੰਤ

ਪੂਰਨ devt_compare_func(ਅਚਲ dev_t *a, ਅਚਲ dev_t *b) ਸ਼ੁੱਧ;
ਬਾਹਰੀ ਅਚਲ ਬਣਤਰ hash_ops devt_hash_ops;
