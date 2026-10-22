/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNIT_NAME_MAX 256

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitNameFlags {
        UNIT_NAME_PLAIN    = 1 << 0, /* Allow foo.service */
        UNIT_NAME_TEMPLATE = 1 << 1, /* Allow foo@.service */
        UNIT_NAME_INSTANCE = 1 << 2, /* Allow foo@bar.service */
        UNIT_NAME_ANY = UNIT_NAME_PLAIN|UNIT_NAME_TEMPLATE|UNIT_NAME_INSTANCE,
        _UNIT_NAME_INVALID = -EINVAL,
} UnitNameFlags;

bool unit_name_is_valid(ਅਚਲ ਅੱਖਰ *n, UnitNameFlags flags) ਸ਼ੁੱਧ;
bool unit_ਅਗੇਤਰ_is_valid(ਅਚਲ ਅੱਖਰ *p) ਸ਼ੁੱਧ;
bool unit_instance_is_valid(ਅਚਲ ਅੱਖਰ *i) ਸ਼ੁੱਧ;
bool unit_suffix_is_valid(ਅਚਲ ਅੱਖਰ *s) ਸ਼ੁੱਧ;

ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret);
UnitNameFlags unit_name_ਮੰਜ਼ਿਲ_instance(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ UnitNameFlags unit_name_classify(ਅਚਲ ਅੱਖਰ *n) {
        ਵਾਪਸ unit_name_ਮੰਜ਼ਿਲ_instance(n, ਨਿਰਾ);
}
ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਅਗੇਤਰ_and_instance(ਅਚਲ ਅੱਖਰ *n, ਅੱਖਰ **ret);

UnitType unit_name_ਮੰਜ਼ਿਲ_type(ਅਚਲ ਅੱਖਰ *n) ਸ਼ੁੱਧ;

ਪੂਰਨ unit_name_change_suffix(ਅਚਲ ਅੱਖਰ *n, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);

ਪੂਰਨ unit_name_build(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *instance, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);
ਪੂਰਨ unit_name_build_from_type(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *instance, UnitType type, ਅੱਖਰ **ret);

ਅੱਖਰ* unit_name_escape(ਅਚਲ ਅੱਖਰ *f);
ਪੂਰਨ unit_name_unescape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret);
ਪੂਰਨ unit_name_ਰਸਤਾ_escape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret);
ਪੂਰਨ unit_name_ਰਸਤਾ_unescape(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret);

ਪੂਰਨ unit_name_replace_instance_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *original,
                ਅਚਲ ਅੱਖਰ *instance,
                bool accept_glob,
                ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ unit_name_replace_instance(ਅਚਲ ਅੱਖਰ *original, ਅਚਲ ਅੱਖਰ *instance, ਅੱਖਰ **ret) {
        ਵਾਪਸ unit_name_replace_instance_ਭਰਿਆ(original, instance, false, ret);
}

ਪੂਰਨ unit_name_template(ਅਚਲ ਅੱਖਰ *f, ਅੱਖਰ **ret);

ਪੂਰਨ unit_name_hash_long(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret);
bool unit_name_is_hashed(ਅਚਲ ਅੱਖਰ *name);

ਪੂਰਨ unit_name_from_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);
ਪੂਰਨ unit_name_from_ਰਸਤਾ_instance(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);
ਪੂਰਨ unit_name_ਮੰਜ਼ਿਲ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ UnitNameMangle {
        UNIT_NAME_MANGLE_GLOB   = 1 << 0,
        UNIT_NAME_MANGLE_WARN   = 1 << 1,
        UNIT_NAME_MANGLE_STRICT = 1 << 2, /* Refuse if the resolved unit type doesn't match the requested suffix */
} UnitNameMangle;

ਪੂਰਨ unit_name_mangle_with_suffix(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *operation, UnitNameMangle flags, ਅਚਲ ਅੱਖਰ *suffix, ਅੱਖਰ **ret);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ unit_name_mangle(ਅਚਲ ਅੱਖਰ *name, UnitNameMangle flags, ਅੱਖਰ **ret) {
        ਵਾਪਸ unit_name_mangle_with_suffix(name, ਨਿਰਾ, flags, ".service", ret);
}

ਪੂਰਨ ਟੁਕੜਾ_build_ਮਾਪਾ_ਟੁਕੜਾ(ਅਚਲ ਅੱਖਰ *ਟੁਕੜਾ, ਅੱਖਰ **ret);
ਪੂਰਨ ਟੁਕੜਾ_build_subslice(ਅਚਲ ਅੱਖਰ *ਟੁਕੜਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret);
bool ਟੁਕੜਾ_name_is_valid(ਅਚਲ ਅੱਖਰ *name);

bool unit_name_ਅਗੇਤਰ_equal(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b);
