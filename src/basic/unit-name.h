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

bool unit_name_is_valid(ध्रुव वर्ण *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ध्रुव वर्ण *p) _pure_;
bool unit_instance_is_valid(ध्रुव वर्ण *i) _pure_;
bool unit_suffix_is_valid(ध्रुव वर्ण *s) _pure_;

पूर्णाङ्क unit_name_to_prefix(ध्रुव वर्ण *n, वर्ण **ret);
UnitNameFlags unit_name_to_instance(ध्रुव वर्ण *n, वर्ण **ret);
स्थिर अन्तःस्थ UnitNameFlags unit_name_classify(ध्रुव वर्ण *n) {
        पटिनिवत्त unit_name_to_instance(n, रिक्त);
}
पूर्णाङ्क unit_name_to_prefix_and_instance(ध्रुव वर्ण *n, वर्ण **ret);

UnitType unit_name_to_type(ध्रुव वर्ण *n) _pure_;

पूर्णाङ्क unit_name_change_suffix(ध्रुव वर्ण *n, ध्रुव वर्ण *suffix, वर्ण **ret);

पूर्णाङ्क unit_name_build(ध्रुव वर्ण *prefix, ध्रुव वर्ण *instance, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_build_from_type(ध्रुव वर्ण *prefix, ध्रुव वर्ण *instance, UnitType type, वर्ण **ret);

वर्ण* unit_name_escape(ध्रुव वर्ण *f);
पूर्णाङ्क unit_name_unescape(ध्रुव वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_path_escape(ध्रुव वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_path_unescape(ध्रुव वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_replace_instance_full(
                ध्रुव वर्ण *original,
                ध्रुव वर्ण *instance,
                bool accept_glob,
                वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क unit_name_replace_instance(ध्रुव वर्ण *original, ध्रुव वर्ण *instance, वर्ण **ret) {
        पटिनिवत्त unit_name_replace_instance_full(original, instance, false, ret);
}

पूर्णाङ्क unit_name_template(ध्रुव वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_hash_long(ध्रुव वर्ण *name, वर्ण **ret);
bool unit_name_is_hashed(ध्रुव वर्ण *name);

पूर्णाङ्क unit_name_from_path(ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_from_path_instance(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_to_path(ध्रुव वर्ण *name, वर्ण **ret);

प्रकारनिर्देश गणना UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

पूर्णाङ्क unit_name_mangle_with_suffix(ध्रुव वर्ण *name, ध्रुव वर्ण *operation, UnitNameMangle flags, ध्रुव वर्ण *suffix, वर्ण **ret);

स्थिर अन्तःस्थ पूर्णाङ्क unit_name_mangle(ध्रुव वर्ण *name, UnitNameMangle flags, वर्ण **ret) {
        पटिनिवत्त unit_name_mangle_with_suffix(name, रिक्त, flags, ".service", ret);
}

पूर्णाङ्क slice_build_parent_slice(ध्रुव वर्ण *slice, वर्ण **ret);
पूर्णाङ्क slice_build_subslice(ध्रुव वर्ण *slice, ध्रुव वर्ण *name, वर्ण **ret);
bool slice_name_is_valid(ध्रुव वर्ण *name);

bool unit_name_prefix_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b);
