/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <sched.h>
#অন্তর্ভাব <string.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "format-util.h"
#অন্তর্ভাব "namespace-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "pidref.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "sort-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "uid-range.h"
#অন্তর্ভাব "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        যদি (!range)
                ফেরত ফাঁকা;

        মুক্ত(range->entries);
        ফেরত mfree(range);
}

স্থির bool uid_range_entry_intersect(ধ্রুবক UIDRangeEntry *a, ধ্রুবক UIDRangeEntry *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ফেরত a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

স্থির পূর্ণসংখ্যা uid_range_entry_compare(ধ্রুবক UIDRangeEntry *a, ধ্রুবক UIDRangeEntry *b) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(a->start, b->start);
        যদি (r != 0)
                ফেরত r;

        ফেরত CMP(a->nr, b->nr);
}

স্থির শূন্য uid_range_coalesce(UIDRange *range) {
        প্রতিজ্ঞা(range);

        যদি (range->n_entries <= 0)
                ফেরত;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        জন্য (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                জন্য (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        যদি (!uid_range_entry_intersect(x, y))
                                বিরতি;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        প্রতিজ্ঞা(x->start <= UINT32_MAX - x->nr);
                        প্রতিজ্ঞা(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        যদি (range->n_entries > j + 1)
                                memmove(y, y + 1, আকার(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        প্রতিজ্ঞা(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        প্রতিজ্ঞা(j > 0);
                        j--;
                }
        }
}

পূর্ণসংখ্যা uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ফাঁকা;
        UIDRange *p;

        প্রতিজ্ঞা(range);

        যদি (nr <= 0)
                ফেরত 0;

        যদি (start > UINT32_MAX - nr) /* overflow check */
                ফেরত -ERANGE;

        যদি (*range)
                p = *range;
        অন্যথা {
                range_new = new0(UIDRange, 1);
                যদি (!range_new)
                        ফেরত -ENOMEM;

                p = range_new;
        }

        যদি (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ফেরত -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        যদি (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_add_str_full(UIDRange **range, ধ্রুবক অক্ষর *s, bool coalesce) {
        uid_t start, end;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(s);

        r = parse_uid_range(s, &start, &end);
        যদি (r < 0)
                ফেরত r;

        ফেরত uid_range_add_internal(range, start, end - start + 1, coalesce);
}

পূর্ণসংখ্যা uid_range_পরবর্তী_lower(ধ্রুবক UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(uid);

        যদি (*uid == 0)
                ফেরত -EBUSY;

        candidate = *uid - 1;

        জন্য (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                যদি (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        ফেরত 1;
                }

                যদি (end < candidate)
                        closest = end;
        }

        যদি (closest == UID_INVALID)
                ফেরত -EBUSY;

        *uid = closest;
        ফেরত 1;
}

bool uid_range_covers(ধ্রুবক UIDRange *range, uid_t start, uid_t nr) {
        যদি (nr == 0) /* empty range? always covered... */
                ফেরত true;

        যদি (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ফেরত false;

        যদি (!range)
                ফেরত false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                যদি (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        ফেরত true;

        ফেরত false;
}

পূর্ণসংখ্যা uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        যদি (r == EOF)
                ফেরত errno_or_else(ENOMSG);
        প্রতিজ্ঞা(r >= 0);
        যদি (r != 3)
                ফেরত -EBADMSG;
        যদি (uid_range <= 0)
                ফেরত -EBADMSG;

        যদি (ret_base)
                *ret_base = uid_base;
        যদি (ret_shift)
                *ret_shift = uid_shift;
        যদি (ret_range)
                *ret_range = uid_range;

        ফেরত 0;
}

unsigned uid_range_আকার(ধ্রুবক UIDRange *range) {
        যদি (!range)
                ফেরত 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ফেরত n;
}

bool uid_range_is_empty(ধ্রুবক UIDRange *range) {

        যদি (!range)
                ফেরত true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                যদি (e->nr > 0)
                        ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা uid_range_load_userns_full(ধ্রুবক অক্ষর *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ফাঁকা;
        _cleanup_fclose_ FILE *f = ফাঁকা;
        পূর্ণসংখ্যা r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        প্রতিজ্ঞা(mode >= 0);
        প্রতিজ্ঞা(mode < _UID_RANGE_USERNS_MODE_MAX);
        প্রতিজ্ঞা(ret);

        যদি (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        যদি (!f) {
                r = -errno;

                যদি (r == -ENOENT && path_startswith(path, "/proc/"))
                        ফেরত proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ফেরত r;
        }

        range = new0(UIDRange, 1);
        যদি (!range)
                ফেরত -ENOMEM;

        জন্য (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                যদি (r == -ENOMSG)
                        বিরতি;
                যদি (r < 0)
                        ফেরত r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_load_userns_by_fd_full(পূর্ণসংখ্যা userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);
        প্রতিজ্ঞা(mode >= 0);
        প্রতিজ্ঞা(mode < _UID_RANGE_USERNS_MODE_MAX);
        প্রতিজ্ঞা(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        যদি (r < 0)
                ফেরত r;
        যদি (r > 0)
                ফেরত uid_range_load_userns_full(/* path= */ ফাঁকা, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        যদি (r < 0)
                ফেরত r;

        ধ্রুবক অক্ষর *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ফেরত uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ধ্রুবক UIDRange *range, uid_t start, uid_t nr) {

        যদি (!range)
                ফেরত false;

        /* Avoid overflow */
        যদি (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        যদি (nr == 0)
                ফেরত false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                যদি (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        ফেরত true;

        ফেরত false;
}

পূর্ণসংখ্যা uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        প্রতিজ্ঞা(range);

        যদি (min > max)
                ফেরত -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                যদি (entry_end <= min || e->start > max)
                        চালু;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                প্রতিজ্ঞা(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_partition(UIDRange *range, uid_t আকার_2) {
        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(আকার_2 > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / আকার_2;

        যদি (n_new_entries == 0) {
                range->n_entries = 0;
                ফেরত 0;
        }

        যদি (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ফেরত -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        জন্য (size_t i = 0; i < range->n_entries; i++)
                যদি (range->entries[i].nr >= আকার_2)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        জন্য (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / আকার_2;

                জন্য (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * আকার_2,
                                .nr = আকার_2,
                        };
        }

        range->n_entries = n_new_entries;

        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_copy(ধ্রুবক UIDRange *range, UIDRange **ret) {
        প্রতিজ্ঞা(ret);

        যদি (!range) {
                *ret = ফাঁকা;
                ফেরত 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        যদি (!copy)
                ফেরত -ENOMEM;

        যদি (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                যদি (!copy->entries)
                        ফেরত -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_remove(UIDRange *range, uid_t start, uid_t আকার_2) {
        প্রতিজ্ঞা(range);

        যদি (আকার_2 == 0)
                ফেরত 0;

        uid_t end = start + আকার_2; /* one past the last UID to remove */

        জন্য (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                যদি (entry_end <= start || e->start >= end)
                        চালু;

                /* Check if this removal splits the entry into two parts */
                যদি (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        যদি (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ফেরত -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * আকার(UIDRangeEntry));
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
                        চালু;
                }

                /* Removal covers the entire entry */
                যদি (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * আকার(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        চালু;
                }

                /* Removal trims the start of the entry */
                যদি (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        চালু;
                }

                /* Removal trims the end of the entry */
                যদি (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        চালু;
                }
        }

        ফেরত 0;
}

পূর্ণসংখ্যা uid_range_translate(ধ্রুবক UIDRange *outside, ধ্রুবক UIDRange *inside, uid_t uid, uid_t *ret) {
        প্রতিজ্ঞা(uid_range_entries(outside) == uid_range_entries(inside));
        প্রতিজ্ঞা(ret);

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

        জন্য (size_t i = 0; i < uid_range_entries(outside); i++)
                প্রতিজ্ঞা(outside->entries[i].nr == inside->entries[i].nr);

        জন্য (size_t i = 0; i < uid_range_entries(outside); i++) {
                ধ্রুবক UIDRangeEntry *e = outside->entries + i;

                যদি (uid < e->start || uid >= e->start + e->nr)
                        চালু;

                *ret = inside->entries[i].start + uid - e->start;
                ফেরত 0;
        }

        ফেরত -ESRCH;
}

পূর্ণসংখ্যা uid_range_translate_userns_fd(পূর্ণসংখ্যা userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(userns_fd >= 0);
        প্রতিজ্ঞা(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ফাঁকা;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        যদি (r < 0)
                ফেরত r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ফাঁকা;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        যদি (r < 0)
                ফেরত r;

        ফেরত uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ধ্রুবক UIDRange *a, ধ্রুবক UIDRange *b) {
        যদি (a == b)
                ফেরত true;

        যদি (!a || !b)
                ফেরত false;

        যদি (a->n_entries != b->n_entries)
                ফেরত false;

        জন্য (size_t i = 0; i < a->n_entries; i++) {
                যদি (a->entries[i].start != b->entries[i].start)
                        ফেরত false;
                যদি (a->entries[i].nr != b->entries[i].nr)
                        ফেরত false;
        }

        ফেরত true;
}

পূর্ণসংখ্যা uid_map_অনুসন্ধান_মূল(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(pid_is_valid(pid));
        প্রতিজ্ঞা(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ধ্রুবক অক্ষর *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        যদি (!f) {
                যদি (errno != ENOENT)
                        ফেরত -errno;

                r = proc_mounted();
                যদি (r < 0)
                        ফেরত -ENOENT; /* original error, if we can't determine /proc/ state */

                ফেরত r ? -ENOPKG : -ENOSYS;
        }

        জন্য (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ ফাঁকা);
                যদি (r < 0)
                        ফেরত r;

                যদি (uid_base == 0) {
                        যদি (ret)
                                *ret = uid_shift;
                        ফেরত 0;
                }
        }
}

uid_t uid_range_base(ধ্রুবক UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        যদি (uid_range_is_empty(range))
                ফেরত UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                যদি (e->nr > 0)
                        ফেরত e->start;

        ফেরত UID_INVALID;
}
