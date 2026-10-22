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
        إذا (!range)
                إرجاع عدم;

        حرر(range->entries);
        إرجاع mfree(range);
}

ساكن bool uid_range_entry_intersect(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        أكد(a);
        أكد(b);

        إرجاع a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

ساكن صحيح uid_range_entry_compare(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(a->start, b->start);
        إذا (r != 0)
                إرجاع r;

        إرجاع CMP(a->nr, b->nr);
}

ساكن فراغ uid_range_coalesce(UIDRange *range) {
        أكد(range);

        إذا (range->n_entries <= 0)
                إرجاع;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        لكل (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                لكل (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        إذا (!uid_range_entry_intersect(x, y))
                                اكسر;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        أكد(x->start <= UINT32_MAX - x->nr);
                        أكد(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        إذا (range->n_entries > j + 1)
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

        إذا (nr <= 0)
                إرجاع 0;

        إذا (start > UINT32_MAX - nr) /* overflow check */
                إرجاع -ERANGE;

        إذا (*range)
                p = *range;
        وإلا {
                range_new = new0(UIDRange, 1);
                إذا (!range_new)
                        إرجاع -ENOMEM;

                p = range_new;
        }

        إذا (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                إرجاع -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        إذا (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        إرجاع 0;
}

صحيح uid_range_add_str_full(UIDRange **range, ثابت حرف *s, bool coalesce) {
        uid_t start, end;
        صحيح r;

        أكد(range);
        أكد(s);

        r = parse_uid_range(s, &start, &end);
        إذا (r < 0)
                إرجاع r;

        إرجاع uid_range_add_internal(range, start, end - start + 1, coalesce);
}

صحيح uid_range_next_lower(ثابت UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        أكد(range);
        أكد(uid);

        إذا (*uid == 0)
                إرجاع -EBUSY;

        candidate = *uid - 1;

        لكل (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                إذا (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        إرجاع 1;
                }

                إذا (end < candidate)
                        closest = end;
        }

        إذا (closest == UID_INVALID)
                إرجاع -EBUSY;

        *uid = closest;
        إرجاع 1;
}

bool uid_range_covers(ثابت UIDRange *range, uid_t start, uid_t nr) {
        إذا (nr == 0) /* empty range? always covered... */
                إرجاع true;

        إذا (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                إرجاع false;

        إذا (!range)
                إرجاع false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                إذا (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        إرجاع true;

        إرجاع false;
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        صحيح r;

        أكد(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        إذا (r == EOF)
                إرجاع errno_or_else(ENOMSG);
        أكد(r >= 0);
        إذا (r != 3)
                إرجاع -EBADMSG;
        إذا (uid_range <= 0)
                إرجاع -EBADMSG;

        إذا (ret_base)
                *ret_base = uid_base;
        إذا (ret_shift)
                *ret_shift = uid_shift;
        إذا (ret_range)
                *ret_range = uid_range;

        إرجاع 0;
}

unsigned uid_range_size(ثابت UIDRange *range) {
        إذا (!range)
                إرجاع 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        إرجاع n;
}

bool uid_range_is_empty(ثابت UIDRange *range) {

        إذا (!range)
                إرجاع true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                إذا (e->nr > 0)
                        إرجاع false;

        إرجاع true;
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

        إذا (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        إذا (!f) {
                r = -errno;

                إذا (r == -ENOENT && path_startswith(path, "/proc/"))
                        إرجاع proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                إرجاع r;
        }

        range = new0(UIDRange, 1);
        إذا (!range)
                إرجاع -ENOMEM;

        لكل (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                إذا (r == -ENOMSG)
                        اكسر;
                إذا (r < 0)
                        إرجاع r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                إذا (r < 0)
                        إرجاع r;
        }

        إذا (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        إرجاع 0;
}

صحيح uid_range_load_userns_by_fd_full(صحيح userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(mode >= 0);
        أكد(mode < _UID_RANGE_USERNS_MODE_MAX);
        أكد(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        إذا (r < 0)
                إرجاع r;
        إذا (r > 0)
                إرجاع uid_range_load_userns_full(/* path= */ عدم, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        إذا (r < 0)
                إرجاع r;

        ثابت حرف *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        إرجاع uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t start, uid_t nr) {

        إذا (!range)
                إرجاع false;

        /* Avoid overflow */
        إذا (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        إذا (nr == 0)
                إرجاع false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                إذا (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        إرجاع true;

        إرجاع false;
}

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        أكد(range);

        إذا (min > max)
                إرجاع -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                إذا (entry_end <= min || e->start > max)
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

        إرجاع 0;
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

        إذا (n_new_entries == 0) {
                range->n_entries = 0;
                إرجاع 0;
        }

        إذا (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                إرجاع -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        لكل (size_t i = 0; i < range->n_entries; i++)
                إذا (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        لكل (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                لكل (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        إرجاع 0;
}

صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret) {
        أكد(ret);

        إذا (!range) {
                *ret = عدم;
                إرجاع 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        إذا (!copy)
                إرجاع -ENOMEM;

        إذا (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                إذا (!copy->entries)
                        إرجاع -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        إرجاع 0;
}

صحيح uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        أكد(range);

        إذا (size == 0)
                إرجاع 0;

        uid_t end = start + size; /* one past the last UID to remove */

        لكل (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                إذا (entry_end <= start || e->start >= end)
                        استمر;

                /* Check if this removal splits the entry into two parts */
                إذا (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        إذا (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                إرجاع -ENOMEM;

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
                إذا (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * حجم(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        استمر;
                }

                /* Removal trims the start of the entry */
                إذا (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        استمر;
                }

                /* Removal trims the end of the entry */
                إذا (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        استمر;
                }
        }

        إرجاع 0;
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

        لكل (size_t i = 0; i < uid_range_entries(outside); i++)
                أكد(outside->entries[i].nr == inside->entries[i].nr);

        لكل (size_t i = 0; i < uid_range_entries(outside); i++) {
                ثابت UIDRangeEntry *e = outside->entries + i;

                إذا (uid < e->start || uid >= e->start + e->nr)
                        استمر;

                *ret = inside->entries[i].start + uid - e->start;
                إرجاع 0;
        }

        إرجاع -ESRCH;
}

صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = عدم;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        إذا (r < 0)
                إرجاع r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = عدم;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        إذا (r < 0)
                إرجاع r;

        إرجاع uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b) {
        إذا (a == b)
                إرجاع true;

        إذا (!a || !b)
                إرجاع false;

        إذا (a->n_entries != b->n_entries)
                إرجاع false;

        لكل (size_t i = 0; i < a->n_entries; i++) {
                إذا (a->entries[i].start != b->entries[i].start)
                        إرجاع false;
                إذا (a->entries[i].nr != b->entries[i].nr)
                        إرجاع false;
        }

        إرجاع true;
}

صحيح uid_map_بحث_جذر(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        صحيح r;

        أكد(pid_is_valid(pid));
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ثابت حرف *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        إذا (!f) {
                إذا (errno != ENOENT)
                        إرجاع -errno;

                r = proc_mounted();
                إذا (r < 0)
                        إرجاع -ENOENT; /* original error, if we can't determine /proc/ state */

                إرجاع r ? -ENOPKG : -ENOSYS;
        }

        لكل (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ عدم);
                إذا (r < 0)
                        إرجاع r;

                إذا (uid_base == 0) {
                        إذا (ret)
                                *ret = uid_shift;
                        إرجاع 0;
                }
        }
}

uid_t uid_range_base(ثابت UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        إذا (uid_range_is_empty(range))
                إرجاع UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                إذا (e->nr > 0)
                        إرجاع e->start;

        إرجاع UID_INVALID;
}
