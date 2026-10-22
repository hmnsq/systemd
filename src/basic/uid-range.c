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
        nếu (!range)
                trả_về NULL;

        free(range->entries);
        trả_về mfree(range);
}

static bool uid_range_entry_intersect(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        assert(a);
        assert(b);

        trả_về a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

static số_nguyên uid_range_entry_compare(const UIDRangeEntry *a, const UIDRangeEntry *b) {
        số_nguyên r;

        assert(a);
        assert(b);

        r = CMP(a->start, b->start);
        nếu (r != 0)
                trả_về r;

        trả_về CMP(a->nr, b->nr);
}

static void uid_range_coalesce(UIDRange *range) {
        assert(range);

        nếu (range->n_entries <= 0)
                trả_về;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        cho (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                cho (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        nếu (!uid_range_entry_intersect(x, y))
                                break;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        assert(x->start <= UINT32_MAX - x->nr);
                        assert(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        nếu (range->n_entries > j + 1)
                                memmove(y, y + 1, sizeof(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        assert(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        assert(j > 0);
                        j--;
                }
        }
}

số_nguyên uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = NULL;
        UIDRange *p;

        assert(range);

        nếu (nr <= 0)
                trả_về 0;

        nếu (start > UINT32_MAX - nr) /* overflow check */
                trả_về -ERANGE;

        nếu (*range)
                p = *range;
        không_thì {
                range_new = new0(UIDRange, 1);
                nếu (!range_new)
                        trả_về -ENOMEM;

                p = range_new;
        }

        nếu (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                trả_về -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        nếu (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        trả_về 0;
}

số_nguyên uid_range_add_str_full(UIDRange **range, const char *s, bool coalesce) {
        uid_t start, end;
        số_nguyên r;

        assert(range);
        assert(s);

        r = parse_uid_range(s, &start, &end);
        nếu (r < 0)
                trả_về r;

        trả_về uid_range_add_internal(range, start, end - start + 1, coalesce);
}

số_nguyên uid_range_next_lower(const UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        assert(range);
        assert(uid);

        nếu (*uid == 0)
                trả_về -EBUSY;

        candidate = *uid - 1;

        cho (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                nếu (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        trả_về 1;
                }

                nếu (end < candidate)
                        closest = end;
        }

        nếu (closest == UID_INVALID)
                trả_về -EBUSY;

        *uid = closest;
        trả_về 1;
}

bool uid_range_covers(const UIDRange *range, uid_t start, uid_t nr) {
        nếu (nr == 0) /* empty range? always covered... */
                trả_về true;

        nếu (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                trả_về false;

        nếu (!range)
                trả_về false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                nếu (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        trả_về true;

        trả_về false;
}

số_nguyên uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        số_nguyên r;

        assert(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        nếu (r == EOF)
                trả_về errno_or_else(ENOMSG);
        assert(r >= 0);
        nếu (r != 3)
                trả_về -EBADMSG;
        nếu (uid_range <= 0)
                trả_về -EBADMSG;

        nếu (ret_base)
                *ret_base = uid_base;
        nếu (ret_shift)
                *ret_shift = uid_shift;
        nếu (ret_range)
                *ret_range = uid_range;

        trả_về 0;
}

unsigned uid_range_size(const UIDRange *range) {
        nếu (!range)
                trả_về 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        trả_về n;
}

bool uid_range_is_empty(const UIDRange *range) {

        nếu (!range)
                trả_về true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                nếu (e->nr > 0)
                        trả_về false;

        trả_về true;
}

số_nguyên uid_range_load_userns_full(const char *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = NULL;
        _cleanup_fclose_ FILE *f = NULL;
        số_nguyên r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        nếu (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        nếu (!f) {
                r = -errno;

                nếu (r == -ENOENT && path_startswith(path, "/proc/"))
                        trả_về proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                trả_về r;
        }

        range = new0(UIDRange, 1);
        nếu (!range)
                trả_về -ENOMEM;

        cho (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                nếu (r == -ENOMSG)
                        break;
                nếu (r < 0)
                        trả_về r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                nếu (r < 0)
                        trả_về r;
        }

        nếu (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        trả_về 0;
}

số_nguyên uid_range_load_userns_by_fd_full(số_nguyên userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        số_nguyên r;

        assert(userns_fd >= 0);
        assert(mode >= 0);
        assert(mode < _UID_RANGE_USERNS_MODE_MAX);
        assert(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        nếu (r < 0)
                trả_về r;
        nếu (r > 0)
                trả_về uid_range_load_userns_full(/* path= */ NULL, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        nếu (r < 0)
                trả_về r;

        const char *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        trả_về uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(const UIDRange *range, uid_t start, uid_t nr) {

        nếu (!range)
                trả_về false;

        /* Avoid overflow */
        nếu (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        nếu (nr == 0)
                trả_về false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                nếu (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        trả_về true;

        trả_về false;
}

số_nguyên uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        assert(range);

        nếu (min > max)
                trả_về -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                nếu (entry_end <= min || e->start > max)
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

        trả_về 0;
}

số_nguyên uid_range_partition(UIDRange *range, uid_t size) {
        assert(range);
        assert(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        nếu (n_new_entries == 0) {
                range->n_entries = 0;
                trả_về 0;
        }

        nếu (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                trả_về -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        cho (size_t i = 0; i < range->n_entries; i++)
                nếu (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        cho (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                cho (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        trả_về 0;
}

số_nguyên uid_range_copy(const UIDRange *range, UIDRange **ret) {
        assert(ret);

        nếu (!range) {
                *ret = NULL;
                trả_về 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        nếu (!copy)
                trả_về -ENOMEM;

        nếu (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                nếu (!copy->entries)
                        trả_về -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        trả_về 0;
}

số_nguyên uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        assert(range);

        nếu (size == 0)
                trả_về 0;

        uid_t end = start + size; /* one past the last UID to remove */

        cho (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                nếu (entry_end <= start || e->start >= end)
                        continue;

                /* Check if this removal splits the entry into two parts */
                nếu (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        nếu (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                trả_về -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * sizeof(UIDRangeEntry));
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
                nếu (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * sizeof(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        continue;
                }

                /* Removal trims the start of the entry */
                nếu (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        continue;
                }

                /* Removal trims the end of the entry */
                nếu (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        continue;
                }
        }

        trả_về 0;
}

số_nguyên uid_range_translate(const UIDRange *outside, const UIDRange *inside, uid_t uid, uid_t *ret) {
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

        cho (size_t i = 0; i < uid_range_entries(outside); i++)
                assert(outside->entries[i].nr == inside->entries[i].nr);

        cho (size_t i = 0; i < uid_range_entries(outside); i++) {
                const UIDRangeEntry *e = outside->entries + i;

                nếu (uid < e->start || uid >= e->start + e->nr)
                        continue;

                *ret = inside->entries[i].start + uid - e->start;
                trả_về 0;
        }

        trả_về -ESRCH;
}

số_nguyên uid_range_translate_userns_fd(số_nguyên userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        số_nguyên r;

        assert(userns_fd >= 0);
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        nếu (r < 0)
                trả_về r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = NULL;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        nếu (r < 0)
                trả_về r;

        trả_về uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(const UIDRange *a, const UIDRange *b) {
        nếu (a == b)
                trả_về true;

        nếu (!a || !b)
                trả_về false;

        nếu (a->n_entries != b->n_entries)
                trả_về false;

        cho (size_t i = 0; i < a->n_entries; i++) {
                nếu (a->entries[i].start != b->entries[i].start)
                        trả_về false;
                nếu (a->entries[i].nr != b->entries[i].nr)
                        trả_về false;
        }

        trả_về true;
}

số_nguyên uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        số_nguyên r;

        assert(pid_is_valid(pid));
        assert(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        const char *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        nếu (!f) {
                nếu (errno != ENOENT)
                        trả_về -errno;

                r = proc_mounted();
                nếu (r < 0)
                        trả_về -ENOENT; /* original error, if we can't determine /proc/ state */

                trả_về r ? -ENOPKG : -ENOSYS;
        }

        cho (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ NULL);
                nếu (r < 0)
                        trả_về r;

                nếu (uid_base == 0) {
                        nếu (ret)
                                *ret = uid_shift;
                        trả_về 0;
                }
        }
}

uid_t uid_range_base(const UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        nếu (uid_range_is_empty(range))
                trả_về UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                nếu (e->nr > 0)
                        trả_về e->start;

        trả_về UID_INVALID;
}
