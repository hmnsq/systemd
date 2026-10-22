/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <linux/capability.h>   /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#define CAPABILITY_QUINTET_NULL         \
        (ಧ್ರುವ CapabilityQuintet) {     \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
        }

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool capability_is_set(uint64_t v) {
        ಪ್ರತ್ಯಾಗಮ v != CAP_MASK_UNSET;
}

ಪೂರ್ಣಾಙ್ಕ capability_get(CapabilityQuintet *ret);

unsigned cap_last_cap(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ have_effective_cap(unsigned cap);
ಪೂರ್ಣಾಙ್ಕ have_inheritable_cap(unsigned cap);
ಪೂರ್ಣಾಙ್ಕ capability_gain_cap_setpcap(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ capability_bounding_set_drop(uint64_t keep, bool ಬಲ_now);
ಪೂರ್ಣಾಙ್ಕ capability_bounding_set_drop_usermode(uint64_t keep);

ಪೂರ್ಣಾಙ್ಕ capability_ambient_set_apply(uint64_t set, bool also_inherit);

ಪೂರ್ಣಾಙ್ಕ drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

ಪೂರ್ಣಾಙ್ಕ drop_capability(unsigned cap);
ಪೂರ್ಣಾಙ್ಕ keep_capability(unsigned cap);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ uint64_t all_capabilities(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ UINT64_MAX >> (63 - cap_last_cap());
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool cap_test_all(uint64_t caps) {
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(caps, all_capabilities());
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool capability_quintet_is_set(ಧ್ರುವ CapabilityQuintet *q) {
        ಪ್ರತ್ಯಾಗಮ capability_is_set(q->effective) ||
                capability_is_set(q->bounding) ||
                capability_is_set(q->inheritable) ||
                capability_is_set(q->permitted) ||
                capability_is_set(q->ambient);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool capability_quintet_is_fully_set(ಧ್ರುವ CapabilityQuintet *q) {
        ಪ್ರತ್ಯಾಗಮ capability_is_set(q->effective) &&
                capability_is_set(q->bounding) &&
                capability_is_set(q->inheritable) &&
                capability_is_set(q->permitted) &&
                capability_is_set(q->ambient);
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool capability_quintet_equal(ಧ್ರುವ CapabilityQuintet *a, ಧ್ರುವ CapabilityQuintet *b) {
        ಪ್ರತ್ಯಾಗಮ a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

ಪೂರ್ಣಾಙ್ಕ capability_quintet_enforce(ಧ್ರುವ CapabilityQuintet *q);

ಪೂರ್ಣಾಙ್ಕ capability_get_ambient(uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ pidref_get_capability(ಧ್ರುವ PidRef *pidref, CapabilityQuintet *ret);
