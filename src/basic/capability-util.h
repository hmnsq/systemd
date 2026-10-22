/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <linux/capability.h>   /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

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

પ્રકારવ્યાખ્યા સંરચના CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#define CAPABILITY_QUINTET_NULL         \
        (અચલ CapabilityQuintet) {     \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
                CAP_MASK_UNSET,         \
        }

સ્થિર અંતઃરેખ bool capability_is_set(uint64_t v) {
        પાછા v != CAP_MASK_UNSET;
}

પૂર્ણાંક capability_get(CapabilityQuintet *ret);

unsigned cap_last_cap(શૂન્ય);
પૂર્ણાંક have_effective_cap(unsigned cap);
પૂર્ણાંક have_inheritable_cap(unsigned cap);
પૂર્ણાંક capability_gain_cap_setpcap(શૂન્ય);
પૂર્ણાંક capability_bounding_set_drop(uint64_t keep, bool જમણું_now);
પૂર્ણાંક capability_bounding_set_drop_usermode(uint64_t keep);

પૂર્ણાંક capability_ambient_set_apply(uint64_t set, bool also_inherit);

પૂર્ણાંક drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

પૂર્ણાંક drop_capability(unsigned cap);
પૂર્ણાંક keep_capability(unsigned cap);

સ્થિર અંતઃરેખ uint64_t all_capabilities(શૂન્ય) {
        પાછા UINT64_MAX >> (63 - cap_last_cap());
}

સ્થિર અંતઃરેખ bool cap_test_all(uint64_t caps) {
        પાછા FLAGS_SET(caps, all_capabilities());
}

સ્થિર અંતઃરેખ bool capability_quintet_is_set(અચલ CapabilityQuintet *q) {
        પાછા capability_is_set(q->effective) ||
                capability_is_set(q->bounding) ||
                capability_is_set(q->inheritable) ||
                capability_is_set(q->permitted) ||
                capability_is_set(q->ambient);
}

સ્થિર અંતઃરેખ bool capability_quintet_is_fully_set(અચલ CapabilityQuintet *q) {
        પાછા capability_is_set(q->effective) &&
                capability_is_set(q->bounding) &&
                capability_is_set(q->inheritable) &&
                capability_is_set(q->permitted) &&
                capability_is_set(q->ambient);
}

સ્થિર અંતઃરેખ bool capability_quintet_equal(અચલ CapabilityQuintet *a, અચલ CapabilityQuintet *b) {
        પાછા a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

પૂર્ણાંક capability_quintet_enforce(અચલ CapabilityQuintet *q);

પૂર્ણાંક capability_get_ambient(uint64_t *ret);

પૂર્ણાંક pidref_get_capability(અચલ PidRef *pidref, CapabilityQuintet *ret);
