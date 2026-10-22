/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <sched.h>
#ಅನ್ತರ್ಭಾವ <string.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "format-util.h"
#ಅನ್ತರ್ಭಾವ "namespace-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "pidref.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"
#ಅನ್ತರ್ಭಾವ "stat-util.h"
#ಅನ್ತರ್ಭಾವ "uid-range.h"
#ಅನ್ತರ್ಭಾವ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಬಿಡುಗಡೆ(range->entries);
        ಹಿಂತಿರುಗಿ mfree(range);
}

ಸ್ಥಿರ bool uid_range_ನಮೂದು_intersect(ಸ್ಥಿರಾಂಕ UIDRange_ನಮೂದು *a, ಸ್ಥಿರಾಂಕ UIDRange_ನಮೂದು *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ a->ಆರಂಭ <= b->ಆರಂಭ + b->nr && a->ಆರಂಭ + a->nr >= b->ಆರಂಭ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ uid_range_ನಮೂದು_compare(ಸ್ಥಿರಾಂಕ UIDRange_ನಮೂದು *a, ಸ್ಥಿರಾಂಕ UIDRange_ನಮೂದು *b) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        r = CMP(a->ಆರಂಭ, b->ಆರಂಭ);
        ಆದರೆ (r != 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ CMP(a->nr, b->nr);
}

ಸ್ಥಿರ ಶೂನ್ಯ uid_range_coalesce(UIDRange *range) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಆದರೆ (range->n_entries <= 0)
                ಹಿಂತಿರುಗಿ;

        typesafe_qsort(range->entries, range->n_entries, uid_range_ನಮೂದು_compare);

        ಗಾಗಿ (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_ನಮೂದು *x = range->entries + i;

                ಗಾಗಿ (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_ನಮೂದು *y = range->entries + j;
                        uid_t begin, ಕೊನೆ;

                        ಆದರೆ (!uid_range_ನಮೂದು_intersect(x, y))
                                ನಿಲ್ಲಿಸು;

                        begin = MIN(x->ಆರಂಭ, y->ಆರಂಭ);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ಪ್ರತಿಜ್ಞಾ(x->ಆರಂಭ <= UINT32_MAX - x->nr);
                        ಪ್ರತಿಜ್ಞಾ(y->ಆರಂಭ <= UINT32_MAX - y->nr);
                        ಕೊನೆ = MAX(x->ಆರಂಭ + x->nr, y->ಆರಂಭ + y->nr);

                        x->ಆರಂಭ = begin;
                        x->nr = ಕೊನೆ - begin;

                        ಆದರೆ (range->n_entries > j + 1)
                                memmove(y, y + 1, ಗಾತ್ರ(UIDRange_ನಮೂದು) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ಪ್ರತಿಜ್ಞಾ(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ಪ್ರತಿಜ್ಞಾ(j > 0);
                        j--;
                }
        }
}

ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು_ಆಂತರಿಕ(UIDRange **range, uid_t ಆರಂಭ, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ಖಾಲಿ;
        UIDRange *p;

        ಪ್ರತಿಜ್ಞಾ(range);

        ಆದರೆ (nr <= 0)
                ಹಿಂತಿರುಗಿ 0;

        ಆದರೆ (ಆರಂಭ > UINT32_MAX - nr) /* overflow check */
                ಹಿಂತಿರುಗಿ -ERANGE;

        ಆದರೆ (*range)
                p = *range;
        ಇಲ್ಲದಿದ್ದರೆ {
                range_new = new0(UIDRange, 1);
                ಆದರೆ (!range_new)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                p = range_new;
        }

        ಆದರೆ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_ನಮೂದು) {
                .ಆರಂಭ = ಆರಂಭ,
                .nr = nr,
        };

        ಆದರೆ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_ಕೂಡಿಸು_str_ತುಂಬಿದ(UIDRange **range, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s, bool coalesce) {
        uid_t ಆರಂಭ, ಕೊನೆ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(s);

        r = parse_uid_range(s, &ಆರಂಭ, &ಕೊನೆ);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ uid_range_ಕೂಡಿಸು_ಆಂತರಿಕ(range, ಆರಂಭ, ಕೊನೆ - ಆರಂಭ + 1, coalesce);
}

ಪೂರ್ಣಾಂಕ uid_range_ಮುಂದಿನ_ಅಧೋ(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(uid);

        ಆದರೆ (*uid == 0)
                ಹಿಂತಿರುಗಿ -EBUSY;

        candidate = *uid - 1;

        ಗಾಗಿ (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, ಕೊನೆ;

                begin = range->entries[i].ಆರಂಭ;
                ಕೊನೆ = range->entries[i].ಆರಂಭ + range->entries[i].nr - 1;

                ಆದರೆ (candidate >= begin && candidate <= ಕೊನೆ) {
                        *uid = candidate;
                        ಹಿಂತಿರುಗಿ 1;
                }

                ಆದರೆ (ಕೊನೆ < candidate)
                        closest = ಕೊನೆ;
        }

        ಆದರೆ (closest == UID_INVALID)
                ಹಿಂತಿರುಗಿ -EBUSY;

        *uid = closest;
        ಹಿಂತಿರುಗಿ 1;
}

bool uid_range_covers(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t ಆರಂಭ, uid_t nr) {
        ಆದರೆ (nr == 0) /* empty range? always covered... */
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (ಆರಂಭ > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ಆದರೆ (ಆರಂಭ >= i->ಆರಂಭ &&
                    ಆರಂಭ + nr <= i->ಆರಂಭ + i->nr)
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ uid_map_read_one(FILE *f, uid_t *ret_ತಳ, uid_t *ret_ಸರಿಸು, uid_t *ret_range) {
        uid_t uid_ತಳ, uid_ಸರಿಸು, uid_range;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_ತಳ, &uid_ಸರಿಸು, &uid_range);
        ಆದರೆ (r == EOF)
                ಹಿಂತಿರುಗಿ errno_or_else(ENOMSG);
        ಪ್ರತಿಜ್ಞಾ(r >= 0);
        ಆದರೆ (r != 3)
                ಹಿಂತಿರುಗಿ -EBADMSG;
        ಆದರೆ (uid_range <= 0)
                ಹಿಂತಿರುಗಿ -EBADMSG;

        ಆದರೆ (ret_ತಳ)
                *ret_ತಳ = uid_ತಳ;
        ಆದರೆ (ret_ಸರಿಸು)
                *ret_ಸರಿಸು = uid_ಸರಿಸು;
        ಆದರೆ (ret_range)
                *ret_range = uid_range;

        ಹಿಂತಿರುಗಿ 0;
}

ಚಿಹ್ನೆರಹಿತ uid_range_ಗಾತ್ರ(ಸ್ಥಿರಾಂಕ UIDRange *range) {
        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ 0;

        ಚಿಹ್ನೆರಹಿತ n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ಹಿಂತಿರುಗಿ n;
}

bool uid_range_is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ UIDRange *range) {

        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ಆದರೆ (e->nr > 0)
                        ಹಿಂತಿರುಗಿ false;

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ಖಾಲಿ;
        _cleanup_fclose_ FILE *f = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ < UID_RANGE_USERNS_ಬಹುಲಕ_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!ಮಾರ್ಗ)
                ಮಾರ್ಗ = ಒಳ_ಗಣ(ಬಹುಲಕ, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(ಮಾರ್ಗ, "re");
        ಆದರೆ (!f) {
                r = -errno;

                ಆದರೆ (r == -ENOENT && ಮಾರ್ಗ_startswith(ಮಾರ್ಗ, "/proc/"))
                        ಹಿಂತಿರುಗಿ proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ಹಿಂತಿರುಗಿ r;
        }

        range = new0(UIDRange, 1);
        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಗಾಗಿ (;;) {
                uid_t uid_ತಳ, uid_ಸರಿಸು, uid_range;

                r = uid_map_read_one(f, &uid_ತಳ, &uid_ಸರಿಸು, &uid_range);
                ಆದರೆ (r == -ENOMSG)
                        ನಿಲ್ಲಿಸು;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                r = uid_range_ಕೂಡಿಸು_ಆಂತರಿಕ(
                                &range,
                                ಒಳ_ಗಣ(ಬಹುಲಕ, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_ತಳ : uid_ಸರಿಸು,
                                uid_range,
                                /* coalesce= */ false);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_ತುಂಬು_userns_by_fd_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ userns_fd, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_ಮುಗಿದ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಬಹುಲಕ < UID_RANGE_USERNS_ಬಹುಲಕ_MAX);
        ಪ್ರತಿಜ್ಞಾ(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r > 0)
                ಹಿಂತಿರುಗಿ uid_range_ತುಂಬು_userns_ತುಂಬಿದ(/* path= */ ಖಾಲಿ, ಬಹುಲಕ, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(
                        pidref.pid,
                        ಒಳ_ಗಣ(ಬಹುಲಕ, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ಹಿಂತಿರುಗಿ uid_range_ತುಂಬು_userns_ತುಂಬಿದ(p, ಬಹುಲಕ, coalesce, ret);
}

bool uid_range_overlaps(ಸ್ಥಿರಾಂಕ UIDRange *range, uid_t ಆರಂಭ, uid_t nr) {

        ಆದರೆ (!range)
                ಹಿಂತಿರುಗಿ false;

        /* Avoid overflow */
        ಆದರೆ (ಆರಂಭ > UINT32_MAX - nr)
                nr = UINT32_MAX - ಆರಂಭ;

        ಆದರೆ (nr == 0)
                ಹಿಂತಿರುಗಿ false;

        FOREACH_ARRAY(ನಮೂದು, range->entries, range->n_entries)
                ಆದರೆ (ಆರಂಭ < ನಮೂದು->ಆರಂಭ + ನಮೂದು->nr &&
                    ಆರಂಭ + nr >= ನಮೂದು->ಆರಂಭ)
                        ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ false;
}

ಪೂರ್ಣಾಂಕ uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಆದರೆ (min > max)
                ಹಿಂತಿರುಗಿ -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t ನಮೂದು_ಕೊನೆ = e->ಆರಂಭ + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ಆದರೆ (ನಮೂದು_ಕೊನೆ <= min || e->ಆರಂಭ > max)
                        ಮುಂದುವರಿ;

                /* Trim the entry to fit within [min, max] */
                uid_t new_ಆರಂಭ = MAX(e->ಆರಂಭ, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_ಕೊನೆ = ನಮೂದು_ಕೊನೆ <= max ? ನಮೂದು_ಕೊನೆ : max + 1;
                ಪ್ರತಿಜ್ಞಾ(new_ಕೊನೆ > new_ಆರಂಭ);

                range->entries[t++] = (UIDRange_ನಮೂದು) {
                        .ಆರಂಭ = new_ಆರಂಭ,
                        .nr = new_ಕೊನೆ - new_ಆರಂಭ,
                };
        }

        range->n_entries = t;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_ವಿಭಜನೆ(UIDRange *range, uid_t ಗಾತ್ರ_2) {
        ಪ್ರತಿಜ್ಞಾ(range);
        ಪ್ರತಿಜ್ಞಾ(ಗಾತ್ರ_2 > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / ಗಾತ್ರ_2;

        ಆದರೆ (n_new_entries == 0) {
                range->n_entries = 0;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಆದರೆ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        ಗಾಗಿ (size_t i = 0; i < range->n_entries; i++)
                ಆದರೆ (range->entries[i].nr >= ಗಾತ್ರ_2)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        ಗಾಗಿ (size_t i = n_src; i > 0; i--) {
                UIDRange_ನಮೂದು *e = range->entries + i - 1;
                ಚಿಹ್ನೆರಹಿತ n_parts = e->nr / ಗಾತ್ರ_2;

                ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_ನಮೂದು) {
                                .ಆರಂಭ = e->ಆರಂಭ + (j - 1) * ಗಾತ್ರ_2,
                                .nr = ಗಾತ್ರ_2,
                        };
        }

        range->n_entries = n_new_entries;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_copy(ಸ್ಥಿರಾಂಕ UIDRange *range, UIDRange **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (!range) {
                *ret = ಖಾಲಿ;
                ಹಿಂತಿರುಗಿ 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ಆದರೆ (!copy)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಆದರೆ (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_ನಮೂದು, range->entries, range->n_entries);
                ಆದರೆ (!copy->entries)
                        ಹಿಂತಿರುಗಿ -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_remove(UIDRange *range, uid_t ಆರಂಭ, uid_t ಗಾತ್ರ_2) {
        ಪ್ರತಿಜ್ಞಾ(range);

        ಆದರೆ (ಗಾತ್ರ_2 == 0)
                ಹಿಂತಿರುಗಿ 0;

        uid_t ಕೊನೆ = ಆರಂಭ + ಗಾತ್ರ_2; /* one past the last UID to remove */

        ಗಾಗಿ (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_ನಮೂದು *e = range->entries + i;
                uid_t ನಮೂದು_ಕೊನೆ = e->ಆರಂಭ + e->nr;

                /* No overlap */
                ಆದರೆ (ನಮೂದು_ಕೊನೆ <= ಆರಂಭ || e->ಆರಂಭ >= ಕೊನೆ)
                        ಮುಂದುವರಿ;

                /* Check if this removal splits the entry into two parts */
                ಆದರೆ (e->ಆರಂಭ < ಆರಂಭ && ನಮೂದು_ಕೊನೆ > ಕೊನೆ) {
                        /* Need to split: grow the array first */
                        ಆದರೆ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        ನಮೂದು_ಕೊನೆ = e->ಆರಂಭ + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * ಗಾತ್ರ(UIDRange_ನಮೂದು));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_ನಮೂದು) {
                                .ಆರಂಭ = e->ಆರಂಭ,
                                .nr = ಆರಂಭ - e->ಆರಂಭ,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_ನಮೂದು) {
                                .ಆರಂಭ = ಕೊನೆ,
                                .nr = ನಮೂದು_ಕೊನೆ - ಕೊನೆ,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        ಮುಂದುವರಿ;
                }

                /* Removal covers the entire entry */
                ಆದರೆ (ಆರಂಭ <= e->ಆರಂಭ && ಕೊನೆ >= ನಮೂದು_ಕೊನೆ) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * ಗಾತ್ರ(UIDRange_ನಮೂದು));
                        range->n_entries--;
                        i--;
                        ಮುಂದುವರಿ;
                }

                /* Removal trims the start of the entry */
                ಆದರೆ (ಆರಂಭ <= e->ಆರಂಭ && ಕೊನೆ > e->ಆರಂಭ) {
                        e->nr = ನಮೂದು_ಕೊನೆ - ಕೊನೆ;
                        e->ಆರಂಭ = ಕೊನೆ;
                        ಮುಂದುವರಿ;
                }

                /* Removal trims the end of the entry */
                ಆದರೆ (ಆರಂಭ < ನಮೂದು_ಕೊನೆ && ಕೊನೆ >= ನಮೂದು_ಕೊನೆ) {
                        e->nr = ಆರಂಭ - e->ಆರಂಭ;
                        ಮುಂದುವರಿ;
                }
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ uid_range_translate(ಸ್ಥಿರಾಂಕ UIDRange *outside, ಸ್ಥಿರಾಂಕ UIDRange *inside, uid_t uid, uid_t *ret) {
        ಪ್ರತಿಜ್ಞಾ(uid_range_entries(outside) == uid_range_entries(inside));
        ಪ್ರತಿಜ್ಞಾ(ret);

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

        ಗಾಗಿ (size_t i = 0; i < uid_range_entries(outside); i++)
                ಪ್ರತಿಜ್ಞಾ(outside->entries[i].nr == inside->entries[i].nr);

        ಗಾಗಿ (size_t i = 0; i < uid_range_entries(outside); i++) {
                ಸ್ಥಿರಾಂಕ UIDRange_ನಮೂದು *e = outside->entries + i;

                ಆದರೆ (uid < e->ಆರಂಭ || uid >= e->ಆರಂಭ + e->nr)
                        ಮುಂದುವರಿ;

                *ret = inside->entries[i].ಆರಂಭ + uid - e->ಆರಂಭ;
                ಹಿಂತಿರುಗಿ 0;
        }

        ಹಿಂತಿರುಗಿ -ESRCH;
}

ಪೂರ್ಣಾಂಕ uid_range_translate_userns_fd(ಪೂರ್ಣಾಂಕ userns_fd, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, uid_t uid, uid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(userns_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಬಹುಲಕ, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ಖಾಲಿ;
        r = uid_range_ತುಂಬು_userns_by_fd_ತುಂಬಿದ(userns_fd, ಬಹುಲಕ, /* coalesce= */ false, &outside_range);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಬಹುಲಕ = ಬಹುಲಕ == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ಖಾಲಿ;
        r = uid_range_ತುಂಬು_userns_by_fd_ತುಂಬಿದ(userns_fd, ಬಹುಲಕ, /* coalesce= */ false, &inside_range);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ಸ್ಥಿರಾಂಕ UIDRange *a, ಸ್ಥಿರಾಂಕ UIDRange *b) {
        ಆದರೆ (a == b)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (!a || !b)
                ಹಿಂತಿರುಗಿ false;

        ಆದರೆ (a->n_entries != b->n_entries)
                ಹಿಂತಿರುಗಿ false;

        ಗಾಗಿ (size_t i = 0; i < a->n_entries; i++) {
                ಆದರೆ (a->entries[i].ಆರಂಭ != b->entries[i].ಆರಂಭ)
                        ಹಿಂತಿರುಗಿ false;
                ಆದರೆ (a->entries[i].nr != b->entries[i].nr)
                        ಹಿಂತಿರುಗಿ false;
        }

        ಹಿಂತಿರುಗಿ true;
}

ಪೂರ್ಣಾಂಕ uid_map_ಹುಡುಕು_ಬೇರು(pid_t pid, UIDRange_Userns_ಬಹುಲಕ ಬಹುಲಕ, uid_t *ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(pid_is_valid(pid));
        ಪ್ರತಿಜ್ಞಾ(ಒಳ_ಗಣ(ಬಹುಲಕ, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = procfs_file_alloca(pid, ಬಹುಲಕ == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ಆದರೆ (!f) {
                ಆದರೆ (errno != ENOENT)
                        ಹಿಂತಿರುಗಿ -errno;

                r = proc_mounted();
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ -ENOENT; /* original error, if we can't determine /proc/ state */

                ಹಿಂತಿರುಗಿ r ? -ENOPKG : -ENOSYS;
        }

        ಗಾಗಿ (;;) {
                uid_t uid_ತಳ = UID_INVALID, uid_ಸರಿಸು = UID_INVALID;

                r = uid_map_read_one(f, &uid_ತಳ, &uid_ಸರಿಸು, /* ret_range= */ ಖಾಲಿ);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (uid_ತಳ == 0) {
                        ಆದರೆ (ret)
                                *ret = uid_ಸರಿಸು;
                        ಹಿಂತಿರುಗಿ 0;
                }
        }
}

uid_t uid_range_ತಳ(ಸ್ಥಿರಾಂಕ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ಆದರೆ (uid_range_is_ಖಾಲಿಗಳು(range))
                ಹಿಂತಿರುಗಿ UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ಆದರೆ (e->nr > 0)
                        ಹಿಂತಿರುಗಿ e->ಆರಂಭ;

        ಹಿಂತಿರುಗಿ UID_INVALID;
}
