/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <sched.h>
#અન્તર્ભાવ <string.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "format-util.h"
#અન્તર્ભાવ "namespace-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "pidref.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "sort-util.h"
#અન્તર્ભાવ "stat-util.h"
#અન્તર્ભાવ "uid-range.h"
#અન્તર્ભાવ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        જો (!range)
                પાછા ખાલી;

        મુક્ત(range->entries);
        પાછા mfree(range);
}

સ્થિર bool uid_range_entry_intersect(અચલ UIDRangeEntry *a, અચલ UIDRangeEntry *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

સ્થિર પૂર્ણાંક uid_range_entry_compare(અચલ UIDRangeEntry *a, અચલ UIDRangeEntry *b) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        r = CMP(a->start, b->start);
        જો (r != 0)
                પાછા r;

        પાછા CMP(a->nr, b->nr);
}

સ્થિર શૂન્ય uid_range_coalesce(UIDRange *range) {
        પ્રતિજ્ઞા(range);

        જો (range->n_entries <= 0)
                પાછા;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        માટે (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                માટે (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        જો (!uid_range_entry_intersect(x, y))
                                તોડો;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        પ્રતિજ્ઞા(x->start <= UINT32_MAX - x->nr);
                        પ્રતિજ્ઞા(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        જો (range->n_entries > j + 1)
                                memmove(y, y + 1, કદ(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        પ્રતિજ્ઞા(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        પ્રતિજ્ઞા(j > 0);
                        j--;
                }
        }
}

પૂર્ણાંક uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ખાલી;
        UIDRange *p;

        પ્રતિજ્ઞા(range);

        જો (nr <= 0)
                પાછા 0;

        જો (start > UINT32_MAX - nr) /* overflow check */
                પાછા -ERANGE;

        જો (*range)
                p = *range;
        નહીંતર {
                range_new = new0(UIDRange, 1);
                જો (!range_new)
                        પાછા -ENOMEM;

                p = range_new;
        }

        જો (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                પાછા -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        જો (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        પાછા 0;
}

પૂર્ણાંક uid_range_add_str_full(UIDRange **range, અચલ અક્ષર *s, bool coalesce) {
        uid_t start, end;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(range);
        પ્રતિજ્ઞા(s);

        r = parse_uid_range(s, &start, &end);
        જો (r < 0)
                પાછા r;

        પાછા uid_range_add_internal(range, start, end - start + 1, coalesce);
}

પૂર્ણાંક uid_range_next_lower(અચલ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        પ્રતિજ્ઞા(range);
        પ્રતિજ્ઞા(uid);

        જો (*uid == 0)
                પાછા -EBUSY;

        candidate = *uid - 1;

        માટે (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                જો (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        પાછા 1;
                }

                જો (end < candidate)
                        closest = end;
        }

        જો (closest == UID_INVALID)
                પાછા -EBUSY;

        *uid = closest;
        પાછા 1;
}

bool uid_range_covers(અચલ UIDRange *range, uid_t start, uid_t nr) {
        જો (nr == 0) /* empty range? always covered... */
                પાછા true;

        જો (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                પાછા false;

        જો (!range)
                પાછા false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                જો (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        પાછા true;

        પાછા false;
}

પૂર્ણાંક uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        જો (r == EOF)
                પાછા errno_or_else(ENOMSG);
        પ્રતિજ્ઞા(r >= 0);
        જો (r != 3)
                પાછા -EBADMSG;
        જો (uid_range <= 0)
                પાછા -EBADMSG;

        જો (ret_base)
                *ret_base = uid_base;
        જો (ret_shift)
                *ret_shift = uid_shift;
        જો (ret_range)
                *ret_range = uid_range;

        પાછા 0;
}

unsigned uid_range_size(અચલ UIDRange *range) {
        જો (!range)
                પાછા 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        પાછા n;
}

bool uid_range_is_empty(અચલ UIDRange *range) {

        જો (!range)
                પાછા true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                જો (e->nr > 0)
                        પાછા false;

        પાછા true;
}

પૂર્ણાંક uid_range_load_userns_full(અચલ અક્ષર *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ખાલી;
        _cleanup_fclose_ FILE *f = ખાલી;
        પૂર્ણાંક r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        પ્રતિજ્ઞા(mode >= 0);
        પ્રતિજ્ઞા(mode < _UID_RANGE_USERNS_MODE_MAX);
        પ્રતિજ્ઞા(ret);

        જો (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        જો (!f) {
                r = -errno;

                જો (r == -ENOENT && path_startswith(path, "/proc/"))
                        પાછા proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                પાછા r;
        }

        range = new0(UIDRange, 1);
        જો (!range)
                પાછા -ENOMEM;

        માટે (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                જો (r == -ENOMSG)
                        તોડો;
                જો (r < 0)
                        પાછા r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                જો (r < 0)
                        પાછા r;
        }

        જો (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        પાછા 0;
}

પૂર્ણાંક uid_range_load_userns_by_fd_full(પૂર્ણાંક userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);
        પ્રતિજ્ઞા(mode >= 0);
        પ્રતિજ્ઞા(mode < _UID_RANGE_USERNS_MODE_MAX);
        પ્રતિજ્ઞા(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        જો (r < 0)
                પાછા r;
        જો (r > 0)
                પાછા uid_range_load_userns_full(/* path= */ ખાલી, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        જો (r < 0)
                પાછા r;

        અચલ અક્ષર *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        પાછા uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(અચલ UIDRange *range, uid_t start, uid_t nr) {

        જો (!range)
                પાછા false;

        /* Avoid overflow */
        જો (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        જો (nr == 0)
                પાછા false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                જો (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        પાછા true;

        પાછા false;
}

પૂર્ણાંક uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        પ્રતિજ્ઞા(range);

        જો (min > max)
                પાછા -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                જો (entry_end <= min || e->start > max)
                        ચાલુ;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                પ્રતિજ્ઞા(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        પાછા 0;
}

પૂર્ણાંક uid_range_partition(UIDRange *range, uid_t size) {
        પ્રતિજ્ઞા(range);
        પ્રતિજ્ઞા(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        જો (n_new_entries == 0) {
                range->n_entries = 0;
                પાછા 0;
        }

        જો (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                પાછા -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        માટે (size_t i = 0; i < range->n_entries; i++)
                જો (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        માટે (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                માટે (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        પાછા 0;
}

પૂર્ણાંક uid_range_copy(અચલ UIDRange *range, UIDRange **ret) {
        પ્રતિજ્ઞા(ret);

        જો (!range) {
                *ret = ખાલી;
                પાછા 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        જો (!copy)
                પાછા -ENOMEM;

        જો (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                જો (!copy->entries)
                        પાછા -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        પાછા 0;
}

પૂર્ણાંક uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        પ્રતિજ્ઞા(range);

        જો (size == 0)
                પાછા 0;

        uid_t end = start + size; /* one past the last UID to remove */

        માટે (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                જો (entry_end <= start || e->start >= end)
                        ચાલુ;

                /* Check if this removal splits the entry into two parts */
                જો (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        જો (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                પાછા -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * કદ(UIDRangeEntry));
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
                        ચાલુ;
                }

                /* Removal covers the entire entry */
                જો (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * કદ(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        ચાલુ;
                }

                /* Removal trims the start of the entry */
                જો (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        ચાલુ;
                }

                /* Removal trims the end of the entry */
                જો (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        ચાલુ;
                }
        }

        પાછા 0;
}

પૂર્ણાંક uid_range_translate(અચલ UIDRange *outside, અચલ UIDRange *inside, uid_t uid, uid_t *ret) {
        પ્રતિજ્ઞા(uid_range_entries(outside) == uid_range_entries(inside));
        પ્રતિજ્ઞા(ret);

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

        માટે (size_t i = 0; i < uid_range_entries(outside); i++)
                પ્રતિજ્ઞા(outside->entries[i].nr == inside->entries[i].nr);

        માટે (size_t i = 0; i < uid_range_entries(outside); i++) {
                અચલ UIDRangeEntry *e = outside->entries + i;

                જો (uid < e->start || uid >= e->start + e->nr)
                        ચાલુ;

                *ret = inside->entries[i].start + uid - e->start;
                પાછા 0;
        }

        પાછા -ESRCH;
}

પૂર્ણાંક uid_range_translate_userns_fd(પૂર્ણાંક userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(userns_fd >= 0);
        પ્રતિજ્ઞા(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ખાલી;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        જો (r < 0)
                પાછા r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ખાલી;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        જો (r < 0)
                પાછા r;

        પાછા uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(અચલ UIDRange *a, અચલ UIDRange *b) {
        જો (a == b)
                પાછા true;

        જો (!a || !b)
                પાછા false;

        જો (a->n_entries != b->n_entries)
                પાછા false;

        માટે (size_t i = 0; i < a->n_entries; i++) {
                જો (a->entries[i].start != b->entries[i].start)
                        પાછા false;
                જો (a->entries[i].nr != b->entries[i].nr)
                        પાછા false;
        }

        પાછા true;
}

પૂર્ણાંક uid_map_શોધ_મૂળ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(pid_is_valid(pid));
        પ્રતિજ્ઞા(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        અચલ અક્ષર *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        જો (!f) {
                જો (errno != ENOENT)
                        પાછા -errno;

                r = proc_mounted();
                જો (r < 0)
                        પાછા -ENOENT; /* original error, if we can't determine /proc/ state */

                પાછા r ? -ENOPKG : -ENOSYS;
        }

        માટે (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ ખાલી);
                જો (r < 0)
                        પાછા r;

                જો (uid_base == 0) {
                        જો (ret)
                                *ret = uid_shift;
                        પાછા 0;
                }
        }
}

uid_t uid_range_base(અચલ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        જો (uid_range_is_empty(range))
                પાછા UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                જો (e->nr > 0)
                        પાછા e->start;

        પાછા UID_INVALID;
}
