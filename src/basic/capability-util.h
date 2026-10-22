/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <linux/capability.h>   /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#define CAP_MASK_UNSET UINT64_MAX

/* All possible capabilities bits on */
#define CAP_MASK_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#define CAP_LIMIT 62
assert_cc(CAP_LAST_CAP <= CAP_LIMIT);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#define CAP_TO_MASK_CORRECTED(x) (1U << ((x) & 31U))

வகைவரை கட்டமைப்பு CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#define CAPABILITY_QUINTET_NULL         \
        (மாறிலி CapabilityQuintet) {     \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
        }

நிலையான உள்வரி bool capability_is_set(uint64_t v) {
        திரும்பு v != CAP_MASK_UNSET;
}

முழு capability_get(CapabilityQuintet *ret);

unsigned cap_last_cap(வெறுமை);
முழு have_effective_cap(unsigned cap);
முழு have_inheritable_cap(unsigned cap);
முழு capability_gain_cap_setpcap(வெறுமை);
முழு capability_bounding_set_drop(uint64_t keep, bool வலது_now);
முழு capability_bounding_set_drop_usermode(uint64_t keep);

முழு capability_ambient_set_apply(uint64_t set, bool also_inherit);

முழு drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

முழு drop_capability(unsigned cap);
முழு keep_capability(unsigned cap);

நிலையான உள்வரி uint64_t all_capabilities(வெறுமை) {
        திரும்பு UINT64_MAX >> (63 - cap_last_cap());
}

நிலையான உள்வரி bool cap_test_all(uint64_t caps) {
        திரும்பு FLAGS_SET(caps, all_capabilities());
}

நிலையான உள்வரி bool capability_quintet_is_set(மாறிலி CapabilityQuintet *q) {
        திரும்பு capability_is_set(q->effective) ||
                capability_is_set(q->bounding) ||
                capability_is_set(q->inheritable) ||
                capability_is_set(q->permitted) ||
                capability_is_set(q->ambient);
}

நிலையான உள்வரி bool capability_quintet_is_fully_set(மாறிலி CapabilityQuintet *q) {
        திரும்பு capability_is_set(q->effective) &&
                capability_is_set(q->bounding) &&
                capability_is_set(q->inheritable) &&
                capability_is_set(q->permitted) &&
                capability_is_set(q->ambient);
}

நிலையான உள்வரி bool capability_quintet_equal(மாறிலி CapabilityQuintet *a, மாறிலி CapabilityQuintet *b) {
        திரும்பு a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

முழு capability_quintet_enforce(மாறிலி CapabilityQuintet *q);

முழு capability_get_ambient(uint64_t *ret);

முழு pidref_get_capability(மாறிலி PidRef *pidref, CapabilityQuintet *ret);
