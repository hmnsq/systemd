/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

#define UNIT_NAME_MAX 256

تعريفنوع تعداد UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ثابت حرف *p) _pure_;
bool unit_instance_is_valid(ثابت حرف *i) _pure_;
bool unit_suffix_is_valid(ثابت حرف *s) _pure_;

صحيح unit_name_to_prefix(ثابت حرف *n, حرف **ret);
UnitNameFlags unit_name_to_instance(ثابت حرف *n, حرف **ret);
ساكن مضمن UnitNameFlags unit_name_classify(ثابت حرف *n) {
        أرجع unit_name_to_instance(n, عدم);
}
صحيح unit_name_to_prefix_and_instance(ثابت حرف *n, حرف **ret);

UnitType unit_name_to_type(ثابت حرف *n) _pure_;

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret);

صحيح unit_name_build(ثابت حرف *prefix, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_build_from_type(ثابت حرف *prefix, ثابت حرف *instance, UnitType type, حرف **ret);

حرف* unit_name_escape(ثابت حرف *f);
صحيح unit_name_unescape(ثابت حرف *f, حرف **ret);
صحيح unit_name_path_escape(ثابت حرف *f, حرف **ret);
صحيح unit_name_path_unescape(ثابت حرف *f, حرف **ret);

صحيح unit_name_replace_instance_full(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret);
ساكن مضمن صحيح unit_name_replace_instance(ثابت حرف *original, ثابت حرف *instance, حرف **ret) {
        أرجع unit_name_replace_instance_full(original, instance, false, ret);
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret);

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret);
bool unit_name_is_hashed(ثابت حرف *name);

صحيح unit_name_from_path(ثابت حرف *path, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_from_path_instance(ثابت حرف *prefix, ثابت حرف *path, ثابت حرف *suffix, حرف **ret);
صحيح unit_name_to_path(ثابت حرف *name, حرف **ret);

تعريفنوع تعداد UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

صحيح unit_name_mangle_with_suffix(ثابت حرف *name, ثابت حرف *operation, UnitNameMangle flags, ثابت حرف *suffix, حرف **ret);

ساكن مضمن صحيح unit_name_mangle(ثابت حرف *name, UnitNameMangle flags, حرف **ret) {
        أرجع unit_name_mangle_with_suffix(name, عدم, flags, ".service", ret);
}

صحيح slice_build_parent_slice(ثابت حرف *slice, حرف **ret);
صحيح slice_build_subslice(ثابت حرف *slice, ثابت حرف *name, حرف **ret);
bool slice_name_is_valid(ثابت حرف *name);

bool unit_name_prefix_equal(ثابت حرف *a, ثابت حرف *b);
