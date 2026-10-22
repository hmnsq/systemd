/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <sched.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "format-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "namespace-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pidref.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "uid-range.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        ਜੇ (!range)
                ਵਾਪਸ ਨਿਰਾ;

        ਮੁਕਤ(range->entries);
        ਵਾਪਸ mfree(range);
}

ਸਥਿਰ bool uid_range_ਇੰਦਰਾਜ_intersect(ਅਚਲ UIDRange_ਇੰਦਰਾਜ *a, ਅਚਲ UIDRange_ਇੰਦਰਾਜ *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ a->ਸ਼ੁਰੂ <= b->ਸ਼ੁਰੂ + b->nr && a->ਸ਼ੁਰੂ + a->nr >= b->ਸ਼ੁਰੂ;
}

ਸਥਿਰ ਪੂਰਨ uid_range_ਇੰਦਰਾਜ_compare(ਅਚਲ UIDRange_ਇੰਦਰਾਜ *a, ਅਚਲ UIDRange_ਇੰਦਰਾਜ *b) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        r = CMP(a->ਸ਼ੁਰੂ, b->ਸ਼ੁਰੂ);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਵਾਪਸ CMP(a->nr, b->nr);
}

ਸਥਿਰ ਖਾਲੀ uid_range_coalesce(UIDRange *range) {
        ਪ੍ਰਤਿਜ੍ਞਾ(range);

        ਜੇ (range->n_entries <= 0)
                ਵਾਪਸ;

        typesafe_qsort(range->entries, range->n_entries, uid_range_ਇੰਦਰਾਜ_compare);

        ਲਈ (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_ਇੰਦਰਾਜ *x = range->entries + i;

                ਲਈ (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_ਇੰਦਰਾਜ *y = range->entries + j;
                        uid_t begin, ਸਿਰਾ;

                        ਜੇ (!uid_range_ਇੰਦਰਾਜ_intersect(x, y))
                                ਰੋਕੋ;

                        begin = MIN(x->ਸ਼ੁਰੂ, y->ਸ਼ੁਰੂ);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        ਪ੍ਰਤਿਜ੍ਞਾ(x->ਸ਼ੁਰੂ <= UINT32_MAX - x->nr);
                        ਪ੍ਰਤਿਜ੍ਞਾ(y->ਸ਼ੁਰੂ <= UINT32_MAX - y->nr);
                        ਸਿਰਾ = MAX(x->ਸ਼ੁਰੂ + x->nr, y->ਸ਼ੁਰੂ + y->nr);

                        x->ਸ਼ੁਰੂ = begin;
                        x->nr = ਸਿਰਾ - begin;

                        ਜੇ (range->n_entries > j + 1)
                                memmove(y, y + 1, ਆਕਾਰ(UIDRange_ਇੰਦਰਾਜ) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        ਪ੍ਰਤਿਜ੍ਞਾ(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        ਪ੍ਰਤਿਜ੍ਞਾ(j > 0);
                        j--;
                }
        }
}

ਪੂਰਨ uid_range_ਜੋੜੋ_ਅੰਦਰੂਨੀ(UIDRange **range, uid_t ਸ਼ੁਰੂ, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = ਨਿਰਾ;
        UIDRange *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(range);

        ਜੇ (nr <= 0)
                ਵਾਪਸ 0;

        ਜੇ (ਸ਼ੁਰੂ > UINT32_MAX - nr) /* overflow check */
                ਵਾਪਸ -ERANGE;

        ਜੇ (*range)
                p = *range;
        ਨਹੀਂਤਾਂ {
                range_new = new0(UIDRange, 1);
                ਜੇ (!range_new)
                        ਵਾਪਸ -ENOMEM;

                p = range_new;
        }

        ਜੇ (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                ਵਾਪਸ -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_ਇੰਦਰਾਜ) {
                .ਸ਼ੁਰੂ = ਸ਼ੁਰੂ,
                .nr = nr,
        };

        ਜੇ (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_ਜੋੜੋ_str_ਭਰਿਆ(UIDRange **range, ਅਚਲ ਅੱਖਰ *s, bool coalesce) {
        uid_t ਸ਼ੁਰੂ, ਸਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(range);
        ਪ੍ਰਤਿਜ੍ਞਾ(s);

        r = parse_uid_range(s, &ਸ਼ੁਰੂ, &ਸਿਰਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ uid_range_ਜੋੜੋ_ਅੰਦਰੂਨੀ(range, ਸ਼ੁਰੂ, ਸਿਰਾ - ਸ਼ੁਰੂ + 1, coalesce);
}

ਪੂਰਨ uid_range_ਅਗਲਾ_ਹੇਠਲਾ(ਅਚਲ UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        ਪ੍ਰਤਿਜ੍ਞਾ(range);
        ਪ੍ਰਤਿਜ੍ਞਾ(uid);

        ਜੇ (*uid == 0)
                ਵਾਪਸ -EBUSY;

        candidate = *uid - 1;

        ਲਈ (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, ਸਿਰਾ;

                begin = range->entries[i].ਸ਼ੁਰੂ;
                ਸਿਰਾ = range->entries[i].ਸ਼ੁਰੂ + range->entries[i].nr - 1;

                ਜੇ (candidate >= begin && candidate <= ਸਿਰਾ) {
                        *uid = candidate;
                        ਵਾਪਸ 1;
                }

                ਜੇ (ਸਿਰਾ < candidate)
                        closest = ਸਿਰਾ;
        }

        ਜੇ (closest == UID_INVALID)
                ਵਾਪਸ -EBUSY;

        *uid = closest;
        ਵਾਪਸ 1;
}

bool uid_range_covers(ਅਚਲ UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t nr) {
        ਜੇ (nr == 0) /* empty range? always covered... */
                ਵਾਪਸ true;

        ਜੇ (ਸ਼ੁਰੂ > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                ਵਾਪਸ false;

        ਜੇ (!range)
                ਵਾਪਸ false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                ਜੇ (ਸ਼ੁਰੂ >= i->ਸ਼ੁਰੂ &&
                    ਸ਼ੁਰੂ + nr <= i->ਸ਼ੁਰੂ + i->nr)
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

ਪੂਰਨ uid_map_read_one(FILE *f, uid_t *ret_ਅਧਾਰ, uid_t *ret_ਖਿਸਕਾਓ, uid_t *ret_range) {
        uid_t uid_ਅਧਾਰ, uid_ਖਿਸਕਾਓ, uid_range;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_ਅਧਾਰ, &uid_ਖਿਸਕਾਓ, &uid_range);
        ਜੇ (r == EOF)
                ਵਾਪਸ errno_or_else(ENOMSG);
        ਪ੍ਰਤਿਜ੍ਞਾ(r >= 0);
        ਜੇ (r != 3)
                ਵਾਪਸ -EBADMSG;
        ਜੇ (uid_range <= 0)
                ਵਾਪਸ -EBADMSG;

        ਜੇ (ret_ਅਧਾਰ)
                *ret_ਅਧਾਰ = uid_ਅਧਾਰ;
        ਜੇ (ret_ਖਿਸਕਾਓ)
                *ret_ਖਿਸਕਾਓ = uid_ਖਿਸਕਾਓ;
        ਜੇ (ret_range)
                *ret_range = uid_range;

        ਵਾਪਸ 0;
}

ਬਿਨਾਨਿਸ਼ਾਨੀ uid_range_ਮਾਪ(ਅਚਲ UIDRange *range) {
        ਜੇ (!range)
                ਵਾਪਸ 0;

        ਬਿਨਾਨਿਸ਼ਾਨੀ n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        ਵਾਪਸ n;
}

bool uid_range_is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ UIDRange *range) {

        ਜੇ (!range)
                ਵਾਪਸ true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ਜੇ (e->nr > 0)
                        ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ uid_range_ਲੱਦੋ_userns_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = ਨਿਰਾ;
        _cleanup_fclose_ FILE *f = ਨਿਰਾ;
        ਪੂਰਨ r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ < UID_RANGE_USERNS_ਬਹੁਲਕ_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!ਰਸਤਾ)
                ਰਸਤਾ = ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(ਰਸਤਾ, "re");
        ਜੇ (!f) {
                r = -errno;

                ਜੇ (r == -ENOENT && ਰਸਤਾ_startswith(ਰਸਤਾ, "/proc/"))
                        ਵਾਪਸ proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                ਵਾਪਸ r;
        }

        range = new0(UIDRange, 1);
        ਜੇ (!range)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (;;) {
                uid_t uid_ਅਧਾਰ, uid_ਖਿਸਕਾਓ, uid_range;

                r = uid_map_read_one(f, &uid_ਅਧਾਰ, &uid_ਖਿਸਕਾਓ, &uid_range);
                ਜੇ (r == -ENOMSG)
                        ਰੋਕੋ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                r = uid_range_ਜੋੜੋ_ਅੰਦਰੂਨੀ(
                                &range,
                                ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_ਅਧਾਰ : uid_ਖਿਸਕਾਓ,
                                uid_range,
                                /* coalesce= */ false);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
        }

        ਜੇ (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_ਲੱਦੋ_userns_by_fd_ਭਰਿਆ(ਪੂਰਨ userns_fd, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_ਪੂਰਾ_sigkill_wait) PidRef pidref = PIDREF_NULL;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਬਹੁਲਕ < UID_RANGE_USERNS_ਬਹੁਲਕ_MAX);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0)
                ਵਾਪਸ uid_range_ਲੱਦੋ_userns_ਭਰਿਆ(/* path= */ ਨਿਰਾ, ਬਹੁਲਕ, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(
                        pidref.pid,
                        ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        ਵਾਪਸ uid_range_ਲੱਦੋ_userns_ਭਰਿਆ(p, ਬਹੁਲਕ, coalesce, ret);
}

bool uid_range_overlaps(ਅਚਲ UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t nr) {

        ਜੇ (!range)
                ਵਾਪਸ false;

        /* Avoid overflow */
        ਜੇ (ਸ਼ੁਰੂ > UINT32_MAX - nr)
                nr = UINT32_MAX - ਸ਼ੁਰੂ;

        ਜੇ (nr == 0)
                ਵਾਪਸ false;

        FOREACH_ARRAY(ਇੰਦਰਾਜ, range->entries, range->n_entries)
                ਜੇ (ਸ਼ੁਰੂ < ਇੰਦਰਾਜ->ਸ਼ੁਰੂ + ਇੰਦਰਾਜ->nr &&
                    ਸ਼ੁਰੂ + nr >= ਇੰਦਰਾਜ->ਸ਼ੁਰੂ)
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

ਪੂਰਨ uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        ਪ੍ਰਤਿਜ੍ਞਾ(range);

        ਜੇ (min > max)
                ਵਾਪਸ -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t ਇੰਦਰਾਜ_ਸਿਰਾ = e->ਸ਼ੁਰੂ + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                ਜੇ (ਇੰਦਰਾਜ_ਸਿਰਾ <= min || e->ਸ਼ੁਰੂ > max)
                        ਜਾਰੀ;

                /* Trim the entry to fit within [min, max] */
                uid_t new_ਸ਼ੁਰੂ = MAX(e->ਸ਼ੁਰੂ, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_ਸਿਰਾ = ਇੰਦਰਾਜ_ਸਿਰਾ <= max ? ਇੰਦਰਾਜ_ਸਿਰਾ : max + 1;
                ਪ੍ਰਤਿਜ੍ਞਾ(new_ਸਿਰਾ > new_ਸ਼ੁਰੂ);

                range->entries[t++] = (UIDRange_ਇੰਦਰਾਜ) {
                        .ਸ਼ੁਰੂ = new_ਸ਼ੁਰੂ,
                        .nr = new_ਸਿਰਾ - new_ਸ਼ੁਰੂ,
                };
        }

        range->n_entries = t;

        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_ਹਿੱਸੇ_ਕਰੋ(UIDRange *range, uid_t ਮਾਪ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(range);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਮਾਪ > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / ਮਾਪ;

        ਜੇ (n_new_entries == 0) {
                range->n_entries = 0;
                ਵਾਪਸ 0;
        }

        ਜੇ (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                ਵਾਪਸ -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        ਲਈ (size_t i = 0; i < range->n_entries; i++)
                ਜੇ (range->entries[i].nr >= ਮਾਪ)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        ਲਈ (size_t i = n_src; i > 0; i--) {
                UIDRange_ਇੰਦਰਾਜ *e = range->entries + i - 1;
                ਬਿਨਾਨਿਸ਼ਾਨੀ n_parts = e->nr / ਮਾਪ;

                ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_ਇੰਦਰਾਜ) {
                                .ਸ਼ੁਰੂ = e->ਸ਼ੁਰੂ + (j - 1) * ਮਾਪ,
                                .nr = ਮਾਪ,
                        };
        }

        range->n_entries = n_new_entries;

        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_copy(ਅਚਲ UIDRange *range, UIDRange **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!range) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        ਜੇ (!copy)
                ਵਾਪਸ -ENOMEM;

        ਜੇ (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_ਇੰਦਰਾਜ, range->entries, range->n_entries);
                ਜੇ (!copy->entries)
                        ਵਾਪਸ -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_remove(UIDRange *range, uid_t ਸ਼ੁਰੂ, uid_t ਮਾਪ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(range);

        ਜੇ (ਮਾਪ == 0)
                ਵਾਪਸ 0;

        uid_t ਸਿਰਾ = ਸ਼ੁਰੂ + ਮਾਪ; /* one past the last UID to remove */

        ਲਈ (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_ਇੰਦਰਾਜ *e = range->entries + i;
                uid_t ਇੰਦਰਾਜ_ਸਿਰਾ = e->ਸ਼ੁਰੂ + e->nr;

                /* No overlap */
                ਜੇ (ਇੰਦਰਾਜ_ਸਿਰਾ <= ਸ਼ੁਰੂ || e->ਸ਼ੁਰੂ >= ਸਿਰਾ)
                        ਜਾਰੀ;

                /* Check if this removal splits the entry into two parts */
                ਜੇ (e->ਸ਼ੁਰੂ < ਸ਼ੁਰੂ && ਇੰਦਰਾਜ_ਸਿਰਾ > ਸਿਰਾ) {
                        /* Need to split: grow the array first */
                        ਜੇ (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                ਵਾਪਸ -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        ਇੰਦਰਾਜ_ਸਿਰਾ = e->ਸ਼ੁਰੂ + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * ਆਕਾਰ(UIDRange_ਇੰਦਰਾਜ));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_ਇੰਦਰਾਜ) {
                                .ਸ਼ੁਰੂ = e->ਸ਼ੁਰੂ,
                                .nr = ਸ਼ੁਰੂ - e->ਸ਼ੁਰੂ,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_ਇੰਦਰਾਜ) {
                                .ਸ਼ੁਰੂ = ਸਿਰਾ,
                                .nr = ਇੰਦਰਾਜ_ਸਿਰਾ - ਸਿਰਾ,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        ਜਾਰੀ;
                }

                /* Removal covers the entire entry */
                ਜੇ (ਸ਼ੁਰੂ <= e->ਸ਼ੁਰੂ && ਸਿਰਾ >= ਇੰਦਰਾਜ_ਸਿਰਾ) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * ਆਕਾਰ(UIDRange_ਇੰਦਰਾਜ));
                        range->n_entries--;
                        i--;
                        ਜਾਰੀ;
                }

                /* Removal trims the start of the entry */
                ਜੇ (ਸ਼ੁਰੂ <= e->ਸ਼ੁਰੂ && ਸਿਰਾ > e->ਸ਼ੁਰੂ) {
                        e->nr = ਇੰਦਰਾਜ_ਸਿਰਾ - ਸਿਰਾ;
                        e->ਸ਼ੁਰੂ = ਸਿਰਾ;
                        ਜਾਰੀ;
                }

                /* Removal trims the end of the entry */
                ਜੇ (ਸ਼ੁਰੂ < ਇੰਦਰਾਜ_ਸਿਰਾ && ਸਿਰਾ >= ਇੰਦਰਾਜ_ਸਿਰਾ) {
                        e->nr = ਸ਼ੁਰੂ - e->ਸ਼ੁਰੂ;
                        ਜਾਰੀ;
                }
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ uid_range_translate(ਅਚਲ UIDRange *outside, ਅਚਲ UIDRange *inside, uid_t uid, uid_t *ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(uid_range_entries(outside) == uid_range_entries(inside));
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

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

        ਲਈ (size_t i = 0; i < uid_range_entries(outside); i++)
                ਪ੍ਰਤਿਜ੍ਞਾ(outside->entries[i].nr == inside->entries[i].nr);

        ਲਈ (size_t i = 0; i < uid_range_entries(outside); i++) {
                ਅਚਲ UIDRange_ਇੰਦਰਾਜ *e = outside->entries + i;

                ਜੇ (uid < e->ਸ਼ੁਰੂ || uid >= e->ਸ਼ੁਰੂ + e->nr)
                        ਜਾਰੀ;

                *ret = inside->entries[i].ਸ਼ੁਰੂ + uid - e->ਸ਼ੁਰੂ;
                ਵਾਪਸ 0;
        }

        ਵਾਪਸ -ESRCH;
}

ਪੂਰਨ uid_range_translate_userns_fd(ਪੂਰਨ userns_fd, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, uid_t uid, uid_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(userns_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = ਨਿਰਾ;
        r = uid_range_ਲੱਦੋ_userns_by_fd_ਭਰਿਆ(userns_fd, ਬਹੁਲਕ, /* coalesce= */ false, &outside_range);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਬਹੁਲਕ = ਬਹੁਲਕ == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = ਨਿਰਾ;
        r = uid_range_ਲੱਦੋ_userns_by_fd_ਭਰਿਆ(userns_fd, ਬਹੁਲਕ, /* coalesce= */ false, &inside_range);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ਅਚਲ UIDRange *a, ਅਚਲ UIDRange *b) {
        ਜੇ (a == b)
                ਵਾਪਸ true;

        ਜੇ (!a || !b)
                ਵਾਪਸ false;

        ਜੇ (a->n_entries != b->n_entries)
                ਵਾਪਸ false;

        ਲਈ (size_t i = 0; i < a->n_entries; i++) {
                ਜੇ (a->entries[i].ਸ਼ੁਰੂ != b->entries[i].ਸ਼ੁਰੂ)
                        ਵਾਪਸ false;
                ਜੇ (a->entries[i].nr != b->entries[i].nr)
                        ਵਾਪਸ false;
        }

        ਵਾਪਸ true;
}

ਪੂਰਨ uid_map_ਖੋਜ_ਜੜ੍ਹ(pid_t pid, UIDRange_Userns_ਬਹੁਲਕ ਬਹੁਲਕ, uid_t *ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(pid_is_valid(pid));
        ਪ੍ਰਤਿਜ੍ਞਾ(ਵਿੱਚ_ਸੈੱਟ(ਬਹੁਲਕ, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ਅਚਲ ਅੱਖਰ *p = procfs_file_alloca(pid, ਬਹੁਲਕ == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        ਜੇ (!f) {
                ਜੇ (errno != ENOENT)
                        ਵਾਪਸ -errno;

                r = proc_mounted();
                ਜੇ (r < 0)
                        ਵਾਪਸ -ENOENT; /* original error, if we can't determine /proc/ state */

                ਵਾਪਸ r ? -ENOPKG : -ENOSYS;
        }

        ਲਈ (;;) {
                uid_t uid_ਅਧਾਰ = UID_INVALID, uid_ਖਿਸਕਾਓ = UID_INVALID;

                r = uid_map_read_one(f, &uid_ਅਧਾਰ, &uid_ਖਿਸਕਾਓ, /* ret_range= */ ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਜੇ (uid_ਅਧਾਰ == 0) {
                        ਜੇ (ret)
                                *ret = uid_ਖਿਸਕਾਓ;
                        ਵਾਪਸ 0;
                }
        }
}

uid_t uid_range_ਅਧਾਰ(ਅਚਲ UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        ਜੇ (uid_range_is_ਖਾਲੀ_ਥਾਵਾਂ(range))
                ਵਾਪਸ UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                ਜੇ (e->nr > 0)
                        ਵਾਪਸ e->ਸ਼ੁਰੂ;

        ਵਾਪਸ UID_INVALID;
}
