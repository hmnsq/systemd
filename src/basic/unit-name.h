/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

#define UNIT_NAME_MAX 256

પ્રકારવ્યાખ્યા ગણના UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(અચલ અક્ષર *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(અચલ અક્ષર *p) _pure_;
bool unit_instance_is_valid(અચલ અક્ષર *i) _pure_;
bool unit_suffix_is_valid(અચલ અક્ષર *s) _pure_;

પૂર્ણાંક unit_name_to_prefix(અચલ અક્ષર *n, અક્ષર **ret);
UnitNameFlags unit_name_to_instance(અચલ અક્ષર *n, અક્ષર **ret);
સ્થિર અંતઃરેખ UnitNameFlags unit_name_classify(અચલ અક્ષર *n) {
        પાછા unit_name_to_instance(n, ખાલી);
}
પૂર્ણાંક unit_name_to_prefix_and_instance(અચલ અક્ષર *n, અક્ષર **ret);

UnitType unit_name_to_type(અચલ અક્ષર *n) _pure_;

પૂર્ણાંક unit_name_change_suffix(અચલ અક્ષર *n, અચલ અક્ષર *suffix, અક્ષર **ret);

પૂર્ણાંક unit_name_build(અચલ અક્ષર *prefix, અચલ અક્ષર *instance, અચલ અક્ષર *suffix, અક્ષર **ret);
પૂર્ણાંક unit_name_build_from_type(અચલ અક્ષર *prefix, અચલ અક્ષર *instance, UnitType type, અક્ષર **ret);

અક્ષર* unit_name_escape(અચલ અક્ષર *f);
પૂર્ણાંક unit_name_unescape(અચલ અક્ષર *f, અક્ષર **ret);
પૂર્ણાંક unit_name_path_escape(અચલ અક્ષર *f, અક્ષર **ret);
પૂર્ણાંક unit_name_path_unescape(અચલ અક્ષર *f, અક્ષર **ret);

પૂર્ણાંક unit_name_replace_instance_full(
                અચલ અક્ષર *original,
                અચલ અક્ષર *instance,
                bool accept_glob,
                અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક unit_name_replace_instance(અચલ અક્ષર *original, અચલ અક્ષર *instance, અક્ષર **ret) {
        પાછા unit_name_replace_instance_full(original, instance, false, ret);
}

પૂર્ણાંક unit_name_template(અચલ અક્ષર *f, અક્ષર **ret);

પૂર્ણાંક unit_name_hash_long(અચલ અક્ષર *name, અક્ષર **ret);
bool unit_name_is_hashed(અચલ અક્ષર *name);

પૂર્ણાંક unit_name_from_path(અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret);
પૂર્ણાંક unit_name_from_path_instance(અચલ અક્ષર *prefix, અચલ અક્ષર *path, અચલ અક્ષર *suffix, અક્ષર **ret);
પૂર્ણાંક unit_name_to_path(અચલ અક્ષર *name, અક્ષર **ret);

પ્રકારવ્યાખ્યા ગણના UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

પૂર્ણાંક unit_name_mangle_with_suffix(અચલ અક્ષર *name, અચલ અક્ષર *operation, UnitNameMangle flags, અચલ અક્ષર *suffix, અક્ષર **ret);

સ્થિર અંતઃરેખ પૂર્ણાંક unit_name_mangle(અચલ અક્ષર *name, UnitNameMangle flags, અક્ષર **ret) {
        પાછા unit_name_mangle_with_suffix(name, ખાલી, flags, ".service", ret);
}

પૂર્ણાંક slice_build_parent_slice(અચલ અક્ષર *slice, અક્ષર **ret);
પૂર્ણાંક slice_build_subslice(અચલ અક્ષર *slice, અચલ અક્ષર *name, અક્ષર **ret);
bool slice_name_is_valid(અચલ અક્ષર *name);

bool unit_name_prefix_equal(અચલ અક્ષર *a, અચલ અક્ષર *b);
