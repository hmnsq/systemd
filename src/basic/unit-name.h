/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

#define UNIT_NAME_MAX 256

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ଅଚଳ ଅକ୍ଷର *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(ଅଚଳ ଅକ୍ଷର *p) _pure_;
bool unit_instance_is_valid(ଅଚଳ ଅକ୍ଷର *i) _pure_;
bool unit_suffix_is_valid(ଅଚଳ ଅକ୍ଷର *s) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_prefix(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret);
UnitNameFlags unit_name_to_instance(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ UnitNameFlags unit_name_classify(ଅଚଳ ଅକ୍ଷର *n) {
        ଫେରନ୍ତୁ unit_name_to_instance(n, ଖାଲି);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_prefix_and_instance(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret);

UnitType unit_name_to_type(ଅଚଳ ଅକ୍ଷର *n) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_change_suffix(ଅଚଳ ଅକ୍ଷର *n, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_build(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *instance, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_build_from_type(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *instance, UnitType type, ଅକ୍ଷର **ret);

ଅକ୍ଷର* unit_name_escape(ଅଚଳ ଅକ୍ଷର *f);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_unescape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_path_escape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_path_unescape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_replace_instance_full(
                ଅଚଳ ଅକ୍ଷର *original,
                ଅଚଳ ଅକ୍ଷର *instance,
                bool accept_glob,
                ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_replace_instance(ଅଚଳ ଅକ୍ଷର *original, ଅଚଳ ଅକ୍ଷର *instance, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ unit_name_replace_instance_full(original, instance, false, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_template(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_hash_long(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret);
bool unit_name_is_hashed(ଅଚଳ ଅକ୍ଷର *name);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_from_path(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_from_path_instance(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_path(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_mangle_with_suffix(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଅକ୍ଷର *operation, UnitNameMangle flags, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_mangle(ଅଚଳ ଅକ୍ଷର *name, UnitNameMangle flags, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ unit_name_mangle_with_suffix(name, ଖାଲି, flags, ".service", ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା slice_build_parent_slice(ଅଚଳ ଅକ୍ଷର *slice, ଅକ୍ଷର **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା slice_build_subslice(ଅଚଳ ଅକ୍ଷର *slice, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret);
bool slice_name_is_valid(ଅଚଳ ଅକ୍ଷର *name);

bool unit_name_prefix_equal(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b);
