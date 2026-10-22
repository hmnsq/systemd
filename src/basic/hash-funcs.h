/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ শূন্য (*hash_func_t)(ধ্রুবক শূন্য *p, কাঠামো siphash *state);
প্রকারলেখ পূর্ণসংখ্যা (*compare_func_t)(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b);

কাঠামো hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_চাবি;
        free_func_t free_মান;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_চাবি_func, free_মান_func, scope) \
        _unused_ স্থির শূন্য (* UNIQ_T(static_hash_wrapper, uq))(ধ্রুবক type *, কাঠামো siphash *) = hash_func; \
        _unused_ স্থির পূর্ণসংখ্যা (* UNIQ_T(static_compare_wrapper, uq))(ধ্রুবক type *, ধ্রুবক type *) = compare_func; \
        scope ধ্রুবক কাঠামো hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_চাবি = free_চাবি_func,                              \
                .free_মান = free_মান_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        স্থির শূন্য UNIQ_T(wrapper_name, uq)(শূন্য *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ফাঁকা, scope)

#define DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_মান, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_মান, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ফাঁকা, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_চাবি_func, type_মান, free_মান_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_চাবি_wrapper, free_চাবি_func); \
        _DEFINE_FREE_FUNC(uq, type_মান, static_free_মান_wrapper, free_মান_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_চাবি_wrapper, uq),           \
                         UNIQ_T(static_free_মান_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ফাঁকা, ফাঁকা,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ফাঁকা, ফাঁকা, স্থির)

#define DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_চাবি_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_চাবি_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, স্থির)

#define DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR(name, type, hash_func, compare_func, মান_type, free_func) \
        DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, মান_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_মান_DESTRUCTOR(name, type, hash_func, compare_func, মান_type, free_func) \
        DEFINE_HASH_OPS_WITH_মান_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, মান_type, free_func, স্থির)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_চাবি_func, মান_type, free_মান_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_চাবি_func, মান_type, free_মান_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_চাবি_func, মান_type, free_মান_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_চাবি_func, মান_type, free_মান_func, স্থির)

শূন্য string_hash_func(ধ্রুবক অক্ষর *p, কাঠামো siphash *state);
#define string_compare_func strcmp
বাহ্যিক ধ্রুবক কাঠামো hash_ops string_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops string_hash_ops_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops string_hash_ops_মান_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops string_hash_ops_free_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops string_hash_ops_free_strv_free;

শূন্য path_hash_func(ধ্রুবক অক্ষর *p, কাঠামো siphash *state);
বাহ্যিক ধ্রুবক কাঠামো hash_ops path_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops path_hash_ops_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
শূন্য trivial_hash_func(ধ্রুবক শূন্য *p, কাঠামো siphash *state);
পূর্ণসংখ্যা trivial_compare_func(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b) _const_;
বাহ্যিক ধ্রুবক কাঠামো hash_ops trivial_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops trivial_hash_ops_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops trivial_hash_ops_মান_free;
বাহ্যিক ধ্রুবক কাঠামো hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
শূন্য uint64_hash_func(ধ্রুবক uint64_t *p, কাঠামো siphash *state);
পূর্ণসংখ্যা uint64_compare_func(ধ্রুবক uint64_t *a, ধ্রুবক uint64_t *b) _pure_;
বাহ্যিক ধ্রুবক কাঠামো hash_ops uint64_hash_ops;
বাহ্যিক ধ্রুবক কাঠামো hash_ops uint64_hash_ops_মান_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#যদি SIZEOF_DEV_T != 8
শূন্য devt_hash_func(ধ্রুবক dev_t *p, কাঠামো siphash *state);
#অন্যথা
#define devt_hash_func uint64_hash_func
#endif

পূর্ণসংখ্যা devt_compare_func(ধ্রুবক dev_t *a, ধ্রুবক dev_t *b) _pure_;
বাহ্যিক ধ্রুবক কাঠামো hash_ops devt_hash_ops;
