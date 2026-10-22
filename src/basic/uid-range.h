/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারলেখ কাঠামো UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

প্রকারলেখ কাঠামো UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

পূর্ণসংখ্যা uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
স্থির অন্তঃরেখ পূর্ণসংখ্যা uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        ফেরত uid_range_add_internal(range, start, nr, true);
}
পূর্ণসংখ্যা uid_range_add_str_full(UIDRange **range, ধ্রুবক অক্ষর *s, bool coalesce);
স্থির অন্তঃরেখ পূর্ণসংখ্যা uid_range_add_str(UIDRange **range, ধ্রুবক অক্ষর *s) {
        ফেরত uid_range_add_str_full(range, s, true);
}

পূর্ণসংখ্যা uid_range_পরবর্তী_lower(ধ্রুবক UIDRange *range, uid_t *uid);

bool uid_range_covers(ধ্রুবক UIDRange *range, uid_t start, uid_t nr);
স্থির অন্তঃরেখ bool uid_range_contains(ধ্রুবক UIDRange *range, uid_t uid) {
        ফেরত uid_range_covers(range, uid, 1);
}

পূর্ণসংখ্যা uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

স্থির অন্তঃরেখ size_t uid_range_entries(ধ্রুবক UIDRange *range) {
        ফেরত range ? range->n_entries : 0;
}

unsigned uid_range_আকার(ধ্রুবক UIDRange *range) _pure_;
bool uid_range_is_empty(ধ্রুবক UIDRange *range) _pure_;

bool uid_range_equal(ধ্রুবক UIDRange *a, ধ্রুবক UIDRange *b);

প্রকারলেখ গণনা UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

পূর্ণসংখ্যা uid_range_load_userns_full(ধ্রুবক অক্ষর *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা uid_range_load_userns(ধ্রুবক অক্ষর *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        ফেরত uid_range_load_userns_full(path, mode, true, ret);
}
পূর্ণসংখ্যা uid_range_load_userns_by_fd_full(পূর্ণসংখ্যা userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা uid_range_load_userns_by_fd(পূর্ণসংখ্যা userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        ফেরত uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ধ্রুবক UIDRange *range, uid_t start, uid_t nr);

পূর্ণসংখ্যা uid_range_clip(UIDRange *range, uid_t min, uid_t max);
পূর্ণসংখ্যা uid_range_partition(UIDRange *range, uid_t আকার_2);
পূর্ণসংখ্যা uid_range_copy(ধ্রুবক UIDRange *range, UIDRange **ret);
পূর্ণসংখ্যা uid_range_remove(UIDRange *range, uid_t start, uid_t আকার_2);
পূর্ণসংখ্যা uid_range_translate(ধ্রুবক UIDRange *outside, ধ্রুবক UIDRange *inside, uid_t uid, uid_t *ret);
পূর্ণসংখ্যা uid_range_translate_userns_fd(পূর্ণসংখ্যা userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

পূর্ণসংখ্যা uid_map_অনুসন্ধান_মূল(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ধ্রুবক UIDRange *range);
