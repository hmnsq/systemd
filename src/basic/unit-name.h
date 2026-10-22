/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#وصف_ڪريو UNIT_NAME_MAX 256

تعريفنوع تعداد UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) خالص;
bool unit_اڳياڙي_is_valid(ثابت حرف *p) خالص;
bool unit_instance_is_valid(ثابت حرف *i) خالص;
bool unit_suffix_is_valid(ثابت حرف *s) خالص;

صحيح unit_name_منزل_اڳياڙي(ثابت حرف *n, حرف **ret);
UnitNameFlags unit_name_منزل_instance(ثابت حرف *n, حرف **ret);
ساكن مضمن UnitNameFlags unit_name_classify(ثابت حرف *n) {
        أرجع unit_name_منزل_instance(n, عدم);
}
صحيح unit_name_منزل_اڳياڙي_and_instance(ثابت حرف *n, حرف **ret);

UnitType unit_name_منزل_type(ثابت حرف *n) خالص;

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret);

صحيح unit_name_build(ثابت حرف *اڳياڙي, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_build_from_type(ثابت حرف *اڳياڙي, ثابت حرف *instance, UnitType type, حرف **ret);

حرف* unit_name_escape(ثابت حرف *f);
صحيح unit_name_unescape(ثابت حرف *f, حرف **ret);
صحيح unit_name_رستو_escape(ثابت حرف *f, حرف **ret);
صحيح unit_name_رستو_unescape(ثابت حرف *f, حرف **ret);

صحيح unit_name_replace_instance_ڀريل(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret);
ساكن مضمن صحيح unit_name_replace_instance(ثابت حرف *original, ثابت حرف *instance, حرف **ret) {
        أرجع unit_name_replace_instance_ڀريل(original, instance, false, ret);
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret);

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret);
bool unit_name_is_hashed(ثابت حرف *name);

صحيح unit_name_from_رستو(ثابت حرف *رستو, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_from_رستو_instance(ثابت حرف *اڳياڙي, ثابت حرف *رستو, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_منزل_رستو(ثابت حرف *name, حرف **ret);

تعريفنوع تعداد UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

صحيح unit_name_mangle_with_suffix(ثابت حرف *name, ثابت حرف *operation, UnitNameMangle flags, ثابت حرف *suffix, حرف **ret);

ساكن مضمن صحيح unit_name_mangle(ثابت حرف *name, UnitNameMangle flags, حرف **ret) {
        أرجع unit_name_mangle_with_suffix(name, عدم, flags, ".service", ret);
}

صحيح ٽڪرو_build_والد_ٽڪرو(ثابت حرف *ٽڪرو, حرف **ret);
صحيح ٽڪرو_build_subslice(ثابت حرف *ٽڪرو, ثابت حرف *name, حرف **ret);
bool ٽڪرو_name_is_valid(ثابت حرف *name);

bool unit_name_اڳياڙي_equal(ثابت حرف *a, ثابت حرف *b);
