/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <linux/capability.h>   /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#నిర్వచించు CAP_ముసుగు_UNSET UINT64_MAX

/* All possible capabilities bits on */
#నిర్వచించు CAP_ముసుగు_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#నిర్వచించు CAP_హద్దు 62
assert_cc(CAP_LAST_CAP <= CAP_హద్దు);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#నిర్వచించు CAP_గమ్యం_ముసుగు_CORRECTED(x) (1U << ((x) & 31U))

రకనిర్వచనం నిర్మాణం CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#నిర్వచించు CAPABILITY_QUINTET_NULL         \
        (స్థిరరాశి CapabilityQuintet) {     \
                CAP_ముసుగు_UNSET,         \
                CAP_ముసుగు_UNSET,         \
                CAP_ముసుగు_UNSET,         \
                CAP_ముసుగు_UNSET,         \
                CAP_ముసుగు_UNSET,         \
        }

స్థిర అంతఃవరుస bool capability_is_సమితి(uint64_t v) {
        తిరిగి v != CAP_ముసుగు_UNSET;
}

పూర్ణాంకం capability_get(CapabilityQuintet *ret);

గుర్తులేని cap_చివరి_cap(శూన్య);
పూర్ణాంకం have_effective_cap(గుర్తులేని cap);
పూర్ణాంకం have_inheritable_cap(గుర్తులేని cap);
పూర్ణాంకం capability_gain_cap_setpcap(శూన్య);
పూర్ణాంకం capability_bounding_సమితి_drop(uint64_t keep, bool కుడి_now);
పూర్ణాంకం capability_bounding_సమితి_drop_usermode(uint64_t keep);

పూర్ణాంకం capability_ambient_సమితి_apply(uint64_t సమితి, bool also_inherit);

పూర్ణాంకం drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

పూర్ణాంకం drop_capability(గుర్తులేని cap);
పూర్ణాంకం keep_capability(గుర్తులేని cap);

స్థిర అంతఃవరుస uint64_t all_capabilities(శూన్య) {
        తిరిగి UINT64_MAX >> (63 - cap_చివరి_cap());
}

స్థిర అంతఃవరుస bool cap_test_all(uint64_t caps) {
        తిరిగి FLAGS_సమితి(caps, all_capabilities());
}

స్థిర అంతఃవరుస bool capability_quintet_is_సమితి(స్థిరరాశి CapabilityQuintet *q) {
        తిరిగి capability_is_సమితి(q->effective) ||
                capability_is_సమితి(q->bounding) ||
                capability_is_సమితి(q->inheritable) ||
                capability_is_సమితి(q->permitted) ||
                capability_is_సమితి(q->ambient);
}

స్థిర అంతఃవరుస bool capability_quintet_is_fully_సమితి(స్థిరరాశి CapabilityQuintet *q) {
        తిరిగి capability_is_సమితి(q->effective) &&
                capability_is_సమితి(q->bounding) &&
                capability_is_సమితి(q->inheritable) &&
                capability_is_సమితి(q->permitted) &&
                capability_is_సమితి(q->ambient);
}

స్థిర అంతఃవరుస bool capability_quintet_equal(స్థిరరాశి CapabilityQuintet *a, స్థిరరాశి CapabilityQuintet *b) {
        తిరిగి a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

పూర్ణాంకం capability_quintet_enforce(స్థిరరాశి CapabilityQuintet *q);

పూర్ణాంకం capability_get_ambient(uint64_t *ret);

పూర్ణాంకం pidref_get_capability(స్థిరరాశి PidRef *pidref, CapabilityQuintet *ret);
