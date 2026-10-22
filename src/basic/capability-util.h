/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <linux/capability.h>   /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ಮುಖವಾಡ_UNSET UINT64_MAX

/* All possible capabilities bits on */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ಮುಖವಾಡ_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ಎಲ್ಲೆ 62
assert_cc(CAP_LAST_CAP <= CAP_ಎಲ್ಲೆ);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ಗಮ್ಯ_ಮುಖವಾಡ_CORRECTED(x) (1U << ((x) & 31U))

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#ವ್ಯಾಖ್ಯಾನಿಸು CAPABILITY_QUINTET_NULL         \
        (ಸ್ಥಿರಾಂಕ CapabilityQuintet) {     \
                CAP_ಮುಖವಾಡ_UNSET,         \
                CAP_ಮುಖವಾಡ_UNSET,         \
                CAP_ಮುಖವಾಡ_UNSET,         \
                CAP_ಮುಖವಾಡ_UNSET,         \
                CAP_ಮುಖವಾಡ_UNSET,         \
        }

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool capability_is_ಗಣ(uint64_t v) {
        ಹಿಂತಿರುಗಿ v != CAP_ಮುಖವಾಡ_UNSET;
}

ಪೂರ್ಣಾಂಕ capability_get(CapabilityQuintet *ret);

ಚಿಹ್ನೆರಹಿತ cap_ಕೊನೆಯ_cap(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ have_effective_cap(ಚಿಹ್ನೆರಹಿತ cap);
ಪೂರ್ಣಾಂಕ have_inheritable_cap(ಚಿಹ್ನೆರಹಿತ cap);
ಪೂರ್ಣಾಂಕ capability_gain_cap_setpcap(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ capability_bounding_ಗಣ_drop(uint64_t keep, bool ಬಲ_now);
ಪೂರ್ಣಾಂಕ capability_bounding_ಗಣ_drop_usermode(uint64_t keep);

ಪೂರ್ಣಾಂಕ capability_ambient_ಗಣ_apply(uint64_t ಗಣ, bool also_inherit);

ಪೂರ್ಣಾಂಕ drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

ಪೂರ್ಣಾಂಕ drop_capability(ಚಿಹ್ನೆರಹಿತ cap);
ಪೂರ್ಣಾಂಕ keep_capability(ಚಿಹ್ನೆರಹಿತ cap);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint64_t all_capabilities(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ UINT64_MAX >> (63 - cap_ಕೊನೆಯ_cap());
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool cap_test_all(uint64_t caps) {
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(caps, all_capabilities());
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool capability_quintet_is_ಗಣ(ಸ್ಥಿರಾಂಕ CapabilityQuintet *q) {
        ಹಿಂತಿರುಗಿ capability_is_ಗಣ(q->effective) ||
                capability_is_ಗಣ(q->bounding) ||
                capability_is_ಗಣ(q->inheritable) ||
                capability_is_ಗಣ(q->permitted) ||
                capability_is_ಗಣ(q->ambient);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool capability_quintet_is_fully_ಗಣ(ಸ್ಥಿರಾಂಕ CapabilityQuintet *q) {
        ಹಿಂತಿರುಗಿ capability_is_ಗಣ(q->effective) &&
                capability_is_ಗಣ(q->bounding) &&
                capability_is_ಗಣ(q->inheritable) &&
                capability_is_ಗಣ(q->permitted) &&
                capability_is_ಗಣ(q->ambient);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool capability_quintet_equal(ಸ್ಥಿರಾಂಕ CapabilityQuintet *a, ಸ್ಥಿರಾಂಕ CapabilityQuintet *b) {
        ಹಿಂತಿರುಗಿ a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

ಪೂರ್ಣಾಂಕ capability_quintet_enforce(ಸ್ಥಿರಾಂಕ CapabilityQuintet *q);

ಪೂರ್ಣಾಂಕ capability_get_ambient(uint64_t *ret);

ಪೂರ್ಣಾಂಕ pidref_get_capability(ಸ್ಥಿರಾಂಕ PidRef *pidref, CapabilityQuintet *ret);
