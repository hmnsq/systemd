/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

#define UNIT_NAME_MAX 256

തരനിർവചനം എണ്ണൽ UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(സ്ഥിരം അക്ഷരം *n, UnitNameFlags flags) _pure_;
bool unit_prefix_is_valid(സ്ഥിരം അക്ഷരം *p) _pure_;
bool unit_instance_is_valid(സ്ഥിരം അക്ഷരം *i) _pure_;
bool unit_suffix_is_valid(സ്ഥിരം അക്ഷരം *s) _pure_;

പൂർണ്ണസംഖ്യ unit_name_to_prefix(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret);
UnitNameFlags unit_name_to_instance(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ UnitNameFlags unit_name_classify(സ്ഥിരം അക്ഷരം *n) {
        തിരികെ unit_name_to_instance(n, ശൂന്യ);
}
പൂർണ്ണസംഖ്യ unit_name_to_prefix_and_instance(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret);

UnitType unit_name_to_type(സ്ഥിരം അക്ഷരം *n) _pure_;

പൂർണ്ണസംഖ്യ unit_name_change_suffix(സ്ഥിരം അക്ഷരം *n, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ unit_name_build(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *instance, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ unit_name_build_from_type(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *instance, UnitType type, അക്ഷരം **ret);

അക്ഷരം* unit_name_escape(സ്ഥിരം അക്ഷരം *f);
പൂർണ്ണസംഖ്യ unit_name_unescape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ unit_name_path_escape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ unit_name_path_unescape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ unit_name_replace_instance_full(
                സ്ഥിരം അക്ഷരം *original,
                സ്ഥിരം അക്ഷരം *instance,
                bool accept_glob,
                അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ unit_name_replace_instance(സ്ഥിരം അക്ഷരം *original, സ്ഥിരം അക്ഷരം *instance, അക്ഷരം **ret) {
        തിരികെ unit_name_replace_instance_full(original, instance, false, ret);
}

പൂർണ്ണസംഖ്യ unit_name_template(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ unit_name_hash_long(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret);
bool unit_name_is_hashed(സ്ഥിരം അക്ഷരം *name);

പൂർണ്ണസംഖ്യ unit_name_from_path(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ unit_name_from_path_instance(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ unit_name_to_path(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret);

തരനിർവചനം എണ്ണൽ UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

പൂർണ്ണസംഖ്യ unit_name_mangle_with_suffix(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *operation, UnitNameMangle flags, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret);

സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ unit_name_mangle(സ്ഥിരം അക്ഷരം *name, UnitNameMangle flags, അക്ഷരം **ret) {
        തിരികെ unit_name_mangle_with_suffix(name, ശൂന്യ, flags, ".service", ret);
}

പൂർണ്ണസംഖ്യ slice_build_parent_slice(സ്ഥിരം അക്ഷരം *slice, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ slice_build_subslice(സ്ഥിരം അക്ഷരം *slice, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret);
bool slice_name_is_valid(സ്ഥിരം അക്ഷരം *name);

bool unit_name_prefix_equal(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b);
