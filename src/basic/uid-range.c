/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <sched.h>
#شامل <string.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "format-util.h"
#شامل "namespace-util.h"
#شامل "path-util.h"
#شامل "pidref.h"
#شامل "process-util.h"
#شامل "sort-util.h"
#شامل "stat-util.h"
#شامل "uid-range.h"
#شامل "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        اگر (!range)
                واپس تہی;

        آزاد(range->entries);
        واپس mfree(range);
}

ساکن bool uid_range_اندراج_intersect(ثابت UIDRange_اندراج *a, ثابت UIDRange_اندراج *b) {
        أكد(a);
        أكد(b);

        واپس a->آغاز <= b->آغاز + b->nr && a->آغاز + a->nr >= b->آغاز;
}

ساکن صحیح uid_range_اندراج_compare(ثابت UIDRange_اندراج *a, ثابت UIDRange_اندراج *b) {
        صحیح r;

        أكد(a);
        أكد(b);

        r = CMP(a->آغاز, b->آغاز);
        اگر (r != 0)
                واپس r;

        واپس CMP(a->nr, b->nr);
}

ساکن خالی uid_range_coalesce(UIDRange *range) {
        أكد(range);

        اگر (range->n_entries <= 0)
                واپس;

        typesafe_qsort(range->entries, range->n_entries, uid_range_اندراج_compare);

        برائے (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_اندراج *x = range->entries + i;

                برائے (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_اندراج *y = range->entries + j;
                        uid_t begin, انتہا;

                        اگر (!uid_range_اندراج_intersect(x, y))
                                توڑو;

                        begin = MIN(x->آغاز, y->آغاز);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        أكد(x->آغاز <= UINT32_MAX - x->nr);
                        أكد(y->آغاز <= UINT32_MAX - y->nr);
                        انتہا = MAX(x->آغاز + x->nr, y->آغاز + y->nr);

                        x->آغاز = begin;
                        x->nr = انتہا - begin;

                        اگر (range->n_entries > j + 1)
                                memmove(y, y + 1, حجم(UIDRange_اندراج) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        أكد(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        أكد(j > 0);
                        j--;
                }
        }
}

صحیح uid_range_جوڑو_اندرونی(UIDRange **range, uid_t آغاز, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = تہی;
        UIDRange *p;

        أكد(range);

        اگر (nr <= 0)
                واپس 0;

        اگر (آغاز > UINT32_MAX - nr) /* overflow check */
                واپس -ERANGE;

        اگر (*range)
                p = *range;
        ورنہ {
                range_new = new0(UIDRange, 1);
                اگر (!range_new)
                        واپس -ENOMEM;

                p = range_new;
        }

        اگر (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                واپس -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_اندراج) {
                .آغاز = آغاز,
                .nr = nr,
        };

        اگر (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        واپس 0;
}

صحیح uid_range_جوڑو_str_بھرا(UIDRange **range, ثابت حرف *s, bool coalesce) {
        uid_t آغاز, انتہا;
        صحیح r;

        أكد(range);
        أكد(s);

        r = parse_uid_range(s, &آغاز, &انتہا);
        اگر (r < 0)
                واپس r;

        واپس uid_range_جوڑو_اندرونی(range, آغاز, انتہا - آغاز + 1, coalesce);
}

صحیح uid_range_اگلا_زیریں(ثابت UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        أكد(range);
        أكد(uid);

        اگر (*uid == 0)
                واپس -EBUSY;

        candidate = *uid - 1;

        برائے (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, انتہا;

                begin = range->entries[i].آغاز;
                انتہا = range->entries[i].آغاز + range->entries[i].nr - 1;

                اگر (candidate >= begin && candidate <= انتہا) {
                        *uid = candidate;
                        واپس 1;
                }

                اگر (انتہا < candidate)
                        closest = انتہا;
        }

        اگر (closest == UID_INVALID)
                واپس -EBUSY;

        *uid = closest;
        واپس 1;
}

bool uid_range_covers(ثابت UIDRange *range, uid_t آغاز, uid_t nr) {
        اگر (nr == 0) /* empty range? always covered... */
                واپس true;

        اگر (آغاز > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                واپس false;

        اگر (!range)
                واپس false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                اگر (آغاز >= i->آغاز &&
                    آغاز + nr <= i->آغاز + i->nr)
                        واپس true;

        واپس false;
}

صحیح uid_map_read_one(FILE *f, uid_t *ret_بنیاد, uid_t *ret_سرکاؤ, uid_t *ret_range) {
        uid_t uid_بنیاد, uid_سرکاؤ, uid_range;
        صحیح r;

        أكد(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_بنیاد, &uid_سرکاؤ, &uid_range);
        اگر (r == EOF)
                واپس errno_or_else(ENOMSG);
        أكد(r >= 0);
        اگر (r != 3)
                واپس -EBADMSG;
        اگر (uid_range <= 0)
                واپس -EBADMSG;

        اگر (ret_بنیاد)
                *ret_بنیاد = uid_بنیاد;
        اگر (ret_سرکاؤ)
                *ret_سرکاؤ = uid_سرکاؤ;
        اگر (ret_range)
                *ret_range = uid_range;

        واپس 0;
}

غیرعلامتی uid_range_سائز(ثابت UIDRange *range) {
        اگر (!range)
                واپس 0;

        غیرعلامتی n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        واپس n;
}

bool uid_range_is_خالی_خانے(ثابت UIDRange *range) {

        اگر (!range)
                واپس true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                اگر (e->nr > 0)
                        واپس false;

        واپس true;
}

صحیح uid_range_بھرو_userns_بھرا(ثابت حرف *راستہ, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = تہی;
        _cleanup_fclose_ FILE *f = تہی;
        صحیح r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        أكد(منوال >= 0);
        أكد(منوال < UID_RANGE_USERNS_منوال_MAX);
        أكد(ret);

        اگر (!راستہ)
                راستہ = اندر_سیٹ(منوال, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(راستہ, "re");
        اگر (!f) {
                r = -errno;

                اگر (r == -ENOENT && راستہ_startswith(راستہ, "/proc/"))
                        واپس proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                واپس r;
        }

        range = new0(UIDRange, 1);
        اگر (!range)
                واپس -ENOMEM;

        برائے (;;) {
                uid_t uid_بنیاد, uid_سرکاؤ, uid_range;

                r = uid_map_read_one(f, &uid_بنیاد, &uid_سرکاؤ, &uid_range);
                اگر (r == -ENOMSG)
                        توڑو;
                اگر (r < 0)
                        واپس r;

                r = uid_range_جوڑو_اندرونی(
                                &range,
                                اندر_سیٹ(منوال, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_بنیاد : uid_سرکاؤ,
                                uid_range,
                                /* coalesce= */ false);
                اگر (r < 0)
                        واپس r;
        }

        اگر (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        واپس 0;
}

صحیح uid_range_بھرو_userns_by_fd_بھرا(صحیح userns_fd, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_مکمل_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحیح r;

        أكد(userns_fd >= 0);
        أكد(منوال >= 0);
        أكد(منوال < UID_RANGE_USERNS_منوال_MAX);
        أكد(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        اگر (r < 0)
                واپس r;
        اگر (r > 0)
                واپس uid_range_بھرو_userns_بھرا(/* path= */ تہی, منوال, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        اگر (r < 0)
                واپس r;

        ثابت حرف *p = procfs_file_alloca(
                        pidref.pid,
                        اندر_سیٹ(منوال, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        واپس uid_range_بھرو_userns_بھرا(p, منوال, coalesce, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t آغاز, uid_t nr) {

        اگر (!range)
                واپس false;

        /* Avoid overflow */
        اگر (آغاز > UINT32_MAX - nr)
                nr = UINT32_MAX - آغاز;

        اگر (nr == 0)
                واپس false;

        FOREACH_ARRAY(اندراج_2, range->entries, range->n_entries)
                اگر (آغاز < اندراج_2->آغاز + اندراج_2->nr &&
                    آغاز + nr >= اندراج_2->آغاز)
                        واپس true;

        واپس false;
}

صحیح uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        أكد(range);

        اگر (min > max)
                واپس -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t اندراج_انتہا = e->آغاز + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                اگر (اندراج_انتہا <= min || e->آغاز > max)
                        جاری;

                /* Trim the entry to fit within [min, max] */
                uid_t new_آغاز = MAX(e->آغاز, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_انتہا = اندراج_انتہا <= max ? اندراج_انتہا : max + 1;
                أكد(new_انتہا > new_آغاز);

                range->entries[t++] = (UIDRange_اندراج) {
                        .آغاز = new_آغاز,
                        .nr = new_انتہا - new_آغاز,
                };
        }

        range->n_entries = t;

        واپس 0;
}

صحیح uid_range_تقسیم(UIDRange *range, uid_t سائز) {
        أكد(range);
        أكد(سائز > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / سائز;

        اگر (n_new_entries == 0) {
                range->n_entries = 0;
                واپس 0;
        }

        اگر (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                واپس -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        برائے (size_t i = 0; i < range->n_entries; i++)
                اگر (range->entries[i].nr >= سائز)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        برائے (size_t i = n_src; i > 0; i--) {
                UIDRange_اندراج *e = range->entries + i - 1;
                غیرعلامتی n_parts = e->nr / سائز;

                برائے (غیرعلامتی j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_اندراج) {
                                .آغاز = e->آغاز + (j - 1) * سائز,
                                .nr = سائز,
                        };
        }

        range->n_entries = n_new_entries;

        واپس 0;
}

صحیح uid_range_copy(ثابت UIDRange *range, UIDRange **ret) {
        أكد(ret);

        اگر (!range) {
                *ret = تہی;
                واپس 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        اگر (!copy)
                واپس -ENOMEM;

        اگر (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_اندراج, range->entries, range->n_entries);
                اگر (!copy->entries)
                        واپس -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        واپس 0;
}

صحیح uid_range_remove(UIDRange *range, uid_t آغاز, uid_t سائز) {
        أكد(range);

        اگر (سائز == 0)
                واپس 0;

        uid_t انتہا = آغاز + سائز; /* one past the last UID to remove */

        برائے (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_اندراج *e = range->entries + i;
                uid_t اندراج_انتہا = e->آغاز + e->nr;

                /* No overlap */
                اگر (اندراج_انتہا <= آغاز || e->آغاز >= انتہا)
                        جاری;

                /* Check if this removal splits the entry into two parts */
                اگر (e->آغاز < آغاز && اندراج_انتہا > انتہا) {
                        /* Need to split: grow the array first */
                        اگر (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                واپس -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        اندراج_انتہا = e->آغاز + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * حجم(UIDRange_اندراج));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_اندراج) {
                                .آغاز = e->آغاز,
                                .nr = آغاز - e->آغاز,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_اندراج) {
                                .آغاز = انتہا,
                                .nr = اندراج_انتہا - انتہا,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        جاری;
                }

                /* Removal covers the entire entry */
                اگر (آغاز <= e->آغاز && انتہا >= اندراج_انتہا) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * حجم(UIDRange_اندراج));
                        range->n_entries--;
                        i--;
                        جاری;
                }

                /* Removal trims the start of the entry */
                اگر (آغاز <= e->آغاز && انتہا > e->آغاز) {
                        e->nr = اندراج_انتہا - انتہا;
                        e->آغاز = انتہا;
                        جاری;
                }

                /* Removal trims the end of the entry */
                اگر (آغاز < اندراج_انتہا && انتہا >= اندراج_انتہا) {
                        e->nr = آغاز - e->آغاز;
                        جاری;
                }
        }

        واپس 0;
}

صحیح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret) {
        أكد(uid_range_entries(outside) == uid_range_entries(inside));
        أكد(ret);

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

        برائے (size_t i = 0; i < uid_range_entries(outside); i++)
                أكد(outside->entries[i].nr == inside->entries[i].nr);

        برائے (size_t i = 0; i < uid_range_entries(outside); i++) {
                ثابت UIDRange_اندراج *e = outside->entries + i;

                اگر (uid < e->آغاز || uid >= e->آغاز + e->nr)
                        جاری;

                *ret = inside->entries[i].آغاز + uid - e->آغاز;
                واپس 0;
        }

        واپس -ESRCH;
}

صحیح uid_range_translate_userns_fd(صحیح userns_fd, UIDRange_Userns_منوال منوال, uid_t uid, uid_t *ret) {
        صحیح r;

        أكد(userns_fd >= 0);
        أكد(اندر_سیٹ(منوال, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = تہی;
        r = uid_range_بھرو_userns_by_fd_بھرا(userns_fd, منوال, /* coalesce= */ false, &outside_range);
        اگر (r < 0)
                واپس r;

        منوال = منوال == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = تہی;
        r = uid_range_بھرو_userns_by_fd_بھرا(userns_fd, منوال, /* coalesce= */ false, &inside_range);
        اگر (r < 0)
                واپس r;

        واپس uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b) {
        اگر (a == b)
                واپس true;

        اگر (!a || !b)
                واپس false;

        اگر (a->n_entries != b->n_entries)
                واپس false;

        برائے (size_t i = 0; i < a->n_entries; i++) {
                اگر (a->entries[i].آغاز != b->entries[i].آغاز)
                        واپس false;
                اگر (a->entries[i].nr != b->entries[i].nr)
                        واپس false;
        }

        واپس true;
}

صحیح uid_map_تلاش_جڑ(pid_t pid, UIDRange_Userns_منوال منوال, uid_t *ret) {
        صحیح r;

        أكد(pid_is_valid(pid));
        أكد(اندر_سیٹ(منوال, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ثابت حرف *p = procfs_file_alloca(pid, منوال == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        اگر (!f) {
                اگر (errno != ENOENT)
                        واپس -errno;

                r = proc_mounted();
                اگر (r < 0)
                        واپس -ENOENT; /* original error, if we can't determine /proc/ state */

                واپس r ? -ENOPKG : -ENOSYS;
        }

        برائے (;;) {
                uid_t uid_بنیاد = UID_INVALID, uid_سرکاؤ = UID_INVALID;

                r = uid_map_read_one(f, &uid_بنیاد, &uid_سرکاؤ, /* ret_range= */ تہی);
                اگر (r < 0)
                        واپس r;

                اگر (uid_بنیاد == 0) {
                        اگر (ret)
                                *ret = uid_سرکاؤ;
                        واپس 0;
                }
        }
}

uid_t uid_range_بنیاد(ثابت UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        اگر (uid_range_is_خالی_خانے(range))
                واپس UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                اگر (e->nr > 0)
                        واپس e->آغاز;

        واپس UID_INVALID;
}
