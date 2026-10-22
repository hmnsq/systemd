/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sched.h>
#include <string.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "format-util.h"
#include "namespace-util.h"
#include "path-util.h"
#include "pidref.h"
#include "process-util.h"
#include "sort-util.h"
#include "stat-util.h"
#include "uid-range.h"
#include "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        အကယ်၍ (!range)
                ပြန်ပေး NULL;

        free(range->entries);
        ပြန်ပေး mfree(range);
}

static bool uid_range_entry_intersect(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        assert(a);
        assert(b);

        ပြန်ပေး a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

static ကိန်းပြည့် uid_range_entry_compare(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        ကိန်းပြည့် r;

        assert(a);
        assert(b);

        r = CMP(a->start, b->start);
        အကယ်၍ (r != 0)
                ပြန်ပေး r;

        ပြန်ပေး CMP(a->nr, b->nr);
}

static void uid_range_coalesce(UIDRange *range) {
        assert(range);

        အကယ်၍ (range->n_entries <= 0)
                ပြန်ပေး;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        အတွက် (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                အတွက် (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        အကယ်၍ (!uid_range_entry_intersect(x, y))
                                bha_nga;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        assert(x->start <= UINT32_MAX - x->nr);
                        assert(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        အကယ်၍ (range->n_entries > j + 1)
                                memmove(y, y + 1, parimaa_na(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        assert(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        assert(j > 0);
                        j--;
                }
        }
}

ကိန်းပြည့် uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = NULL;
        UIDRange *p;

        assert(range);

        အကယ်၍ (nr <= 0)
                ပြန်ပေး 0;

        အကယ်၍ (start > UINT32_MAX - nr) /* overflow check */
                ပြန်ပေး -ERANGE;

        အကယ်၍ (*range)
                p = *range;
        မဟုတ်ပါက {
                range_new = new0(UIDRange, 1);
                အကယ်၍ (!range_new)
                        ပြန်ပေး -ENOMEM;

                p = range_new;
        }

        အကယ်၍ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ပြန်ပေး -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        အကယ်၍ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_add_str_full(UIDRange **range, const var_na *s, bool coalesce) {
        uid_t start, end;
        ကိန်းပြည့် r;

        assert(range);
        assert(s);

        r = parse_uid_range(s, &start, &end);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး uid_range_add_internal(range, start, end - start + 1, coalesce);
}

ကိန်းပြည့် uid_range_next_lower(const UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        assert(range);
        assert(uid);

        အကယ်၍ (*uid == 0)
                ပြန်ပေး -EBUSY;

        candidate = *uid - 1;

        အတွက် (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                အကယ်၍ (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        ပြန်ပေး 1;
                }

                အကယ်၍ (end < candidate)
                        closest = end;
        }

        အကယ်၍ (closest == UID_INVALID)
                ပြန်ပေး -EBUSY;

        *uid = closest;
        ပြန်ပေး 1;
}

bool uid_range_covers(const UIDRange *range, uid_t start, uid_t nr) {
        အကယ်၍ (nr == 0) /* empty range? always covered... */
                ပြန်ပေး true;

        အကယ်၍ (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ပြန်ပေး false;

        အကယ်၍ (!range)
                ပြန်ပေး false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                အကယ်၍ (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

ကိန်းပြည့် uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        ကိန်းပြည့် r;

        assert(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        အကယ်၍ (r == EOF)
                ပြန်ပေး errno_or_else(ENOMSG);
        assert(r >= 0);
        အကယ်၍ (r != 3)
                ပြန်ပေး -EBADMSG;
        အကယ်၍ (uid_range <= 0)
                ပြန်ပေး -EBADMSG;

        အကယ်၍ (ret_base)
                *ret_base = uid_base;
        အကယ်၍ (ret_shift)
                *ret_shift = uid_shift;
        အကယ်၍ (ret_range)
                *ret_range = uid_range;

        ပြန်ပေး 0;
}

unsigned uid_range_size(const UIDRange *range) {
        အကယ်၍ (!range)
                ပြန်ပေး 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ပြန်ပေး n;
}

bool uid_range_is_empty(const UIDRange *range) {

        အကယ်၍ (!range)
                ပြန်ပေး true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                အကယ်၍ (e->nr > 0)
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် uid_range_load_userns_full(const var_na *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        ကိန်းပြည့် r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        အကယ်၍ (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        အကယ်၍ (!f) {
                r = -errno;

                အကယ်၍ (r == -ENOENT && path_startswith(path, "/proc/"))
                        ပြန်ပေး proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ပြန်ပေး r;
        }

        range = new0(UIDRange, 1);
        အကယ်၍ (!range)
                ပြန်ပေး -ENOMEM;

        အတွက် (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                အကယ်၍ (r == -ENOMSG)
                        bha_nga;
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
        }

        အကယ်၍ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_load_userns_by_fd_full(ကိန်းပြည့် userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ကိန်းပြည့် r;

        assert(userns_fd >= 0);
        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;
        အကယ်၍ (r > 0)
                ပြန်ပေး uid_range_load_userns_full(/* path= */ NULL, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        const var_na *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ပြန်ပေး uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(const UIDRange *range, uid_t start, uid_t nr) {

        အကယ်၍ (!range)
                ပြန်ပေး false;

        /* Avoid overflow */
        အကယ်၍ (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        အကယ်၍ (nr == 0)
                ပြန်ပေး false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                အကယ်၍ (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

ကိန်းပြည့် uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        assert(range);

        အကယ်၍ (min > max)
                ပြန်ပေး -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                အကယ်၍ (entry_end <= min || e->start > max)
                        continue;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                assert(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_partition(UIDRange *range, uid_t size) {
        assert(range);
        assert(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        အကယ်၍ (n_new_entries == 0) {
                range->n_entries = 0;
                ပြန်ပေး 0;
        }

        အကယ်၍ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ပြန်ပေး -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        အတွက် (size_t i = 0; i < range->n_entries; i++)
                အကယ်၍ (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        အတွက် (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                အတွက် (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_copy(const UIDRange *range, UIDRange **ret) {
        assert(ret);

        အကယ်၍ (!range) {
                *ret = NULL;
                ပြန်ပေး 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        အကယ်၍ (!copy)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                အကယ်၍ (!copy->entries)
                        ပြန်ပေး -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        assert(range);

        အကယ်၍ (size == 0)
                ပြန်ပေး 0;

        uid_t end = start + size; /* one past the last UID to remove */

        အတွက် (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                အကယ်၍ (entry_end <= start || e->start >= end)
                        continue;

                /* Check if this removal splits the entry into two parts */
                အကယ်၍ (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        အကယ်၍ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ပြန်ပေး -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * parimaa_na(UIDRangeEntry));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRangeEntry) {
                                .start = e->start,
                                .nr = start - e->start,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRangeEntry) {
                                .start = end,
                                .nr = entry_end - end,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        continue;
                }

                /* Removal covers the entire entry */
                အကယ်၍ (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * parimaa_na(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        continue;
                }

                /* Removal trims the start of the entry */
                အကယ်၍ (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        continue;
                }

                /* Removal trims the end of the entry */
                အကယ်၍ (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        continue;
                }
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် uid_range_translate(const UIDRange *outside, const UIDRange *inside, uid_t uid, uid_t *ret) {
        assert(uid_range_entries(outside) == uid_range_entries(inside));
        assert(ret);

        /* Given two UID ranges that represent the outside UID range of a user namespace (the 2nd and 3rd
         * columns in /proc/xxx/uid_map) and the inside UID range of a user namespace (the 1st and 3rd
         * columns in /proc/xxx/uid_map), translates the given UID from the outside range to the inside
         * range. For example, given the following UID range:
         *
         * 0 1000 1
         *
         * calling uid_range_translate(outside, inside, 1000) will return 0 as the output UID. Alternatively,
         * calling uid_range_translate(inside, outside, 0) will return 1000 as the output UID.
         */

        အတွက် (size_t i = 0; i < uid_range_entries(outside); i++)
                assert(outside->entries[i].nr == inside->entries[i].nr);

        အတွက် (size_t i = 0; i < uid_range_entries(outside); i++) {
                const UIDRangeEntry *e = outside->entries + i;

                အကယ်၍ (uid < e->start || uid >= e->start + e->nr)
                        continue;

                *ret = inside->entries[i].start + uid - e->start;
                ပြန်ပေး 0;
        }

        ပြန်ပေး -ESRCH;
}

ကိန်းပြည့် uid_range_translate_userns_fd(ကိန်းပြည့် userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        ကိန်းပြည့် r;

        assert(userns_fd >= 0);
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        ပြန်ပေး uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(const UIDRange *a, const UIDRange *b) {
        အကယ်၍ (a == b)
                ပြန်ပေး true;

        အကယ်၍ (!a || !b)
                ပြန်ပေး false;

        အကယ်၍ (a->n_entries != b->n_entries)
                ပြန်ပေး false;

        အတွက် (size_t i = 0; i < a->n_entries; i++) {
                အကယ်၍ (a->entries[i].start != b->entries[i].start)
                        ပြန်ပေး false;
                အကယ်၍ (a->entries[i].nr != b->entries[i].nr)
                        ပြန်ပေး false;
        }

        ပြန်ပေး true;
}

ကိန်းပြည့် uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        ကိန်းပြည့် r;

        assert(pid_is_valid(pid));
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        const var_na *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        အကယ်၍ (!f) {
                အကယ်၍ (errno != ENOENT)
                        ပြန်ပေး -errno;

                r = proc_mounted();
                အကယ်၍ (r < 0)
                        ပြန်ပေး -ENOENT; /* original error, if we can't determine /proc/ state */

                ပြန်ပေး r ? -ENOPKG : -ENOSYS;
        }

        အတွက် (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ NULL);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (uid_base == 0) {
                        အကယ်၍ (ret)
                                *ret = uid_shift;
                        ပြန်ပေး 0;
                }
        }
}

uid_t uid_range_base(const UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        အကယ်၍ (uid_range_is_empty(range))
                ပြန်ပေး UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                အကယ်၍ (e->nr > 0)
                        ပြန်ပေး e->start;

        ပြန်ပေး UID_INVALID;
}
