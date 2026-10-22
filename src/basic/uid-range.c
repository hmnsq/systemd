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
        जर (!range)
                परत रिक्त;

        मुक्त(range->entries);
        परत mfree(range);
}

स्थिर bool uid_range_entry_intersect(स्थिरांक UIDRangeEntry *a, स्थिरांक UIDRangeEntry *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        परत a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

स्थिर पूर्णांक uid_range_entry_compare(स्थिरांक UIDRangeEntry *a, स्थिरांक UIDRangeEntry *b) {
        पूर्णांक r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->start, b->start);
        जर (r != 0)
                परत r;

        परत CMP(a->nr, b->nr);
}

स्थिर शून्य uid_range_coalesce(UIDRange *range) {
        प्रतिज्ञा(range);

        जर (range->n_entries <= 0)
                परत;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        साठी (माप_प्रकार i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                साठी (माप_प्रकार j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        जर (!uid_range_entry_intersect(x, y))
                                थांबा;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        प्रतिज्ञा(x->start <= UINT32_MAX - x->nr);
                        प्रतिज्ञा(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        जर (range->n_entries > j + 1)
                                स_हटाओ(y, y + 1, आकार(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        प्रतिज्ञा(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        प्रतिज्ञा(j > 0);
                        j--;
                }
        }
}

पूर्णांक uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = रिक्त;
        UIDRange *p;

        प्रतिज्ञा(range);

        जर (nr <= 0)
                परत 0;

        जर (start > UINT32_MAX - nr) /* overflow check */
                परत -दुस्फल;

        जर (*range)
                p = *range;
        अन्यथा {
                range_new = new0(UIDRange, 1);
                जर (!range_new)
                        परत -ENOMEM;

                p = range_new;
        }

        जर (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                परत -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        जर (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        परत 0;
}

पूर्णांक uid_range_add_str_full(UIDRange **range, स्थिरांक अक्षर *s, bool coalesce) {
        uid_t start, end;
        पूर्णांक r;

        प्रतिज्ञा(range);
        प्रतिज्ञा(s);

        r = parse_uid_range(s, &start, &end);
        जर (r < 0)
                परत r;

        परत uid_range_add_internal(range, start, end - start + 1, coalesce);
}

पूर्णांक uid_range_next_lower(स्थिरांक UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        प्रतिज्ञा(range);
        प्रतिज्ञा(uid);

        जर (*uid == 0)
                परत -EBUSY;

        candidate = *uid - 1;

        साठी (माप_प्रकार i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                जर (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        परत 1;
                }

                जर (end < candidate)
                        closest = end;
        }

        जर (closest == UID_INVALID)
                परत -EBUSY;

        *uid = closest;
        परत 1;
}

bool uid_range_covers(स्थिरांक UIDRange *range, uid_t start, uid_t nr) {
        जर (nr == 0) /* empty range? always covered... */
                परत true;

        जर (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                परत false;

        जर (!range)
                परत false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                जर (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        परत true;

        परत false;
}

पूर्णांक uid_map_read_one(खाता *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        पूर्णांक r;

        प्रतिज्ञा(f);

        त्रुटि_सं = 0;
        r = ख_पूछो(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        जर (r == खातापूर्ण)
                परत errno_or_else(ENOMSG);
        प्रतिज्ञा(r >= 0);
        जर (r != 3)
                परत -EBADMSG;
        जर (uid_range <= 0)
                परत -EBADMSG;

        जर (ret_base)
                *ret_base = uid_base;
        जर (ret_shift)
                *ret_shift = uid_shift;
        जर (ret_range)
                *ret_range = uid_range;

        परत 0;
}

अचिन्हित uid_range_size(स्थिरांक UIDRange *range) {
        जर (!range)
                परत 0;

        अचिन्हित n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        परत n;
}

bool uid_range_is_empty(स्थिरांक UIDRange *range) {

        जर (!range)
                परत true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                जर (e->nr > 0)
                        परत false;

        परत true;
}

पूर्णांक uid_range_load_userns_full(स्थिरांक अक्षर *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = रिक्त;
        _cleanup_fclose_ खाता *f = रिक्त;
        पूर्णांक r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _UID_RANGE_USERNS_MODE_MAX);
        प्रतिज्ञा(ret);

        जर (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = ख_खोलो(path, "re");
        जर (!f) {
                r = -त्रुटि_सं;

                जर (r == -ENOENT && path_startswith(path, "/proc/"))
                        परत proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                परत r;
        }

        range = new0(UIDRange, 1);
        जर (!range)
                परत -ENOMEM;

        साठी (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                जर (r == -ENOMSG)
                        थांबा;
                जर (r < 0)
                        परत r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                जर (r < 0)
                        परत r;
        }

        जर (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        परत 0;
}

पूर्णांक uid_range_load_userns_by_fd_full(पूर्णांक userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(mode >= 0);
        प्रतिज्ञा(mode < _UID_RANGE_USERNS_MODE_MAX);
        प्रतिज्ञा(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        जर (r < 0)
                परत r;
        जर (r > 0)
                परत uid_range_load_userns_full(/* path= */ रिक्त, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        जर (r < 0)
                परत r;

        स्थिरांक अक्षर *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        परत uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(स्थिरांक UIDRange *range, uid_t start, uid_t nr) {

        जर (!range)
                परत false;

        /* Avoid overflow */
        जर (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        जर (nr == 0)
                परत false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                जर (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        परत true;

        परत false;
}

पूर्णांक uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        प्रतिज्ञा(range);

        जर (min > max)
                परत -EINVAL;

        माप_प्रकार t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                जर (entry_end <= min || e->start > max)
                        सुरूठेवा;

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

        परत 0;
}

पूर्णांक uid_range_partition(UIDRange *range, uid_t size) {
        प्रतिज्ञा(range);
        प्रतिज्ञा(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        माप_प्रकार n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        जर (n_new_entries == 0) {
                range->n_entries = 0;
                परत 0;
        }

        जर (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                परत -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        माप_प्रकार n_src = 0;
        साठी (माप_प्रकार i = 0; i < range->n_entries; i++)
                जर (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        माप_प्रकार t = n_new_entries;
        साठी (माप_प्रकार i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                अचिन्हित n_parts = e->nr / size;

                साठी (अचिन्हित j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        परत 0;
}

पूर्णांक uid_range_copy(स्थिरांक UIDRange *range, UIDRange **ret) {
        प्रतिज्ञा(ret);

        जर (!range) {
                *ret = रिक्त;
                परत 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        जर (!copy)
                परत -ENOMEM;

        जर (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                जर (!copy->entries)
                        परत -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        परत 0;
}

पूर्णांक uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        प्रतिज्ञा(range);

        जर (size == 0)
                परत 0;

        uid_t end = start + size; /* one past the last UID to remove */

        साठी (माप_प्रकार i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                जर (entry_end <= start || e->start >= end)
                        सुरूठेवा;

                /* Check if this removal splits the entry into two parts */
                जर (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        जर (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                परत -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        स_हटाओ(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * आकार(UIDRangeEntry));
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
                        सुरूठेवा;
                }

                /* Removal covers the entire entry */
                जर (start <= e->start && end >= entry_end) {
                        स_हटाओ(e, e + 1, (range->n_entries - i - 1) * आकार(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        सुरूठेवा;
                }

                /* Removal trims the start of the entry */
                जर (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        सुरूठेवा;
                }

                /* Removal trims the end of the entry */
                जर (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        सुरूठेवा;
                }
        }

        परत 0;
}

पूर्णांक uid_range_translate(स्थिरांक UIDRange *outside, स्थिरांक UIDRange *inside, uid_t uid, uid_t *ret) {
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

        साठी (माप_प्रकार i = 0; i < uid_range_entries(outside); i++)
                प्रतिज्ञा(outside->entries[i].nr == inside->entries[i].nr);

        साठी (माप_प्रकार i = 0; i < uid_range_entries(outside); i++) {
                स्थिरांक UIDRangeEntry *e = outside->entries + i;

                जर (uid < e->start || uid >= e->start + e->nr)
                        सुरूठेवा;

                *ret = inside->entries[i].start + uid - e->start;
                परत 0;
        }

        परत -ESRCH;
}

पूर्णांक uid_range_translate_userns_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        पूर्णांक r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = रिक्त;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        जर (r < 0)
                परत r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = रिक्त;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        जर (r < 0)
                परत r;

        परत uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(स्थिरांक UIDRange *a, स्थिरांक UIDRange *b) {
        जर (a == b)
                परत true;

        जर (!a || !b)
                परत false;

        जर (a->n_entries != b->n_entries)
                परत false;

        साठी (माप_प्रकार i = 0; i < a->n_entries; i++) {
                जर (a->entries[i].start != b->entries[i].start)
                        परत false;
                जर (a->entries[i].nr != b->entries[i].nr)
                        परत false;
        }

        परत true;
}

पूर्णांक uid_map_शोध_मूळ(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        पूर्णांक r;

        प्रतिज्ञा(pid_is_valid(pid));
        प्रतिज्ञा(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        स्थिरांक अक्षर *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ खाता *f = ख_खोलो(p, "re");
        जर (!f) {
                जर (त्रुटि_सं != ENOENT)
                        परत -त्रुटि_सं;

                r = proc_mounted();
                जर (r < 0)
                        परत -ENOENT; /* original error, if we can't determine /proc/ state */

                परत r ? -ENOPKG : -ENOSYS;
        }

        साठी (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ रिक्त);
                जर (r < 0)
                        परत r;

                जर (uid_base == 0) {
                        जर (ret)
                                *ret = uid_shift;
                        परत 0;
                }
        }
}

uid_t uid_range_base(स्थिरांक UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        जर (uid_range_is_empty(range))
                परत UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                जर (e->nr > 0)
                        परत e->start;

        परत UID_INVALID;
}
