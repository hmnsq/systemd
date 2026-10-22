/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <sched.h>
#அந்தர்பாவ <string.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "format-util.h"
#அந்தர்பாவ "namespace-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "pidref.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "sort-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "uid-range.h"
#அந்தர்பாவ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        எனில் (!range)
                திரும்பு வெற்று;

        விடுவி(range->entries);
        திரும்பு mfree(range);
}

நிலையான bool uid_range_entry_intersect(மாறிலி UIDRangeEntry *a, மாறிலி UIDRangeEntry *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

நிலையான முழு uid_range_entry_compare(மாறிலி UIDRangeEntry *a, மாறிலி UIDRangeEntry *b) {
        முழு r;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        r = CMP(a->start, b->start);
        எனில் (r != 0)
                திரும்பு r;

        திரும்பு CMP(a->nr, b->nr);
}

நிலையான வெறுமை uid_range_coalesce(UIDRange *range) {
        ப்ரதிஜ்ஞா(range);

        எனில் (range->n_entries <= 0)
                திரும்பு;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        க்கு (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                க்கு (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        எனில் (!uid_range_entry_intersect(x, y))
                                நிறுத்து;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ப்ரதிஜ்ஞா(x->start <= UINT32_MAX - x->nr);
                        ப்ரதிஜ்ஞா(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        எனில் (range->n_entries > j + 1)
                                memmove(y, y + 1, அளவு(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ப்ரதிஜ்ஞா(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ப்ரதிஜ்ஞா(j > 0);
                        j--;
                }
        }
}

முழு uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = வெற்று;
        UIDRange *p;

        ப்ரதிஜ்ஞா(range);

        எனில் (nr <= 0)
                திரும்பு 0;

        எனில் (start > UINT32_MAX - nr) /* overflow check */
                திரும்பு -ERANGE;

        எனில் (*range)
                p = *range;
        இல்லையெனில் {
                range_new = new0(UIDRange, 1);
                எனில் (!range_new)
                        திரும்பு -ENOMEM;

                p = range_new;
        }

        எனில் (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                திரும்பு -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        எனில் (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        திரும்பு 0;
}

முழு uid_range_add_str_full(UIDRange **range, மாறிலி எழுத்து *s, bool coalesce) {
        uid_t start, end;
        முழு r;

        ப்ரதிஜ்ஞா(range);
        ப்ரதிஜ்ஞா(s);

        r = parse_uid_range(s, &start, &end);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு uid_range_add_internal(range, start, end - start + 1, coalesce);
}

முழு uid_range_next_lower(மாறிலி UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ப்ரதிஜ்ஞா(range);
        ப்ரதிஜ்ஞா(uid);

        எனில் (*uid == 0)
                திரும்பு -EBUSY;

        candidate = *uid - 1;

        க்கு (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                எனில் (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        திரும்பு 1;
                }

                எனில் (end < candidate)
                        closest = end;
        }

        எனில் (closest == UID_INVALID)
                திரும்பு -EBUSY;

        *uid = closest;
        திரும்பு 1;
}

bool uid_range_covers(மாறிலி UIDRange *range, uid_t start, uid_t nr) {
        எனில் (nr == 0) /* empty range? always covered... */
                திரும்பு true;

        எனில் (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                திரும்பு false;

        எனில் (!range)
                திரும்பு false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                எனில் (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        திரும்பு true;

        திரும்பு false;
}

முழு uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        முழு r;

        ப்ரதிஜ்ஞா(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        எனில் (r == EOF)
                திரும்பு errno_or_else(ENOMSG);
        ப்ரதிஜ்ஞா(r >= 0);
        எனில் (r != 3)
                திரும்பு -EBADMSG;
        எனில் (uid_range <= 0)
                திரும்பு -EBADMSG;

        எனில் (ret_base)
                *ret_base = uid_base;
        எனில் (ret_shift)
                *ret_shift = uid_shift;
        எனில் (ret_range)
                *ret_range = uid_range;

        திரும்பு 0;
}

unsigned uid_range_size(மாறிலி UIDRange *range) {
        எனில் (!range)
                திரும்பு 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        திரும்பு n;
}

bool uid_range_is_empty(மாறிலி UIDRange *range) {

        எனில் (!range)
                திரும்பு true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                எனில் (e->nr > 0)
                        திரும்பு false;

        திரும்பு true;
}

முழு uid_range_load_userns_full(மாறிலி எழுத்து *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = வெற்று;
        _cleanup_fclose_ FILE *f = வெற்று;
        முழு r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ப்ரதிஜ்ஞா(mode >= 0);
        ப்ரதிஜ்ஞா(mode < _UID_RANGE_USERNS_MODE_MAX);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        எனில் (!f) {
                r = -errno;

                எனில் (r == -ENOENT && path_startswith(path, "/proc/"))
                        திரும்பு proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                திரும்பு r;
        }

        range = new0(UIDRange, 1);
        எனில் (!range)
                திரும்பு -ENOMEM;

        க்கு (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                எனில் (r == -ENOMSG)
                        நிறுத்து;
                எனில் (r < 0)
                        திரும்பு r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                எனில் (r < 0)
                        திரும்பு r;
        }

        எனில் (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        திரும்பு 0;
}

முழு uid_range_load_userns_by_fd_full(முழு userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);
        ப்ரதிஜ்ஞா(mode >= 0);
        ப்ரதிஜ்ஞா(mode < _UID_RANGE_USERNS_MODE_MAX);
        ப்ரதிஜ்ஞா(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0)
                திரும்பு uid_range_load_userns_full(/* path= */ வெற்று, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        எனில் (r < 0)
                திரும்பு r;

        மாறிலி எழுத்து *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        திரும்பு uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(மாறிலி UIDRange *range, uid_t start, uid_t nr) {

        எனில் (!range)
                திரும்பு false;

        /* Avoid overflow */
        எனில் (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        எனில் (nr == 0)
                திரும்பு false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                எனில் (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        திரும்பு true;

        திரும்பு false;
}

முழு uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ப்ரதிஜ்ஞா(range);

        எனில் (min > max)
                திரும்பு -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                எனில் (entry_end <= min || e->start > max)
                        தொடர்;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                ப்ரதிஜ்ஞா(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        திரும்பு 0;
}

முழு uid_range_partition(UIDRange *range, uid_t size) {
        ப்ரதிஜ்ஞா(range);
        ப்ரதிஜ்ஞா(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        எனில் (n_new_entries == 0) {
                range->n_entries = 0;
                திரும்பு 0;
        }

        எனில் (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                திரும்பு -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        க்கு (size_t i = 0; i < range->n_entries; i++)
                எனில் (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        க்கு (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                க்கு (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        திரும்பு 0;
}

முழு uid_range_copy(மாறிலி UIDRange *range, UIDRange **ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (!range) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        எனில் (!copy)
                திரும்பு -ENOMEM;

        எனில் (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                எனில் (!copy->entries)
                        திரும்பு -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        திரும்பு 0;
}

முழு uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        ப்ரதிஜ்ஞா(range);

        எனில் (size == 0)
                திரும்பு 0;

        uid_t end = start + size; /* one past the last UID to remove */

        க்கு (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                எனில் (entry_end <= start || e->start >= end)
                        தொடர்;

                /* Check if this removal splits the entry into two parts */
                எனில் (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        எனில் (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                திரும்பு -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * அளவு(UIDRangeEntry));
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
                        தொடர்;
                }

                /* Removal covers the entire entry */
                எனில் (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * அளவு(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        தொடர்;
                }

                /* Removal trims the start of the entry */
                எனில் (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        தொடர்;
                }

                /* Removal trims the end of the entry */
                எனில் (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        தொடர்;
                }
        }

        திரும்பு 0;
}

முழு uid_range_translate(மாறிலி UIDRange *outside, மாறிலி UIDRange *inside, uid_t uid, uid_t *ret) {
        ப்ரதிஜ்ஞா(uid_range_entries(outside) == uid_range_entries(inside));
        ப்ரதிஜ்ஞா(ret);

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

        க்கு (size_t i = 0; i < uid_range_entries(outside); i++)
                ப்ரதிஜ்ஞா(outside->entries[i].nr == inside->entries[i].nr);

        க்கு (size_t i = 0; i < uid_range_entries(outside); i++) {
                மாறிலி UIDRangeEntry *e = outside->entries + i;

                எனில் (uid < e->start || uid >= e->start + e->nr)
                        தொடர்;

                *ret = inside->entries[i].start + uid - e->start;
                திரும்பு 0;
        }

        திரும்பு -ESRCH;
}

முழு uid_range_translate_userns_fd(முழு userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(userns_fd >= 0);
        ப்ரதிஜ்ஞா(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = வெற்று;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        எனில் (r < 0)
                திரும்பு r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = வெற்று;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(மாறிலி UIDRange *a, மாறிலி UIDRange *b) {
        எனில் (a == b)
                திரும்பு true;

        எனில் (!a || !b)
                திரும்பு false;

        எனில் (a->n_entries != b->n_entries)
                திரும்பு false;

        க்கு (size_t i = 0; i < a->n_entries; i++) {
                எனில் (a->entries[i].start != b->entries[i].start)
                        திரும்பு false;
                எனில் (a->entries[i].nr != b->entries[i].nr)
                        திரும்பு false;
        }

        திரும்பு true;
}

முழு uid_map_தேடல்_வேர்(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        முழு r;

        ப்ரதிஜ்ஞா(pid_is_valid(pid));
        ப்ரதிஜ்ஞா(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        மாறிலி எழுத்து *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        எனில் (!f) {
                எனில் (errno != ENOENT)
                        திரும்பு -errno;

                r = proc_mounted();
                எனில் (r < 0)
                        திரும்பு -ENOENT; /* original error, if we can't determine /proc/ state */

                திரும்பு r ? -ENOPKG : -ENOSYS;
        }

        க்கு (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ வெற்று);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (uid_base == 0) {
                        எனில் (ret)
                                *ret = uid_shift;
                        திரும்பு 0;
                }
        }
}

uid_t uid_range_base(மாறிலி UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        எனில் (uid_range_is_empty(range))
                திரும்பு UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                எனில் (e->nr > 0)
                        திரும்பு e->start;

        திரும்பு UID_INVALID;
}
