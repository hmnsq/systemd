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
        यदि (!range)
                प्रत्यागम रिक्त;

        मुक्त(range->entries);
        प्रत्यागम mfree(range);
}

स्थिर bool uid_range_प्रविष्टिः_intersect(ध्रुव UIDRange_प्रविष्टिः *a, ध्रुव UIDRange_प्रविष्टिः *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        प्रत्यागम a->start <= b->start + b->nr && a->start + a->nr >= b->start;
}

स्थिर पूर्णाङ्क uid_range_प्रविष्टिः_compare(ध्रुव UIDRange_प्रविष्टिः *a, ध्रुव UIDRange_प्रविष्टिः *b) {
        पूर्णाङ्क r;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        r = CMP(a->start, b->start);
        यदि (r != 0)
                प्रत्यागम r;

        प्रत्यागम CMP(a->nr, b->nr);
}

स्थिर शून्य uid_range_coalesce(UIDRange *range) {
        प्रतिज्ञा(range);

        यदि (range->n_entries <= 0)
                प्रत्यागम;

        typesafe_qsort(range->entries, range->n_entries, uid_range_प्रविष्टिः_compare);

        प्रति (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_प्रविष्टिः *x = range->entries + i;

                प्रति (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_प्रविष्टिः *y = range->entries + j;
                        uid_t begin, अन्तः;

                        यदि (!uid_range_प्रविष्टिः_intersect(x, y))
                                भङ्ग;

                        begin = MIN(x->start, y->start);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        प्रतिज्ञा(x->start <= UINT32_MAX - x->nr);
                        प्रतिज्ञा(y->start <= UINT32_MAX - y->nr);
                        अन्तः = MAX(x->start + x->nr, y->start + y->nr);

                        x->start = begin;
                        x->nr = अन्तः - begin;

                        यदि (range->n_entries > j + 1)
                                memmove(y, y + 1, परिमाण(UIDRange_प्रविष्टिः) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        प्रतिज्ञा(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        प्रतिज्ञा(j > 0);
                        j--;
                }
        }
}

पूर्णाङ्क uid_range_सङ्कलय_आन्तरिकम्(UIDRange **range, uid_t start, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = रिक्त;
        UIDRange *p;

        प्रतिज्ञा(range);

        यदि (nr <= 0)
                प्रत्यागम 0;

        यदि (start > UINT32_MAX - nr) /* overflow check */
                प्रत्यागम -ERANGE;

        यदि (*range)
                p = *range;
        अन्यथा {
                range_new = new0(UIDRange, 1);
                यदि (!range_new)
                        प्रत्यागम -ENOMEM;

                p = range_new;
        }

        यदि (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                प्रत्यागम -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_प्रविष्टिः) {
                .start = start,
                .nr = nr,
        };

        यदि (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        प्रत्यागम 0;
}

पूर्णाङ्क uid_range_सङ्कलय_str_पूर्णम्(UIDRange **range, ध्रुव वर्ण *s, bool coalesce) {
        uid_t start, अन्तः;
        पूर्णाङ्क r;

        प्रतिज्ञा(range);
        प्रतिज्ञा(s);

        r = parse_uid_range(s, &start, &अन्तः);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम uid_range_सङ्कलय_आन्तरिकम्(range, start, अन्तः - start + 1, coalesce);
}

पूर्णाङ्क uid_range_अग्र_अधस्तनम्(ध्रुव UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        प्रतिज्ञा(range);
        प्रतिज्ञा(uid);

        यदि (*uid == 0)
                प्रत्यागम -EBUSY;

        candidate = *uid - 1;

        प्रति (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, अन्तः;

                begin = range->entries[i].start;
                अन्तः = range->entries[i].start + range->entries[i].nr - 1;

                यदि (candidate >= begin && candidate <= अन्तः) {
                        *uid = candidate;
                        प्रत्यागम 1;
                }

                यदि (अन्तः < candidate)
                        closest = अन्तः;
        }

        यदि (closest == UID_INVALID)
                प्रत्यागम -EBUSY;

        *uid = closest;
        प्रत्यागम 1;
}

bool uid_range_covers(ध्रुव UIDRange *range, uid_t start, uid_t nr) {
        यदि (nr == 0) /* empty range? always covered... */
                प्रत्यागम true;

        यदि (start > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                प्रत्यागम false;

        यदि (!range)
                प्रत्यागम false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                यदि (start >= i->start &&
                    start + nr <= i->start + i->nr)
                        प्रत्यागम true;

        प्रत्यागम false;
}

पूर्णाङ्क uid_map_read_one(FILE *f, uid_t *ret_आधारः, uid_t *ret_सरणम्, uid_t *ret_range) {
        uid_t uid_आधारः, uid_सरणम्, uid_range;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_आधारः, &uid_सरणम्, &uid_range);
        यदि (r == EOF)
                प्रत्यागम errno_or_else(ENOMSG);
        प्रतिज्ञा(r >= 0);
        यदि (r != 3)
                प्रत्यागम -EBADMSG;
        यदि (uid_range <= 0)
                प्रत्यागम -EBADMSG;

        यदि (ret_आधारः)
                *ret_आधारः = uid_आधारः;
        यदि (ret_सरणम्)
                *ret_सरणम् = uid_सरणम्;
        यदि (ret_range)
                *ret_range = uid_range;

        प्रत्यागम 0;
}

अचिह्नित uid_range_आकार(ध्रुव UIDRange *range) {
        यदि (!range)
                प्रत्यागम 0;

        अचिह्नित n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        प्रत्यागम n;
}

bool uid_range_is_रिक्तानि(ध्रुव UIDRange *range) {

        यदि (!range)
                प्रत्यागम true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                यदि (e->nr > 0)
                        प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क uid_range_आरोपय_userns_पूर्णम्(ध्रुव वर्ण *मार्गः, UIDRange_Userns_बहुलकम् बहुलकम्, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = रिक्त;
        _cleanup_fclose_ FILE *f = रिक्त;
        पूर्णाङ्क r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        प्रतिज्ञा(बहुलकम् >= 0);
        प्रतिज्ञा(बहुलकम् < UID_RANGE_USERNS_बहुलकम्_MAX);
        प्रतिज्ञा(ret);

        यदि (!मार्गः)
                मार्गः = अन्तर्_समुच्चयः(बहुलकम्, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(मार्गः, "re");
        यदि (!f) {
                r = -errno;

                यदि (r == -ENOENT && मार्गः_startswith(मार्गः, "/proc/"))
                        प्रत्यागम proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                प्रत्यागम r;
        }

        range = new0(UIDRange, 1);
        यदि (!range)
                प्रत्यागम -ENOMEM;

        प्रति (;;) {
                uid_t uid_आधारः, uid_सरणम्, uid_range;

                r = uid_map_read_one(f, &uid_आधारः, &uid_सरणम्, &uid_range);
                यदि (r == -ENOMSG)
                        भङ्ग;
                यदि (r < 0)
                        प्रत्यागम r;

                r = uid_range_सङ्कलय_आन्तरिकम्(
                                &range,
                                अन्तर्_समुच्चयः(बहुलकम्, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_आधारः : uid_सरणम्,
                                uid_range,
                                /* coalesce= */ false);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        प्रत्यागम 0;
}

पूर्णाङ्क uid_range_आरोपय_userns_by_fd_पूर्णम्(पूर्णाङ्क userns_fd, UIDRange_Userns_बहुलकम् बहुलकम्, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_समाप्तम्_sigkill_wait) PidRef pidref = PIDREF_NULL;
        पूर्णाङ्क r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(बहुलकम् >= 0);
        प्रतिज्ञा(बहुलकम् < UID_RANGE_USERNS_बहुलकम्_MAX);
        प्रतिज्ञा(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0)
                प्रत्यागम uid_range_आरोपय_userns_पूर्णम्(/* path= */ रिक्त, बहुलकम्, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        यदि (r < 0)
                प्रत्यागम r;

        ध्रुव वर्ण *p = procfs_file_alloca(
                        pidref.pid,
                        अन्तर्_समुच्चयः(बहुलकम्, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        प्रत्यागम uid_range_आरोपय_userns_पूर्णम्(p, बहुलकम्, coalesce, ret);
}

bool uid_range_overlaps(ध्रुव UIDRange *range, uid_t start, uid_t nr) {

        यदि (!range)
                प्रत्यागम false;

        /* Avoid overflow */
        यदि (start > UINT32_MAX - nr)
                nr = UINT32_MAX - start;

        यदि (nr == 0)
                प्रत्यागम false;

        FOREACH_ARRAY(प्रविष्टिः, range->entries, range->n_entries)
                यदि (start < प्रविष्टिः->start + प्रविष्टिः->nr &&
                    start + nr >= प्रविष्टिः->start)
                        प्रत्यागम true;

        प्रत्यागम false;
}

पूर्णाङ्क uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        प्रतिज्ञा(range);

        यदि (min > max)
                प्रत्यागम -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t प्रविष्टिः_अन्तः = e->start + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                यदि (प्रविष्टिः_अन्तः <= min || e->start > max)
                        अनुवर्त;

                /* Trim the entry to fit within [min, max] */
                uid_t new_start = MAX(e->start, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_अन्तः = प्रविष्टिः_अन्तः <= max ? प्रविष्टिः_अन्तः : max + 1;
                प्रतिज्ञा(new_अन्तः > new_start);

                range->entries[t++] = (UIDRange_प्रविष्टिः) {
                        .start = new_start,
                        .nr = new_अन्तः - new_start,
                };
        }

        range->n_entries = t;

        प्रत्यागम 0;
}

पूर्णाङ्क uid_range_partition(UIDRange *range, uid_t आकार) {
        प्रतिज्ञा(range);
        प्रतिज्ञा(आकार > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / आकार;

        यदि (n_new_entries == 0) {
                range->n_entries = 0;
                प्रत्यागम 0;
        }

        यदि (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                प्रत्यागम -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        प्रति (size_t i = 0; i < range->n_entries; i++)
                यदि (range->entries[i].nr >= आकार)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        प्रति (size_t i = n_src; i > 0; i--) {
                UIDRange_प्रविष्टिः *e = range->entries + i - 1;
                अचिह्नित n_parts = e->nr / आकार;

                प्रति (अचिह्नित j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_प्रविष्टिः) {
                                .start = e->start + (j - 1) * आकार,
                                .nr = आकार,
                        };
        }

        range->n_entries = n_new_entries;

        प्रत्यागम 0;
}

पूर्णाङ्क uid_range_copy(ध्रुव UIDRange *range, UIDRange **ret) {
        प्रतिज्ञा(ret);

        यदि (!range) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        यदि (!copy)
                प्रत्यागम -ENOMEM;

        यदि (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_प्रविष्टिः, range->entries, range->n_entries);
                यदि (!copy->entries)
                        प्रत्यागम -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        प्रत्यागम 0;
}

पूर्णाङ्क uid_range_remove(UIDRange *range, uid_t start, uid_t आकार) {
        प्रतिज्ञा(range);

        यदि (आकार == 0)
                प्रत्यागम 0;

        uid_t अन्तः = start + आकार; /* one past the last UID to remove */

        प्रति (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_प्रविष्टिः *e = range->entries + i;
                uid_t प्रविष्टिः_अन्तः = e->start + e->nr;

                /* No overlap */
                यदि (प्रविष्टिः_अन्तः <= start || e->start >= अन्तः)
                        अनुवर्त;

                /* Check if this removal splits the entry into two parts */
                यदि (e->start < start && प्रविष्टिः_अन्तः > अन्तः) {
                        /* Need to split: grow the array first */
                        यदि (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                प्रत्यागम -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        प्रविष्टिः_अन्तः = e->start + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * परिमाण(UIDRange_प्रविष्टिः));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_प्रविष्टिः) {
                                .start = e->start,
                                .nr = start - e->start,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_प्रविष्टिः) {
                                .start = अन्तः,
                                .nr = प्रविष्टिः_अन्तः - अन्तः,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        अनुवर्त;
                }

                /* Removal covers the entire entry */
                यदि (start <= e->start && अन्तः >= प्रविष्टिः_अन्तः) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * परिमाण(UIDRange_प्रविष्टिः));
                        range->n_entries--;
                        i--;
                        अनुवर्त;
                }

                /* Removal trims the start of the entry */
                यदि (start <= e->start && अन्तः > e->start) {
                        e->nr = प्रविष्टिः_अन्तः - अन्तः;
                        e->start = अन्तः;
                        अनुवर्त;
                }

                /* Removal trims the end of the entry */
                यदि (start < प्रविष्टिः_अन्तः && अन्तः >= प्रविष्टिः_अन्तः) {
                        e->nr = start - e->start;
                        अनुवर्त;
                }
        }

        प्रत्यागम 0;
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

        प्रति (size_t i = 0; i < uid_range_entries(outside); i++)
                प्रतिज्ञा(outside->entries[i].nr == inside->entries[i].nr);

        प्रति (size_t i = 0; i < uid_range_entries(outside); i++) {
                ध्रुव UIDRange_प्रविष्टिः *e = outside->entries + i;

                यदि (uid < e->start || uid >= e->start + e->nr)
                        अनुवर्त;

                *ret = inside->entries[i].start + uid - e->start;
                प्रत्यागम 0;
        }

        प्रत्यागम -ESRCH;
}

पूर्णाङ्क uid_range_translate_userns_fd(पूर्णाङ्क userns_fd, UIDRange_Userns_बहुलकम् बहुलकम्, uid_t uid, uid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(userns_fd >= 0);
        प्रतिज्ञा(अन्तर्_समुच्चयः(बहुलकम्, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = रिक्त;
        r = uid_range_आरोपय_userns_by_fd_पूर्णम्(userns_fd, बहुलकम्, /* coalesce= */ false, &outside_range);
        यदि (r < 0)
                प्रत्यागम r;

        बहुलकम् = बहुलकम् == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = रिक्त;
        r = uid_range_आरोपय_userns_by_fd_पूर्णम्(userns_fd, बहुलकम्, /* coalesce= */ false, &inside_range);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ध्रुव UIDRange *a, ध्रुव UIDRange *b) {
        यदि (a == b)
                प्रत्यागम true;

        यदि (!a || !b)
                प्रत्यागम false;

        यदि (a->n_entries != b->n_entries)
                प्रत्यागम false;

        प्रति (size_t i = 0; i < a->n_entries; i++) {
                यदि (a->entries[i].start != b->entries[i].start)
                        प्रत्यागम false;
                यदि (a->entries[i].nr != b->entries[i].nr)
                        प्रत्यागम false;
        }

        प्रत्यागम true;
}

पूर्णाङ्क uid_map_अन्वेषणम्_मूलम्(pid_t pid, UIDRange_Userns_बहुलकम् बहुलकम्, uid_t *ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(pid_is_valid(pid));
        प्रतिज्ञा(अन्तर्_समुच्चयः(बहुलकम्, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ध्रुव वर्ण *p = procfs_file_alloca(pid, बहुलकम् == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        यदि (!f) {
                यदि (errno != ENOENT)
                        प्रत्यागम -errno;

                r = proc_mounted();
                यदि (r < 0)
                        प्रत्यागम -ENOENT; /* original error, if we can't determine /proc/ state */

                प्रत्यागम r ? -ENOPKG : -ENOSYS;
        }

        प्रति (;;) {
                uid_t uid_आधारः = UID_INVALID, uid_सरणम् = UID_INVALID;

                r = uid_map_read_one(f, &uid_आधारः, &uid_सरणम्, /* ret_range= */ रिक्त);
                यदि (r < 0)
                        प्रत्यागम r;

                यदि (uid_आधारः == 0) {
                        यदि (ret)
                                *ret = uid_सरणम्;
                        प्रत्यागम 0;
                }
        }
}

uid_t uid_range_आधारः(ध्रुव UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        यदि (uid_range_is_रिक्तानि(range))
                प्रत्यागम UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                यदि (e->nr > 0)
                        प्रत्यागम e->start;

        प्रत्यागम UID_INVALID;
}
