/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം ഘടന UIDRangeEntry {
        uid_t start, nr;
} UIDRangeEntry;

തരനിർവചനം ഘടന UIDRange {
        UIDRangeEntry *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

പൂർണ്ണസംഖ്യ uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ uid_range_add(UIDRange **range, uid_t start, uid_t nr) {
        തിരികെ uid_range_add_internal(range, start, nr, true);
}
പൂർണ്ണസംഖ്യ uid_range_add_str_full(UIDRange **range, സ്ഥിരം അക്ഷരം *s, bool coalesce);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ uid_range_add_str(UIDRange **range, സ്ഥിരം അക്ഷരം *s) {
        തിരികെ uid_range_add_str_full(range, s, true);
}

പൂർണ്ണസംഖ്യ uid_range_next_lower(സ്ഥിരം UIDRange *range, uid_t *uid);

bool uid_range_covers(സ്ഥിരം UIDRange *range, uid_t start, uid_t nr);
സ്ഥിതം വരിയിൽ bool uid_range_contains(സ്ഥിരം UIDRange *range, uid_t uid) {
        തിരികെ uid_range_covers(range, uid, 1);
}

പൂർണ്ണസംഖ്യ uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range);

സ്ഥിതം വരിയിൽ size_t uid_range_entries(സ്ഥിരം UIDRange *range) {
        തിരികെ range ? range->n_entries : 0;
}

unsigned uid_range_size(സ്ഥിരം UIDRange *range) _pure_;
bool uid_range_is_empty(സ്ഥിരം UIDRange *range) _pure_;

bool uid_range_equal(സ്ഥിരം UIDRange *a, സ്ഥിരം UIDRange *b);

തരനിർവചനം എണ്ണൽ UIDRangeUsernsMode {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        _UID_RANGE_USERNS_MODE_MAX,
        _UID_RANGE_USERNS_MODE_INVALID = -EINVAL,
} UIDRangeUsernsMode;

പൂർണ്ണസംഖ്യ uid_range_load_userns_full(സ്ഥിരം അക്ഷരം *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ uid_range_load_userns(സ്ഥിരം അക്ഷരം *path, UIDRangeUsernsMode mode, UIDRange **ret) {
        തിരികെ uid_range_load_userns_full(path, mode, true, ret);
}
പൂർണ്ണസംഖ്യ uid_range_load_userns_by_fd_full(പൂർണ്ണസംഖ്യ userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ uid_range_load_userns_by_fd(പൂർണ്ണസംഖ്യ userns_fd, UIDRangeUsernsMode mode, UIDRange **ret) {
        തിരികെ uid_range_load_userns_by_fd_full(userns_fd, mode, true, ret);
}

bool uid_range_overlaps(സ്ഥിരം UIDRange *range, uid_t start, uid_t nr);

പൂർണ്ണസംഖ്യ uid_range_clip(UIDRange *range, uid_t min, uid_t max);
പൂർണ്ണസംഖ്യ uid_range_partition(UIDRange *range, uid_t size);
പൂർണ്ണസംഖ്യ uid_range_copy(സ്ഥിരം UIDRange *range, UIDRange **ret);
പൂർണ്ണസംഖ്യ uid_range_remove(UIDRange *range, uid_t start, uid_t size);
പൂർണ്ണസംഖ്യ uid_range_translate(സ്ഥിരം UIDRange *outside, സ്ഥിരം UIDRange *inside, uid_t uid, uid_t *ret);
പൂർണ്ണസംഖ്യ uid_range_translate_userns_fd(പൂർണ്ണസംഖ്യ userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret);

പൂർണ്ണസംഖ്യ uid_map_തിരയൽ_വേര്(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret);

uid_t uid_range_base(സ്ഥിരം UIDRange *range);
