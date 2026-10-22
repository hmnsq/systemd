/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <sched.h>
#అన్తర్భావ <string.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "format-util.h"
#అన్తర్భావ "namespace-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "pidref.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "sort-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "uid-range.h"
#అన్తర్భావ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        ఒకవేళ (!range)
                తిరిగి ఖాళీ;

        విడుదల(range->entries);
        తిరిగి mfree(range);
}

స్థిర bool uid_range_నమోదు_intersect(స్థిరరాశి UIDRange_నమోదు *a, స్థిరరాశి UIDRange_నమోదు *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి a->ప్రారంభం <= b->ప్రారంభం + b->nr && a->ప్రారంభం + a->nr >= b->ప్రారంభం;
}

స్థిర పూర్ణాంకం uid_range_నమోదు_compare(స్థిరరాశి UIDRange_నమోదు *a, స్థిరరాశి UIDRange_నమోదు *b) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        r = CMP(a->ప్రారంభం, b->ప్రారంభం);
        ఒకవేళ (r != 0)
                తిరిగి r;

        తిరిగి CMP(a->nr, b->nr);
}

స్థిర శూన్య uid_range_coalesce(UIDRange *range) {
        ప్రతిజ్ఞా(range);

        ఒకవేళ (range->n_entries <= 0)
                తిరిగి;

        typesafe_qsort(range->entries, range->n_entries, uid_range_నమోదు_compare);

        కొరకు (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_నమోదు *x = range->entries + i;

                కొరకు (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_నమోదు *y = range->entries + j;
                        uid_t begin, చివర;

                        ఒకవేళ (!uid_range_నమోదు_intersect(x, y))
                                ఆపు;

                        begin = MIN(x->ప్రారంభం, y->ప్రారంభం);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ప్రతిజ్ఞా(x->ప్రారంభం <= UINT32_MAX - x->nr);
                        ప్రతిజ్ఞా(y->ప్రారంభం <= UINT32_MAX - y->nr);
                        చివర = MAX(x->ప్రారంభం + x->nr, y->ప్రారంభం + y->nr);

                        x->ప్రారంభం = begin;
                        x->nr = చివర - begin;

                        ఒకవేళ (range->n_entries > j + 1)
                                memmove(y, y + 1, పరిమాణం(UIDRange_నమోదు) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ప్రతిజ్ఞా(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ప్రతిజ్ఞా(j > 0);
                        j--;
                }
        }
}

పూర్ణాంకం uid_range_కూడు_అంతర్గత(UIDRange **range, uid_t ప్రారంభం, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ఖాళీ;
        UIDRange *p;

        ప్రతిజ్ఞా(range);

        ఒకవేళ (nr <= 0)
                తిరిగి 0;

        ఒకవేళ (ప్రారంభం > UINT32_MAX - nr) /* overflow check */
                తిరిగి -ERANGE;

        ఒకవేళ (*range)
                p = *range;
        లేకపోతే {
                range_new = new0(UIDRange, 1);
                ఒకవేళ (!range_new)
                        తిరిగి -ENOMEM;

                p = range_new;
        }

        ఒకవేళ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                తిరిగి -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_నమోదు) {
                .ప్రారంభం = ప్రారంభం,
                .nr = nr,
        };

        ఒకవేళ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        తిరిగి 0;
}

పూర్ణాంకం uid_range_కూడు_str_నిండిన(UIDRange **range, స్థిరరాశి అక్షరం *s, bool coalesce) {
        uid_t ప్రారంభం, చివర;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(range);
        ప్రతిజ్ఞా(s);

        r = parse_uid_range(s, &ప్రారంభం, &చివర);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి uid_range_కూడు_అంతర్గత(range, ప్రారంభం, చివర - ప్రారంభం + 1, coalesce);
}

పూర్ణాంకం uid_range_తదుపరి_క్రింది(స్థిరరాశి UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ప్రతిజ్ఞా(range);
        ప్రతిజ్ఞా(uid);

        ఒకవేళ (*uid == 0)
                తిరిగి -EBUSY;

        candidate = *uid - 1;

        కొరకు (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, చివర;

                begin = range->entries[i].ప్రారంభం;
                చివర = range->entries[i].ప్రారంభం + range->entries[i].nr - 1;

                ఒకవేళ (candidate >= begin && candidate <= చివర) {
                        *uid = candidate;
                        తిరిగి 1;
                }

                ఒకవేళ (చివర < candidate)
                        closest = చివర;
        }

        ఒకవేళ (closest == UID_INVALID)
                తిరిగి -EBUSY;

        *uid = closest;
        తిరిగి 1;
}

bool uid_range_covers(స్థిరరాశి UIDRange *range, uid_t ప్రారంభం, uid_t nr) {
        ఒకవేళ (nr == 0) /* empty range? always covered... */
                తిరిగి true;

        ఒకవేళ (ప్రారంభం > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                తిరిగి false;

        ఒకవేళ (!range)
                తిరిగి false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ఒకవేళ (ప్రారంభం >= i->ప్రారంభం &&
                    ప్రారంభం + nr <= i->ప్రారంభం + i->nr)
                        తిరిగి true;

        తిరిగి false;
}

పూర్ణాంకం uid_map_read_one(FILE *f, uid_t *ret_పునాది, uid_t *ret_జరుపు, uid_t *ret_range) {
        uid_t uid_పునాది, uid_జరుపు, uid_range;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_పునాది, &uid_జరుపు, &uid_range);
        ఒకవేళ (r == EOF)
                తిరిగి errno_or_else(ENOMSG);
        ప్రతిజ్ఞా(r >= 0);
        ఒకవేళ (r != 3)
                తిరిగి -EBADMSG;
        ఒకవేళ (uid_range <= 0)
                తిరిగి -EBADMSG;

        ఒకవేళ (ret_పునాది)
                *ret_పునాది = uid_పునాది;
        ఒకవేళ (ret_జరుపు)
                *ret_జరుపు = uid_జరుపు;
        ఒకవేళ (ret_range)
                *ret_range = uid_range;

        తిరిగి 0;
}

గుర్తులేని uid_range_కొలత(స్థిరరాశి UIDRange *range) {
        ఒకవేళ (!range)
                తిరిగి 0;

        గుర్తులేని n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        తిరిగి n;
}

bool uid_range_is_ఖాళీలు(స్థిరరాశి UIDRange *range) {

        ఒకవేళ (!range)
                తిరిగి true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ఒకవేళ (e->nr > 0)
                        తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం uid_range_ఎక్కించు_userns_నిండిన(స్థిరరాశి అక్షరం *మార్గం, UIDRange_Userns_బహుళకం బహుళకం, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ఖాళీ;
        _cleanup_fclose_ FILE *f = ఖాళీ;
        పూర్ణాంకం r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ప్రతిజ్ఞా(బహుళకం >= 0);
        ప్రతిజ్ఞా(బహుళకం < UID_RANGE_USERNS_బహుళకం_MAX);
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!మార్గం)
                మార్గం = లోపల_సమితి(బహుళకం, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(మార్గం, "re");
        ఒకవేళ (!f) {
                r = -errno;

                ఒకవేళ (r == -ENOENT && మార్గం_startswith(మార్గం, "/proc/"))
                        తిరిగి proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                తిరిగి r;
        }

        range = new0(UIDRange, 1);
        ఒకవేళ (!range)
                తిరిగి -ENOMEM;

        కొరకు (;;) {
                uid_t uid_పునాది, uid_జరుపు, uid_range;

                r = uid_map_read_one(f, &uid_పునాది, &uid_జరుపు, &uid_range);
                ఒకవేళ (r == -ENOMSG)
                        ఆపు;
                ఒకవేళ (r < 0)
                        తిరిగి r;

                r = uid_range_కూడు_అంతర్గత(
                                &range,
                                లోపల_సమితి(బహుళకం, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_పునాది : uid_జరుపు,
                                uid_range,
                                /* coalesce= */ false);
                ఒకవేళ (r < 0)
                        తిరిగి r;
        }

        ఒకవేళ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        తిరిగి 0;
}

పూర్ణాంకం uid_range_ఎక్కించు_userns_by_fd_నిండిన(పూర్ణాంకం userns_fd, UIDRange_Userns_బహుళకం బహుళకం, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_పూర్తి_sigkill_wait) PidRef pidref = PIDREF_NULL;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);
        ప్రతిజ్ఞా(బహుళకం >= 0);
        ప్రతిజ్ఞా(బహుళకం < UID_RANGE_USERNS_బహుళకం_MAX);
        ప్రతిజ్ఞా(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0)
                తిరిగి uid_range_ఎక్కించు_userns_నిండిన(/* path= */ ఖాళీ, బహుళకం, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ఒకవేళ (r < 0)
                తిరిగి r;

        స్థిరరాశి అక్షరం *p = procfs_file_alloca(
                        pidref.pid,
                        లోపల_సమితి(బహుళకం, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        తిరిగి uid_range_ఎక్కించు_userns_నిండిన(p, బహుళకం, coalesce, ret);
}

bool uid_range_overlaps(స్థిరరాశి UIDRange *range, uid_t ప్రారంభం, uid_t nr) {

        ఒకవేళ (!range)
                తిరిగి false;

        /* Avoid overflow */
        ఒకవేళ (ప్రారంభం > UINT32_MAX - nr)
                nr = UINT32_MAX - ప్రారంభం;

        ఒకవేళ (nr == 0)
                తిరిగి false;

        FOREACH_ARRAY(నమోదు_2, range->entries, range->n_entries)
                ఒకవేళ (ప్రారంభం < నమోదు_2->ప్రారంభం + నమోదు_2->nr &&
                    ప్రారంభం + nr >= నమోదు_2->ప్రారంభం)
                        తిరిగి true;

        తిరిగి false;
}

పూర్ణాంకం uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ప్రతిజ్ఞా(range);

        ఒకవేళ (min > max)
                తిరిగి -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t నమోదు_చివర = e->ప్రారంభం + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ఒకవేళ (నమోదు_చివర <= min || e->ప్రారంభం > max)
                        కొనసాగు;

                /* Trim the entry to fit within [min, max] */
                uid_t new_ప్రారంభం = MAX(e->ప్రారంభం, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_చివర = నమోదు_చివర <= max ? నమోదు_చివర : max + 1;
                ప్రతిజ్ఞా(new_చివర > new_ప్రారంభం);

                range->entries[t++] = (UIDRange_నమోదు) {
                        .ప్రారంభం = new_ప్రారంభం,
                        .nr = new_చివర - new_ప్రారంభం,
                };
        }

        range->n_entries = t;

        తిరిగి 0;
}

పూర్ణాంకం uid_range_విభజన(UIDRange *range, uid_t కొలత) {
        ప్రతిజ్ఞా(range);
        ప్రతిజ్ఞా(కొలత > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / కొలత;

        ఒకవేళ (n_new_entries == 0) {
                range->n_entries = 0;
                తిరిగి 0;
        }

        ఒకవేళ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                తిరిగి -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        కొరకు (size_t i = 0; i < range->n_entries; i++)
                ఒకవేళ (range->entries[i].nr >= కొలత)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        కొరకు (size_t i = n_src; i > 0; i--) {
                UIDRange_నమోదు *e = range->entries + i - 1;
                గుర్తులేని n_parts = e->nr / కొలత;

                కొరకు (గుర్తులేని j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_నమోదు) {
                                .ప్రారంభం = e->ప్రారంభం + (j - 1) * కొలత,
                                .nr = కొలత,
                        };
        }

        range->n_entries = n_new_entries;

        తిరిగి 0;
}

పూర్ణాంకం uid_range_copy(స్థిరరాశి UIDRange *range, UIDRange **ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!range) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ఒకవేళ (!copy)
                తిరిగి -ENOMEM;

        ఒకవేళ (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_నమోదు, range->entries, range->n_entries);
                ఒకవేళ (!copy->entries)
                        తిరిగి -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        తిరిగి 0;
}

పూర్ణాంకం uid_range_remove(UIDRange *range, uid_t ప్రారంభం, uid_t కొలత) {
        ప్రతిజ్ఞా(range);

        ఒకవేళ (కొలత == 0)
                తిరిగి 0;

        uid_t చివర = ప్రారంభం + కొలత; /* one past the last UID to remove */

        కొరకు (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_నమోదు *e = range->entries + i;
                uid_t నమోదు_చివర = e->ప్రారంభం + e->nr;

                /* No overlap */
                ఒకవేళ (నమోదు_చివర <= ప్రారంభం || e->ప్రారంభం >= చివర)
                        కొనసాగు;

                /* Check if this removal splits the entry into two parts */
                ఒకవేళ (e->ప్రారంభం < ప్రారంభం && నమోదు_చివర > చివర) {
                        /* Need to split: grow the array first */
                        ఒకవేళ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                తిరిగి -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        నమోదు_చివర = e->ప్రారంభం + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * పరిమాణం(UIDRange_నమోదు));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_నమోదు) {
                                .ప్రారంభం = e->ప్రారంభం,
                                .nr = ప్రారంభం - e->ప్రారంభం,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_నమోదు) {
                                .ప్రారంభం = చివర,
                                .nr = నమోదు_చివర - చివర,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        కొనసాగు;
                }

                /* Removal covers the entire entry */
                ఒకవేళ (ప్రారంభం <= e->ప్రారంభం && చివర >= నమోదు_చివర) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * పరిమాణం(UIDRange_నమోదు));
                        range->n_entries--;
                        i--;
                        కొనసాగు;
                }

                /* Removal trims the start of the entry */
                ఒకవేళ (ప్రారంభం <= e->ప్రారంభం && చివర > e->ప్రారంభం) {
                        e->nr = నమోదు_చివర - చివర;
                        e->ప్రారంభం = చివర;
                        కొనసాగు;
                }

                /* Removal trims the end of the entry */
                ఒకవేళ (ప్రారంభం < నమోదు_చివర && చివర >= నమోదు_చివర) {
                        e->nr = ప్రారంభం - e->ప్రారంభం;
                        కొనసాగు;
                }
        }

        తిరిగి 0;
}

