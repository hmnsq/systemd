/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <linux/capability.h>   /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

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

প্রকারনির্দেশ সংরচনা CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#define CAPABILITY_QUINTET_NULL         \
        (ধ্রুব CapabilityQuintet) {     \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
        }

স্থির অন্তঃস্থ bool capability_is_set(uint64_t v) {
        প্রত্যাগম v != CAP_MASK_UNSET;
}

পূর্ণাঙ্ক capability_get(CapabilityQuintet *ret);

unsigned cap_last_cap(শূন্য);
পূর্ণাঙ্ক have_effective_cap(unsigned cap);
পূর্ণাঙ্ক have_inheritable_cap(unsigned cap);
পূর্ণাঙ্ক capability_gain_cap_setpcap(শূন্য);
পূর্ণাঙ্ক capability_bounding_set_drop(uint64_t keep, bool ডান_now);
পূর্ণাঙ্ক capability_bounding_set_drop_usermode(uint64_t keep);

পূর্ণাঙ্ক capability_ambient_set_apply(uint64_t set, bool also_inherit);

পূর্ণাঙ্ক drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

পূর্ণাঙ্ক drop_capability(unsigned cap);
পূর্ণাঙ্ক keep_capability(unsigned cap);

স্থির অন্তঃস্থ uint64_t all_capabilities(শূন্য) {
        প্রত্যাগম UINT64_MAX >> (63 - cap_last_cap());
}

স্থির অন্তঃস্থ bool cap_test_all(uint64_t caps) {
        প্রত্যাগম FLAGS_SET(caps, all_capabilities());
}

স্থির অন্তঃস্থ bool capability_quintet_is_set(ধ্রুব CapabilityQuintet *q) {
        প্রত্যাগম capability_is_set(q->effective) ||
                capability_is_set(q->bounding) ||
                capability_is_set(q->inheritable) ||
                capability_is_set(q->permitted) ||
                capability_is_set(q->ambient);
}

স্থির অন্তঃস্থ bool capability_quintet_is_fully_set(ধ্রুব CapabilityQuintet *q) {
        প্রত্যাগম capability_is_set(q->effective) &&
                capability_is_set(q->bounding) &&
                capability_is_set(q->inheritable) &&
                capability_is_set(q->permitted) &&
                capability_is_set(q->ambient);
}

স্থির অন্তঃস্থ bool capability_quintet_equal(ধ্রুব CapabilityQuintet *a, ধ্রুব CapabilityQuintet *b) {
        প্রত্যাগম a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

পূর্ণাঙ্ক capability_quintet_enforce(ধ্রুব CapabilityQuintet *q);

পূর্ণাঙ্ক capability_get_ambient(uint64_t *ret);

পূর্ণাঙ্ক pidref_get_capability(ধ্রুব PidRef *pidref, CapabilityQuintet *ret);
