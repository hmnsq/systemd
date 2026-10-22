/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*hash_func_t)(ଅଚଳ ଶୂନ୍ୟ *p, ସଂରଚନା siphash *state);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*compare_func_t)(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b);

ସଂରଚନା hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_ଚାବି;
        free_func_t free_ମୂଲ୍ୟ;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_ଚାବି_func, free_ମୂଲ୍ୟ_func, scope) \
        _unused_ ସ୍ଥିର ଶୂନ୍ୟ (* UNIQ_T(static_hash_wrapper, uq))(ଅଚଳ type *, ସଂରଚନା siphash *) = hash_func; \
        _unused_ ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (* UNIQ_T(static_compare_wrapper, uq))(ଅଚଳ type *, ଅଚଳ type *) = compare_func; \
        scope ଅଚଳ ସଂରଚନା hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_ଚାବି = free_ଚାବି_func,                              \
                .free_ମୂଲ୍ୟ = free_ମୂଲ୍ୟ_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ସ୍ଥିର ଶୂନ୍ୟ UNIQ_T(wrapper_name, uq)(ଶୂନ୍ୟ *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ଖାଲି, scope)

#define DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_ମୂଲ୍ୟ, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_ମୂଲ୍ୟ, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ଖାଲି, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_ଚାବି_func, type_ମୂଲ୍ୟ, free_ମୂଲ୍ୟ_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_ଚାବି_wrapper, free_ଚାବି_func); \
        _DEFINE_FREE_FUNC(uq, type_ମୂଲ୍ୟ, static_free_ମୂଲ୍ୟ_wrapper, free_ମୂଲ୍ୟ_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_ଚାବି_wrapper, uq),           \
                         UNIQ_T(static_free_ମୂଲ୍ୟ_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ଖାଲି, ଖାଲି,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ଖାଲି, ଖାଲି, ସ୍ଥିର)

#define DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_ଚାବି_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ସ୍ଥିର)

#define DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(name, type, hash_func, compare_func, ମୂଲ୍ୟ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ମୂଲ୍ୟ_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR(name, type, hash_func, compare_func, ମୂଲ୍ୟ_type, free_func) \
        DEFINE_HASH_OPS_WITH_ମୂଲ୍ୟ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, ମୂଲ୍ୟ_type, free_func, ସ୍ଥିର)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ଚାବି_func, ମୂଲ୍ୟ_type, free_ମୂଲ୍ୟ_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ଚାବି_func, ମୂଲ୍ୟ_type, free_ମୂଲ୍ୟ_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_ଚାବି_func, ମୂଲ୍ୟ_type, free_ମୂଲ୍ୟ_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_ଚାବି_func, ମୂଲ୍ୟ_type, free_ମୂଲ୍ୟ_func, ସ୍ଥିର)

ଶୂନ୍ୟ string_hash_func(ଅଚଳ ଅକ୍ଷର *p, ସଂରଚନା siphash *state);
#define string_compare_func strcmp
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops string_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops string_hash_ops_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops string_hash_ops_ମୂଲ୍ୟ_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops string_hash_ops_free_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops string_hash_ops_free_strv_free;

ଶୂନ୍ୟ path_hash_func(ଅଚଳ ଅକ୍ଷର *p, ସଂରଚନା siphash *state);
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops path_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops path_hash_ops_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
ଶୂନ୍ୟ trivial_hash_func(ଅଚଳ ଶୂନ୍ୟ *p, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା trivial_compare_func(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b) _const_;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops trivial_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops trivial_hash_ops_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops trivial_hash_ops_ମୂଲ୍ୟ_free;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
ଶୂନ୍ୟ uint64_hash_func(ଅଚଳ uint64_t *p, ସଂରଚନା siphash *state);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uint64_compare_func(ଅଚଳ uint64_t *a, ଅଚଳ uint64_t *b) _pure_;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops uint64_hash_ops;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops uint64_hash_ops_ମୂଲ୍ୟ_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#ଯଦି SIZEOF_DEV_T != 8
ଶୂନ୍ୟ devt_hash_func(ଅଚଳ dev_t *p, ସଂରଚନା siphash *state);
#ଅନ୍ୟଥା
#define devt_hash_func uint64_hash_func
#endif

ପୂର୍ଣ୍ଣସଂଖ୍ୟା devt_compare_func(ଅଚଳ dev_t *a, ଅଚଳ dev_t *b) _pure_;
ବାହ୍ୟ ଅଚଳ ସଂରଚନା hash_ops devt_hash_ops;
