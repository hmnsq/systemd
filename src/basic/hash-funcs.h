/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం శూన్య (*hash_func_t)(స్థిరరాశి శూన్య *p, నిర్మాణం siphash *state);
రకనిర్వచనం పూర్ణాంకం (*compare_func_t)(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b);

నిర్మాణం hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_కీలకం;
        free_func_t free_విలువ;
};

#నిర్వచించు _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_కీలకం_func, free_విలువ_func, scope) \
        వాడని స్థిర శూన్య (* UNIQ_T(static_hash_wrapper, uq))(స్థిరరాశి type *, నిర్మాణం siphash *) = hash_func; \
        వాడని స్థిర పూర్ణాంకం (* UNIQ_T(static_compare_wrapper, uq))(స్థిరరాశి type *, స్థిరరాశి type *) = compare_func; \
        scope స్థిరరాశి నిర్మాణం hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_కీలకం = free_కీలకం_func,                              \
                .free_విలువ = free_విలువ_func,                          \
        }

#నిర్వచించు _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        స్థిర శూన్య UNIQ_T(wrapper_name, uq)(శూన్య *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#నిర్వచించు DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ఖాళీ, scope)

#నిర్వచించు DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_విలువ, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_విలువ, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ఖాళీ, UNIQ_T(static_free_wrapper, uq), scope)

#నిర్వచించు DEFINE_HASH_OPS_నిండిన_2(uq, name, type, hash_func, compare_func, free_కీలకం_func, type_విలువ, free_విలువ_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_కీలకం_wrapper, free_కీలకం_func); \
        _DEFINE_FREE_FUNC(uq, type_విలువ, static_free_విలువ_wrapper, free_విలువ_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_కీలకం_wrapper, uq),           \
                         UNIQ_T(static_free_విలువ_wrapper, uq), scope)

#నిర్వచించు DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ఖాళీ, ఖాళీ,)

#నిర్వచించు DEFINE_వ్యక్తిగత_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ఖాళీ, ఖాళీ, స్థిర)

#నిర్వచించు DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#నిర్వచించు DEFINE_వ్యక్తిగత_HASH_OPS_WITH_కీలకం_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_కీలకం_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, స్థిర)

#నిర్వచించు DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR(name, type, hash_func, compare_func, విలువ_type, free_func) \
        DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, విలువ_type, free_func,)

#నిర్వచించు DEFINE_వ్యక్తిగత_HASH_OPS_WITH_విలువ_DESTRUCTOR(name, type, hash_func, compare_func, విలువ_type, free_func) \
        DEFINE_HASH_OPS_WITH_విలువ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, విలువ_type, free_func, స్థిర)

#నిర్వచించు DEFINE_HASH_OPS_నిండిన(name, type, hash_func, compare_func, free_కీలకం_func, విలువ_type, free_విలువ_func) \
        DEFINE_HASH_OPS_నిండిన_2(UNIQ, name, type, hash_func, compare_func, free_కీలకం_func, విలువ_type, free_విలువ_func,)

#నిర్వచించు DEFINE_వ్యక్తిగత_HASH_OPS_నిండిన(name, type, hash_func, compare_func, free_కీలకం_func, విలువ_type, free_విలువ_func) \
        DEFINE_HASH_OPS_నిండిన_2(UNIQ, name, type, hash_func, compare_func, free_కీలకం_func, విలువ_type, free_విలువ_func, స్థిర)

శూన్య string_hash_func(స్థిరరాశి అక్షరం *p, నిర్మాణం siphash *state);
#నిర్వచించు string_compare_func strcmp
బాహ్య స్థిరరాశి నిర్మాణం hash_ops string_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops string_hash_ops_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops string_hash_ops_విలువ_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops string_hash_ops_free_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops string_hash_ops_free_strv_free;

శూన్య మార్గం_hash_func(స్థిరరాశి అక్షరం *p, నిర్మాణం siphash *state);
బాహ్య స్థిరరాశి నిర్మాణం hash_ops మార్గం_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops మార్గం_hash_ops_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops మార్గం_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
శూన్య trivial_hash_func(స్థిరరాశి శూన్య *p, నిర్మాణం siphash *state);
పూర్ణాంకం trivial_compare_func(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b) _const_;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops trivial_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops trivial_hash_ops_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops trivial_hash_ops_విలువ_free;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
శూన్య uint64_hash_func(స్థిరరాశి uint64_t *p, నిర్మాణం siphash *state);
పూర్ణాంకం uint64_compare_func(స్థిరరాశి uint64_t *a, స్థిరరాశి uint64_t *b) శుద్ధ;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops uint64_hash_ops;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops uint64_hash_ops_విలువ_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#ఒకవేళ SIZEOF_DEV_T != 8
శూన్య devt_hash_func(స్థిరరాశి dev_t *p, నిర్మాణం siphash *state);
#లేకపోతే
#నిర్వచించు devt_hash_func uint64_hash_func
#అయితే_ముగింపు

పూర్ణాంకం devt_compare_func(స్థిరరాశి dev_t *a, స్థిరరాశి dev_t *b) శుద్ధ;
బాహ్య స్థిరరాశి నిర్మాణం hash_ops devt_hash_ops;
