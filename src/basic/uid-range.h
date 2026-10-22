/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

تعريفنوع بنية UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

تعريفنوع بنية UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحيح uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
ساكن مضمن صحيح uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        أرجع uid_range_add_internal(range, start, nr, true);
}
صحيح uid_range_add_str_full(UIDRange **range, ثابت حرف *s, bool coalesce);
ساكن مضمن صحيح uid_range_add_str(UIDRange **range, ثابت حرف *s) {
        أرجع uid_range_add_str_full(range, s, true);
}

صحيح uid_range_next_lower(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t start, uid_t nr);
ساكن مضمن bool uid_range_contains(ثابت UIDRange *range, uid_t uid) {
        أرجع uid_range_covers(range, uid, 1);
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

ساكن مضمن size_t uid_range_entries(ثابت UIDRange *range) {
        أرجع range ? range->n_entries : 0;
}

unsigned uid_range_size(ثابت UIDRange *range) _pure_;
bool uid_range_is_empty(ثابت UIDRange *range) _pure_;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعريفنوع تعداد UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

صحيح uid_range_load_userns_full(ثابت حرف *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_load_userns(ثابت حرف *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        أرجع uid_range_load_userns_full(path, mode, true, ret);
}
صحيح uid_range_load_userns_by_fd_full(صحيح userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_load_userns_by_fd(صحيح userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        أرجع uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t start, uid_t nr);

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحيح uid_range_partition(UIDRange *range, uid_t size);
صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحيح uid_range_remove(UIDRange *range, uid_t start, uid_t size);
صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

صحيح uid_map_گەڕان_ڕەگ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ثابت UIDRange *range);
