/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

#تعریف_کن UNIT_NAME_MAX 256

تعريفنوع تعداد UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) خالص;
bool unit_پیشوند_is_valid(ثابت حرف *p) خالص;
bool unit_instance_is_valid(ثابت حرف *i) خالص;
bool unit_suffix_is_valid(ثابت حرف *s) خالص;

صحيح unit_name_مقصد_پیشوند(ثابت حرف *n, حرف **ret);
UnitNameFlags unit_name_مقصد_instance(ثابت حرف *n, حرف **ret);
ساكن مضمن UnitNameFlags unit_name_classify(ثابت حرف *n) {
        أرجع unit_name_مقصد_instance(n, عدم);
}
صحيح unit_name_مقصد_پیشوند_and_instance(ثابت حرف *n, حرف **ret);

UnitType unit_name_مقصد_type(ثابت حرف *n) خالص;

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret);

صحيح unit_name_build(ثابت حرف *پیشوند, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_build_from_type(ثابت حرف *پیشوند, ثابت حرف *instance, UnitType type, حرف **ret);

حرف* unit_name_escape(ثابت حرف *f);
صحيح unit_name_unescape(ثابت حرف *f, حرف **ret);
صحيح unit_name_مسیر_escape(ثابت حرف *f, حرف **ret);
صحيح unit_name_مسیر_unescape(ثابت حرف *f, حرف **ret);

صحيح unit_name_replace_instance_پر(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret);
ساكن مضمن صحيح unit_name_replace_instance(ثابت حرف *original, ثابت حرف *instance, حرف **ret) {
        أرجع unit_name_replace_instance_پر(original, instance, false, ret);
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret);

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret);
bool unit_name_is_hashed(ثابت حرف *name);

صحيح unit_name_from_مسیر(ثابت حرف *مسیر, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_from_مسیر_instance(ثابت حرف *پیشوند, ثابت حرف *مسیر, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_مقصد_مسیر(ثابت حرف *name, حرف **ret);

تعريفنوع تعداد UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

صحيح unit_name_mangle_with_suffix(ثابت حرف *name, ثابت حرف *operation, UnitNameMangle flags, ثابت حرف *suffix, حرف **ret);

ساكن مضمن صحيح unit_name_mangle(ثابت حرف *name, UnitNameMangle flags, حرف **ret) {
        أرجع unit_name_mangle_with_suffix(name, عدم, flags, ".service", ret);
}

صحيح برش_build_والد_برش(ثابت حرف *برش, حرف **ret);
صحيح برش_build_subslice(ثابت حرف *برش, ثابت حرف *name, حرف **ret);
bool برش_name_is_valid(ثابت حرف *name);

bool unit_name_پیشوند_equal(ثابت حرف *a, ثابت حرف *b);
