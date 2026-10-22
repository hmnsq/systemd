/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <sched.h>
#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "format-util.h"
#ضمن "namespace-util.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "sort-util.h"
#ضمن "stat-util.h"
#ضمن "uid-range.h"
#ضمن "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        אם (!range)
                החזר عدم;

        حرر(range->entries);
        החזר mfree(range);
}

ساكن bool uid_range_entry_intersect(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        أكد(a);
        أكد(b);

        החזר a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

ساكن صحيح uid_range_entry_compare(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(a->start, b->start);
        אם (r != 0)
                החזר r;

        החזר CMP(a->nr, b->nr);
}

ساكن فراغ uid_range_coalesce(UIDRange *range) {
        أكد(range);

        אם (range->n_entries <= 0)
                החזר;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        עבור (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                עבור (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        אם (!uid_range_entry_intersect(x, y))
                                اكسر;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        أكد(x->start <= UINT32_MAX - x->nr);
                        أكد(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        אם (range->n_entries > j + 1)
                                memmove(y, y + 1, حجم(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        أكد(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        أكد(j > 0);
                        j--;
                }
        }
}

صحيح uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = عدم;
        UIDRange *p;

        أكد(range);

        אם (nr <= 0)
                החזר 0;

        אם (start > UINT32_MAX - nr) /* overflow check */
                החזר -ERANGE;

        אם (*range)
                p = *range;
        אחרת {
                range_new = new0(UIDRange, 1);
                אם (!range_new)
                        החזר -ENOMEM;

                p = range_new;
        }

        אם (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                החזר -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        אם (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        החזר 0;
}

صحيح uid_range_add_str_full(UIDRange **range, ثابت حرف *s, bool coalesce) {
        uid_t start, end;
        صحيح r;

        أكد(range);
        أكد(s);

        r = parse_uid_range(s, &start, &end);
        אם (r < 0)
                החזר r;

        החזר uid_range_add_internal(range, start, end - start + 1, coalesce);
}

صحيح uid_range_next_lower(ثابت UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        أكد(range);
        أكد(uid);

        אם (*uid == 0)
                החזר -EBUSY;

        candidate = *uid - 1;

        עבור (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                אם (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        החזר 1;
                }

                אם (end < candidate)
                        closest = end;
        }

        אם (closest == UID_INVALID)
                החזר -EBUSY;

        *uid = closest;
        החזר 1;
}

bool uid_range_covers(ثابت UIDRange *range, uid_t start, uid_t nr) {
        אם (nr == 0) /* empty range? always covered... */
                החזר true;

        אם (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                החזר false;

        אם (!range)
                החזר false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                אם (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        החזר true;

        החזר false;
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        صحيح r;

        أكد(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        אם (r == EOF)
                החזר errno_or_else(ENOMSG);
        أكد(r >= 0);
        אם (r != 3)
                החזר -EBADMSG;
        אם (uid_range <= 0)
                החזר -EBADMSG;

        אם (ret_base)
                *ret_base = uid_base;
        אם (ret_shift)
                *ret_shift = uid_shift;
        אם (ret_range)
                *ret_range = uid_range;

        החזר 0;
}

unsigned uid_range_size(ثابت UIDRange *range) {
        אם (!range)
                החזר 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        החזר n;
}

bool uid_range_is_empty(ثابت UIDRange *range) {

        אם (!range)
                החזר true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                אם (e->nr > 0)
                        החזר false;

        החזר true;
}

صحيح uid_range_load_userns_full(ثابت حرف *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        أكد(mode >= 0);
        أكد(mode < _UID_RANGE_USERNS_MODE_MAX);
        أكد(ret);

        אם (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        אם (!f) {
                r = -errno;

                אם (r == -ENOENT && path_startswith(path, "/proc/"))
                        החזר proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                החזר r;
        }

        range = new0(UIDRange, 1);
        אם (!range)
                החזר -ENOMEM;

        עבור (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                אם (r == -ENOMSG)
                        اكسر;
                אם (r < 0)
                        החזר r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                אם (r < 0)
                        החזר r;
        }

        אם (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        החזר 0;
}

صحيح uid_range_load_userns_by_fd_full(صحيح userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(mode >= 0);
        أكد(mode < _UID_RANGE_USERNS_MODE_MAX);
        أكد(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        אם (r < 0)
                החזר r;
        אם (r > 0)
                החזר uid_range_load_userns_full(/* path= */ عدم, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        אם (r < 0)
                החזר r;

        ثابت حرف *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        החזר uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t start, uid_t nr) {

        אם (!range)
                החזר false;

        /* Avoid overflow */
        אם (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        אם (nr == 0)
                החזר false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                אם (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        החזר true;

        החזר false;
}

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        أكد(range);

        אם (min > max)
                החזר -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                אם (entry_end <= min || e->start > max)
                        استمر;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                أكد(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        החזר 0;
}

صحيح uid_range_partition(UIDRange *range, uid_t size) {
        أكد(range);
        أكد(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        אם (n_new_entries == 0) {
                range->n_entries = 0;
                החזר 0;
        }

        אם (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                החזר -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        עבור (size_t i = 0; i < range->n_entries; i++)
                אם (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        עבור (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                עבור (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        החזר 0;
}

صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret) {
        أكد(ret);

        אם (!range) {
                *ret = عدم;
                החזר 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        אם (!copy)
                החזר -ENOMEM;

        אם (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                אם (!copy->entries)
                        החזר -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        החזר 0;
}

صحيح uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        أكد(range);

        אם (size == 0)
                החזר 0;

        uid_t end = start + size; /* one past the last UID to remove */

        עבור (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                אם (entry_end <= start || e->start >= end)
                        استمر;

                /* Check if this removal splits the entry into two parts */
                אם (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        אם (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                החזר -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * حجم(UIDRangeEntry));
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
                        استمر;
                }

                /* Removal covers the entire entry */
                אם (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * حجم(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        استمر;
                }

                /* Removal trims the start of the entry */
                אם (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        استمر;
                }

                /* Removal trims the end of the entry */
                אם (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        استمر;
                }
        }

        החזר 0;
}

صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret) {
        أكد(uid_range_entries(outside) == uid_range_entries(inside));
        أكد(ret);

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

        עבור (size_t i = 0; i < uid_range_entries(outside); i++)
                أكد(outside->entries[i].nr == inside->entries[i].nr);

        עבור (size_t i = 0; i < uid_range_entries(outside); i++) {
                ثابت UIDRangeEntry *e = outside->entries + i;

                אם (uid < e->start || uid >= e->start + e->nr)
                        استمر;

                *ret = inside->entries[i].start + uid - e->start;
                החזר 0;
        }

        החזר -ESRCH;
}

صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = عدم;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        אם (r < 0)
                החזר r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = عدم;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        אם (r < 0)
                החזר r;

        החזר uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b) {
        אם (a == b)
                החזר true;

        אם (!a || !b)
                החזר false;

        אם (a->n_entries != b->n_entries)
                החזר false;

        עבור (size_t i = 0; i < a->n_entries; i++) {
                אם (a->entries[i].start != b->entries[i].start)
                        החזר false;
                אם (a->entries[i].nr != b->entries[i].nr)
                        החזר false;
        }

        החזר true;
}

صحيح uid_map_search_root(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        صحيح r;

        أكد(pid_is_valid(pid));
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ثابت حرف *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        אם (!f) {
                אם (errno != ENOENT)
                        החזר -errno;

                r = proc_mounted();
                אם (r < 0)
                        החזר -ENOENT; /* original error, if we can't determine /proc/ state */

                החזר r ? -ENOPKG : -ENOSYS;
        }

        עבור (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ عدم);
                אם (r < 0)
                        החזר r;

                אם (uid_base == 0) {
                        אם (ret)
                                *ret = uid_shift;
                        החזר 0;
                }
        }
}

uid_t uid_range_base(ثابت UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        אם (uid_range_is_empty(range))
                החזר UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                אם (e->nr > 0)
                        החזר e->start;

        החזר UID_INVALID;
}
