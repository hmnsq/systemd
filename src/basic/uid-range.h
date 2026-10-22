/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef struct UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

typedef struct UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

ціле uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
static inline ціле uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        повернути uid_range_add_internal(range, start, nr, true);
}
ціле uid_range_add_str_full(UIDRange **range, const char *s, bool coalesce);
static inline ціле uid_range_add_str(UIDRange **range, const char *s) {
        повернути uid_range_add_str_full(range, s, true);
}

ціле uid_range_next_lower(const UIDRange *range, uid_t *uid);

bool uid_range_covers(const UIDRange *range, uid_t start, uid_t nr);
static inline bool uid_range_contains(const UIDRange *range, uid_t uid) {
        повернути uid_range_covers(range, uid, 1);
}

ціле uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

static inline size_t uid_range_entries(const UIDRange *range) {
        повернути range ? range->n_entries : 0;
}

unsigned uid_range_size(const UIDRange *range) _pure_;
bool uid_range_is_empty(const UIDRange *range) _pure_;

bool uid_range_equal(const UIDRange *a, const UIDRange *b);

typedef enum UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

ціле uid_range_load_userns_full(const char *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
static inline ціле uid_range_load_userns(const char *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        повернути uid_range_load_userns_full(path, mode, true, ret);
}
ціле uid_range_load_userns_by_fd_full(ціле userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
static inline ціле uid_range_load_userns_by_fd(ціле userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        повернути uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(const UIDRange *range, uid_t start, uid_t nr);

ціле uid_range_clip(UIDRange *range, uid_t min, uid_t max);
ціле uid_range_partition(UIDRange *range, uid_t size);
ціле uid_range_copy(const UIDRange *range, UIDRange **ret);
ціле uid_range_remove(UIDRange *range, uid_t start, uid_t size);
ціле uid_range_translate(const UIDRange *outside, const UIDRange *inside, uid_t uid, uid_t *ret);
ціле uid_range_translate_userns_fd(ціле userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

ціле uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(const UIDRange *range);
