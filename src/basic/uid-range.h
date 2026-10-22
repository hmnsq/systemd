/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை கட்டமைப்பு UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

வகைவரை கட்டமைப்பு UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

முழு uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
நிலையான உள்வரி முழு uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        திரும்பு uid_range_add_internal(range, start, nr, true);
}
முழு uid_range_add_str_full(UIDRange **range, மாறிலி எழுத்து *s, bool coalesce);
நிலையான உள்வரி முழு uid_range_add_str(UIDRange **range, மாறிலி எழுத்து *s) {
        திரும்பு uid_range_add_str_full(range, s, true);
}

முழு uid_range_next_lower(மாறிலி UIDRange *range, uid_t *uid);

bool uid_range_covers(மாறிலி UIDRange *range, uid_t start, uid_t nr);
நிலையான உள்வரி bool uid_range_contains(மாறிலி UIDRange *range, uid_t uid) {
        திரும்பு uid_range_covers(range, uid, 1);
}

முழு uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

நிலையான உள்வரி size_t uid_range_entries(மாறிலி UIDRange *range) {
        திரும்பு range ? range->n_entries : 0;
}

unsigned uid_range_size(மாறிலி UIDRange *range) _pure_;
bool uid_range_is_empty(மாறிலி UIDRange *range) _pure_;

bool uid_range_equal(மாறிலி UIDRange *a, மாறிலி UIDRange *b);

வகைவரை எண்ணிக்கை UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

முழு uid_range_load_userns_full(மாறிலி எழுத்து *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
நிலையான உள்வரி முழு uid_range_load_userns(மாறிலி எழுத்து *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        திரும்பு uid_range_load_userns_full(path, mode, true, ret);
}
முழு uid_range_load_userns_by_fd_full(முழு userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
நிலையான உள்வரி முழு uid_range_load_userns_by_fd(முழு userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        திரும்பு uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(மாறிலி UIDRange *range, uid_t start, uid_t nr);

முழு uid_range_clip(UIDRange *range, uid_t min, uid_t max);
முழு uid_range_partition(UIDRange *range, uid_t size);
முழு uid_range_copy(மாறிலி UIDRange *range, UIDRange **ret);
முழு uid_range_remove(UIDRange *range, uid_t start, uid_t size);
முழு uid_range_translate(மாறிலி UIDRange *outside, மாறிலி UIDRange *inside, uid_t uid, uid_t *ret);
முழு uid_range_translate_userns_fd(முழு userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

முழு uid_map_தேடல்_வேர்(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(மாறிலி UIDRange *range);