పూర్ణాంకం uid_range_translate(స్థిరరాశి UIDRange *outside, స్థిరరాశి UIDRange *inside, uid_t uid, uid_t *ret) {
        ప్రతిజ్ఞా(uid_range_entries(outside) == uid_range_entries(inside));
        ప్రతిజ్ఞా(ret);

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

        కొరకు (size_t i = 0; i < uid_range_entries(outside); i++)
                ప్రతిజ్ఞా(outside->entries[i].nr == inside->entries[i].nr);

        కొరకు (size_t i = 0; i < uid_range_entries(outside); i++) {
                స్థిరరాశి UIDRange_నమోదు *e = outside->entries + i;

                ఒకవేళ (uid < e->ప్రారంభం || uid >= e->ప్రారంభం + e->nr)
                        కొనసాగు;

                *ret = inside->entries[i].ప్రారంభం + uid - e->ప్రారంభం;
                తిరిగి 0;
        }

        తిరిగి -ESRCH;
}

పూర్ణాంకం uid_range_translate_userns_fd(పూర్ణాంకం userns_fd, UIDRange_Userns_బహుళకం బహుళకం, uid_t uid, uid_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(userns_fd >= 0);
        ప్రతిజ్ఞా(లోపల_సమితి(బహుళకం, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ఖాళీ;
        r = uid_range_ఎక్కించు_userns_by_fd_నిండిన(userns_fd, బహుళకం, /* coalesce= */ false, &outside_range);
        ఒకవేళ (r < 0)
                తిరిగి r;

        బహుళకం = బహుళకం == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ఖాళీ;
        r = uid_range_ఎక్కించు_userns_by_fd_నిండిన(userns_fd, బహుళకం, /* coalesce= */ false, &inside_range);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(స్థిరరాశి UIDRange *a, స్థిరరాశి UIDRange *b) {
        ఒకవేళ (a == b)
                తిరిగి true;

        ఒకవేళ (!a || !b)
                తిరిగి false;

        ఒకవేళ (a->n_entries != b->n_entries)
                తిరిగి false;

        కొరకు (size_t i = 0; i < a->n_entries; i++) {
                ఒకవేళ (a->entries[i].ప్రారంభం != b->entries[i].ప్రారంభం)
                        తిరిగి false;
                ఒకవేళ (a->entries[i].nr != b->entries[i].nr)
                        తిరిగి false;
        }

        తిరిగి true;
}

పూర్ణాంకం uid_map_శోధన_మూలం(pid_t pid, UIDRange_Userns_బహుళకం బహుళకం, uid_t *ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(pid_is_valid(pid));
        ప్రతిజ్ఞా(లోపల_సమితి(బహుళకం, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        స్థిరరాశి అక్షరం *p = procfs_file_alloca(pid, బహుళకం == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ఒకవేళ (!f) {
                ఒకవేళ (errno != ENOENT)
                        తిరిగి -errno;

                r = proc_mounted();
                ఒకవేళ (r < 0)
                        తిరిగి -ENOENT; /* original error, if we can't determine /proc/ state */

                తిరిగి r ? -ENOPKG : -ENOSYS;
        }

        కొరకు (;;) {
                uid_t uid_పునాది = UID_INVALID, uid_జరుపు = UID_INVALID;

                r = uid_map_read_one(f, &uid_పునాది, &uid_జరుపు, /* ret_range= */ ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                ఒకవేళ (uid_పునాది == 0) {
                        ఒకవేళ (ret)
                                *ret = uid_జరుపు;
                        తిరిగి 0;
                }
        }
}

uid_t uid_range_పునాది(స్థిరరాశి UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ఒకవేళ (uid_range_is_ఖాళీలు(range))
                తిరిగి UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ఒకవేళ (e->nr > 0)
                        తిరిగి e->ప్రారంభం;

        తిరిగి UID_INVALID;
}
