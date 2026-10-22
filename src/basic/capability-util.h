/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <linux/capability.h>   /* IWYU pragma: export */

#شامل "forward.h"

/* Special marker used when storing a capabilities mask as "unset". This would need to be updated as soon as
 * Linux learns more than 63 caps. */
#تعریف_کرو CAP_نقاب_UNSET UINT64_MAX

/* All possible capabilities bits on */
#تعریف_کرو CAP_نقاب_ALL UINT64_C(0x7fffffffffffffff)

/* The largest capability we can deal with, given we want to be able to store cap masks in uint64_t but still
 * be able to use UINT64_MAX as indicator for "not set". The latter makes capability 63 unavailable. */
#تعریف_کرو CAP_حد_بندی 62
assert_cc(CAP_LAST_CAP <= CAP_حد_بندی);

/* Identical to linux/capability.h's CAP_TO_MASK(), but uses an unsigned 1U instead of a signed 1 for shifting left, in
 * order to avoid complaints about shifting a signed int left by 31 bits, which would make it negative. */
#تعریف_کرو CAP_مقصد_نقاب_CORRECTED(x) (1U << ((x) & 31U))

قسمتعریف ساخت CapabilityQuintet {
        /* Stores all five types of capabilities in one go. */
        uint64_t effective;
        uint64_t bounding;
        uint64_t inheritable;
        uint64_t permitted;
        uint64_t ambient;
} CapabilityQuintet;

#تعریف_کرو CAPABILITY_QUINTET_NULL         \
        (ثابت CapabilityQuintet) {     \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
                CAP_نقاب_UNSET,         \
        }

ساکن اندرونی bool capability_is_سیٹ(uint64_t v) {
        واپس v != CAP_نقاب_UNSET;
}

صحیح capability_get(CapabilityQuintet *ret);

غیرعلامتی cap_آخری_cap(خالی);
صحیح have_effective_cap(غیرعلامتی cap);
صحیح have_inheritable_cap(غیرعلامتی cap);
صحیح capability_gain_cap_setpcap(خالی);
صحیح capability_bounding_سیٹ_drop(uint64_t keep, bool دایاں_now);
صحیح capability_bounding_سیٹ_drop_usermode(uint64_t keep);

صحیح capability_ambient_سیٹ_apply(uint64_t سیٹ, bool also_inherit);

صحیح drop_privileges(uid_t uid, gid_t gid, uint64_t keep_capabilities);

صحیح drop_capability(غیرعلامتی cap);
صحیح keep_capability(غیرعلامتی cap);

ساکن اندرونی uint64_t all_capabilities(خالی) {
        واپس UINT64_MAX >> (63 - cap_آخری_cap());
}

ساکن اندرونی bool cap_test_all(uint64_t caps) {
        واپس FLAGS_سیٹ(caps, all_capabilities());
}

ساکن اندرونی bool capability_quintet_is_سیٹ(ثابت CapabilityQuintet *q) {
        واپس capability_is_سیٹ(q->effective) ||
                capability_is_سیٹ(q->bounding) ||
                capability_is_سیٹ(q->inheritable) ||
                capability_is_سیٹ(q->permitted) ||
                capability_is_سیٹ(q->ambient);
}

ساکن اندرونی bool capability_quintet_is_fully_سیٹ(ثابت CapabilityQuintet *q) {
        واپس capability_is_سیٹ(q->effective) &&
                capability_is_سیٹ(q->bounding) &&
                capability_is_سیٹ(q->inheritable) &&
                capability_is_سیٹ(q->permitted) &&
                capability_is_سیٹ(q->ambient);
}

ساکن اندرونی bool capability_quintet_equal(ثابت CapabilityQuintet *a, ثابت CapabilityQuintet *b) {
        واپس a->effective == b->effective &&
                a->bounding == b->bounding &&
                a->inheritable == b->inheritable &&
                a->permitted == b->permitted &&
                a->ambient == b->ambient;
}

/* Mangles the specified caps quintet taking the current bounding set into account:
 * drops all caps from all five sets if our bounding set doesn't allow them.
 * Returns true if the quintet was modified. */
bool capability_quintet_mangle(CapabilityQuintet *q);

صحیح capability_quintet_enforce(ثابت CapabilityQuintet *q);

صحیح capability_get_ambient(uint64_t *ret);

صحیح pidref_get_capability(ثابت PidRef *pidref, CapabilityQuintet *ret);
