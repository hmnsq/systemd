/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ সংরচনা UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

প্রকারনির্দেশ সংরচনা UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

পূর্ণাঙ্ক uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
স্থির অন্তঃস্থ পূর্ণাঙ্ক uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        প্রত্যাগম uid_range_add_internal(range, start, nr, true);
}
পূর্ণাঙ্ক uid_range_add_str_full(UIDRange **range, ধ্রুব বর্ণ *s, bool coalesce);
স্থির অন্তঃস্থ পূর্ণাঙ্ক uid_range_add_str(UIDRange **range, ধ্রুব বর্ণ *s) {
        প্রত্যাগম uid_range_add_str_full(range, s, true);
}

পূর্ণাঙ্ক uid_range_next_lower(ধ্রুব UIDRange *range, uid_t *uid);

bool uid_range_covers(ধ্রুব UIDRange *range, uid_t start, uid_t nr);
স্থির অন্তঃস্থ bool uid_range_contains(ধ্রুব UIDRange *range, uid_t uid) {
        প্রত্যাগম uid_range_covers(range, uid, 1);
}

পূর্ণাঙ্ক uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

স্থির অন্তঃস্থ size_t uid_range_entries(ধ্রুব UIDRange *range) {
        প্রত্যাগম range ? range->n_entries : 0;
}

unsigned uid_range_size(ধ্রুব UIDRange *range) _pure_;
bool uid_range_is_empty(ধ্রুব UIDRange *range) _pure_;

bool uid_range_equal(ধ্রুব UIDRange *a, ধ্রুব UIDRange *b);

প্রকারনির্দেশ গণনা UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

পূর্ণাঙ্ক uid_range_load_userns_full(ধ্রুব বর্ণ *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক uid_range_load_userns(ধ্রুব বর্ণ *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        প্রত্যাগম uid_range_load_userns_full(path, mode, true, ret);
}
পূর্ণাঙ্ক uid_range_load_userns_by_fd_full(পূর্ণাঙ্ক userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক uid_range_load_userns_by_fd(পূর্ণাঙ্ক userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        প্রত্যাগম uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ধ্রুব UIDRange *range, uid_t start, uid_t nr);

পূর্ণাঙ্ক uid_range_clip(UIDRange *range, uid_t min, uid_t max);
পূর্ণাঙ্ক uid_range_partition(UIDRange *range, uid_t size);
পূর্ণাঙ্ক uid_range_copy(ধ্রুব UIDRange *range, UIDRange **ret);
পূর্ণাঙ্ক uid_range_remove(UIDRange *range, uid_t start, uid_t size);
পূর্ণাঙ্ক uid_range_translate(ধ্রুব UIDRange *outside, ধ্রুব UIDRange *inside, uid_t uid, uid_t *ret);
পূর্ণাঙ্ক uid_range_translate_userns_fd(পূর্ণাঙ্ক userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

পূর্ণাঙ্ক uid_map_অনুসন্ধান_মূল(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ধ্রুব UIDRange *range);
