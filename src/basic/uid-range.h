/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા સંરચના UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

પ્રકારવ્યાખ્યા સંરચના UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

પૂર્ણાંક uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
સ્થિર અંતઃરેખ પૂર્ણાંક uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        પાછા uid_range_add_internal(range, start, nr, true);
}
પૂર્ણાંક uid_range_add_str_full(UIDRange **range, અચલ અક્ષર *s, bool coalesce);
સ્થિર અંતઃરેખ પૂર્ણાંક uid_range_add_str(UIDRange **range, અચલ અક્ષર *s) {
        પાછા uid_range_add_str_full(range, s, true);
}

પૂર્ણાંક uid_range_next_lower(અચલ UIDRange *range, uid_t *uid);

bool uid_range_covers(અચલ UIDRange *range, uid_t start, uid_t nr);
સ્થિર અંતઃરેખ bool uid_range_contains(અચલ UIDRange *range, uid_t uid) {
        પાછા uid_range_covers(range, uid, 1);
}

પૂર્ણાંક uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

સ્થિર અંતઃરેખ size_t uid_range_entries(અચલ UIDRange *range) {
        પાછા range ? range->n_entries : 0;
}

unsigned uid_range_size(અચલ UIDRange *range) _pure_;
bool uid_range_is_empty(અચલ UIDRange *range) _pure_;

bool uid_range_equal(અચલ UIDRange *a, અચલ UIDRange *b);

પ્રકારવ્યાખ્યા ગણના UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

પૂર્ણાંક uid_range_load_userns_full(અચલ અક્ષર *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક uid_range_load_userns(અચલ અક્ષર *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        પાછા uid_range_load_userns_full(path, mode, true, ret);
}
પૂર્ણાંક uid_range_load_userns_by_fd_full(પૂર્ણાંક userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક uid_range_load_userns_by_fd(પૂર્ણાંક userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        પાછા uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(અચલ UIDRange *range, uid_t start, uid_t nr);

પૂર્ણાંક uid_range_clip(UIDRange *range, uid_t min, uid_t max);
પૂર્ણાંક uid_range_partition(UIDRange *range, uid_t size);
પૂર્ણાંક uid_range_copy(અચલ UIDRange *range, UIDRange **ret);
પૂર્ણાંક uid_range_remove(UIDRange *range, uid_t start, uid_t size);
પૂર્ણાંક uid_range_translate(અચલ UIDRange *outside, અચલ UIDRange *inside, uid_t uid, uid_t *ret);
પૂર્ણાંક uid_range_translate_userns_fd(પૂર્ણાંક userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

પૂર્ણાંક uid_map_શોધ_મૂળ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(અચલ UIDRange *range);
