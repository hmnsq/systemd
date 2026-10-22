/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश संरचना UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

प्रकारनिर्देश संरचना UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

पूर्णाङ्क uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        प्रत्यागम uid_range_add_internal(range, start, nr, true);
}
पूर्णाङ्क uid_range_add_str_full(UIDRange **range, ध्रुव वर्ण *s, bool coalesce);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_add_str(UIDRange **range, ध्रुव वर्ण *s) {
        प्रत्यागम uid_range_add_str_full(range, s, true);
}

पूर्णाङ्क uid_range_next_lower(ध्रुव UIDRange *range, uid_t *uid);

bool uid_range_covers(ध्रुव UIDRange *range, uid_t start, uid_t nr);
स्थिर अन्तःस्थ bool uid_range_contains(ध्रुव UIDRange *range, uid_t uid) {
        प्रत्यागम uid_range_covers(range, uid, 1);
}

पूर्णाङ्क uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

स्थिर अन्तःस्थ size_t uid_range_entries(ध्रुव UIDRange *range) {
        प्रत्यागम range ? range->n_entries : 0;
}

unsigned uid_range_size(ध्रुव UIDRange *range) _pure_;
bool uid_range_is_empty(ध्रुव UIDRange *range) _pure_;

bool uid_range_equal(ध्रुव UIDRange *a, ध्रुव UIDRange *b);

प्रकारनिर्देश गणना UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

पूर्णाङ्क uid_range_load_userns_full(ध्रुव वर्ण *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_load_userns(ध्रुव वर्ण *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        प्रत्यागम uid_range_load_userns_full(path, mode, true, ret);
}
पूर्णाङ्क uid_range_load_userns_by_fd_full(पूर्णाङ्क userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
स्थिर अन्तःस्थ पूर्णाङ्क uid_range_load_userns_by_fd(पूर्णाङ्क userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        प्रत्यागम uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ध्रुव UIDRange *range, uid_t start, uid_t nr);

पूर्णाङ्क uid_range_clip(UIDRange *range, uid_t min, uid_t max);
पूर्णाङ्क uid_range_partition(UIDRange *range, uid_t size);
पूर्णाङ्क uid_range_copy(ध्रुव UIDRange *range, UIDRange **ret);
पूर्णाङ्क uid_range_remove(UIDRange *range, uid_t start, uid_t size);
पूर्णाङ्क uid_range_translate(ध्रुव UIDRange *outside, ध्रुव UIDRange *inside, uid_t uid, uid_t *ret);
पूर्णाङ्क uid_range_translate_userns_fd(पूर्णाङ्क userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

पूर्णाङ्क uid_map_अन्वेषणम्_मूलम्(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ध्रुव UIDRange *range);
