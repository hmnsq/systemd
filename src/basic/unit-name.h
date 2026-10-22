/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय UNIT_NAME_MAX 256

प्रकारनिर्देश गणना UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ध्रुव वर्ण *n, UnitNameFlags flags) विशुद्धम्;
bool unit_उपसर्गः_is_valid(ध्रुव वर्ण *p) विशुद्धम्;
bool unit_instance_is_valid(ध्रुव वर्ण *i) विशुद्धम्;
bool unit_suffix_is_valid(ध्रुव वर्ण *s) विशुद्धम्;

पूर्णाङ्क unit_name_गन्तव्यम्_उपसर्गः(ध्रुव वर्ण *n, वर्ण **ret);
UnitNameFlags unit_name_गन्तव्यम्_instance(ध्रुव वर्ण *n, वर्ण **ret);
स्थिर अन्तःस्थ UnitNameFlags unit_name_classify(ध्रुव वर्ण *n) {
        प्रत्यागम unit_name_गन्तव्यम्_instance(n, रिक्त);
}
पूर्णाङ्क unit_name_गन्तव्यम्_उपसर्गः_and_instance(ध्रुव वर्ण *n, वर्ण **ret);

UnitType unit_name_गन्तव्यम्_type(ध्रुव वर्ण *n) विशुद्धम्;

पूर्णाङ्क unit_name_change_suffix(ध्रुव वर्ण *n, ध्रुव वर्ण *suffix, वर्ण **ret);

पूर्णाङ्क unit_name_build(ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *instance, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_build_from_type(ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *instance, UnitType type, वर्ण **ret);

वर्ण* unit_name_escape(ध्रुव वर्ण *f);
पूर्णाङ्क unit_name_unescape(ध्रुव वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_मार्गः_escape(ध्रुव वर्ण *f, वर्ण **ret);
पूर्णाङ्क unit_name_मार्गः_unescape(ध्रुव वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_replace_instance_पूर्णम्(
                ध्रुव वर्ण *original,
                ध्रुव वर्ण *instance,
                bool accept_glob,
                वर्ण **ret);
स्थिर अन्तःस्थ पूर्णाङ्क unit_name_replace_instance(ध्रुव वर्ण *original, ध्रुव वर्ण *instance, वर्ण **ret) {
        प्रत्यागम unit_name_replace_instance_पूर्णम्(original, instance, false, ret);
}

पूर्णाङ्क unit_name_template(ध्रुव वर्ण *f, वर्ण **ret);

पूर्णाङ्क unit_name_hash_long(ध्रुव वर्ण *name, वर्ण **ret);
bool unit_name_is_hashed(ध्रुव वर्ण *name);

पूर्णाङ्क unit_name_from_मार्गः(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_from_मार्गः_instance(ध्रुव वर्ण *उपसर्गः, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *suffix, वर्ण **ret);
पूर्णाङ्क unit_name_गन्तव्यम्_मार्गः(ध्रुव वर्ण *name, वर्ण **ret);

प्रकारनिर्देश गणना UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

पूर्णाङ्क unit_name_mangle_with_suffix(ध्रुव वर्ण *name, ध्रुव वर्ण *operation, UnitNameMangle flags, ध्रुव वर्ण *suffix, वर्ण **ret);

स्थिर अन्तःस्थ पूर्णाङ्क unit_name_mangle(ध्रुव वर्ण *name, UnitNameMangle flags, वर्ण **ret) {
        प्रत्यागम unit_name_mangle_with_suffix(name, रिक्त, flags, ".service", ret);
}

पूर्णाङ्क खण्डिका_build_parent_खण्डिका(ध्रुव वर्ण *खण्डिका, वर्ण **ret);
पूर्णाङ्क खण्डिका_build_subslice(ध्रुव वर्ण *खण्डिका, ध्रुव वर्ण *name, वर्ण **ret);
bool खण्डिका_name_is_valid(ध्रुव वर्ण *name);

bool unit_name_उपसर्गः_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b);
