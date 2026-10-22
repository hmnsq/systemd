/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <sched.h>
#شامل <string.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "format-util.h"
#شامل "namespace-util.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "sort-util.h"
#شامل "stat-util.h"
#شامل "uid-range.h"
#شامل "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        اگر (!range)
                بازگشت پوچ;

        آزاد(range->entries);
        بازگشت mfree(range);
}

ایستا bool uid_range_entry_intersect(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        أكد(a);
        أكد(b);

        بازگشت a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

ایستا صحیح uid_range_entry_compare(ثابت UIDRangeEntry *a, ثابت UIDRangeEntry *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(a->start, b->start);
        اگر (r != 0)
                بازگشت r;

        بازگشت CMP(a->nr, b->nr);
}

ایستا تهی uid_range_coalesce(UIDRange *range) {
        أكد(range);

        اگر (range->n_entries <= 0)
                بازگشت;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        برای (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                برای (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        اگر (!uid_range_entry_intersect(x, y))
                                بشکن;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        أكد(x->start <= UINT32_MAX - x->nr);
                        أكد(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        اگر (range->n_entries > j + 1)
                                memmove(y, y + 1, اندازه(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        أكد(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        أكد(j > 0);
                        j--;
                }
        }
}

صحیح uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = پوچ;
        UIDRange *p;

        أكد(range);

        اگر (nr <= 0)
                بازگشت 0;

        اگر (start > UINT32_MAX - nr) /* overflow check */
                بازگشت -ERANGE;

        اگر (*range)
                p = *range;
        وگرنه {
                range_new = new0(UIDRange, 1);
                اگر (!range_new)
                        بازگشت -ENOMEM;

                p = range_new;
        }

        اگر (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                بازگشت -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        اگر (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        بازگشت 0;
}

صحیح uid_range_add_str_full(UIDRange **range, ثابت نویسه *s, bool coalesce) {
        uid_t start, end;
        صحیح r;

        أكد(range);
        أكد(s);

        r = parse_uid_range(s, &start, &end);
        اگر (r < 0)
                بازگشت r;

        بازگشت uid_range_add_internal(range, start, end - start + 1, coalesce);
}

صحیح uid_range_next_lower(ثابت UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        أكد(range);
        أكد(uid);

        اگر (*uid == 0)
                بازگشت -EBUSY;

        candidate = *uid - 1;

        برای (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                اگر (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        بازگشت 1;
                }

                اگر (end < candidate)
                        closest = end;
        }

        اگر (closest == UID_INVALID)
                بازگشت -EBUSY;

        *uid = closest;
        بازگشت 1;
}

bool uid_range_covers(ثابت UIDRange *range, uid_t start, uid_t nr) {
        اگر (nr == 0) /* empty range? always covered... */
                بازگشت true;

        اگر (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                بازگشت false;

        اگر (!range)
                بازگشت false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                اگر (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        بازگشت true;

        بازگشت false;
}

صحیح uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        صحیح r;

        أكد(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        اگر (r == EOF)
                بازگشت errno_or_else(ENOMSG);
        أكد(r >= 0);
        اگر (r != 3)
                بازگشت -EBADMSG;
        اگر (uid_range <= 0)
                بازگشت -EBADMSG;

        اگر (ret_base)
                *ret_base = uid_base;
        اگر (ret_shift)
                *ret_shift = uid_shift;
        اگر (ret_range)
                *ret_range = uid_range;

        بازگشت 0;
}

unsigned uid_range_size(ثابت UIDRange *range) {
        اگر (!range)
                بازگشت 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        بازگشت n;
}

bool uid_range_is_empty(ثابت UIDRange *range) {

        اگر (!range)
                بازگشت true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                اگر (e->nr > 0)
                        بازگشت false;

        بازگشت true;
}

صحیح uid_range_load_userns_full(ثابت نویسه *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = پوچ;
        _cleanup_fclose_ FILE *f = پوچ;
        صحیح r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        أكد(mode >= 0);
        أكد(mode < _UID_RANGE_USERNS_MODE_MAX);
        أكد(ret);

        اگر (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        اگر (!f) {
                r = -errno;

                اگر (r == -ENOENT && path_startswith(path, "/proc/"))
                        بازگشت proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                بازگشت r;
        }

        range = new0(UIDRange, 1);
        اگر (!range)
                بازگشت -ENOMEM;

        برای (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                اگر (r == -ENOMSG)
                        بشکن;
                اگر (r < 0)
                        بازگشت r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        بازگشت 0;
}

صحیح uid_range_load_userns_by_fd_full(صحیح userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحیح r;

        أكد(userns_fd >= 0);
        أكد(mode >= 0);
        أكد(mode < _UID_RANGE_USERNS_MODE_MAX);
        أكد(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        اگر (r < 0)
                بازگشت r;
        اگر (r > 0)
                بازگشت uid_range_load_userns_full(/* path= */ پوچ, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        اگر (r < 0)
                بازگشت r;

        ثابت نویسه *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        بازگشت uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t start, uid_t nr) {

        اگر (!range)
                بازگشت false;

        /* Avoid overflow */
        اگر (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        اگر (nr == 0)
                بازگشت false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                اگر (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        بازگشت true;

        بازگشت false;
}

صحیح uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        أكد(range);

        اگر (min > max)
                بازگشت -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                اگر (entry_end <= min || e->start > max)
                        ادامه;

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

        بازگشت 0;
}

صحیح uid_range_partition(UIDRange *range, uid_t size) {
        أكد(range);
        أكد(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        اگر (n_new_entries == 0) {
                range->n_entries = 0;
                بازگشت 0;
        }

        اگر (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                بازگشت -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        برای (size_t i = 0; i < range->n_entries; i++)
                اگر (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        برای (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                برای (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        بازگشت 0;
}

صحیح uid_range_copy(ثابت UIDRange *range, UIDRange **ret) {
        أكد(ret);

        اگر (!range) {
                *ret = پوچ;
                بازگشت 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        اگر (!copy)
                بازگشت -ENOMEM;

        اگر (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                اگر (!copy->entries)
                        بازگشت -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        بازگشت 0;
}

صحیح uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        أكد(range);

        اگر (size == 0)
                بازگشت 0;

        uid_t end = start + size; /* one past the last UID to remove */

        برای (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                اگر (entry_end <= start || e->start >= end)
                        ادامه;

                /* Check if this removal splits the entry into two parts */
                اگر (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        اگر (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                بازگشت -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * اندازه(UIDRangeEntry));
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
                        ادامه;
                }

                /* Removal covers the entire entry */
                اگر (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * اندازه(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        ادامه;
                }

                /* Removal trims the start of the entry */
                اگر (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        ادامه;
                }

                /* Removal trims the end of the entry */
                اگر (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        ادامه;
                }
        }

        بازگشت 0;
}

صحیح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret) {
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

        برای (size_t i = 0; i < uid_range_entries(outside); i++)
                أكد(outside->entries[i].nr == inside->entries[i].nr);

        برای (size_t i = 0; i < uid_range_entries(outside); i++) {
                ثابت UIDRangeEntry *e = outside->entries + i;

                اگر (uid < e->start || uid >= e->start + e->nr)
                        ادامه;

                *ret = inside->entries[i].start + uid - e->start;
                بازگشت 0;
        }

        بازگشت -ESRCH;
}

صحیح uid_range_translate_userns_fd(صحیح userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        صحیح r;

        أكد(userns_fd >= 0);
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = پوچ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        اگر (r < 0)
                بازگشت r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = پوچ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        اگر (r < 0)
                بازگشت r;

        بازگشت uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b) {
        اگر (a == b)
                بازگشت true;

        اگر (!a || !b)
                بازگشت false;

        اگر (a->n_entries != b->n_entries)
                بازگشت false;

        برای (size_t i = 0; i < a->n_entries; i++) {
                اگر (a->entries[i].start != b->entries[i].start)
                        بازگشت false;
                اگر (a->entries[i].nr != b->entries[i].nr)
                        بازگشت false;
        }

        بازگشت true;
}

صحیح uid_map_جستجو_ریشه(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        صحیح r;

        أكد(pid_is_valid(pid));
        أكد(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ثابت نویسه *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        اگر (!f) {
                اگر (errno != ENOENT)
                        بازگشت -errno;

                r = proc_mounted();
                اگر (r < 0)
                        بازگشت -ENOENT; /* original error, if we can't determine /proc/ state */

                بازگشت r ? -ENOPKG : -ENOSYS;
        }

        برای (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ پوچ);
                اگر (r < 0)
                        بازگشت r;

                اگر (uid_base == 0) {
                        اگر (ret)
                                *ret = uid_shift;
                        بازگشت 0;
                }
        }
}

uid_t uid_range_base(ثابت UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        اگر (uid_range_is_empty(range))
                بازگشت UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                اگر (e->nr > 0)
                        بازگشت e->start;

        بازگشت UID_INVALID;
}
