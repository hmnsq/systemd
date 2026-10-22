/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

تعریفنوع ساختار UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

تعریفنوع ساختار UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحیح uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
ایستا درونخطی صحیح uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        بازگشت uid_range_add_internal(range, start, nr, true);
}
صحیح uid_range_add_str_full(UIDRange **range, ثابت نویسه *s, bool coalesce);
ایستا درونخطی صحیح uid_range_add_str(UIDRange **range, ثابت نویسه *s) {
        بازگشت uid_range_add_str_full(range, s, true);
}

صحیح uid_range_next_lower(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t start, uid_t nr);
ایستا درونخطی bool uid_range_contains(ثابت UIDRange *range, uid_t uid) {
        بازگشت uid_range_covers(range, uid, 1);
}

صحیح uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

ایستا درونخطی size_t uid_range_entries(ثابت UIDRange *range) {
        بازگشت range ? range->n_entries : 0;
}

unsigned uid_range_size(ثابت UIDRange *range) _pure_;
bool uid_range_is_empty(ثابت UIDRange *range) _pure_;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعریفنوع شمارش UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

صحیح uid_range_load_userns_full(ثابت نویسه *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ایستا درونخطی صحیح uid_range_load_userns(ثابت نویسه *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        بازگشت uid_range_load_userns_full(path, mode, true, ret);
}
صحیح uid_range_load_userns_by_fd_full(صحیح userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ایستا درونخطی صحیح uid_range_load_userns_by_fd(صحیح userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        بازگشت uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t start, uid_t nr);

صحیح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحیح uid_range_partition(UIDRange *range, uid_t size);
صحیح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحیح uid_range_remove(UIDRange *range, uid_t start, uid_t size);
صحیح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحیح uid_range_translate_userns_fd(صحیح userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

صحیح uid_map_جستجو_ریشه(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ثابت UIDRange *range);
