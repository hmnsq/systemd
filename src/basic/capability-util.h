/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <linux/capability.h>   /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#परिभाषय CAP_आवरणम्_UNSET UINT64_MAX

/* All possible capabilities bits on */
#परिभाषय CAP_आवरणम्_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#परिभाषय CAP_सीमा 62
assert_cc(CAP_LAST_CAP <= CAP_सीमा);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#परिभाषय CAP_गन्तव्यम्_आवरणम्_CORRECTED(x) (1U << ((x) & 31U))

प्रकारनिर्देश संरचना CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#परिभाषय CAPABILITY_QUINTET_NULL         \
        (ध्रुव CapabilityQuintet) {     \
                CAP_आवरणम्_UNSET,         \
                CAP_आवरणम्_UNSET,         \
                CAP_आवरणम्_UNSET,         \
                CAP_आवरणम्_UNSET,         \
                CAP_आवरणम्_UNSET,         \
        }

स्थिर अन्तःस्थ bool capability_is_समुच्चयः(uint64_t v) {
        प्रत्यागम v != CAP_आवरणम्_UNSET;
}

पूर्णाङ्क capability_get(CapabilityQuintet *ret);

अचिह्नित cap_चरमः_cap(शून्य);
पूर्णाङ्क have_effective_cap(अचिह्नित cap);
पूर्णाङ्क have_inheritable_cap(अचिह्नित cap);
पूर्णाङ्क capability_gain_cap_setpcap(शून्य);
पूर्णाङ्क capability_bounding_समुच्चयः_drop(uint64_t keep, bool दक्षिणः_now);
पूर्णाङ्क capability_bounding_समुच्चयः_drop_usermode(uint64_t keep);

पूर्णाङ्क capability_ambient_समुच्चयः_apply(uint64_t समुच्चयः, bool also_inherit);

पूर्णाङ्क drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

पूर्णाङ्क drop_capability(अचिह्नित cap);
पूर्णाङ्क keep_capability(अचिह्नित cap);

स्थिर अन्तःस्थ uint64_t all_capabilities(शून्य) {
        प्रत्यागम UINT64_MAX >> (63 - cap_चरमः_cap());
}

स्थिर अन्तःस्थ bool cap_test_all(uint64_t caps) {
        प्रत्यागम FLAGS_समुच्चयः(caps, all_capabilities());
}

स्थिर अन्तःस्थ bool capability_quintet_is_समुच्चयः(ध्रुव CapabilityQuintet *q) {
        प्रत्यागम capability_is_समुच्चयः(q->effective) ||
                capability_is_समुच्चयः(q->bounding) ||
                capability_is_समुच्चयः(q->inheritable) ||
                capability_is_समुच्चयः(q->permitted) ||
                capability_is_समुच्चयः(q->ambient);
}

स्थिर अन्तःस्थ bool capability_quintet_is_fully_समुच्चयः(ध्रुव CapabilityQuintet *q) {
        प्रत्यागम capability_is_समुच्चयः(q->effective) &&
                capability_is_समुच्चयः(q->bounding) &&
                capability_is_समुच्चयः(q->inheritable) &&
                capability_is_समुच्चयः(q->permitted) &&
                capability_is_समुच्चयः(q->ambient);
}

स्थिर अन्तःस्थ bool capability_quintet_equal(ध्रुव CapabilityQuintet *a, ध्रुव CapabilityQuintet *b) {
        प्रत्यागम a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

पूर्णाङ्क capability_quintet_enforce(ध्रुव CapabilityQuintet *q);

पूर्णाङ्क capability_get_ambient(uint64_t *ret);

पूर्णाङ्क pidref_get_capability(ध्रुव PidRef *pidref, CapabilityQuintet *ret);
