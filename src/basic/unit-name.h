/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو UNIT_NAME_MAX 256

قسمتعریف شمار UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) خالص;
bool unit_سابقہ_is_valid(ثابت حرف *p) خالص;
bool unit_instance_is_valid(ثابت حرف *i) خالص;
bool unit_suffix_is_valid(ثابت حرف *s) خالص;

صحیح unit_name_مقصد_سابقہ(ثابت حرف *n, حرف **ret);
UnitNameFlags unit_name_مقصد_instance(ثابت حرف *n, حرف **ret);
ساکن اندرونی UnitNameFlags unit_name_classify(ثابت حرف *n) {
        واپس unit_name_مقصد_instance(n, تہی);
}
صحیح unit_name_مقصد_سابقہ_and_instance(ثابت حرف *n, حرف **ret);

UnitType unit_name_مقصد_type(ثابت حرف *n) خالص;

صحیح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret);

صحیح unit_name_build(ثابت حرف *سابقہ, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret);
صحیح unit_name_build_from_type(ثابت حرف *سابقہ, ثابت حرف *instance, UnitType type, حرف **ret);

حرف* unit_name_escape(ثابت حرف *f);
صحیح unit_name_unescape(ثابت حرف *f, حرف **ret);
صحیح unit_name_راستہ_escape(ثابت حرف *f, حرف **ret);
صحیح unit_name_راستہ_unescape(ثابت حرف *f, حرف **ret);

صحیح unit_name_replace_instance_بھرا(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret);
ساکن اندرونی صحیح unit_name_replace_instance(ثابت حرف *original, ثابت حرف *instance, حرف **ret) {
        واپس unit_name_replace_instance_بھرا(original, instance, false, ret);
}

صحیح unit_name_template(ثابت حرف *f, حرف **ret);

صحیح unit_name_hash_long(ثابت حرف *name, حرف **ret);
bool unit_name_is_hashed(ثابت حرف *name);

صحیح unit_name_from_راستہ(ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret);
صحیح unit_name_from_راستہ_instance(ثابت حرف *سابقہ, ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret);
صحیح unit_name_مقصد_راستہ(ثابت حرف *name, حرف **ret);

قسمتعریف شمار UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

صحیح unit_name_mangle_with_suffix(ثابت حرف *name, ثابت حرف *operation, UnitNameMangle flags, ثابت حرف *suffix, حرف **ret);

ساکن اندرونی صحیح unit_name_mangle(ثابت حرف *name, UnitNameMangle flags, حرف **ret) {
        واپس unit_name_mangle_with_suffix(name, تہی, flags, ".service", ret);
}

صحیح قاش_build_والد_قاش(ثابت حرف *قاش, حرف **ret);
صحیح قاش_build_subslice(ثابت حرف *قاش, ثابت حرف *name, حرف **ret);
bool قاش_name_is_valid(ثابت حرف *name);

bool unit_name_سابقہ_equal(ثابت حرف *a, ثابت حرف *b);
