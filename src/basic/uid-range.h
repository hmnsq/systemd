/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef sa_nracanaa UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

typedef sa_nracanaa UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

පූර්ණ_සංඛ්‍යා uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
static anta_hstha පූර්ණ_සංඛ්‍යා uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        ආපසු uid_range_add_internal(range, start, nr, true);
}
පූර්ණ_සංඛ්‍යා uid_range_add_str_full(UIDRange **range, const var_na *s, bool coalesce);
static anta_hstha පූර්ණ_සංඛ්‍යා uid_range_add_str(UIDRange **range, const var_na *s) {
        ආපසු uid_range_add_str_full(range, s, true);
}

පූර්ණ_සංඛ්‍යා uid_range_next_lower(const UIDRange *range, uid_t *uid);

bool uid_range_covers(const UIDRange *range, uid_t start, uid_t nr);
static anta_hstha bool uid_range_contains(const UIDRange *range, uid_t uid) {
        ආපසු uid_range_covers(range, uid, 1);
}

පූර්ණ_සංඛ්‍යා uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

static anta_hstha size_t uid_range_entries(const UIDRange *range) {
        ආපසු range ? range->n_entries : 0;
}

unsigned uid_range_size(const UIDRange *range) _pure_;
bool uid_range_is_empty(const UIDRange *range) _pure_;

bool uid_range_equal(const UIDRange *a, const UIDRange *b);

typedef ga_nanaa UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

පූර්ණ_සංඛ්‍යා uid_range_load_userns_full(const var_na *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා uid_range_load_userns(const var_na *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        ආපසු uid_range_load_userns_full(path, mode, true, ret);
}
පූර්ණ_සංඛ්‍යා uid_range_load_userns_by_fd_full(පූර්ණ_සංඛ්‍යා userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
static anta_hstha පූර්ණ_සංඛ්‍යා uid_range_load_userns_by_fd(පූර්ණ_සංඛ්‍යා userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        ආපසු uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(const UIDRange *range, uid_t start, uid_t nr);

පූර්ණ_සංඛ්‍යා uid_range_clip(UIDRange *range, uid_t min, uid_t max);
පූර්ණ_සංඛ්‍යා uid_range_partition(UIDRange *range, uid_t size);
පූර්ණ_සංඛ්‍යා uid_range_copy(const UIDRange *range, UIDRange **ret);
පූර්ණ_සංඛ්‍යා uid_range_remove(UIDRange *range, uid_t start, uid_t size);
පූර්ණ_සංඛ්‍යා uid_range_translate(const UIDRange *outside, const UIDRange *inside, uid_t uid, uid_t *ret);
පූර්ණ_සංඛ්‍යා uid_range_translate_userns_fd(පූර්ණ_සංඛ්‍යා userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

පූර්ණ_සංඛ්‍යා uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(const UIDRange *range);
