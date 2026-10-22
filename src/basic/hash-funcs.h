/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या शून्य (*hash_func_t)(स्थिरांक शून्य *p, संरचना siphash *state);
प्रकारव्याख्या पूर्णांक (*compare_func_t)(स्थिरांक शून्य *a, स्थिरांक शून्य *b);

संरचना hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_किल्ली;
        free_func_t free_मूल्य;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_किल्ली_func, free_मूल्य_func, scope) \
        _unused_ स्थिर शून्य (* UNIQ_T(static_hash_wrapper, uq))(स्थिरांक type *, संरचना siphash *) = hash_func; \
        _unused_ स्थिर पूर्णांक (* UNIQ_T(static_compare_wrapper, uq))(स्थिरांक type *, स्थिरांक type *) = compare_func; \
        scope स्थिरांक संरचना hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_किल्ली = free_किल्ली_func,                              \
                .free_मूल्य = free_मूल्य_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        स्थिर शून्य UNIQ_T(wrapper_name, uq)(शून्य *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), रिक्त, scope)

#define DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_मूल्य, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_मूल्य, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         रिक्त, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_किल्ली_func, type_मूल्य, free_मूल्य_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_किल्ली_wrapper, free_किल्ली_func); \
        _DEFINE_FREE_FUNC(uq, type_मूल्य, static_free_मूल्य_wrapper, free_मूल्य_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_किल्ली_wrapper, uq),           \
                         UNIQ_T(static_free_मूल्य_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, रिक्त, रिक्त,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, रिक्त, रिक्त, स्थिर)

#define DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_किल्ली_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_किल्ली_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, स्थिर)

#define DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(name, type, hash_func, compare_func, मूल्य_type, free_func) \
        DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, मूल्य_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(name, type, hash_func, compare_func, मूल्य_type, free_func) \
        DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, मूल्य_type, free_func, स्थिर)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_किल्ली_func, मूल्य_type, free_मूल्य_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_किल्ली_func, मूल्य_type, free_मूल्य_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_किल्ली_func, मूल्य_type, free_मूल्य_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_किल्ली_func, मूल्य_type, free_मूल्य_func, स्थिर)

शून्य string_hash_func(स्थिरांक अक्षर *p, संरचना siphash *state);
#define string_compare_func म_भेद
बाह्य स्थिरांक संरचना hash_ops string_hash_ops;
बाह्य स्थिरांक संरचना hash_ops string_hash_ops_free;
बाह्य स्थिरांक संरचना hash_ops string_hash_ops_मूल्य_free;
बाह्य स्थिरांक संरचना hash_ops string_hash_ops_free_free;
बाह्य स्थिरांक संरचना hash_ops string_hash_ops_free_strv_free;

शून्य path_hash_func(स्थिरांक अक्षर *p, संरचना siphash *state);
बाह्य स्थिरांक संरचना hash_ops path_hash_ops;
बाह्य स्थिरांक संरचना hash_ops path_hash_ops_free;
बाह्य स्थिरांक संरचना hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
शून्य trivial_hash_func(स्थिरांक शून्य *p, संरचना siphash *state);
पूर्णांक trivial_compare_func(स्थिरांक शून्य *a, स्थिरांक शून्य *b) _const_;
बाह्य स्थिरांक संरचना hash_ops trivial_hash_ops;
बाह्य स्थिरांक संरचना hash_ops trivial_hash_ops_free;
बाह्य स्थिरांक संरचना hash_ops trivial_hash_ops_मूल्य_free;
बाह्य स्थिरांक संरचना hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
शून्य uint64_hash_func(स्थिरांक uint64_t *p, संरचना siphash *state);
पूर्णांक uint64_compare_func(स्थिरांक uint64_t *a, स्थिरांक uint64_t *b) _pure_;
बाह्य स्थिरांक संरचना hash_ops uint64_hash_ops;
बाह्य स्थिरांक संरचना hash_ops uint64_hash_ops_मूल्य_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#जर SIZEOF_DEV_T != 8
शून्य devt_hash_func(स्थिरांक dev_t *p, संरचना siphash *state);
#अन्यथा
#define devt_hash_func uint64_hash_func
#endif

पूर्णांक devt_compare_func(स्थिरांक dev_t *a, स्थिरांक dev_t *b) _pure_;
बाह्य स्थिरांक संरचना hash_ops devt_hash_ops;
