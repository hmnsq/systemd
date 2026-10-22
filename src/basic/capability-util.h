/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن <linux/capability.h>   /* IWYU pragma: export */

#ضمن "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#وصف_ڪريو CAP_نقاب_UNSET UINT64_MAX

/* All possible capabilities bits on */
#وصف_ڪريو CAP_نقاب_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#وصف_ڪريو CAP_حد_بندي 62
assert_cc(CAP_LAST_CAP <= CAP_حد_بندي);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#وصف_ڪريو CAP_منزل_نقاب_CORRECTED(x) (1U << ((x) & 31U))

تعريفنوع بنية CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#وصف_ڪريو CAPABILITY_QUINTET_NULL         \
        (ثابت CapabilityQuintet) {     \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
        }

ساكن مضمن bool capability_is_سيٽ(uint64_t v) {
        أرجع v != CAP_نقاب_UNSET;
}

صحيح capability_get(CapabilityQuintet *ret);

بنانشان cap_آخري_cap(فراغ);
صحيح have_effective_cap(بنانشان cap);
صحيح have_inheritable_cap(بنانشان cap);
صحيح capability_gain_cap_setpcap(فراغ);
صحيح capability_bounding_سيٽ_drop(uint64_t keep, bool ساڄو_now);
صحيح capability_bounding_سيٽ_drop_usermode(uint64_t keep);

صحيح capability_ambient_سيٽ_apply(uint64_t سيٽ, bool also_inherit);

صحيح drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

صحيح drop_capability(بنانشان cap);
صحيح keep_capability(بنانشان cap);

ساكن مضمن uint64_t all_capabilities(فراغ) {
        أرجع UINT64_MAX >> (63 - cap_آخري_cap());
}

ساكن مضمن bool cap_test_all(uint64_t caps) {
        أرجع FLAGS_سيٽ(caps, all_capabilities());
}

ساكن مضمن bool capability_quintet_is_سيٽ(ثابت CapabilityQuintet *q) {
        أرجع capability_is_سيٽ(q->effective) ||
                capability_is_سيٽ(q->bounding) ||
                capability_is_سيٽ(q->inheritable) ||
                capability_is_سيٽ(q->permitted) ||
                capability_is_سيٽ(q->ambient);
}

ساكن مضمن bool capability_quintet_is_fully_سيٽ(ثابت CapabilityQuintet *q) {
        أرجع capability_is_سيٽ(q->effective) &&
                capability_is_سيٽ(q->bounding) &&
                capability_is_سيٽ(q->inheritable) &&
                capability_is_سيٽ(q->permitted) &&
                capability_is_سيٽ(q->ambient);
}

ساكن مضمن bool capability_quintet_equal(ثابت CapabilityQuintet *a, ثابت CapabilityQuintet *b) {
        أرجع a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

صحيح capability_quintet_enforce(ثابت CapabilityQuintet *q);

صحيح capability_get_ambient(uint64_t *ret);

صحيح pidref_get_capability(ثابت PidRef *pidref, CapabilityQuintet *ret);
