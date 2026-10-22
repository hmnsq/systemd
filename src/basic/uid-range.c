/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <sched.h>
#ଅନ୍ତର୍ଭାଵ <string.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "format-util.h"
#ଅନ୍ତର୍ଭାଵ "namespace-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "pidref.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"
#ଅନ୍ତର୍ଭାଵ "stat-util.h"
#ଅନ୍ତର୍ଭାଵ "uid-range.h"
#ଅନ୍ତର୍ଭାଵ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        ଯଦି (!range)
                ଫେରନ୍ତୁ ଖାଲି;

        ମୁକ୍ତ(range->entries);
        ଫେରନ୍ତୁ mfree(range);
}

ସ୍ଥିର bool uid_range_entry_intersect(ଅଚଳ UIDRangeEntry *a, ଅଚଳ UIDRangeEntry *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_entry_compare(ଅଚଳ UIDRangeEntry *a, ଅଚଳ UIDRangeEntry *b) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        r = CMP(a->start, b->start);
        ଯଦି (r != 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ CMP(a->nr, b->nr);
}

ସ୍ଥିର ଶୂନ୍ୟ uid_range_coalesce(UIDRange *range) {
        ପ୍ରତିଜ୍ଞା(range);

        ଯଦି (range->n_entries <= 0)
                ଫେରନ୍ତୁ;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        ପାଇଁ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                ପାଇଁ (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        ଯଦି (!uid_range_entry_intersect(x, y))
                                ଭାଙ୍ଗ;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ପ୍ରତିଜ୍ଞା(x->start <= UINT32_MAX - x->nr);
                        ପ୍ରତିଜ୍ଞା(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        ଯଦି (range->n_entries > j + 1)
                                memmove(y, y + 1, ଆକାର(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ପ୍ରତିଜ୍ଞା(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ପ୍ରତିଜ୍ଞା(j > 0);
                        j--;
                }
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ଖାଲି;
        UIDRange *p;

        ପ୍ରତିଜ୍ଞା(range);

        ଯଦି (nr <= 0)
                ଫେରନ୍ତୁ 0;

        ଯଦି (start > UINT32_MAX - nr) /* overflow check */
                ଫେରନ୍ତୁ -ERANGE;

        ଯଦି (*range)
                p = *range;
        ଅନ୍ୟଥା {
                range_new = new0(UIDRange, 1);
                ଯଦି (!range_new)
                        ଫେରନ୍ତୁ -ENOMEM;

                p = range_new;
        }

        ଯଦି (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ଫେରନ୍ତୁ -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        ଯଦି (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_add_str_full(UIDRange **range, ଅଚଳ ଅକ୍ଷର *s, bool coalesce) {
        uid_t start, end;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(range);
        ପ୍ରତିଜ୍ଞା(s);

        r = parse_uid_range(s, &start, &end);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ uid_range_add_internal(range, start, end - start + 1, coalesce);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_next_lower(ଅଚଳ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ପ୍ରତିଜ୍ଞା(range);
        ପ୍ରତିଜ୍ଞା(uid);

        ଯଦି (*uid == 0)
                ଫେରନ୍ତୁ -EBUSY;

        candidate = *uid - 1;

        ପାଇଁ (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                ଯଦି (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        ଫେରନ୍ତୁ 1;
                }

                ଯଦି (end < candidate)
                        closest = end;
        }

        ଯଦି (closest == UID_INVALID)
                ଫେରନ୍ତୁ -EBUSY;

        *uid = closest;
        ଫେରନ୍ତୁ 1;
}

bool uid_range_covers(ଅଚଳ UIDRange *range, uid_t start, uid_t nr) {
        ଯଦି (nr == 0) /* empty range? always covered... */
                ଫେରନ୍ତୁ true;

        ଯଦି (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ଫେରନ୍ତୁ false;

        ଯଦି (!range)
                ଫେରନ୍ତୁ false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ଯଦି (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        ଯଦି (r == EOF)
                ଫେରନ୍ତୁ errno_or_else(ENOMSG);
        ପ୍ରତିଜ୍ଞା(r >= 0);
        ଯଦି (r != 3)
                ଫେରନ୍ତୁ -EBADMSG;
        ଯଦି (uid_range <= 0)
                ଫେରନ୍ତୁ -EBADMSG;

        ଯଦି (ret_base)
                *ret_base = uid_base;
        ଯଦି (ret_shift)
                *ret_shift = uid_shift;
        ଯଦି (ret_range)
                *ret_range = uid_range;

        ଫେରନ୍ତୁ 0;
}

unsigned uid_range_size(ଅଚଳ UIDRange *range) {
        ଯଦି (!range)
                ଫେରନ୍ତୁ 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ଫେରନ୍ତୁ n;
}

bool uid_range_is_empty(ଅଚଳ UIDRange *range) {

        ଯଦି (!range)
                ଫେରନ୍ତୁ true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ଯଦି (e->nr > 0)
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns_full(ଅଚଳ ଅକ୍ଷର *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ଖାଲି;
        _cleanup_fclose_ FILE *f = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ପ୍ରତିଜ୍ଞା(mode >= 0);
        ପ୍ରତିଜ୍ଞା(mode < _UID_RANGE_USERNS_MODE_MAX);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        ଯଦି (!f) {
                r = -errno;

                ଯଦି (r == -ENOENT && path_startswith(path, "/proc/"))
                        ଫେରନ୍ତୁ proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ଫେରନ୍ତୁ r;
        }

        range = new0(UIDRange, 1);
        ଯଦି (!range)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                ଯଦି (r == -ENOMSG)
                        ଭାଙ୍ଗ;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_load_userns_by_fd_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);
        ପ୍ରତିଜ୍ଞା(mode >= 0);
        ପ୍ରତିଜ୍ଞା(mode < _UID_RANGE_USERNS_MODE_MAX);
        ପ୍ରତିଜ୍ଞା(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r > 0)
                ଫେରନ୍ତୁ uid_range_load_userns_full(/* path= */ ଖାଲି, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ଫେରନ୍ତୁ uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ଅଚଳ UIDRange *range, uid_t start, uid_t nr) {

        ଯଦି (!range)
                ଫେରନ୍ତୁ false;

        /* Avoid overflow */
        ଯଦି (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        ଯଦି (nr == 0)
                ଫେରନ୍ତୁ false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                ଯଦି (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ପ୍ରତିଜ୍ଞା(range);

        ଯଦି (min > max)
                ଫେରନ୍ତୁ -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ଯଦି (entry_end <= min || e->start > max)
                        ଜାରି;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                ପ୍ରତିଜ୍ଞା(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_partition(UIDRange *range, uid_t size) {
        ପ୍ରତିଜ୍ଞା(range);
        ପ୍ରତିଜ୍ଞା(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        ଯଦି (n_new_entries == 0) {
                range->n_entries = 0;
                ଫେରନ୍ତୁ 0;
        }

        ଯଦି (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ଫେରନ୍ତୁ -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        ପାଇଁ (size_t i = 0; i < range->n_entries; i++)
                ଯଦି (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        ପାଇଁ (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                ପାଇଁ (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_copy(ଅଚଳ UIDRange *range, UIDRange **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!range) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ଯଦି (!copy)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                ଯଦି (!copy->entries)
                        ଫେରନ୍ତୁ -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        ପ୍ରତିଜ୍ଞା(range);

        ଯଦି (size == 0)
                ଫେରନ୍ତୁ 0;

        uid_t end = start + size; /* one past the last UID to remove */

        ପାଇଁ (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                ଯଦି (entry_end <= start || e->start >= end)
                        ଜାରି;

                /* Check if this removal splits the entry into two parts */
                ଯଦି (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        ଯଦି (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ଫେରନ୍ତୁ -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * ଆକାର(UIDRangeEntry));
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
                        ଜାରି;
                }

                /* Removal covers the entire entry */
                ଯଦି (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * ଆକାର(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        ଜାରି;
                }

                /* Removal trims the start of the entry */
                ଯଦି (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        ଜାରି;
                }

                /* Removal trims the end of the entry */
                ଯଦି (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        ଜାରି;
                }
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_translate(ଅଚଳ UIDRange *outside, ଅଚଳ UIDRange *inside, uid_t uid, uid_t *ret) {
        ପ୍ରତିଜ୍ଞା(uid_range_entries(outside) == uid_range_entries(inside));
        ପ୍ରତିଜ୍ଞା(ret);

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

        ପାଇଁ (size_t i = 0; i < uid_range_entries(outside); i++)
                ପ୍ରତିଜ୍ଞା(outside->entries[i].nr == inside->entries[i].nr);

        ପାଇଁ (size_t i = 0; i < uid_range_entries(outside); i++) {
                ଅଚଳ UIDRangeEntry *e = outside->entries + i;

                ଯଦି (uid < e->start || uid >= e->start + e->nr)
                        ଜାରି;

                *ret = inside->entries[i].start + uid - e->start;
                ଫେରନ୍ତୁ 0;
        }

        ଫେରନ୍ତୁ -ESRCH;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_range_translate_userns_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(userns_fd >= 0);
        ପ୍ରତିଜ୍ଞା(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ଖାଲି;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ଖାଲି;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ଅଚଳ UIDRange *a, ଅଚଳ UIDRange *b) {
        ଯଦି (a == b)
                ଫେରନ୍ତୁ true;

        ଯଦି (!a || !b)
                ଫେରନ୍ତୁ false;

        ଯଦି (a->n_entries != b->n_entries)
                ଫେରନ୍ତୁ false;

        ପାଇଁ (size_t i = 0; i < a->n_entries; i++) {
                ଯଦି (a->entries[i].start != b->entries[i].start)
                        ଫେରନ୍ତୁ false;
                ଯଦି (a->entries[i].nr != b->entries[i].nr)
                        ଫେରନ୍ତୁ false;
        }

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା uid_map_ସନ୍ଧାନ_ମୂଳ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(pid_is_valid(pid));
        ପ୍ରତିଜ୍ଞା(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ଅଚଳ ଅକ୍ଷର *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ଯଦି (!f) {
                ଯଦି (errno != ENOENT)
                        ଫେରନ୍ତୁ -errno;

                r = proc_mounted();
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ -ENOENT; /* original error, if we can't determine /proc/ state */

                ଫେରନ୍ତୁ r ? -ENOPKG : -ENOSYS;
        }

        ପାଇଁ (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ ଖାଲି);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (uid_base == 0) {
                        ଯଦି (ret)
                                *ret = uid_shift;
                        ଫେରନ୍ତୁ 0;
                }
        }
}

uid_t uid_range_base(ଅଚଳ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ଯଦି (uid_range_is_empty(range))
                ଫେରନ୍ତୁ UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ଯଦି (e->nr > 0)
                        ଫେରନ୍ତୁ e->start;

        ଫେରନ୍ତୁ UID_INVALID;
}
