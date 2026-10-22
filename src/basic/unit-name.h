/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"

#define UNIT_NAME_MAX 256

প্রকারনির্দেশ গণনা UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ধ্রুৱ ৱর্ণ *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ধ্রুৱ ৱর্ণ *p) _pure_;
bool unit_instance_is_valid(ধ্রুৱ ৱর্ণ *i) _pure_;
bool unit_suffix_is_valid(ধ্রুৱ ৱর্ণ *s) _pure_;

পূর্ণাঙ্ক unit_name_to_prefix(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret);
UnitNameFlags unit_name_to_instance(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret);
স্থির অন্তঃস্থ UnitNameFlags unit_name_classify(ধ্রুৱ ৱর্ণ *n) {
        ঘূৰাই unit_name_to_instance(n, রিক্ত);
}
পূর্ণাঙ্ক unit_name_to_prefix_and_instance(ধ্রুৱ ৱর্ণ *n, ৱর্ণ **ret);

UnitType unit_name_to_type(ধ্রুৱ ৱর্ণ *n) _pure_;

পূর্ণাঙ্ক unit_name_change_suffix(ধ্রুৱ ৱর্ণ *n, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret);

পূর্ণাঙ্ক unit_name_build(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *instance, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret);
পূর্ণাঙ্ক unit_name_build_from_type(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *instance, UnitType type, ৱর্ণ **ret);

ৱর্ণ* unit_name_escape(ধ্রুৱ ৱর্ণ *f);
পূর্ণাঙ্ক unit_name_unescape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret);
পূর্ণাঙ্ক unit_name_path_escape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret);
পূর্ণাঙ্ক unit_name_path_unescape(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret);

পূর্ণাঙ্ক unit_name_replace_instance_full(
                ধ্রুৱ ৱর্ণ *original,
                ধ্রুৱ ৱর্ণ *instance,
                bool accept_glob,
                ৱর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক unit_name_replace_instance(ধ্রুৱ ৱর্ণ *original, ধ্রুৱ ৱর্ণ *instance, ৱর্ণ **ret) {
        ঘূৰাই unit_name_replace_instance_full(original, instance, false, ret);
}

পূর্ণাঙ্ক unit_name_template(ধ্রুৱ ৱর্ণ *f, ৱর্ণ **ret);

পূর্ণাঙ্ক unit_name_hash_long(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret);
bool unit_name_is_hashed(ধ্রুৱ ৱর্ণ *name);

পূর্ণাঙ্ক unit_name_from_path(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret);
পূর্ণাঙ্ক unit_name_from_path_instance(ধ্রুৱ ৱর্ণ *prefix, ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret);
পূর্ণাঙ্ক unit_name_to_path(ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret);

প্রকারনির্দেশ গণনা UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

পূর্ণাঙ্ক unit_name_mangle_with_suffix(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ ৱর্ণ *operation, UnitNameMangle flags, ধ্রুৱ ৱর্ণ *suffix, ৱর্ণ **ret);

স্থির অন্তঃস্থ পূর্ণাঙ্ক unit_name_mangle(ধ্রুৱ ৱর্ণ *name, UnitNameMangle flags, ৱর্ণ **ret) {
        ঘূৰাই unit_name_mangle_with_suffix(name, রিক্ত, flags, ".service", ret);
}

পূর্ণাঙ্ক slice_build_parent_slice(ধ্রুৱ ৱর্ণ *slice, ৱর্ণ **ret);
পূর্ণাঙ্ক slice_build_subslice(ধ্রুৱ ৱর্ণ *slice, ধ্রুৱ ৱর্ণ *name, ৱর্ণ **ret);
bool slice_name_is_valid(ধ্রুৱ ৱর্ণ *name);

bool unit_name_prefix_equal(ধ্রুৱ ৱর্ণ *a, ধ্রুৱ ৱর্ণ *b);
