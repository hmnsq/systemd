/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

#define UNIT_NAME_MAX 256

प्रकारनिर्देश गणना UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(अचर वर्ण *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(अचर वर्ण *p) _pure_;
bool unit_instance_is_valid(अचर वर्ण *i) _pure_;
bool unit_suffix_is_valid(अचर वर्ण *s) _pure_;

पूर्णाङ्क unit_name_to_prefix(अचर वर्ण *n, वर्ण **ret);
UnitNameFlags unit_name_to_instance(अचर वर्ण *n, वर्ण **ret);
स्थिर अन्तःस्थ UnitNameFlags unit_name_classify(अचर वर्ण *n) {
        फर्काऊ unit_name_to_instance(n, रिक्त);
}
पूर्णाङ्क unit_name_to_prefix_and_instance(अचर वर्ण *n, वर्ण **ret);

UnitType unit_name_to_type(अचर वर्ण *n) _pure_;

पूर्णाङ्क unit_name_change_suffix(अचर वर्ण *n, अचर वर्ण *suffix, वर्ण **ret);

पूर्णाङ्क unit_name_build(अचर वर्ण *prefix, अचर वर्ण *instance, अचर वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_build_from_type(अचर वर्ण *prefix, अचर वर्ण *instance, UnitType type, वर्ण **ret);

वर्ण* unit_name_escape(अचर वर्ण *f);
पूर्णाङ्क unit_name_unescape(अचर वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_path_escape(अचर वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_path_unescape(अचर वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_replace_instance_full(
                अचर वर्ण *original,
                अचर वर्ण *instance,
                bool accept_glob,
                वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क unit_name_replace_instance(अचर वर्ण *original, अचर वर्ण *instance, वर्ण **ret) {
        फर्काऊ unit_name_replace_instance_full(original, instance, false, ret);
}

पूर्णाङ्क unit_name_template(अचर वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_hash_long(अचर वर्ण *name, वर्ण **ret);
bool unit_name_is_hashed(अचर वर्ण *name);

पूर्णाङ्क unit_name_from_path(अचर वर्ण *path, अचर वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_from_path_instance(अचर वर्ण *prefix, अचर वर्ण *path, अचर वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_to_path(अचर वर्ण *name, वर्ण **ret);

प्रकारनिर्देश गणना UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

पूर्णाङ्क unit_name_mangle_with_suffix(अचर वर्ण *name, अचर वर्ण *operation, UnitNameMangle flags, अचर वर्ण *suffix, वर्ण **ret);

स्थिर अन्तःस्थ पूर्णाङ्क unit_name_mangle(अचर वर्ण *name, UnitNameMangle flags, वर्ण **ret) {
        फर्काऊ unit_name_mangle_with_suffix(name, रिक्त, flags, ".service", ret);
}

पूर्णाङ्क slice_build_parent_slice(अचर वर्ण *slice, वर्ण **ret);
पूर्णाङ्क slice_build_subslice(अचर वर्ण *slice, अचर वर्ण *name, वर्ण **ret);
bool slice_name_is_valid(अचर वर्ण *name);

bool unit_name_prefix_equal(अचर वर्ण *a, अचर वर्ण *b);
