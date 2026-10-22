/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف خالی (*hash_func_t)(ثابت خالی *p, ساخت siphash *state);
قسمتعریف صحیح (*compare_func_t)(ثابت خالی *a, ثابت خالی *b);

ساخت hash_ops {
        hash_func_t hash;
        compare_func_t compare;
        free_func_t free_کلید;
        free_func_t free_قدر;
};

#تعریف_کرو _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func, free_کلید_func, free_قدر_func, scope) \
        غیر_مستعمل ساکن خالی (* UNIQ_T(static_hash_wrapper, uq))(ثابت type *, ساخت siphash *) = hash_func; \
        غیر_مستعمل ساکن صحیح (* UNIQ_T(static_compare_wrapper, uq))(ثابت type *, ثابت type *) = compare_func; \
        scope ثابت ساخت hash_ops name = {                            \
                .hash = (hash_func_t) hash_func,                        \
                .compare = (compare_func_t) compare_func,               \
                .free_کلید = free_کلید_func,                              \
                .free_قدر = free_قدر_func,                          \
        }

#تعریف_کرو _DEFINE_FREE_FUNC(uq, type, wrapper_name, func)                 \
        /* Type-safe free function */                                   \
        ساکن خالی UNIQ_T(wrapper_name, uq)(خالی *a) {                 \
                type *_a = a;                                           \
                func(_a);                                               \
        }

#تعریف_کرو DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_wrapper, free_func);    \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_wrapper, uq), تہی, scope)

#تعریف_کرو DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(uq, name, type, hash_func, compare_func, type_قدر, free_func, scope) \
        _DEFINE_FREE_FUNC(uq, type_قدر, static_free_wrapper, free_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         تہی, UNIQ_T(static_free_wrapper, uq), scope)

#تعریف_کرو DEFINE_HASH_OPS_بھرا_2(uq, name, type, hash_func, compare_func, free_کلید_func, type_قدر, free_قدر_func, scope) \
        _DEFINE_FREE_FUNC(uq, type, static_free_کلید_wrapper, free_کلید_func); \
        _DEFINE_FREE_FUNC(uq, type_قدر, static_free_قدر_wrapper, free_قدر_func); \
        _DEFINE_HASH_OPS(uq, name, type, hash_func, compare_func,       \
                         UNIQ_T(static_free_کلید_wrapper, uq),           \
                         UNIQ_T(static_free_قدر_wrapper, uq), scope)

#تعریف_کرو DEFINE_HASH_OPS(name, type, hash_func, compare_func)            \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, تہی, تہی,)

#تعریف_کرو DEFINE_نجی_HASH_OPS(name, type, hash_func, compare_func)    \
        _DEFINE_HASH_OPS(UNIQ, name, type, hash_func, compare_func, تہی, تہی, ساکن)

#تعریف_کرو DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func,)

#تعریف_کرو DEFINE_نجی_HASH_OPS_WITH_کلید_DESTRUCTOR(name, type, hash_func, compare_func, free_func) \
        DEFINE_HASH_OPS_WITH_کلید_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, free_func, ساکن)

#تعریف_کرو DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR(name, type, hash_func, compare_func, قدر_type, free_func) \
        DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, قدر_type, free_func,)

#تعریف_کرو DEFINE_نجی_HASH_OPS_WITH_قدر_DESTRUCTOR(name, type, hash_func, compare_func, قدر_type, free_func) \
        DEFINE_HASH_OPS_WITH_قدر_DESTRUCTOR_2(UNIQ, name, type, hash_func, compare_func, قدر_type, free_func, ساکن)

#تعریف_کرو DEFINE_HASH_OPS_بھرا(name, type, hash_func, compare_func, free_کلید_func, قدر_type, free_قدر_func) \
        DEFINE_HASH_OPS_بھرا_2(UNIQ, name, type, hash_func, compare_func, free_کلید_func, قدر_type, free_قدر_func,)

#تعریف_کرو DEFINE_نجی_HASH_OPS_بھرا(name, type, hash_func, compare_func, free_کلید_func, قدر_type, free_قدر_func) \
        DEFINE_HASH_OPS_بھرا_2(UNIQ, name, type, hash_func, compare_func, free_کلید_func, قدر_type, free_قدر_func, ساکن)

خالی string_hash_func(ثابت حرف *p, ساخت siphash *state);
#تعریف_کرو string_compare_func strcmp
خارجی ثابت ساخت hash_ops string_hash_ops;
خارجی ثابت ساخت hash_ops string_hash_ops_free;
خارجی ثابت ساخت hash_ops string_hash_ops_قدر_free;
خارجی ثابت ساخت hash_ops string_hash_ops_free_free;
خارجی ثابت ساخت hash_ops string_hash_ops_free_strv_free;

خالی راستہ_hash_func(ثابت حرف *p, ساخت siphash *state);
خارجی ثابت ساخت hash_ops راستہ_hash_ops;
خارجی ثابت ساخت hash_ops راستہ_hash_ops_free;
خارجی ثابت ساخت hash_ops راستہ_hash_ops_free_free;

/* This will compare the passed pointers directly, and will not dereference them. This is hence not useful for strings
 * or suchlike. */
خالی trivial_hash_func(ثابت خالی *p, ساخت siphash *state);
صحیح trivial_compare_func(ثابت خالی *a, ثابت خالی *b) _const_;
خارجی ثابت ساخت hash_ops trivial_hash_ops;
خارجی ثابت ساخت hash_ops trivial_hash_ops_free;
خارجی ثابت ساخت hash_ops trivial_hash_ops_قدر_free;
خارجی ثابت ساخت hash_ops trivial_hash_ops_free_free;

/* 32-bit values we can always just embed in the pointer itself, but in order to support 32-bit archs we need store 64-bit
 * values indirectly, since they don't fit in a pointer. */
خالی uint64_hash_func(ثابت uint64_t *p, ساخت siphash *state);
صحیح uint64_compare_func(ثابت uint64_t *a, ثابت uint64_t *b) خالص;
خارجی ثابت ساخت hash_ops uint64_hash_ops;
خارجی ثابت ساخت hash_ops uint64_hash_ops_قدر_free;

/* On some archs dev_t is 32-bit, and on others 64-bit. And sometimes it's 64-bit on 32-bit archs, and sometimes 32-bit on
 * 64-bit archs. Yuck! */
#اگر SIZEOF_DEV_T != 8
خالی devt_hash_func(ثابت dev_t *p, ساخت siphash *state);
#ورنہ
#تعریف_کرو devt_hash_func uint64_hash_func
#اگر_ختم

صحیح devt_compare_func(ثابت dev_t *a, ثابت dev_t *b) خالص;
خارجی ثابت ساخت hash_ops devt_hash_ops;
