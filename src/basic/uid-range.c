/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <sched.h>
#അന്തര്ഭാവ <string.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "format-util.h"
#അന്തര്ഭാവ "namespace-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "pidref.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "sort-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "uid-range.h"
#അന്തര്ഭാവ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        എങ്കിൽ (!range)
                തിരികെ ശൂന്യ;

        സ്വതന്ത്രം(range->entries);
        തിരികെ mfree(range);
}

സ്ഥിതം bool uid_range_entry_intersect(സ്ഥിരം UIDRangeEntry *a, സ്ഥിരം UIDRangeEntry *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ uid_range_entry_compare(സ്ഥിരം UIDRangeEntry *a, സ്ഥിരം UIDRangeEntry *b) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        r = CMP(a->start, b->start);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        തിരികെ CMP(a->nr, b->nr);
}

സ്ഥിതം ശൂന്യം uid_range_coalesce(UIDRange *range) {
        പ്രതിജ്ഞാ(range);

        എങ്കിൽ (range->n_entries <= 0)
                തിരികെ;

        typesafe_qsort(range->entries, range->n_entries, uid_range_entry_compare);

        വേണ്ടി (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *x = range->entries + i;

                വേണ്ടി (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRangeEntry *y = range->entries + j;
                        uid_t begin, end;

                        എങ്കിൽ (!uid_range_entry_intersect(x, y))
                                നിർത്തുക;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        പ്രതിജ്ഞാ(x->start <= UINT32_MAX - x->nr);
                        പ്രതിജ്ഞാ(y->start <= UINT32_MAX - y->nr);
                        end = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = end - begin;

                        എങ്കിൽ (range->n_entries > j + 1)
                                memmove(y, y + 1, വലുപ്പം(UIDRangeEntry) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        പ്രതിജ്ഞാ(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        പ്രതിജ്ഞാ(j > 0);
                        j--;
                }
        }
}

പൂർണ്ണസംഖ്യ uid_range_add_internal(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ശൂന്യ;
        UIDRange *p;

        പ്രതിജ്ഞാ(range);

        എങ്കിൽ (nr <= 0)
                തിരികെ 0;

        എങ്കിൽ (start > UINT32_MAX - nr) /* overflow check */
                തിരികെ -ERANGE;

        എങ്കിൽ (*range)
                p = *range;
        അല്ലെങ്കിൽ {
                range_new = new0(UIDRange, 1);
                എങ്കിൽ (!range_new)
                        തിരികെ -ENOMEM;

                p = range_new;
        }

        എങ്കിൽ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                തിരികെ -ENOMEM;

        p->entries[p->n_entries++] = (UIDRangeEntry) {
                .start = start,
                .nr = nr,
        };

        എങ്കിൽ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_add_str_full(UIDRange **range, സ്ഥിരം അക്ഷരം *s, bool coalesce) {
        uid_t start, end;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(range);
        പ്രതിജ്ഞാ(s);

        r = parse_uid_range(s, &start, &end);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ uid_range_add_internal(range, start, end - start + 1, coalesce);
}

പൂർണ്ണസംഖ്യ uid_range_next_lower(സ്ഥിരം UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        പ്രതിജ്ഞാ(range);
        പ്രതിജ്ഞാ(uid);

        എങ്കിൽ (*uid == 0)
                തിരികെ -EBUSY;

        candidate = *uid - 1;

        വേണ്ടി (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, end;

                begin = range->entries[i].start;
                end = range->entries[i].start + range->entries[i].nr - 1;

                എങ്കിൽ (candidate >= begin && candidate <= end) {
                        *uid = candidate;
                        തിരികെ 1;
                }

                എങ്കിൽ (end < candidate)
                        closest = end;
        }

        എങ്കിൽ (closest == UID_INVALID)
                തിരികെ -EBUSY;

        *uid = closest;
        തിരികെ 1;
}

bool uid_range_covers(സ്ഥിരം UIDRange *range, uid_t start, uid_t nr) {
        എങ്കിൽ (nr == 0) /* empty range? always covered... */
                തിരികെ true;

        എങ്കിൽ (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                തിരികെ false;

        എങ്കിൽ (!range)
                തിരികെ false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                എങ്കിൽ (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        തിരികെ true;

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ uid_map_read_one(FILE *f, uid_t *ret_base, uid_t *ret_shift, uid_t *ret_range) {
        uid_t uid_base, uid_shift, uid_range;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_base, &uid_shift, &uid_range);
        എങ്കിൽ (r == EOF)
                തിരികെ errno_or_else(ENOMSG);
        പ്രതിജ്ഞാ(r >= 0);
        എങ്കിൽ (r != 3)
                തിരികെ -EBADMSG;
        എങ്കിൽ (uid_range <= 0)
                തിരികെ -EBADMSG;

        എങ്കിൽ (ret_base)
                *ret_base = uid_base;
        എങ്കിൽ (ret_shift)
                *ret_shift = uid_shift;
        എങ്കിൽ (ret_range)
                *ret_range = uid_range;

        തിരികെ 0;
}

unsigned uid_range_size(സ്ഥിരം UIDRange *range) {
        എങ്കിൽ (!range)
                തിരികെ 0;

        unsigned n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        തിരികെ n;
}

bool uid_range_is_empty(സ്ഥിരം UIDRange *range) {

        എങ്കിൽ (!range)
                തിരികെ true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                എങ്കിൽ (e->nr > 0)
                        തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ uid_range_load_userns_full(സ്ഥിരം അക്ഷരം *path, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ശൂന്യ;
        _cleanup_fclose_ FILE *f = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        പ്രതിജ്ഞാ(mode >= 0);
        പ്രതിജ്ഞാ(mode < _UID_RANGE_USERNS_MODE_MAX);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!path)
                path = IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(path, "re");
        എങ്കിൽ (!f) {
                r = -errno;

                എങ്കിൽ (r == -ENOENT && path_startswith(path, "/proc/"))
                        തിരികെ proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                തിരികെ r;
        }

        range = new0(UIDRange, 1);
        എങ്കിൽ (!range)
                തിരികെ -ENOMEM;

        വേണ്ടി (;;) {
                uid_t uid_base, uid_shift, uid_range;

                r = uid_map_read_one(f, &uid_base, &uid_shift, &uid_range);
                എങ്കിൽ (r == -ENOMSG)
                        നിർത്തുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                r = uid_range_add_internal(
                                &range,
                                IN_SET(mode, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_base : uid_shift,
                                uid_range,
                                /* coalesce= */ false);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_load_userns_by_fd_full(പൂർണ്ണസംഖ്യ userns_fd, UIDRangeUsernsMode mode, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_done_sigkill_wait) PidRef pidref = PIDREF_NULL;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);
        പ്രതിജ്ഞാ(mode >= 0);
        പ്രതിജ്ഞാ(mode < _UID_RANGE_USERNS_MODE_MAX);
        പ്രതിജ്ഞാ(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0)
                തിരികെ uid_range_load_userns_full(/* path= */ ശൂന്യ, mode, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(
                        pidref.pid,
                        IN_SET(mode, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        തിരികെ uid_range_load_userns_full(p, mode, coalesce, ret);
}

bool uid_range_overlaps(സ്ഥിരം UIDRange *range, uid_t start, uid_t nr) {

        എങ്കിൽ (!range)
                തിരികെ false;

        /* Avoid overflow */
        എങ്കിൽ (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        എങ്കിൽ (nr == 0)
                തിരികെ false;

        FOREACH_ARRAY(entry, range->entries, range->n_entries)
                എങ്കിൽ (start < entry->start + entry->nr &&
                    start + nr >= entry->start)
                        തിരികെ true;

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        പ്രതിജ്ഞാ(range);

        എങ്കിൽ (min > max)
                തിരികെ -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t entry_end = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                എങ്കിൽ (entry_end <= min || e->start > max)
                        തുടരുക;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_end = entry_end <= max ? entry_end : max + 1;
                പ്രതിജ്ഞാ(new_end > new_start);

                range->entries[t++] = (UIDRangeEntry) {
                        .start = new_start,
                        .nr = new_end - new_start,
                };
        }

        range->n_entries = t;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_partition(UIDRange *range, uid_t size) {
        പ്രതിജ്ഞാ(range);
        പ്രതിജ്ഞാ(size > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / size;

        എങ്കിൽ (n_new_entries == 0) {
                range->n_entries = 0;
                തിരികെ 0;
        }

        എങ്കിൽ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                തിരികെ -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        വേണ്ടി (size_t i = 0; i < range->n_entries; i++)
                എങ്കിൽ (range->entries[i].nr >= size)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        വേണ്ടി (size_t i = n_src; i > 0; i--) {
                UIDRangeEntry *e = range->entries + i - 1;
                unsigned n_parts = e->nr / size;

                വേണ്ടി (unsigned j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRangeEntry) {
                                .start = e->start + (j - 1) * size,
                                .nr = size,
                        };
        }

        range->n_entries = n_new_entries;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_copy(സ്ഥിരം UIDRange *range, UIDRange **ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!range) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        എങ്കിൽ (!copy)
                തിരികെ -ENOMEM;

        എങ്കിൽ (range->n_entries > 0) {
                copy->entries = newdup(UIDRangeEntry, range->entries, range->n_entries);
                എങ്കിൽ (!copy->entries)
                        തിരികെ -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_remove(UIDRange *range, uid_t start, uid_t size) {
        പ്രതിജ്ഞാ(range);

        എങ്കിൽ (size == 0)
                തിരികെ 0;

        uid_t end = start + size; /* one past the last UID to remove */

        വേണ്ടി (size_t i = 0; i < range->n_entries; i++) {
                UIDRangeEntry *e = range->entries + i;
                uid_t entry_end = e->start + e->nr;

                /* No overlap */
                എങ്കിൽ (entry_end <= start || e->start >= end)
                        തുടരുക;

                /* Check if this removal splits the entry into two parts */
                എങ്കിൽ (e->start < start && entry_end > end) {
                        /* Need to split: grow the array first */
                        എങ്കിൽ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                തിരികെ -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        entry_end = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * വലുപ്പം(UIDRangeEntry));
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
                        തുടരുക;
                }

                /* Removal covers the entire entry */
                എങ്കിൽ (start <= e->start && end >= entry_end) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * വലുപ്പം(UIDRangeEntry));
                        range->n_entries--;
                        i--;
                        തുടരുക;
                }

                /* Removal trims the start of the entry */
                എങ്കിൽ (start <= e->start && end > e->start) {
                        e->nr = entry_end - end;
                        e->start = end;
                        തുടരുക;
                }

                /* Removal trims the end of the entry */
                എങ്കിൽ (start < entry_end && end >= entry_end) {
                        e->nr = start - e->start;
                        തുടരുക;
                }
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ uid_range_translate(സ്ഥിരം UIDRange *outside, സ്ഥിരം UIDRange *inside, uid_t uid, uid_t *ret) {
        പ്രതിജ്ഞാ(uid_range_entries(outside) == uid_range_entries(inside));
        പ്രതിജ്ഞാ(ret);

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

        വേണ്ടി (size_t i = 0; i < uid_range_entries(outside); i++)
                പ്രതിജ്ഞാ(outside->entries[i].nr == inside->entries[i].nr);

        വേണ്ടി (size_t i = 0; i < uid_range_entries(outside); i++) {
                സ്ഥിരം UIDRangeEntry *e = outside->entries + i;

                എങ്കിൽ (uid < e->start || uid >= e->start + e->nr)
                        തുടരുക;

                *ret = inside->entries[i].start + uid - e->start;
                തിരികെ 0;
        }

        തിരികെ -ESRCH;
}

പൂർണ്ണസംഖ്യ uid_range_translate_userns_fd(പൂർണ്ണസംഖ്യ userns_fd, UIDRangeUsernsMode mode, uid_t uid, uid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(userns_fd >= 0);
        പ്രതിജ്ഞാ(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ശൂന്യ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &outside_range);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        mode = mode == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ശൂന്യ;
        r = uid_range_load_userns_by_fd_full(userns_fd, mode, /* coalesce= */ false, &inside_range);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(സ്ഥിരം UIDRange *a, സ്ഥിരം UIDRange *b) {
        എങ്കിൽ (a == b)
                തിരികെ true;

        എങ്കിൽ (!a || !b)
                തിരികെ false;

        എങ്കിൽ (a->n_entries != b->n_entries)
                തിരികെ false;

        വേണ്ടി (size_t i = 0; i < a->n_entries; i++) {
                എങ്കിൽ (a->entries[i].start != b->entries[i].start)
                        തിരികെ false;
                എങ്കിൽ (a->entries[i].nr != b->entries[i].nr)
                        തിരികെ false;
        }

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ uid_map_തിരയൽ_വേര്(pid_t pid, UIDRangeUsernsMode mode, uid_t *ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(pid_is_valid(pid));
        പ്രതിജ്ഞാ(IN_SET(mode, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        സ്ഥിരം അക്ഷരം *p = procfs_file_alloca(pid, mode == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        എങ്കിൽ (!f) {
                എങ്കിൽ (errno != ENOENT)
                        തിരികെ -errno;

                r = proc_mounted();
                എങ്കിൽ (r < 0)
                        തിരികെ -ENOENT; /* original error, if we can't determine /proc/ state */

                തിരികെ r ? -ENOPKG : -ENOSYS;
        }

        വേണ്ടി (;;) {
                uid_t uid_base = UID_INVALID, uid_shift = UID_INVALID;

                r = uid_map_read_one(f, &uid_base, &uid_shift, /* ret_range= */ ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (uid_base == 0) {
                        എങ്കിൽ (ret)
                                *ret = uid_shift;
                        തിരികെ 0;
                }
        }
}

uid_t uid_range_base(സ്ഥിരം UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        എങ്കിൽ (uid_range_is_empty(range))
                തിരികെ UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                എങ്കിൽ (e->nr > 0)
                        തിരികെ e->start;

        തിരികെ UID_INVALID;
}
