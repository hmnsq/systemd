/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <linux/capability.h>   /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ਨਕਾਬ_UNSET UINT64_MAX

/* All possible capabilities bits on */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ਨਕਾਬ_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ਹੱਦਬੰਦੀ 62
assert_cc(CAP_LAST_CAP <= CAP_ਹੱਦਬੰਦੀ);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ਮੰਜ਼ਿਲ_ਨਕਾਬ_CORRECTED(x) (1U << ((x) & 31U))

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAPABILITY_QUINTET_NULL         \
        (ਅਚਲ CapabilityQuintet) {     \
                CAP_ਨਕਾਬ_UNSET,         \
                CAP_ਨਕਾਬ_UNSET,         \
                CAP_ਨਕਾਬ_UNSET,         \
                CAP_ਨਕਾਬ_UNSET,         \
                CAP_ਨਕਾਬ_UNSET,         \
        }

ਸਥਿਰ ਸਤਰਅੰਦਰ bool capability_is_ਸੈੱਟ(uint64_t v) {
        ਵਾਪਸ v != CAP_ਨਕਾਬ_UNSET;
}

ਪੂਰਨ capability_get(CapabilityQuintet *ret);

ਬਿਨਾਨਿਸ਼ਾਨੀ cap_ਆਖਰੀ_cap(ਖਾਲੀ);
ਪੂਰਨ have_effective_cap(ਬਿਨਾਨਿਸ਼ਾਨੀ cap);
ਪੂਰਨ have_inheritable_cap(ਬਿਨਾਨਿਸ਼ਾਨੀ cap);
ਪੂਰਨ capability_gain_cap_setpcap(ਖਾਲੀ);
ਪੂਰਨ capability_bounding_ਸੈੱਟ_drop(uint64_t keep, bool ਸੱਜਾ_now);
ਪੂਰਨ capability_bounding_ਸੈੱਟ_drop_usermode(uint64_t keep);

ਪੂਰਨ capability_ambient_ਸੈੱਟ_apply(uint64_t ਸੈੱਟ, bool also_inherit);

ਪੂਰਨ drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

ਪੂਰਨ drop_capability(ਬਿਨਾਨਿਸ਼ਾਨੀ cap);
ਪੂਰਨ keep_capability(ਬਿਨਾਨਿਸ਼ਾਨੀ cap);

ਸਥਿਰ ਸਤਰਅੰਦਰ uint64_t all_capabilities(ਖਾਲੀ) {
        ਵਾਪਸ UINT64_MAX >> (63 - cap_ਆਖਰੀ_cap());
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool cap_test_all(uint64_t caps) {
        ਵਾਪਸ FLAGS_ਸੈੱਟ(caps, all_capabilities());
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool capability_quintet_is_ਸੈੱਟ(ਅਚਲ CapabilityQuintet *q) {
        ਵਾਪਸ capability_is_ਸੈੱਟ(q->effective) ||
                capability_is_ਸੈੱਟ(q->bounding) ||
                capability_is_ਸੈੱਟ(q->inheritable) ||
                capability_is_ਸੈੱਟ(q->permitted) ||
                capability_is_ਸੈੱਟ(q->ambient);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool capability_quintet_is_fully_ਸੈੱਟ(ਅਚਲ CapabilityQuintet *q) {
        ਵਾਪਸ capability_is_ਸੈੱਟ(q->effective) &&
                capability_is_ਸੈੱਟ(q->bounding) &&
                capability_is_ਸੈੱਟ(q->inheritable) &&
                capability_is_ਸੈੱਟ(q->permitted) &&
                capability_is_ਸੈੱਟ(q->ambient);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool capability_quintet_equal(ਅਚਲ CapabilityQuintet *a, ਅਚਲ CapabilityQuintet *b) {
        ਵਾਪਸ a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

ਪੂਰਨ capability_quintet_enforce(ਅਚਲ CapabilityQuintet *q);

ਪੂਰਨ capability_get_ambient(uint64_t *ret);

ਪੂਰਨ pidref_get_capability(ਅਚਲ PidRef *pidref, CapabilityQuintet *ret);
