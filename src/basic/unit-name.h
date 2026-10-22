/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು UNIT_NAME_MAX 256

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, UnitNameFlags flags) ಶುದ್ಧ;
bool unit_ಪೂರ್ವಪ್ರತ್ಯಯ_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p) ಶುದ್ಧ;
bool unit_instance_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *i) ಶುದ್ಧ;
bool unit_suffix_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret);
UnitNameFlags unit_name_ಗಮ್ಯ_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ UnitNameFlags unit_name_classify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n) {
        ಹಿಂತಿರುಗಿ unit_name_ಗಮ್ಯ_instance(n, ಖಾಲಿ);
}
ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಪೂರ್ವಪ್ರತ್ಯಯ_and_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಅಕ್ಷರ **ret);

UnitType unit_name_ಗಮ್ಯ_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ unit_name_change_suffix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ unit_name_build(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ unit_name_build_from_type(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance, UnitType type, ಅಕ್ಷರ **ret);

ಅಕ್ಷರ* unit_name_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f);
ಪೂರ್ಣಾಂಕ unit_name_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ unit_name_ಮಾರ್ಗ_escape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ unit_name_ಮಾರ್ಗ_unescape(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ unit_name_replace_instance_ತುಂಬಿದ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance,
                bool accept_glob,
                ಅಕ್ಷರ **ret);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ unit_name_replace_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *original, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *instance, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ unit_name_replace_instance_ತುಂಬಿದ(original, instance, false, ret);
}

ಪೂರ್ಣಾಂಕ unit_name_template(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *f, ಅಕ್ಷರ **ret);

ಪೂರ್ಣಾಂಕ unit_name_hash_long(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret);
bool unit_name_is_hashed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

ಪೂರ್ಣಾಂಕ unit_name_from_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ unit_name_from_ಮಾರ್ಗ_instance(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ unit_name_ಗಮ್ಯ_ಮಾರ್ಗ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

ಪೂರ್ಣಾಂಕ unit_name_mangle_with_suffix(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *operation, UnitNameMangle flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಅಕ್ಷರ **ret);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ unit_name_mangle(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, UnitNameMangle flags, ಅಕ್ಷರ **ret) {
        ಹಿಂತಿರುಗಿ unit_name_mangle_with_suffix(name, ಖಾಲಿ, flags, ".service", ret);
}

ಪೂರ್ಣಾಂಕ ತುಂಡು_build_ಪೋಷಕ_ತುಂಡು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುಂಡು, ಅಕ್ಷರ **ret);
ಪೂರ್ಣಾಂಕ ತುಂಡು_build_subslice(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತುಂಡು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಅಕ್ಷರ **ret);
bool ತುಂಡು_name_is_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name);

bool unit_name_ಪೂರ್ವಪ್ರತ್ಯಯ_equal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *a, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *b);
