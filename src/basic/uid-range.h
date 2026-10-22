/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

ಪೂರ್ಣಾಙ್ಕ uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        ಪ್ರತ್ಯಾಗಮ uid_range_add_internal(range, start, nr, true);
}
ಪೂರ್ಣಾಙ್ಕ uid_range_add_str_full(UIDRange **range, ಧ್ರುವ ವರ್ಣ *s, bool coalesce);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ uid_range_add_str(UIDRange **range, ಧ್ರುವ ವರ್ಣ *s) {
        ಪ್ರತ್ಯಾಗಮ uid_range_add_str_full(range, s, true);
}

ಪೂರ್ಣಾಙ್ಕ uid_range_next_lower(ಧ್ರುವ UIDRange *range, uid_t *uid);

bool uid_range_covers(ಧ್ರುವ UIDRange *range, uid_t start, uid_t nr);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool uid_range_contains(ಧ್ರುವ UIDRange *range, uid_t uid) {
        ಪ್ರತ್ಯಾಗಮ uid_range_covers(range, uid, 1);
}

ಪೂರ್ಣಾಙ್ಕ uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ size_t uid_range_entries(ಧ್ರುವ UIDRange *range) {
        ಪ್ರತ್ಯಾಗಮ range ? range->n_entries : 0;
}

unsigned uid_range_size(ಧ್ರುವ UIDRange *range) _pure_;
bool uid_range_is_empty(ಧ್ರುವ UIDRange *range) _pure_;

bool uid_range_equal(ಧ್ರುವ UIDRange *a, ಧ್ರುವ UIDRange *b);

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns_full(ಧ್ರುವ ವರ್ಣ *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns(ಧ್ರುವ ವರ್ಣ *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        ಪ್ರತ್ಯಾಗಮ uid_range_load_userns_full(path, mode, true, ret);
}
ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns_by_fd_full(ಪೂರ್ಣಾಙ್ಕ userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns_by_fd(ಪೂರ್ಣಾಙ್ಕ userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        ಪ್ರತ್ಯಾಗಮ uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ಧ್ರುವ UIDRange *range, uid_t start, uid_t nr);

ಪೂರ್ಣಾಙ್ಕ uid_range_clip(UIDRange *range, uid_t min, uid_t max);
ಪೂರ್ಣಾಙ್ಕ uid_range_partition(UIDRange *range, uid_t size);
ಪೂರ್ಣಾಙ್ಕ uid_range_copy(ಧ್ರುವ UIDRange *range, UIDRange **ret);
ಪೂರ್ಣಾಙ್ಕ uid_range_remove(UIDRange *range, uid_t start, uid_t size);
ಪೂರ್ಣಾಙ್ಕ uid_range_translate(ಧ್ರುವ UIDRange *outside, ಧ್ರುವ UIDRange *inside, uid_t uid, uid_t *ret);
ಪೂರ್ಣಾಙ್ಕ uid_range_translate_userns_fd(ಪೂರ್ಣಾಙ್ಕ userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

ಪೂರ್ಣಾಙ್ಕ uid_map_ಹುಡುಕು_ಬೇರು(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ಧ್ರುವ UIDRange *range);
