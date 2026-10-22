/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

#define UNIT_NAME_MAX 256

تعریفنوع شمارش UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ثابت نویسه *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ثابت نویسه *p) _pure_;
bool unit_instance_is_valid(ثابت نویسه *i) _pure_;
bool unit_suffix_is_valid(ثابت نویسه *s) _pure_;

صحیح unit_name_to_prefix(ثابت نویسه *n, نویسه **ret);
UnitNameFlags unit_name_to_instance(ثابت نویسه *n, نویسه **ret);
ایستا درونخطی UnitNameFlags unit_name_classify(ثابت نویسه *n) {
        بازگشت unit_name_to_instance(n, پوچ);
}
صحیح unit_name_to_prefix_and_instance(ثابت نویسه *n, نویسه **ret);

UnitType unit_name_to_type(ثابت نویسه *n) _pure_;

صحیح unit_name_change_suffix(ثابت نویسه *n, ثابت نویسه *suffix, نویسه **ret);

صحیح unit_name_build(ثابت نویسه *prefix, ثابت نویسه *instance, ثابت نویسه *suffix, نویسه **ret);
صحیح unit_name_build_from_type(ثابت نویسه *prefix, ثابت نویسه *instance, UnitType type, نویسه **ret);

نویسه* unit_name_escape(ثابت نویسه *f);
صحیح unit_name_unescape(ثابت نویسه *f, نویسه **ret);
صحیح unit_name_path_escape(ثابت نویسه *f, نویسه **ret);
صحیح unit_name_path_unescape(ثابت نویسه *f, نویسه **ret);

صحیح unit_name_replace_instance_full(
                ثابت نویسه *original,
                ثابت نویسه *instance,
                bool accept_glob,
                نویسه **ret);
ایستا درونخطی صحیح unit_name_replace_instance(ثابت نویسه *original, ثابت نویسه *instance, نویسه **ret) {
        بازگشت unit_name_replace_instance_full(original, instance, false, ret);
}

صحیح unit_name_template(ثابت نویسه *f, نویسه **ret);

صحیح unit_name_hash_long(ثابت نویسه *name, نویسه **ret);
bool unit_name_is_hashed(ثابت نویسه *name);

صحیح unit_name_from_path(ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret);
صحیح unit_name_from_path_instance(ثابت نویسه *prefix, ثابت نویسه *path, ثابت نویسه *suffix, نویسه **ret);
صحیح unit_name_to_path(ثابت نویسه *name, نویسه **ret);

تعریفنوع شمارش UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

صحیح unit_name_mangle_with_suffix(ثابت نویسه *name, ثابت نویسه *operation, UnitNameMangle flags, ثابت نویسه *suffix, نویسه **ret);

ایستا درونخطی صحیح unit_name_mangle(ثابت نویسه *name, UnitNameMangle flags, نویسه **ret) {
        بازگشت unit_name_mangle_with_suffix(name, پوچ, flags, ".service", ret);
}

صحیح slice_build_parent_slice(ثابت نویسه *slice, نویسه **ret);
صحیح slice_build_subslice(ثابت نویسه *slice, ثابت نویسه *name, نویسه **ret);
bool slice_name_is_valid(ثابت نویسه *name);

bool unit_name_prefix_equal(ثابت نویسه *a, ثابت نویسه *b);
