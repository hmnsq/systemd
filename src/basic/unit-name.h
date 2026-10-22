/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

#define UNIT_NAME_MAX 256

வகைவரை எண்ணிக்கை UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(மாறிலி எழுத்து *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(மாறிலி எழுத்து *p) _pure_;
bool unit_instance_is_valid(மாறிலி எழுத்து *i) _pure_;
bool unit_suffix_is_valid(மாறிலி எழுத்து *s) _pure_;

முழு unit_name_to_prefix(மாறிலி எழுத்து *n, எழுத்து **ret);
UnitNameFlags unit_name_to_instance(மாறிலி எழுத்து *n, எழுத்து **ret);
நிலையான உள்வரி UnitNameFlags unit_name_classify(மாறிலி எழுத்து *n) {
        திரும்பு unit_name_to_instance(n, வெற்று);
}
முழு unit_name_to_prefix_and_instance(மாறிலி எழுத்து *n, எழுத்து **ret);

UnitType unit_name_to_type(மாறிலி எழுத்து *n) _pure_;

முழு unit_name_change_suffix(மாறிலி எழுத்து *n, மாறிலி எழுத்து *suffix, எழுத்து **ret);

முழு unit_name_build(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *instance, மாறிலி எழுத்து *suffix, எழுத்து **ret);
முழு unit_name_build_from_type(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *instance, UnitType type, எழுத்து **ret);

எழுத்து* unit_name_escape(மாறிலி எழுத்து *f);
முழு unit_name_unescape(மாறிலி எழுத்து *f, எழுத்து **ret);
முழு unit_name_path_escape(மாறிலி எழுத்து *f, எழுத்து **ret);
முழு unit_name_path_unescape(மாறிலி எழுத்து *f, எழுத்து **ret);

முழு unit_name_replace_instance_full(
                மாறிலி எழுத்து *original,
                மாறிலி எழுத்து *instance,
                bool accept_glob,
                எழுத்து **ret);
நிலையான உள்வரி முழு unit_name_replace_instance(மாறிலி எழுத்து *original, மாறிலி எழுத்து *instance, எழுத்து **ret) {
        திரும்பு unit_name_replace_instance_full(original, instance, false, ret);
}

முழு unit_name_template(மாறிலி எழுத்து *f, எழுத்து **ret);

முழு unit_name_hash_long(மாறிலி எழுத்து *name, எழுத்து **ret);
bool unit_name_is_hashed(மாறிலி எழுத்து *name);

முழு unit_name_from_path(மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret);
முழு unit_name_from_path_instance(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret);
முழு unit_name_to_path(மாறிலி எழுத்து *name, எழுத்து **ret);

வகைவரை எண்ணிக்கை UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

முழு unit_name_mangle_with_suffix(மாறிலி எழுத்து *name, மாறிலி எழுத்து *operation, UnitNameMangle flags, மாறிலி எழுத்து *suffix, எழுத்து **ret);

நிலையான உள்வரி முழு unit_name_mangle(மாறிலி எழுத்து *name, UnitNameMangle flags, எழுத்து **ret) {
        திரும்பு unit_name_mangle_with_suffix(name, வெற்று, flags, ".service", ret);
}

முழு slice_build_parent_slice(மாறிலி எழுத்து *slice, எழுத்து **ret);
முழு slice_build_subslice(மாறிலி எழுத்து *slice, மாறிலி எழுத்து *name, எழுத்து **ret);
bool slice_name_is_valid(மாறிலி எழுத்து *name);

bool unit_name_prefix_equal(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b);
