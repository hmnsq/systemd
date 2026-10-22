/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा UNIT_NAME_MAX 256

प्रकार क्रमागत UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(स्थिर अक्षर *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(स्थिर अक्षर *p) _pure_;
bool unit_instance_is_valid(स्थिर अक्षर *i) _pure_;
bool unit_suffix_is_valid(स्थिर अक्षर *s) _pure_;

पूर्णांक unit_name_to_prefix(स्थिर अक्षर *n, अक्षर **ret);
UnitNameFlags unit_name_to_instance(स्थिर अक्षर *n, अक्षर **ret);
अटल अंतरभूत UnitNameFlags unit_name_classify(स्थिर अक्षर *n) {
        वापस unit_name_to_instance(n, शून्य);
}
पूर्णांक unit_name_to_prefix_and_instance(स्थिर अक्षर *n, अक्षर **ret);

UnitType unit_name_to_type(स्थिर अक्षर *n) _pure_;

पूर्णांक unit_name_change_suffix(स्थिर अक्षर *n, स्थिर अक्षर *suffix, अक्षर **ret);

पूर्णांक unit_name_build(स्थिर अक्षर *prefix, स्थिर अक्षर *instance, स्थिर अक्षर *suffix, अक्षर **ret);
पूर्णांक unit_name_build_from_type(स्थिर अक्षर *prefix, स्थिर अक्षर *instance, UnitType type, अक्षर **ret);

अक्षर* unit_name_escape(स्थिर अक्षर *f);
पूर्णांक unit_name_unescape(स्थिर अक्षर *f, अक्षर **ret);
पूर्णांक unit_name_path_escape(स्थिर अक्षर *f, अक्षर **ret);
पूर्णांक unit_name_path_unescape(स्थिर अक्षर *f, अक्षर **ret);

पूर्णांक unit_name_replace_instance_full(
                स्थिर अक्षर *original,
                स्थिर अक्षर *instance,
                bool accept_glob,
                अक्षर **ret);
अटल अंतरभूत पूर्णांक unit_name_replace_instance(स्थिर अक्षर *original, स्थिर अक्षर *instance, अक्षर **ret) {
        वापस unit_name_replace_instance_full(original, instance, false, ret);
}

पूर्णांक unit_name_template(स्थिर अक्षर *f, अक्षर **ret);

पूर्णांक unit_name_hash_long(स्थिर अक्षर *name, अक्षर **ret);
bool unit_name_is_hashed(स्थिर अक्षर *name);

पूर्णांक unit_name_from_path(स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret);
पूर्णांक unit_name_from_path_instance(स्थिर अक्षर *prefix, स्थिर अक्षर *path, स्थिर अक्षर *suffix, अक्षर **ret);
पूर्णांक unit_name_to_path(स्थिर अक्षर *name, अक्षर **ret);

प्रकार क्रमागत UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

पूर्णांक unit_name_mangle_with_suffix(स्थिर अक्षर *name, स्थिर अक्षर *operation, UnitNameMangle flags, स्थिर अक्षर *suffix, अक्षर **ret);

अटल अंतरभूत पूर्णांक unit_name_mangle(स्थिर अक्षर *name, UnitNameMangle flags, अक्षर **ret) {
        वापस unit_name_mangle_with_suffix(name, शून्य, flags, ".service", ret);
}

पूर्णांक slice_build_parent_slice(स्थिर अक्षर *slice, अक्षर **ret);
पूर्णांक slice_build_subslice(स्थिर अक्षर *slice, स्थिर अक्षर *name, अक्षर **ret);
bool slice_name_is_valid(स्थिर अक्षर *name);

bool unit_name_prefix_equal(स्थिर अक्षर *a, स्थिर अक्षर *b);
