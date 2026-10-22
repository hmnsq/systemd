/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <sched.h>
#समावेश <string.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "format-util.h"
#समावेश "namespace-util.h"
#समावेश "path-util.h"
#समावेश "pidref.h"
#समावेश "process-util.h"
#समावेश "sort-util.h"
#समावेश "stat-util.h"
#समावेश "uid-range.h"
#समावेश "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        अगर (!range)
                वापस शून्य;

        मुक्त(range->entries);
        वापस mfree(range);
}

अटल bool uid_range_entry_intersect(स्थिर UIDRangeEntry *a, स्थिर UIDRangeEntry *b) {
        निश्चित(a);
        निश्चित(b);

        वापस a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

अटल पूर्णांक uid_range_entry_compare(स्थिर UIDRangeEntry *a, स्थिर UIDRangeEntry *b) {
        पूर्णांक r;

        निश्चित(a);
        निश्चित(b);

        r = CMP(a->start, b->start);
        अगर (r != 0)
                वापस r;

        वापस CMP(a->nr, b->nr);
}

अटल व्योम uid_range_coalesce(UIDRange *range) {
        निश्चित(range);

        अगर (range->n_entries <= 0)
                वापस;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        क्रम (माप_प्रकार i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                क्रम (माप_प्रकार j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        अगर (!uid_range_entry_intersect(x, y))
                                अवरोध;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        निश्चित(x->start <= UINT32_MAX - x->nr);
                        निश्चित(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        अगर (range->n_entries > j + 1)
                                स_हटाओ(y, y + 1, माप(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        निश्चित(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        निश्चित(j > 0);
                        j--;
                }
        }
}

पूर्णांक uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = शून्य;
        UIDRange *p;

        निश्चित(range);

        अगर (nr <= 0)
                वापस 0;

        अगर (start > UINT32_MAX - nr) /* overflow check */
                वापस -दुस्फल;

        अगर (*range)
                p = *range;
        अन्यथा {
                range_new = new0(UIDRange, 1);
                अगर (!range_new)
                        वापस -ENOMEM;

                p = range_new;
        }

        अगर (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                वापस -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        अगर (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        वापस 0;
}

पूर्णांक uid_range_add_str_full(UIDRange **range, स्थिर अक्षर *s, bool coalesce) {
        uid_t start, end;
        पूर्णांक r;

        निश्चित(range);
        निश्चित(s);

        r = parse_uid_range(s, &start, &end);
        अगर (r < 0)
                वापस r;

        वापस uid_range_add_internal(range, start, end - start + 1, coalesce);
}

पूर्णांक uid_range_अग्र_lower(स्थिर UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        निश्चित(range);
        निश्चित(uid);

        अगर (*uid == 0)
                वापस -EBUSY;

        candidate = *uid - 1;

        क्रम (माप_प्रकार i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                अगर (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        वापस 1;
                }

                अगर (end < candidate)
                        closest = end;
        }

        अगर (closest == UID_INVALID)
                वापस -EBUSY;

        *uid = closest;
        वापस 1;
}

bool uid_range_covers(स्थिर UIDRange *range, uid_t start, uid_t nr) {
        अगर (nr == 0) /* empty range? always covered... */
                वापस true;

        अगर (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                वापस false;

        अगर (!range)
                वापस false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                अगर (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        वापस true;

        वापस false;
}

पूर्णांक uid_map_read_one(खाता *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        पूर्णांक r;

        निश्चित(f);

        त्रुटि_सं = 0;
        r = ख_पूछो(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        अगर (r == खातापूर्ण)
                वापस errno_or_else(ENOMSG);
        निश्चित(r >= 0);
        अगर (r != 3)
                वापस -EBADMSG;
        अगर (uid_range <= 0)
                वापस -EBADMSG;

        अगर (ret_base)
                *ret_base = uid_base;
        अगर (ret_shift)
                *ret_shift = uid_shift;
        अगर (ret_range)
                *ret_range = uid_range;

        वापस 0;
}

अचिन्हित uid_range_आकार(स्थिर UIDRange *range) {
        अगर (!range)
                वापस 0;

        अचिन्हित n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        वापस n;
}

bool uid_range_is_empty(स्थिर UIDRange *range) {

        अगर (!range)
                वापस true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                अगर (e->nr > 0)
                        वापस false;

        वापस true;
}

पूर्णांक uid_range_load_userns_full(स्थिर अक्षर *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = शून्य;
        _cleanup_fclose_ खाता *f = शून्य;
        पूर्णांक r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        निश्चित(mode >= 0);
        निश्चित(mode < _UID_RANGE_USERNS_MODE_MAX);
        निश्चित(ret);

        अगर (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = ख_खोलो(path, "re");
        अगर (!f) {
                r = -त्रुटि_सं;

                अगर (r == -ENOENT && path_startswith(path, "/proc/"))
                        वापस proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                वापस r;
        }

        range = new0(UIDRange, 1);
        अगर (!range)
                वापस -ENOMEM;

        क्रम (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                अगर (r == -ENOMSG)
                        अवरोध;
                अगर (r < 0)
                        वापस r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                अगर (r < 0)
                        वापस r;
        }

        अगर (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        वापस 0;
}

पूर्णांक uid_range_load_userns_by_fd_full(पूर्णांक userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णांक r;

        निश्चित(userns_fd >= 0);
        निश्चित(mode >= 0);
        निश्चित(mode < _UID_RANGE_USERNS_MODE_MAX);
        निश्चित(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        अगर (r < 0)
                वापस r;
        अगर (r > 0)
                वापस uid_range_load_userns_full(/* path= */ शून्य, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        अगर (r < 0)
                वापस r;

        स्थिर अक्षर *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        वापस uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(स्थिर UIDRange *range, uid_t start, uid_t nr) {

        अगर (!range)
                वापस false;

        /* Avoid overflow */
        अगर (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        अगर (nr == 0)
                वापस false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                अगर (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        वापस true;

        वापस false;
}

पूर्णांक uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        निश्चित(range);

        अगर (min > max)
                वापस -EINVAL;

        माप_प्रकार t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                अगर (entry_end <= min || e->start > max)
                        जारी;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                निश्चित(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        वापस 0;
}

पूर्णांक uid_range_partition(UIDRange *range, uid_t आकार) {
        निश्चित(range);
        निश्चित(आकार > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        माप_प्रकार n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / आकार;

        अगर (n_new_entries == 0) {
                range->n_entries = 0;
                वापस 0;
        }

        अगर (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                वापस -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        माप_प्रकार n_src = 0;
        क्रम (माप_प्रकार i = 0; i < range->n_entries; i++)
                अगर (range->entries[i].nr >= आकार)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        माप_प्रकार t = n_new_entries;
        क्रम (माप_प्रकार i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                अचिन्हित n_parts = e->nr / आकार;

                क्रम (अचिन्हित j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * आकार,
                                .nr = आकार,
                        };
        }

        range->n_entries = n_new_entries;

        वापस 0;
}

पूर्णांक uid_range_copy(स्थिर UIDRange *range, UIDRange **ret) {
        निश्चित(ret);

        अगर (!range) {
                *ret = शून्य;
                वापस 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        अगर (!copy)
                वापस -ENOMEM;

        अगर (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                अगर (!copy->entries)
                        वापस -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        वापस 0;
}

पूर्णांक uid_range_remove(UIDRange *range, uid_t start, uid_t आकार) {
        निश्चित(range);

        अगर (आकार == 0)
                वापस 0;

        uid_t end = start + आकार; /* one past the last UID to remove */

        क्रम (माप_प्रकार i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                अगर (entry_end <= start || e->start >= end)
                        जारी;

                /* Check if this removal splits the entry into two parts */
                अगर (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        अगर (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                वापस -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        स_हटाओ(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * माप(UIDRangeEntry));
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
                        जारी;
                }

                /* Removal covers the entire entry */
                अगर (start <= e->start && end >= entry_end) {
                        स_हटाओ(e, e + 1, (range->n_entries - i - 1) * माप(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        जारी;
                }

                /* Removal trims the start of the entry */
                अगर (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        जारी;
                }

                /* Removal trims the end of the entry */
                अगर (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        जारी;
                }
        }

        वापस 0;
}

पूर्णांक uid_range_translate(स्थिर UIDRange *outside, स्थिर UIDRange *inside, uid_t uid, uid_t *ret) {
        निश्चित(uid_range_entries(outside) == uid_range_entries(inside));
        निश्चित(ret);

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

        क्रम (माप_प्रकार i = 0; i < uid_range_entries(outside); i++)
                निश्चित(outside->entries[i].nr == inside->entries[i].nr);

        क्रम (माप_प्रकार i = 0; i < uid_range_entries(outside); i++) {
                स्थिर UIDRangeEntry *e = outside->entries + i;

                अगर (uid < e->start || uid >= e->start + e->nr)
                        जारी;

                *ret = inside->entries[i].start + uid - e->start;
                वापस 0;
        }

        वापस -ESRCH;
}

पूर्णांक uid_range_translate_userns_fd(पूर्णांक userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        पूर्णांक r;

        निश्चित(userns_fd >= 0);
        निश्चित(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = शून्य;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        अगर (r < 0)
                वापस r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = शून्य;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        अगर (r < 0)
                वापस r;

        वापस uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(स्थिर UIDRange *a, स्थिर UIDRange *b) {
        अगर (a == b)
                वापस true;

        अगर (!a || !b)
                वापस false;

        अगर (a->n_entries != b->n_entries)
                वापस false;

        क्रम (माप_प्रकार i = 0; i < a->n_entries; i++) {
                अगर (a->entries[i].start != b->entries[i].start)
                        वापस false;
                अगर (a->entries[i].nr != b->entries[i].nr)
                        वापस false;
        }

        वापस true;
}

पूर्णांक uid_map_खोज_मूल(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        पूर्णांक r;

        निश्चित(pid_is_valid(pid));
        निश्चित(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        स्थिर अक्षर *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ खाता *f = ख_खोलो(p, "re");
        अगर (!f) {
                अगर (त्रुटि_सं != ENOENT)
                        वापस -त्रुटि_सं;

                r = proc_mounted();
                अगर (r < 0)
                        वापस -ENOENT; /* original error, if we can't determine /proc/ state */

                वापस r ? -ENOPKG : -ENOSYS;
        }

        क्रम (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ शून्य);
                अगर (r < 0)
                        वापस r;

                अगर (uid_base == 0) {
                        अगर (ret)
                                *ret = uid_shift;
                        वापस 0;
                }
        }
}

uid_t uid_range_base(स्थिर UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        अगर (uid_range_is_empty(range))
                वापस UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                अगर (e->nr > 0)
                        वापस e->start;

        वापस UID_INVALID;
}
