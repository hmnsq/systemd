/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <sched.h>
#अन्तर्भाव <string.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "format-util.h"
#अन्तर्भाव "namespace-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "pidref.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "sort-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "uid-range.h"
#अन्तर्भाव "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        सचे (!range)
                पटिनिवत्त रिक्त;

        मुक्त(range->entries);
        पटिनिवत्त mfree(range);
}

स्थिर bool uid_range_entry_intersect(ध्रुव UIDRangeEntry *a, ध्रुव UIDRangeEntry *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

स्थिर पूर्णाङ्क uid_range_entry_compare(ध्रुव UIDRangeEntry *a, ध्रुव UIDRangeEntry *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->start, b->start);
        सचे (r != 0)
                पटिनिवत्त r;

        पटिनिवत्त CMP(a->nr, b->nr);
}

स्थिर शून्य uid_range_coalesce(UIDRange *range) {
        प्रतिज्ञा(range);

        सचे (range->n_entries <= 0)
                पटिनिवत्त;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        अत्थाय (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                अत्थाय (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        सचे (!uid_range_entry_intersect(x, y))
                                भङ्ग;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        प्रतिज्ञा(x->start <= UINT32_MAX - x->nr);
                        प्रतिज्ञा(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        सचे (range->n_entries > j + 1)
                                memmove(y, y + 1, परिमाण(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        प्रतिज्ञा(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        प्रतिज्ञा(j > 0);
                        j--;
                }
        }
}

पूर्णाङ्क uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = रिक्त;
        UIDRange *p;

        प्रतिज्ञा(range);

        सचे (nr <= 0)
                पटिनिवत्त 0;

        सचे (start > UINT32_MAX - nr) /* overflow check */
                पटिनिवत्त -ERANGE;

        सचे (*range)
                p = *range;
        अञ्ञथा {
                range_new = new0(UIDRange, 1);
                सचे (!range_new)
                        पटिनिवत्त -ENOMEM;

                p = range_new;
        }

        सचे (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                पटिनिवत्त -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        सचे (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_add_str_full(UIDRange **range, ध्रुव वर्ण *s, bool coalesce) {
        uid_t start, end;
        पूर्णाङ्क r;

        प्रतिज्ञा(range);
        प्रतिज्ञा(s);

        r = parse_uid_range(s, &start, &end);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त uid_range_add_internal(range, start, end - start + 1, coalesce);
}

पूर्णाङ्क uid_range_next_lower(ध्रुव UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        प्रतिज्ञा(range);
        प्रतिज्ञा(uid);

        सचे (*uid == 0)
                पटिनिवत्त -EBUSY;

        candidate = *uid - 1;

        अत्थाय (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                सचे (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        पटिनिवत्त 1;
                }

                सचे (end < candidate)
                        closest = end;
        }

        सचे (closest == UID_INVALID)
                पटिनिवत्त -EBUSY;

        *uid = closest;
        पटिनिवत्त 1;
}

bool uid_range_covers(ध्रुव UIDRange *range, uid_t start, uid_t nr) {
        सचे (nr == 0) /* empty range? always covered... */
                पटिनिवत्त true;

        सचे (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                पटिनिवत्त false;

        सचे (!range)
                पटिनिवत्त false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                सचे (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        पटिनिवत्त true;

        पटिनिवत्त false;
}

पूर्णाङ्क uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        सचे (r == EOF)
                पटिनिवत्त errno_or_else(ENOMSG);
        प्रतिज्ञा(r >= 0);
        सचे (r != 3)
                पटिनिवत्त -EBADMSG;
        सचे (uid_range <= 0)
                पटिनिवत्त -EBADMSG;

        सचे (ret_base)
                *ret_base = uid_base;
        सचे (ret_shift)
                *ret_shift = uid_shift;
        सचे (ret_range)
                *ret_range = uid_range;

        पटिनिवत्त 0;
}

unsigned uid_range_size(ध्रुव UIDRange *range) {
        सचे (!range)
                पटिनिवत्त 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        पटिनिवत्त n;
}

bool uid_range_is_empty(ध्रुव UIDRange *range) {

        सचे (!range)
                पटिनिवत्त true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                सचे (e->nr > 0)
                        पटिनिवत्त false;

        पटिनिवत्त true;
}

पूर्णाङ्क uid_range_load_userns_full(ध्रुव वर्ण *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = रिक्त;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _UID_RANGE_USERNS_MODE_MAX);
        प्रतिज्ञा(ret);

        सचे (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        सचे (!f) {
                r = -errno;

                सचे (r == -ENOENT && path_startswith(path, "/proc/"))
                        पटिनिवत्त proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                पटिनिवत्त r;
        }

        range = new0(UIDRange, 1);
        सचे (!range)
                पटिनिवत्त -ENOMEM;

        अत्थाय (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                सचे (r == -ENOMSG)
                        भङ्ग;
                सचे (r < 0)
                        पटिनिवत्त r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_load_userns_by_fd_full(पूर्णाङ्क userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णाङ्क r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _UID_RANGE_USERNS_MODE_MAX);
        प्रतिज्ञा(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r > 0)
                पटिनिवत्त uid_range_load_userns_full(/* path= */ रिक्त, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        सचे (r < 0)
                पटिनिवत्त r;

        ध्रुव वर्ण *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        पटिनिवत्त uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(ध्रुव UIDRange *range, uid_t start, uid_t nr) {

        सचे (!range)
                पटिनिवत्त false;

        /* Avoid overflow */
        सचे (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        सचे (nr == 0)
                पटिनिवत्त false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                सचे (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        पटिनिवत्त true;

        पटिनिवत्त false;
}

पूर्णाङ्क uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        प्रतिज्ञा(range);

        सचे (min > max)
                पटिनिवत्त -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                सचे (entry_end <= min || e->start > max)
                        अनुवर्त;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                प्रतिज्ञा(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_partition(UIDRange *range, uid_t size) {
        प्रतिज्ञा(range);
        प्रतिज्ञा(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        सचे (n_new_entries == 0) {
                range->n_entries = 0;
                पटिनिवत्त 0;
        }

        सचे (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                पटिनिवत्त -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        अत्थाय (size_t i = 0; i < range->n_entries; i++)
                सचे (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        अत्थाय (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                अत्थाय (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_copy(ध्रुव UIDRange *range, UIDRange **ret) {
        प्रतिज्ञा(ret);

        सचे (!range) {
                *ret = रिक्त;
                पटिनिवत्त 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        सचे (!copy)
                पटिनिवत्त -ENOMEM;

        सचे (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                सचे (!copy->entries)
                        पटिनिवत्त -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        प्रतिज्ञा(range);

        सचे (size == 0)
                पटिनिवत्त 0;

        uid_t end = start + size; /* one past the last UID to remove */

        अत्थाय (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                सचे (entry_end <= start || e->start >= end)
                        अनुवर्त;

                /* Check if this removal splits the entry into two parts */
                सचे (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        सचे (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                पटिनिवत्त -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * परिमाण(UIDRangeEntry));
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
                        अनुवर्त;
                }

                /* Removal covers the entire entry */
                सचे (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * परिमाण(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        अनुवर्त;
                }

                /* Removal trims the start of the entry */
                सचे (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        अनुवर्त;
                }

                /* Removal trims the end of the entry */
                सचे (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        अनुवर्त;
                }
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क uid_range_translate(ध्रुव UIDRange *outside, ध्रुव UIDRange *inside, uid_t uid, uid_t *ret) {
        प्रतिज्ञा(uid_range_entries(outside) == uid_range_entries(inside));
        प्रतिज्ञा(ret);

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

        अत्थाय (size_t i = 0; i < uid_range_entries(outside); i++)
                प्रतिज्ञा(outside->entries[i].nr == inside->entries[i].nr);

        अत्थाय (size_t i = 0; i < uid_range_entries(outside); i++) {
                ध्रुव UIDRangeEntry *e = outside->entries + i;

                सचे (uid < e->start || uid >= e->start + e->nr)
                        अनुवर्त;

                *ret = inside->entries[i].start + uid - e->start;
                पटिनिवत्त 0;
        }

        पटिनिवत्त -ESRCH;
}

पूर्णाङ्क uid_range_translate_userns_fd(पूर्णाङ्क userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = रिक्त;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        सचे (r < 0)
                पटिनिवत्त r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = रिक्त;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ध्रुव UIDRange *a, ध्रुव UIDRange *b) {
        सचे (a == b)
                पटिनिवत्त true;

        सचे (!a || !b)
                पटिनिवत्त false;

        सचे (a->n_entries != b->n_entries)
                पटिनिवत्त false;

        अत्थाय (size_t i = 0; i < a->n_entries; i++) {
                सचे (a->entries[i].start != b->entries[i].start)
                        पटिनिवत्त false;
                सचे (a->entries[i].nr != b->entries[i].nr)
                        पटिनिवत्त false;
        }

        पटिनिवत्त true;
}

पूर्णाङ्क uid_map_अन्वेषणम्_मूलम्(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pid_is_valid(pid));
        प्रतिज्ञा(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ध्रुव वर्ण *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        सचे (!f) {
                सचे (errno != ENOENT)
                        पटिनिवत्त -errno;

                r = proc_mounted();
                सचे (r < 0)
                        पटिनिवत्त -ENOENT; /* original error, if we can't determine /proc/ state */

                पटिनिवत्त r ? -ENOPKG : -ENOSYS;
        }

        अत्थाय (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ रिक्त);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (uid_base == 0) {
                        सचे (ret)
                                *ret = uid_shift;
                        पटिनिवत्त 0;
                }
        }
}

uid_t uid_range_base(ध्रुव UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        सचे (uid_range_is_empty(range))
                पटिनिवत्त UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                सचे (e->nr > 0)
                        पटिनिवत्त e->start;

        पटिनिवत्त UID_INVALID;
}
