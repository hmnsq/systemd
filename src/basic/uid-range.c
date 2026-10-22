/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <sched.h>
#অন্তর্ভাৱ <string.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "format-util.h"
#অন্তর্ভাৱ "namespace-util.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "pidref.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "sort-util.h"
#অন্তর্ভাৱ "stat-util.h"
#অন্তর্ভাৱ "uid-range.h"
#অন্তর্ভাৱ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        যদি (!range)
                ঘূৰাই রিক্ত;

        মুক্ত(range->entries);
        ঘূৰাই mfree(range);
}

স্থির bool uid_range_entry_intersect(ধ্রুৱ UIDRangeEntry *a, ধ্রুৱ UIDRangeEntry *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        ঘূৰাই a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

স্থির পূর্ণাঙ্ক uid_range_entry_compare(ধ্রুৱ UIDRangeEntry *a, ধ্রুৱ UIDRangeEntry *b) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        r = CMP(a->start, b->start);
        যদি (r != 0)
                ঘূৰাই r;

        ঘূৰাই CMP(a->nr, b->nr);
}

স্থির শূন্য uid_range_coalesce(UIDRange *range) {
        প্রতিজ্ঞা(range);

        যদি (range->n_entries <= 0)
                ঘূৰাই;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        বাবে (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                বাবে (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        যদি (!uid_range_entry_intersect(x, y))
                                ভঙ্গ;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        প্রতিজ্ঞা(x->start <= UINT32_MAX - x->nr);
                        প্রতিজ্ঞা(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        যদি (range->n_entries > j + 1)
                                memmove(y, y + 1, পরিমাণ(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        প্রতিজ্ঞা(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        প্রতিজ্ঞা(j > 0);
                        j--;
                }
        }
}

পূর্ণাঙ্ক uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = রিক্ত;
        UIDRange *p;

        প্রতিজ্ঞা(range);

        যদি (nr <= 0)
                ঘূৰাই 0;

        যদি (start > UINT32_MAX - nr) /* overflow check */
                ঘূৰাই -ERANGE;

        যদি (*range)
                p = *range;
        অন্যথা {
                range_new = new0(UIDRange, 1);
                যদি (!range_new)
                        ঘূৰাই -ENOMEM;

                p = range_new;
        }

        যদি (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ঘূৰাই -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        যদি (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_add_str_full(UIDRange **range, ধ্রুৱ ৱর্ণ *s, bool coalesce) {
        uid_t start, end;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(s);

        r = parse_uid_range(s, &start, &end);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই uid_range_add_internal(range, start, end - start + 1, coalesce);
}

পূর্ণাঙ্ক uid_range_next_lower(ধ্রুৱ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(uid);

        যদি (*uid == 0)
                ঘূৰাই -EBUSY;

        candidate = *uid - 1;

        বাবে (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                যদি (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        ঘূৰাই 1;
                }

                যদি (end < candidate)
                        closest = end;
        }

        যদি (closest == UID_INVALID)
                ঘূৰাই -EBUSY;

        *uid = closest;
        ঘূৰাই 1;
}

bool uid_range_covers(ধ্রুৱ UIDRange *range, uid_t start, uid_t nr) {
        যদি (nr == 0) /* empty range? always covered... */
                ঘূৰাই true;

        যদি (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ঘূৰাই false;

        যদি (!range)
                ঘূৰাই false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                যদি (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        ঘূৰাই true;

        ঘূৰাই false;
}

পূর্ণাঙ্ক uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        যদি (r == EOF)
                ঘূৰাই errno_or_else(ENOMSG);
        প্রতিজ্ঞা(r >= 0);
        যদি (r != 3)
                ঘূৰাই -EBADMSG;
        যদি (uid_range <= 0)
                ঘূৰাই -EBADMSG;

        যদি (ret_base)
                *ret_base = uid_base;
        যদি (ret_shift)
                *ret_shift = uid_shift;
        যদি (ret_range)
                *ret_range = uid_range;

        ঘূৰাই 0;
}

unsigned uid_range_size(ধ্রুৱ UIDRange *range) {
        যদি (!range)
                ঘূৰাই 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ঘূৰাই n;
}

bool uid_range_is_empty(ধ্রুৱ UIDRange *range) {

        যদি (!range)
                ঘূৰাই true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                যদি (e->nr > 0)
                        ঘূৰাই false;

        ঘূৰাই true;
}

পূর্ণাঙ্ক uid_range_load_userns_full(ধ্রুৱ ৱর্ণ *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = রিক্ত;
        _cleanup_fclose_ FILE *f = রিক্ত;
        পূর্ণাঙ্ক r;

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
                        ঘূৰাই proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ঘূৰাই r;
        }

        range = new0(UIDRange, 1);
        যদি (!range)
                ঘূৰাই -ENOMEM;

        বাবে (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                যদি (r == -ENOMSG)
                        ভঙ্গ;
                যদি (r < 0)
                        ঘূৰাই r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        যদি (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_load_userns_by_fd_full(পূর্ণাঙ্ক userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(userns_fd >= 0);
        প্রতিজ্ঞা(mode >= 0);
        প্রতিজ্ঞা(mode < _UID_RANGE_USERNS_MODE_MAX);
        প্রতিজ্ঞা(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r > 0)
                ঘূৰাই uid_range_load_userns_full(/* path= */ রিক্ত, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        যদি (r < 0)
                ঘূৰাই r;

        ধ্রুৱ ৱর্ণ *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ঘূৰাই uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ধ্রুৱ UIDRange *range, uid_t start, uid_t nr) {

        যদি (!range)
                ঘূৰাই false;

        /* Avoid overflow */
        যদি (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        যদি (nr == 0)
                ঘূৰাই false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                যদি (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        ঘূৰাই true;

        ঘূৰাই false;
}

পূর্ণাঙ্ক uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        প্রতিজ্ঞা(range);

        যদি (min > max)
                ঘূৰাই -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                যদি (entry_end <= min || e->start > max)
                        অনুৱর্ত;

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

        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_partition(UIDRange *range, uid_t size) {
        প্রতিজ্ঞা(range);
        প্রতিজ্ঞা(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        যদি (n_new_entries == 0) {
                range->n_entries = 0;
                ঘূৰাই 0;
        }

        যদি (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ঘূৰাই -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        বাবে (size_t i = 0; i < range->n_entries; i++)
                যদি (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        বাবে (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                বাবে (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_copy(ধ্রুৱ UIDRange *range, UIDRange **ret) {
        প্রতিজ্ঞা(ret);

        যদি (!range) {
                *ret = রিক্ত;
                ঘূৰাই 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        যদি (!copy)
                ঘূৰাই -ENOMEM;

        যদি (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                যদি (!copy->entries)
                        ঘূৰাই -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        প্রতিজ্ঞা(range);

        যদি (size == 0)
                ঘূৰাই 0;

        uid_t end = start + size; /* one past the last UID to remove */

        বাবে (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                যদি (entry_end <= start || e->start >= end)
                        অনুৱর্ত;

                /* Check if this removal splits the entry into two parts */
                যদি (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        যদি (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ঘূৰাই -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * পরিমাণ(UIDRangeEntry));
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
                        অনুৱর্ত;
                }

                /* Removal covers the entire entry */
                যদি (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * পরিমাণ(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        অনুৱর্ত;
                }

                /* Removal trims the start of the entry */
                যদি (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        অনুৱর্ত;
                }

                /* Removal trims the end of the entry */
                যদি (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        অনুৱর্ত;
                }
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক uid_range_translate(ধ্রুৱ UIDRange *outside, ধ্রুৱ UIDRange *inside, uid_t uid, uid_t *ret) {
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

        বাবে (size_t i = 0; i < uid_range_entries(outside); i++)
                প্রতিজ্ঞা(outside->entries[i].nr == inside->entries[i].nr);

        বাবে (size_t i = 0; i < uid_range_entries(outside); i++) {
                ধ্রুৱ UIDRangeEntry *e = outside->entries + i;

                যদি (uid < e->start || uid >= e->start + e->nr)
                        অনুৱর্ত;

                *ret = inside->entries[i].start + uid - e->start;
                ঘূৰাই 0;
        }

        ঘূৰাই -ESRCH;
}

পূর্ণাঙ্ক uid_range_translate_userns_fd(পূর্ণাঙ্ক userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(userns_fd >= 0);
        প্রতিজ্ঞা(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = রিক্ত;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        যদি (r < 0)
                ঘূৰাই r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = রিক্ত;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ধ্রুৱ UIDRange *a, ধ্রুৱ UIDRange *b) {
        যদি (a == b)
                ঘূৰাই true;

        যদি (!a || !b)
                ঘূৰাই false;

        যদি (a->n_entries != b->n_entries)
                ঘূৰাই false;

        বাবে (size_t i = 0; i < a->n_entries; i++) {
                যদি (a->entries[i].start != b->entries[i].start)
                        ঘূৰাই false;
                যদি (a->entries[i].nr != b->entries[i].nr)
                        ঘূৰাই false;
        }

        ঘূৰাই true;
}

পূর্ণাঙ্ক uid_map_অনুসন্ধান_মূল(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(pid_is_valid(pid));
        প্রতিজ্ঞা(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ধ্রুৱ ৱর্ণ *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        যদি (!f) {
                যদি (errno != ENOENT)
                        ঘূৰাই -errno;

                r = proc_mounted();
                যদি (r < 0)
                        ঘূৰাই -ENOENT; /* original error, if we can't determine /proc/ state */

                ঘূৰাই r ? -ENOPKG : -ENOSYS;
        }

        বাবে (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ রিক্ত);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (uid_base == 0) {
                        যদি (ret)
                                *ret = uid_shift;
                        ঘূৰাই 0;
                }
        }
}

uid_t uid_range_base(ধ্রুৱ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        যদি (uid_range_is_empty(range))
                ঘূৰাই UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                যদি (e->nr > 0)
                        ঘূৰাই e->start;

        ঘূৰাই UID_INVALID;
}
