/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार काष्ठा UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

प्रकार काष्ठा UIDRange {
        UIDRangeEntry *entries;
        माप_प्रकार n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

पूर्णांक uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
अटल अंतरभूत पूर्णांक uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        वापस uid_range_add_internal(range, start, nr, true);
}
पूर्णांक uid_range_add_str_full(UIDRange **range, स्थिर अक्षर *s, bool coalesce);
अटल अंतरभूत पूर्णांक uid_range_add_str(UIDRange **range, स्थिर अक्षर *s) {
        वापस uid_range_add_str_full(range, s, true);
}

पूर्णांक uid_range_अग्र_lower(स्थिर UIDRange *range, uid_t *uid);

bool uid_range_covers(स्थिर UIDRange *range, uid_t start, uid_t nr);
अटल अंतरभूत bool uid_range_contains(स्थिर UIDRange *range, uid_t uid) {
        वापस uid_range_covers(range, uid, 1);
}

पूर्णांक uid_map_read_one(खाता *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

अटल अंतरभूत माप_प्रकार uid_range_entries(स्थिर UIDRange *range) {
        वापस range ? range->n_entries : 0;
}

अचिन्हित uid_range_आकार(स्थिर UIDRange *range) _pure_;
bool uid_range_is_empty(स्थिर UIDRange *range) _pure_;

bool uid_range_equal(स्थिर UIDRange *a, स्थिर UIDRange *b);

प्रकार क्रमागत UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

पूर्णांक uid_range_load_userns_full(स्थिर अक्षर *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
अटल अंतरभूत पूर्णांक uid_range_load_userns(स्थिर अक्षर *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        वापस uid_range_load_userns_full(path, mode, true, ret);
}
पूर्णांक uid_range_load_userns_by_fd_full(पूर्णांक userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
अटल अंतरभूत पूर्णांक uid_range_load_userns_by_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        वापस uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(स्थिर UIDRange *range, uid_t start, uid_t nr);

पूर्णांक uid_range_clip(UIDRange *range, uid_t min, uid_t max);
पूर्णांक uid_range_partition(UIDRange *range, uid_t आकार);
पूर्णांक uid_range_copy(स्थिर UIDRange *range, UIDRange **ret);
पूर्णांक uid_range_remove(UIDRange *range, uid_t start, uid_t आकार);
पूर्णांक uid_range_translate(स्थिर UIDRange *outside, स्थिर UIDRange *inside, uid_t uid, uid_t *ret);
पूर्णांक uid_range_translate_userns_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

पूर्णांक uid_map_खोज_मूल(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(स्थिर UIDRange *range);
