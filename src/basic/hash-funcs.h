/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार व्योम (*hash_func_t)(स्थिर व्योम *p, काष्ठा siphash *state);
प्रकार पूर्णांक (*compare_func_t)(स्थिर व्योम *a, स्थिर व्योम *b);

काष्ठा hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_कुंजी;
        free_func_t free_मूल्य;
};

#घोषणा _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_कुंजी_func, free_मूल्य_func, scope) \
        _unused_ अटल व्योम (* UNIQ_T(static_hash_wrapper, uq))(स्थिर type *, काष्ठा siphash *) = hash_func; \
        _unused_ अटल पूर्णांक (* UNIQ_T(static_compare_wrapper, uq))(स्थिर type *, स्थिर type *) = compare_func; \
        scope स्थिर काष्ठा hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_कुंजी = free_कुंजी_func,                              \
                .free_मूल्य = free_मूल्य_func,                          \
        }

#घोषणा _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        अटल व्योम UNIQ_T(wrapper_name, uq)(व्योम *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#घोषणा DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), शून्य, scope)

#घोषणा DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_मूल्य, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_मूल्य, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         शून्य, UNIQ_T(static_free_wrapper, uq), scope)

#घोषणा _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_कुंजी_func, type_मूल्य, free_मूल्य_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_कुंजी_wrapper, free_कुंजी_func); \
        _DEFINE_FREE_FUNC(uq, type_मूल्य, static_free_मूल्य_wrapper, free_मूल्य_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_कुंजी_wrapper, uq),           \
                         UNIQ_T(static_free_मूल्य_wrapper, uq), scope)

#घोषणा DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, शून्य, शून्य,)

#घोषणा DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, शून्य, शून्य, अटल)

#घोषणा DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#घोषणा DEFINE_PRIVATE_HASH_OPS_WITH_कुंजी_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_कुंजी_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, अटल)

#घोषणा DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(name, type, hash_func, compare_func, मूल्य_type, free_func) \
        DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, मूल्य_type, free_func,)

#घोषणा DEFINE_PRIVATE_HASH_OPS_WITH_मूल्य_DESTRUCTOR(name, type, hash_func, compare_func, मूल्य_type, free_func) \
        DEFINE_HASH_OPS_WITH_मूल्य_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, मूल्य_type, free_func, अटल)

#घोषणा DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_कुंजी_func, मूल्य_type, free_मूल्य_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_कुंजी_func, मूल्य_type, free_मूल्य_func,)

#घोषणा DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_कुंजी_func, मूल्य_type, free_मूल्य_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_कुंजी_func, मूल्य_type, free_मूल्य_func, अटल)

व्योम string_hash_func(स्थिर अक्षर *p, काष्ठा siphash *state);
#घोषणा string_compare_func म_भेद
बाह्य स्थिर काष्ठा hash_ops string_hash_ops;
बाह्य स्थिर काष्ठा hash_ops string_hash_ops_free;
बाह्य स्थिर काष्ठा hash_ops string_hash_ops_मूल्य_free;
बाह्य स्थिर काष्ठा hash_ops string_hash_ops_free_free;
बाह्य स्थिर काष्ठा hash_ops string_hash_ops_free_strv_free;

व्योम path_hash_func(स्थिर अक्षर *p, काष्ठा siphash *state);
बाह्य स्थिर काष्ठा hash_ops path_hash_ops;
बाह्य स्थिर काष्ठा hash_ops path_hash_ops_free;
बाह्य स्थिर काष्ठा hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
व्योम trivial_hash_func(स्थिर व्योम *p, काष्ठा siphash *state);
पूर्णांक trivial_compare_func(स्थिर व्योम *a, स्थिर व्योम *b) _const_;
बाह्य स्थिर काष्ठा hash_ops trivial_hash_ops;
बाह्य स्थिर काष्ठा hash_ops trivial_hash_ops_free;
बाह्य स्थिर काष्ठा hash_ops trivial_hash_ops_मूल्य_free;
बाह्य स्थिर काष्ठा hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
व्योम uint64_hash_func(स्थिर uint64_t *p, काष्ठा siphash *state);
पूर्णांक uint64_compare_func(स्थिर uint64_t *a, स्थिर uint64_t *b) _pure_;
बाह्य स्थिर काष्ठा hash_ops uint64_hash_ops;
बाह्य स्थिर काष्ठा hash_ops uint64_hash_ops_मूल्य_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#अगर SIZEOF_DEV_T != 8
व्योम devt_hash_func(स्थिर dev_t *p, काष्ठा siphash *state);
#अन्यथा
#घोषणा devt_hash_func uint64_hash_func
#पूर्ण_अगर

पूर्णांक devt_compare_func(स्थिर dev_t *a, स्थिर dev_t *b) _pure_;
बाह्य स्थिर काष्ठा hash_ops devt_hash_ops;
