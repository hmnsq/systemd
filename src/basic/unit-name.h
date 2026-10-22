/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

#define UNIT_NAME_MAX 256

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ಧ್ರುವ ವರ್ಣ *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ಧ್ರುವ ವರ್ಣ *p) _pure_;
bool unit_instance_is_valid(ಧ್ರುವ ವರ್ಣ *i) _pure_;
bool unit_suffix_is_valid(ಧ್ರುವ ವರ್ಣ *s) _pure_;

ಪೂರ್ಣಾಙ್ಕ unit_name_to_prefix(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret);
UnitNameFlags unit_name_to_instance(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ UnitNameFlags unit_name_classify(ಧ್ರುವ ವರ್ಣ *n) {
        ಪ್ರತ್ಯಾಗಮ unit_name_to_instance(n, ರಿಕ್ತ);
}
ಪೂರ್ಣಾಙ್ಕ unit_name_to_prefix_and_instance(ಧ್ರುವ ವರ್ಣ *n, ವರ್ಣ **ret);

UnitType unit_name_to_type(ಧ್ರುವ ವರ್ಣ *n) _pure_;

ಪೂರ್ಣಾಙ್ಕ unit_name_change_suffix(ಧ್ರುವ ವರ್ಣ *n, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ unit_name_build(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *instance, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ unit_name_build_from_type(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *instance, UnitType type, ವರ್ಣ **ret);

ವರ್ಣ* unit_name_escape(ಧ್ರುವ ವರ್ಣ *f);
ಪೂರ್ಣಾಙ್ಕ unit_name_unescape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ unit_name_path_escape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ unit_name_path_unescape(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ unit_name_replace_instance_full(
                ಧ್ರುವ ವರ್ಣ *original,
                ಧ್ರುವ ವರ್ಣ *instance,
                bool accept_glob,
                ವರ್ಣ **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ unit_name_replace_instance(ಧ್ರುವ ವರ್ಣ *original, ಧ್ರುವ ವರ್ಣ *instance, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ unit_name_replace_instance_full(original, instance, false, ret);
}

ಪೂರ್ಣಾಙ್ಕ unit_name_template(ಧ್ರುವ ವರ್ಣ *f, ವರ್ಣ **ret);

ಪೂರ್ಣಾಙ್ಕ unit_name_hash_long(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret);
bool unit_name_is_hashed(ಧ್ರುವ ವರ್ಣ *name);

ಪೂರ್ಣಾಙ್ಕ unit_name_from_path(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ unit_name_from_path_instance(ಧ್ರುವ ವರ್ಣ *prefix, ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ unit_name_to_path(ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

ಪೂರ್ಣಾಙ್ಕ unit_name_mangle_with_suffix(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ವರ್ಣ *operation, UnitNameMangle flags, ಧ್ರುವ ವರ್ಣ *suffix, ವರ್ಣ **ret);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ unit_name_mangle(ಧ್ರುವ ವರ್ಣ *name, UnitNameMangle flags, ವರ್ಣ **ret) {
        ಪ್ರತ್ಯಾಗಮ unit_name_mangle_with_suffix(name, ರಿಕ್ತ, flags, ".service", ret);
}

ಪೂರ್ಣಾಙ್ಕ slice_build_parent_slice(ಧ್ರುವ ವರ್ಣ *slice, ವರ್ಣ **ret);
ಪೂರ್ಣಾಙ್ಕ slice_build_subslice(ಧ್ರುವ ವರ್ಣ *slice, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ret);
bool slice_name_is_valid(ಧ್ರುವ ವರ್ಣ *name);

bool unit_name_prefix_equal(ಧ್ರುವ ವರ್ಣ *a, ಧ್ರುವ ವರ್ಣ *b);
