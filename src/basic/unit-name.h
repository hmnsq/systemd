/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define UNIT_NAME_MAX 256

প্রকারনির্দেশ গণনা UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ধ্রুব বর্ণ *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ধ্রুব বর্ণ *p) _pure_;
bool unit_instance_is_valid(ধ্রুব বর্ণ *i) _pure_;
bool unit_suffix_is_valid(ধ্রুব বর্ণ *s) _pure_;

পূর্ণাঙ্ক unit_name_to_prefix(ধ্রুব বর্ণ *n, বর্ণ **ret);
UnitNameFlags unit_name_to_instance(ধ্রুব বর্ণ *n, বর্ণ **ret);
স্থির অন্তঃস্থ UnitNameFlags unit_name_classify(ধ্রুব বর্ণ *n) {
        প্রত্যাগম unit_name_to_instance(n, রিক্ত);
}
পূর্ণাঙ্ক unit_name_to_prefix_and_instance(ধ্রুব বর্ণ *n, বর্ণ **ret);

UnitType unit_name_to_type(ধ্রুব বর্ণ *n) _pure_;

পূর্ণাঙ্ক unit_name_change_suffix(ধ্রুব বর্ণ *n, ধ্রুব বর্ণ *suffix, বর্ণ **ret);

পূর্ণাঙ্ক unit_name_build(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *instance, ধ্রুব বর্ণ *suffix, বর্ণ **ret);
পূর্ণাঙ্ক unit_name_build_from_type(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *instance, UnitType type, বর্ণ **ret);

বর্ণ* unit_name_escape(ধ্রুব বর্ণ *f);
পূর্ণাঙ্ক unit_name_unescape(ধ্রুব বর্ণ *f, বর্ণ **ret);
পূর্ণাঙ্ক unit_name_path_escape(ধ্রুব বর্ণ *f, বর্ণ **ret);
পূর্ণাঙ্ক unit_name_path_unescape(ধ্রুব বর্ণ *f, বর্ণ **ret);

পূর্ণাঙ্ক unit_name_replace_instance_full(
                ধ্রুব বর্ণ *original,
                ধ্রুব বর্ণ *instance,
                bool accept_glob,
                বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক unit_name_replace_instance(ধ্রুব বর্ণ *original, ধ্রুব বর্ণ *instance, বর্ণ **ret) {
        প্রত্যাগম unit_name_replace_instance_full(original, instance, false, ret);
}

পূর্ণাঙ্ক unit_name_template(ধ্রুব বর্ণ *f, বর্ণ **ret);

পূর্ণাঙ্ক unit_name_hash_long(ধ্রুব বর্ণ *name, বর্ণ **ret);
bool unit_name_is_hashed(ধ্রুব বর্ণ *name);

পূর্ণাঙ্ক unit_name_from_path(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *suffix, বর্ণ **ret);
পূর্ণাঙ্ক unit_name_from_path_instance(ধ্রুব বর্ণ *prefix, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *suffix, বর্ণ **ret);
পূর্ণাঙ্ক unit_name_to_path(ধ্রুব বর্ণ *name, বর্ণ **ret);

প্রকারনির্দেশ গণনা UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

পূর্ণাঙ্ক unit_name_mangle_with_suffix(ধ্রুব বর্ণ *name, ধ্রুব বর্ণ *operation, UnitNameMangle flags, ধ্রুব বর্ণ *suffix, বর্ণ **ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক unit_name_mangle(ধ্রুব বর্ণ *name, UnitNameMangle flags, বর্ণ **ret) {
        প্রত্যাগম unit_name_mangle_with_suffix(name, রিক্ত, flags, ".service", ret);
}

পূর্ণাঙ্ক slice_build_parent_slice(ধ্রুব বর্ণ *slice, বর্ণ **ret);
পূর্ণাঙ্ক slice_build_subslice(ধ্রুব বর্ণ *slice, ধ্রুব বর্ণ *name, বর্ণ **ret);
bool slice_name_is_valid(ধ্রুব বর্ণ *name);

bool unit_name_prefix_equal(ধ্রুব বর্ণ *a, ধ্রুব বর্ণ *b);
