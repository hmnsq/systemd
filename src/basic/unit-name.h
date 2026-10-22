/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

#define UNIT_NAME_MAX 256

প্রকারলেখ গণনা UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ধ্রুবক অক্ষর *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ধ্রুবক অক্ষর *p) _pure_;
bool unit_instance_is_valid(ধ্রুবক অক্ষর *i) _pure_;
bool unit_suffix_is_valid(ধ্রুবক অক্ষর *s) _pure_;

পূর্ণসংখ্যা unit_name_to_prefix(ধ্রুবক অক্ষর *n, অক্ষর **ret);
UnitNameFlags unit_name_to_instance(ধ্রুবক অক্ষর *n, অক্ষর **ret);
স্থির অন্তঃরেখ UnitNameFlags unit_name_classify(ধ্রুবক অক্ষর *n) {
        ফেরত unit_name_to_instance(n, ফাঁকা);
}
পূর্ণসংখ্যা unit_name_to_prefix_and_instance(ধ্রুবক অক্ষর *n, অক্ষর **ret);

UnitType unit_name_to_type(ধ্রুবক অক্ষর *n) _pure_;

পূর্ণসংখ্যা unit_name_change_suffix(ধ্রুবক অক্ষর *n, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);

পূর্ণসংখ্যা unit_name_build(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *instance, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);
পূর্ণসংখ্যা unit_name_build_from_type(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *instance, UnitType type, অক্ষর **ret);

অক্ষর* unit_name_escape(ধ্রুবক অক্ষর *f);
পূর্ণসংখ্যা unit_name_unescape(ধ্রুবক অক্ষর *f, অক্ষর **ret);
পূর্ণসংখ্যা unit_name_path_escape(ধ্রুবক অক্ষর *f, অক্ষর **ret);
পূর্ণসংখ্যা unit_name_path_unescape(ধ্রুবক অক্ষর *f, অক্ষর **ret);

পূর্ণসংখ্যা unit_name_replace_instance_full(
                ধ্রুবক অক্ষর *original,
                ধ্রুবক অক্ষর *instance,
                bool accept_glob,
                অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা unit_name_replace_instance(ধ্রুবক অক্ষর *original, ধ্রুবক অক্ষর *instance, অক্ষর **ret) {
        ফেরত unit_name_replace_instance_full(original, instance, false, ret);
}

পূর্ণসংখ্যা unit_name_template(ধ্রুবক অক্ষর *f, অক্ষর **ret);

পূর্ণসংখ্যা unit_name_hash_long(ধ্রুবক অক্ষর *name, অক্ষর **ret);
bool unit_name_is_hashed(ধ্রুবক অক্ষর *name);

পূর্ণসংখ্যা unit_name_from_path(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);
পূর্ণসংখ্যা unit_name_from_path_instance(ধ্রুবক অক্ষর *prefix, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);
পূর্ণসংখ্যা unit_name_to_path(ধ্রুবক অক্ষর *name, অক্ষর **ret);

প্রকারলেখ গণনা UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

পূর্ণসংখ্যা unit_name_mangle_with_suffix(ধ্রুবক অক্ষর *name, ধ্রুবক অক্ষর *operation, UnitNameMangle flags, ধ্রুবক অক্ষর *suffix, অক্ষর **ret);

স্থির অন্তঃরেখ পূর্ণসংখ্যা unit_name_mangle(ধ্রুবক অক্ষর *name, UnitNameMangle flags, অক্ষর **ret) {
        ফেরত unit_name_mangle_with_suffix(name, ফাঁকা, flags, ".service", ret);
}

পূর্ণসংখ্যা slice_build_parent_slice(ধ্রুবক অক্ষর *slice, অক্ষর **ret);
পূর্ণসংখ্যা slice_build_subslice(ধ্রুবক অক্ষর *slice, ধ্রুবক অক্ষর *name, অক্ষর **ret);
bool slice_name_is_valid(ধ্রুবক অক্ষর *name);

bool unit_name_prefix_equal(ধ্রুবক অক্ষর *a, ধ্রুবক অক্ষর *b);
