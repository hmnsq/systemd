/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <sched.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "uid-range.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಮುಕ್ತ(range->entries);
        ಪ್ರತ್ಯಾಗಮ mfree(range);
}

ಸ್ಥಿರ bool uid_range_entry_intersect(ಧ್ರುವ UIDRangeEntry *a, ಧ್ರುವ UIDRangeEntry *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ uid_range_entry_compare(ಧ್ರುವ UIDRangeEntry *a, ಧ್ರುವ UIDRangeEntry *b) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->start, b->start);
        ಯದಿ (r != 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ CMP(a->nr, b->nr);
}

ಸ್ಥಿರ ಶೂನ್ಯ uid_range_coalesce(UIDRange *range) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಯದಿ (range->n_entries <= 0)
                ಪ್ರತ್ಯಾಗಮ;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        ಪ್ರತಿ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                ಪ್ರತಿ (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        ಯದಿ (!uid_range_entry_intersect(x, y))
                                ಭಙ್ಗ;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ಪ್ರತಿಜ್ಞಾ(x->start <= UINT32_MAX - x->nr);
                        ಪ್ರತಿಜ್ಞಾ(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        ಯದಿ (range->n_entries > j + 1)
                                memmove(y, y + 1, ಪರಿಮಾಣ(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ಪ್ರತಿಜ್ಞಾ(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ಪ್ರತಿಜ್ಞಾ(j > 0);
                        j--;
                }
        }
}

ಪೂರ್ಣಾಙ್ಕ uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ರಿಕ್ತ;
        UIDRange *p;

        ಪ್ರತಿಜ್ಞಾ(range);

        ಯದಿ (nr <= 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        ಯದಿ (start > UINT32_MAX - nr) /* overflow check */
                ಪ್ರತ್ಯಾಗಮ -ERANGE;

        ಯದಿ (*range)
                p = *range;
        ಅನ್ಯಥಾ {
                range_new = new0(UIDRange, 1);
                ಯದಿ (!range_new)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                p = range_new;
        }

        ಯದಿ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        ಯದಿ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_add_str_full(UIDRange **range, ಧ್ರುವ ವರ್ಣ *s, bool coalesce) {
        uid_t start, end;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(s);

        r = parse_uid_range(s, &start, &end);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ uid_range_add_internal(range, start, end - start + 1, coalesce);
}

ಪೂರ್ಣಾಙ್ಕ uid_range_next_lower(ಧ್ರುವ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(uid);

        ಯದಿ (*uid == 0)
                ಪ್ರತ್ಯಾಗಮ -EBUSY;

        candidate = *uid - 1;

        ಪ್ರತಿ (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                ಯದಿ (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        ಪ್ರತ್ಯಾಗಮ 1;
                }

                ಯದಿ (end < candidate)
                        closest = end;
        }

        ಯದಿ (closest == UID_INVALID)
                ಪ್ರತ್ಯಾಗಮ -EBUSY;

        *uid = closest;
        ಪ್ರತ್ಯಾಗಮ 1;
}

bool uid_range_covers(ಧ್ರುವ UIDRange *range, uid_t start, uid_t nr) {
        ಯದಿ (nr == 0) /* empty range? always covered... */
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ಯದಿ (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        ಯದಿ (r == EOF)
                ಪ್ರತ್ಯಾಗಮ errno_or_else(ENOMSG);
        ಪ್ರತಿಜ್ಞಾ(r >= 0);
        ಯದಿ (r != 3)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;
        ಯದಿ (uid_range <= 0)
                ಪ್ರತ್ಯಾಗಮ -EBADMSG;

        ಯದಿ (ret_base)
                *ret_base = uid_base;
        ಯದಿ (ret_shift)
                *ret_shift = uid_shift;
        ಯದಿ (ret_range)
                *ret_range = uid_range;

        ಪ್ರತ್ಯಾಗಮ 0;
}

unsigned uid_range_size(ಧ್ರುವ UIDRange *range) {
        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ಪ್ರತ್ಯಾಗಮ n;
}

bool uid_range_is_empty(ಧ್ರುವ UIDRange *range) {

        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ಯದಿ (e->nr > 0)
                        ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns_full(ಧ್ರುವ ವರ್ಣ *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ರಿಕ್ತ;
        _cleanup_fclose_ FILE *f = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ಪ್ರತಿಜ್ಞಾ(mode >= 0);
        ಪ್ರತಿಜ್ಞಾ(mode < _UID_RANGE_USERNS_MODE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        ಯದಿ (!f) {
                r = -errno;

                ಯದಿ (r == -ENOENT && path_startswith(path, "/proc/"))
                        ಪ್ರತ್ಯಾಗಮ proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ಪ್ರತ್ಯಾಗಮ r;
        }

        range = new0(UIDRange, 1);
        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಪ್ರತಿ (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                ಯದಿ (r == -ENOMSG)
                        ಭಙ್ಗ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_load_userns_by_fd_full(ಪೂರ್ಣಾಙ್ಕ userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(mode >= 0);
        ಪ್ರತಿಜ್ಞಾ(mode < _UID_RANGE_USERNS_MODE_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r > 0)
                ಪ್ರತ್ಯಾಗಮ uid_range_load_userns_full(/* path= */ ರಿಕ್ತ, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಧ್ರುವ ವರ್ಣ *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ಪ್ರತ್ಯಾಗಮ uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ಧ್ರುವ UIDRange *range, uid_t start, uid_t nr) {

        ಯದಿ (!range)
                ಪ್ರತ್ಯಾಗಮ false;

        /* Avoid overflow */
        ಯದಿ (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        ಯದಿ (nr == 0)
                ಪ್ರತ್ಯಾಗಮ false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                ಯದಿ (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ false;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಯದಿ (min > max)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ಯದಿ (entry_end <= min || e->start > max)
                        ಅನುವರ್ತ;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                ಪ್ರತಿಜ್ಞಾ(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_partition(UIDRange *range, uid_t size) {
        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        ಯದಿ (n_new_entries == 0) {
                range->n_entries = 0;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಯದಿ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        ಪ್ರತಿ (size_t i = 0; i < range->n_entries; i++)
                ಯದಿ (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        ಪ್ರತಿ (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                ಪ್ರತಿ (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_copy(ಧ್ರುವ UIDRange *range, UIDRange **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (!range) {
                *ret = ರಿಕ್ತ;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ಯದಿ (!copy)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಯದಿ (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                ಯದಿ (!copy->entries)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಯದಿ (size == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        uid_t end = start + size; /* one past the last UID to remove */

        ಪ್ರತಿ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                ಯದಿ (entry_end <= start || e->start >= end)
                        ಅನುವರ್ತ;

                /* Check if this removal splits the entry into two parts */
                ಯದಿ (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        ಯದಿ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * ಪರಿಮಾಣ(UIDRangeEntry));
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
                        ಅನುವರ್ತ;
                }

                /* Removal covers the entire entry */
                ಯದಿ (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * ಪರಿಮಾಣ(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        ಅನುವರ್ತ;
                }

                /* Removal trims the start of the entry */
                ಯದಿ (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        ಅನುವರ್ತ;
                }

                /* Removal trims the end of the entry */
                ಯದಿ (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        ಅನುವರ್ತ;
                }
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_translate(ಧ್ರುವ UIDRange *outside, ಧ್ರುವ UIDRange *inside, uid_t uid, uid_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(uid_range_entries(outside) == uid_range_entries(inside));
        ಪ್ರತಿಜ್ಞಾ(ret);

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

        ಪ್ರತಿ (size_t i = 0; i < uid_range_entries(outside); i++)
                ಪ್ರತಿಜ್ಞಾ(outside->entries[i].nr == inside->entries[i].nr);

        ಪ್ರತಿ (size_t i = 0; i < uid_range_entries(outside); i++) {
                ಧ್ರುವ UIDRangeEntry *e = outside->entries + i;

                ಯದಿ (uid < e->start || uid >= e->start + e->nr)
                        ಅನುವರ್ತ;

                *ret = inside->entries[i].start + uid - e->start;
                ಪ್ರತ್ಯಾಗಮ 0;
        }

        ಪ್ರತ್ಯಾಗಮ -ESRCH;
}

ಪೂರ್ಣಾಙ್ಕ uid_range_translate_userns_fd(ಪೂರ್ಣಾಙ್ಕ userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ರಿಕ್ತ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ರಿಕ್ತ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ಧ್ರುವ UIDRange *a, ಧ್ರುವ UIDRange *b) {
        ಯದಿ (a == b)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (!a || !b)
                ಪ್ರತ್ಯಾಗಮ false;

        ಯದಿ (a->n_entries != b->n_entries)
                ಪ್ರತ್ಯಾಗಮ false;

        ಪ್ರತಿ (size_t i = 0; i < a->n_entries; i++) {
                ಯದಿ (a->entries[i].start != b->entries[i].start)
                        ಪ್ರತ್ಯಾಗಮ false;
                ಯದಿ (a->entries[i].nr != b->entries[i].nr)
                        ಪ್ರತ್ಯಾಗಮ false;
        }

        ಪ್ರತ್ಯಾಗಮ true;
}

ಪೂರ್ಣಾಙ್ಕ uid_map_ಹುಡುಕು_ಬೇರು(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid_is_valid(pid));
        ಪ್ರತಿಜ್ಞಾ(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ಧ್ರುವ ವರ್ಣ *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ಯದಿ (!f) {
                ಯದಿ (errno != ENOENT)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                r = proc_mounted();
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ -ENOENT; /* original error, if we can't determine /proc/ state */

                ಪ್ರತ್ಯಾಗಮ r ? -ENOPKG : -ENOSYS;
        }

        ಪ್ರತಿ (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ ರಿಕ್ತ);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (uid_base == 0) {
                        ಯದಿ (ret)
                                *ret = uid_shift;
                        ಪ್ರತ್ಯಾಗಮ 0;
                }
        }
}

uid_t uid_range_base(ಧ್ರುವ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ಯದಿ (uid_range_is_empty(range))
                ಪ್ರತ್ಯಾಗಮ UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ಯದಿ (e->nr > 0)
                        ಪ್ರತ್ಯಾಗಮ e->start;

        ಪ್ರತ್ಯಾಗಮ UID_INVALID;
}
