/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

#define UNIT_NAME_MAX 256

typedef ga_nanaa UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(const var_na *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(const var_na *p) _pure_;
bool unit_instance_is_valid(const var_na *i) _pure_;
bool unit_suffix_is_valid(const var_na *s) _pure_;

ཧྲིལ unit_name_to_prefix(const var_na *n, var_na **ret);
UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret);
static anta_hstha UnitNameFlags unit_name_classify(const var_na *n) {
        ཕྱིར unit_name_to_instance(n, NULL);
}
ཧྲིལ unit_name_to_prefix_and_instance(const var_na *n, var_na **ret);

UnitType unit_name_to_type(const var_na *n) _pure_;

ཧྲིལ unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret);

ཧྲིལ unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret);
ཧྲིལ unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret);

var_na* unit_name_escape(const var_na *f);
ཧྲིལ unit_name_unescape(const var_na *f, var_na **ret);
ཧྲིལ unit_name_path_escape(const var_na *f, var_na **ret);
ཧྲིལ unit_name_path_unescape(const var_na *f, var_na **ret);

ཧྲིལ unit_name_replace_instance_full(
                const var_na *original,
                const var_na *instance,
                bool accept_glob,
                var_na **ret);
static anta_hstha ཧྲིལ unit_name_replace_instance(const var_na *original, const var_na *instance, var_na **ret) {
        ཕྱིར unit_name_replace_instance_full(original, instance, false, ret);
}

ཧྲིལ unit_name_template(const var_na *f, var_na **ret);

ཧྲིལ unit_name_hash_long(const var_na *name, var_na **ret);
bool unit_name_is_hashed(const var_na *name);

ཧྲིལ unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret);
ཧྲིལ unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret);
ཧྲིལ unit_name_to_path(const var_na *name, var_na **ret);

typedef ga_nanaa UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

ཧྲིལ unit_name_mangle_with_suffix(const var_na *name, const var_na *operation, UnitNameMangle flags, const var_na *suffix, var_na **ret);

static anta_hstha ཧྲིལ unit_name_mangle(const var_na *name, UnitNameMangle flags, var_na **ret) {
        ཕྱིར unit_name_mangle_with_suffix(name, NULL, flags, ".service", ret);
}

ཧྲིལ slice_build_parent_slice(const var_na *slice, var_na **ret);
ཧྲིལ slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret);
bool slice_name_is_valid(const var_na *name);

bool unit_name_prefix_equal(const var_na *a, const var_na *b);
