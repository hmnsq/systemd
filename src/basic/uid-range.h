/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

प्रकारव्याख्या संरचना UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

प्रकारव्याख्या संरचना UIDRange {
        UIDRangeEntry *entries;
        माप_प्रकार n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

पूर्णांक uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
स्थिर ओळीत पूर्णांक uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        परत uid_range_add_internal(range, start, nr, true);
}
पूर्णांक uid_range_add_str_full(UIDRange **range, स्थिरांक अक्षर *s, bool coalesce);
स्थिर ओळीत पूर्णांक uid_range_add_str(UIDRange **range, स्थिरांक अक्षर *s) {
        परत uid_range_add_str_full(range, s, true);
}

पूर्णांक uid_range_next_lower(स्थिरांक UIDRange *range, uid_t *uid);

bool uid_range_covers(स्थिरांक UIDRange *range, uid_t start, uid_t nr);
स्थिर ओळीत bool uid_range_contains(स्थिरांक UIDRange *range, uid_t uid) {
        परत uid_range_covers(range, uid, 1);
}

पूर्णांक uid_map_read_one(खाता *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

स्थिर ओळीत माप_प्रकार uid_range_entries(स्थिरांक UIDRange *range) {
        परत range ? range->n_entries : 0;
}

अचिन्हित uid_range_size(स्थिरांक UIDRange *range) _pure_;
bool uid_range_is_empty(स्थिरांक UIDRange *range) _pure_;

bool uid_range_equal(स्थिरांक UIDRange *a, स्थिरांक UIDRange *b);

प्रकारव्याख्या गणना UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

पूर्णांक uid_range_load_userns_full(स्थिरांक अक्षर *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
स्थिर ओळीत पूर्णांक uid_range_load_userns(स्थिरांक अक्षर *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        परत uid_range_load_userns_full(path, mode, true, ret);
}
पूर्णांक uid_range_load_userns_by_fd_full(पूर्णांक userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
स्थिर ओळीत पूर्णांक uid_range_load_userns_by_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        परत uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(स्थिरांक UIDRange *range, uid_t start, uid_t nr);

पूर्णांक uid_range_clip(UIDRange *range, uid_t min, uid_t max);
पूर्णांक uid_range_partition(UIDRange *range, uid_t size);
पूर्णांक uid_range_copy(स्थिरांक UIDRange *range, UIDRange **ret);
पूर्णांक uid_range_remove(UIDRange *range, uid_t start, uid_t size);
पूर्णांक uid_range_translate(स्थिरांक UIDRange *outside, स्थिरांक UIDRange *inside, uid_t uid, uid_t *ret);
पूर्णांक uid_range_translate_userns_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

पूर्णांक uid_map_शोध_मूळ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(स्थिरांक UIDRange *range);
