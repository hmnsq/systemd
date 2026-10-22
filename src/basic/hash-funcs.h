/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ശൂന്യം (*hash_func_t)(സ്ഥിരം ശൂന്യം *p, ഘടന siphash *state);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*compare_func_t)(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b);

ഘടന hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_താക്കോൽ;
        free_func_t free_മൂല്യം;
};

#define _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_താക്കോൽ_func, free_മൂല്യം_func, scope) \
        _unused_ സ്ഥിതം ശൂന്യം (* UNIQ_T(static_hash_wrapper, uq))(സ്ഥിരം type *, ഘടന siphash *) = hash_func; \
        _unused_ സ്ഥിതം പൂർണ്ണസംഖ്യ (* UNIQ_T(static_compare_wrapper, uq))(സ്ഥിരം type *, സ്ഥിരം type *) = compare_func; \
        scope സ്ഥിരം ഘടന hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_താക്കോൽ = free_താക്കോൽ_func,                              \
                .free_മൂല്യം = free_മൂല്യം_func,                          \
        }

#define _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        സ്ഥിതം ശൂന്യം UNIQ_T(wrapper_name, uq)(ശൂന്യം *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#define DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), ശൂന്യ, scope)

#define DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_മൂല്യം, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_മൂല്യം, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         ശൂന്യ, UNIQ_T(static_free_wrapper, uq), scope)

#define _DEFINE_HASH_OPS_FULL(uq, name, type, hash_func, compare_func, free_താക്കോൽ_func, type_മൂല്യം, free_മൂല്യം_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_താക്കോൽ_wrapper, free_താക്കോൽ_func); \
        _DEFINE_FREE_FUNC(uq, type_മൂല്യം, static_free_മൂല്യം_wrapper, free_മൂല്യം_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_താക്കോൽ_wrapper, uq),           \
                         UNIQ_T(static_free_മൂല്യം_wrapper, uq), scope)

#define DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ശൂന്യ, ശൂന്യ,)

#define DEFINE_PRIVATE_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, ശൂന്യ, ശൂന്യ, സ്ഥിതം)

#define DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_താക്കോൽ_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, സ്ഥിതം)

#define DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(name, type, hash_func, compare_func, മൂല്യം_type, free_func) \
        DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, മൂല്യം_type, free_func,)

#define DEFINE_PRIVATE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR(name, type, hash_func, compare_func, മൂല്യം_type, free_func) \
        DEFINE_HASH_OPS_WITH_മൂല്യം_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, മൂല്യം_type, free_func, സ്ഥിതം)

#define DEFINE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_താക്കോൽ_func, മൂല്യം_type, free_മൂല്യം_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_താക്കോൽ_func, മൂല്യം_type, free_മൂല്യം_func,)

#define DEFINE_PRIVATE_HASH_OPS_FULL(name, type, hash_func, compare_func, free_താക്കോൽ_func, മൂല്യം_type, free_മൂല്യം_func) \
        _DEFINE_HASH_OPS_FULL(UNIQ, name, type, hash_func, compare_func, free_താക്കോൽ_func, മൂല്യം_type, free_മൂല്യം_func, സ്ഥിതം)

ശൂന്യം string_hash_func(സ്ഥിരം അക്ഷരം *p, ഘടന siphash *state);
#define string_compare_func strcmp
ബാഹ്യ സ്ഥിരം ഘടന hash_ops string_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops string_hash_ops_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops string_hash_ops_മൂല്യം_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops string_hash_ops_free_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops string_hash_ops_free_strv_free;

ശൂന്യം path_hash_func(സ്ഥിരം അക്ഷരം *p, ഘടന siphash *state);
ബാഹ്യ സ്ഥിരം ഘടന hash_ops path_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops path_hash_ops_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops path_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
ശൂന്യം trivial_hash_func(സ്ഥിരം ശൂന്യം *p, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ trivial_compare_func(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b) _const_;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops trivial_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops trivial_hash_ops_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops trivial_hash_ops_മൂല്യം_free;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
ശൂന്യം uint64_hash_func(സ്ഥിരം uint64_t *p, ഘടന siphash *state);
പൂർണ്ണസംഖ്യ uint64_compare_func(സ്ഥിരം uint64_t *a, സ്ഥിരം uint64_t *b) _pure_;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops uint64_hash_ops;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops uint64_hash_ops_മൂല്യം_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#എങ്കിൽ SIZEOF_DEV_T != 8
ശൂന്യം devt_hash_func(സ്ഥിരം dev_t *p, ഘടന siphash *state);
#അല്ലെങ്കിൽ
#define devt_hash_func uint64_hash_func
#endif

പൂർണ്ണസംഖ്യ devt_compare_func(സ്ഥിരം dev_t *a, സ്ഥിരം dev_t *b) _pure_;
ബാഹ്യ സ്ഥിരം ഘടന hash_ops devt_hash_ops;
