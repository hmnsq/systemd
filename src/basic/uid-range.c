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
        ถ้า (!range)
                คืน NULL;

        free(range->entries);
        คืน mfree(range);
}

static bool uid_range_entry_intersect(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        assert(a);
        assert(b);

        คืน a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

static จำนวนเต็ม uid_range_entry_compare(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        จำนวนเต็ม r;

        assert(a);
        assert(b);

        r = CMP(a->start, b->start);
        ถ้า (r != 0)
                คืน r;

        คืน CMP(a->nr, b->nr);
}

static void uid_range_coalesce(UIDRange *range) {
        assert(range);

        ถ้า (range->n_entries <= 0)
                คืน;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        สำหรับ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                สำหรับ (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        ถ้า (!uid_range_entry_intersect(x, y))
                                bha_nga;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        assert(x->start <= UINT32_MAX - x->nr);
                        assert(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        ถ้า (range->n_entries > j + 1)
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

จำนวนเต็ม uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = NULL;
        UIDRange *p;

        assert(range);

        ถ้า (nr <= 0)
                คืน 0;

        ถ้า (start > UINT32_MAX - nr) /* overflow check */
                คืน -ERANGE;

        ถ้า (*range)
                p = *range;
        มิฉะนั้น {
                range_new = new0(UIDRange, 1);
                ถ้า (!range_new)
                        คืน -ENOMEM;

                p = range_new;
        }

        ถ้า (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                คืน -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        ถ้า (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        คืน 0;
}

จำนวนเต็ม uid_range_add_str_full(UIDRange **range, const var_na *s, bool coalesce) {
        uid_t start, end;
        จำนวนเต็ม r;

        assert(range);
        assert(s);

        r = parse_uid_range(s, &start, &end);
        ถ้า (r < 0)
                คืน r;

        คืน uid_range_add_internal(range, start, end - start + 1, coalesce);
}

จำนวนเต็ม uid_range_next_lower(const UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        assert(range);
        assert(uid);

        ถ้า (*uid == 0)
                คืน -EBUSY;

        candidate = *uid - 1;

        สำหรับ (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                ถ้า (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        คืน 1;
                }

                ถ้า (end < candidate)
                        closest = end;
        }

        ถ้า (closest == UID_INVALID)
                คืน -EBUSY;

        *uid = closest;
        คืน 1;
}

bool uid_range_covers(const UIDRange *range, uid_t start, uid_t nr) {
        ถ้า (nr == 0) /* empty range? always covered... */
                คืน true;

        ถ้า (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                คืน false;

        ถ้า (!range)
                คืน false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ถ้า (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        คืน true;

        คืน false;
}

จำนวนเต็ม uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        จำนวนเต็ม r;

        assert(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        ถ้า (r == EOF)
                คืน errno_or_else(ENOMSG);
        assert(r >= 0);
        ถ้า (r != 3)
                คืน -EBADMSG;
        ถ้า (uid_range <= 0)
                คืน -EBADMSG;

        ถ้า (ret_base)
                *ret_base = uid_base;
        ถ้า (ret_shift)
                *ret_shift = uid_shift;
        ถ้า (ret_range)
                *ret_range = uid_range;

        คืน 0;
}

unsigned uid_range_size(const UIDRange *range) {
        ถ้า (!range)
                คืน 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        คืน n;
}

bool uid_range_is_empty(const UIDRange *range) {

        ถ้า (!range)
                คืน true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ถ้า (e->nr > 0)
                        คืน false;

        คืน true;
}

จำนวนเต็ม uid_range_load_userns_full(const var_na *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        จำนวนเต็ม r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        ถ้า (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        ถ้า (!f) {
                r = -errno;

                ถ้า (r == -ENOENT && path_startswith(path, "/proc/"))
                        คืน proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                คืน r;
        }

        range = new0(UIDRange, 1);
        ถ้า (!range)
                คืน -ENOMEM;

        สำหรับ (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                ถ้า (r == -ENOMSG)
                        bha_nga;
                ถ้า (r < 0)
                        คืน r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                ถ้า (r < 0)
                        คืน r;
        }

        ถ้า (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        คืน 0;
}

จำนวนเต็ม uid_range_load_userns_by_fd_full(จำนวนเต็ม userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        จำนวนเต็ม r;

        assert(userns_fd >= 0);
        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ถ้า (r < 0)
                คืน r;
        ถ้า (r > 0)
                คืน uid_range_load_userns_full(/* path= */ NULL, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ถ้า (r < 0)
                คืน r;

        const var_na *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        คืน uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(const UIDRange *range, uid_t start, uid_t nr) {

        ถ้า (!range)
                คืน false;

        /* Avoid overflow */
        ถ้า (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        ถ้า (nr == 0)
                คืน false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                ถ้า (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        คืน true;

        คืน false;
}

จำนวนเต็ม uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        assert(range);

        ถ้า (min > max)
                คืน -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ถ้า (entry_end <= min || e->start > max)
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

        คืน 0;
}

จำนวนเต็ม uid_range_partition(UIDRange *range, uid_t size) {
        assert(range);
        assert(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        ถ้า (n_new_entries == 0) {
                range->n_entries = 0;
                คืน 0;
        }

        ถ้า (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                คืน -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        สำหรับ (size_t i = 0; i < range->n_entries; i++)
                ถ้า (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        สำหรับ (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                สำหรับ (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        คืน 0;
}

จำนวนเต็ม uid_range_copy(const UIDRange *range, UIDRange **ret) {
        assert(ret);

        ถ้า (!range) {
                *ret = NULL;
                คืน 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ถ้า (!copy)
                คืน -ENOMEM;

        ถ้า (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                ถ้า (!copy->entries)
                        คืน -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        คืน 0;
}

จำนวนเต็ม uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        assert(range);

        ถ้า (size == 0)
                คืน 0;

        uid_t end = start + size; /* one past the last UID to remove */

        สำหรับ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                ถ้า (entry_end <= start || e->start >= end)
                        continue;

                /* Check if this removal splits the entry into two parts */
                ถ้า (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        ถ้า (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                คืน -ENOMEM;

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
                ถ้า (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * parimaa_na(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        continue;
                }

                /* Removal trims the start of the entry */
                ถ้า (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        continue;
                }

                /* Removal trims the end of the entry */
                ถ้า (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        continue;
                }
        }

        คืน 0;
}

จำนวนเต็ม uid_range_translate(const UIDRange *outside, const UIDRange *inside, uid_t uid, uid_t *ret) {
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

        สำหรับ (size_t i = 0; i < uid_range_entries(outside); i++)
                assert(outside->entries[i].nr == inside->entries[i].nr);

        สำหรับ (size_t i = 0; i < uid_range_entries(outside); i++) {
                const UIDRangeEntry *e = outside->entries + i;

                ถ้า (uid < e->start || uid >= e->start + e->nr)
                        continue;

                *ret = inside->entries[i].start + uid - e->start;
                คืน 0;
        }

        คืน -ESRCH;
}

จำนวนเต็ม uid_range_translate_userns_fd(จำนวนเต็ม userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        จำนวนเต็ม r;

        assert(userns_fd >= 0);
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        ถ้า (r < 0)
                คืน r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        ถ้า (r < 0)
                คืน r;

        คืน uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(const UIDRange *a, const UIDRange *b) {
        ถ้า (a == b)
                คืน true;

        ถ้า (!a || !b)
                คืน false;

        ถ้า (a->n_entries != b->n_entries)
                คืน false;

        สำหรับ (size_t i = 0; i < a->n_entries; i++) {
                ถ้า (a->entries[i].start != b->entries[i].start)
                        คืน false;
                ถ้า (a->entries[i].nr != b->entries[i].nr)
                        คืน false;
        }

        คืน true;
}

จำนวนเต็ม uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        จำนวนเต็ม r;

        assert(pid_is_valid(pid));
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        const var_na *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ถ้า (!f) {
                ถ้า (errno != ENOENT)
                        คืน -errno;

                r = proc_mounted();
                ถ้า (r < 0)
                        คืน -ENOENT; /* original error, if we can't determine /proc/ state */

                คืน r ? -ENOPKG : -ENOSYS;
        }

        สำหรับ (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ NULL);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (uid_base == 0) {
                        ถ้า (ret)
                                *ret = uid_shift;
                        คืน 0;
                }
        }
}

uid_t uid_range_base(const UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ถ้า (uid_range_is_empty(range))
                คืน UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ถ้า (e->nr > 0)
                        คืน e->start;

        คืน UID_INVALID;
}
