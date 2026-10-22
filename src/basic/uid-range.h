/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        ଫେରନ୍ତୁ uid_range_add_internal(range, start, nr, true);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add_str_full(UIDRange **range, ଅଚଳ ଅକ୍ଷର *s, bool coalesce);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add_str(UIDRange **range, ଅଚଳ ଅକ୍ଷର *s) {
        ଫେରନ୍ତୁ uid_range_add_str_full(range, s, true);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_next_lower(ଅଚଳ UIDRange *range, uid_t *uid);

bool uid_range_covers(ଅଚଳ UIDRange *range, uid_t start, uid_t nr);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool uid_range_contains(ଅଚଳ UIDRange *range, uid_t uid) {
        ଫେରନ୍ତୁ uid_range_covers(range, uid, 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ size_t uid_range_entries(ଅଚଳ UIDRange *range) {
        ଫେରନ୍ତୁ range ? range->n_entries : 0;
}

unsigned uid_range_size(ଅଚଳ UIDRange *range) _pure_;
bool uid_range_is_empty(ଅଚଳ UIDRange *range) _pure_;

bool uid_range_equal(ଅଚଳ UIDRange *a, ଅଚଳ UIDRange *b);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns_full(ଅଚଳ ଅକ୍ଷର *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns(ଅଚଳ ଅକ୍ଷର *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        ଫେରନ୍ତୁ uid_range_load_userns_full(path, mode, true, ret);
}
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns_by_fd_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns_by_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        ଫେରନ୍ତୁ uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(ଅଚଳ UIDRange *range, uid_t start, uid_t nr);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_clip(UIDRange *range, uid_t min, uid_t max);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_partition(UIDRange *range, uid_t size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_copy(ଅଚଳ UIDRange *range, UIDRange **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_remove(UIDRange *range, uid_t start, uid_t size);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_translate(ଅଚଳ UIDRange *outside, ଅଚଳ UIDRange *inside, uid_t uid, uid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_translate_userns_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_map_ସନ୍ଧାନ_ମୂଳ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(ଅଚଳ UIDRange *range);
