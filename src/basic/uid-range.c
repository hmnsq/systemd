/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <sched.h>
#ضمن <string.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "format-util.h"
#ضمن "namespace-util.h"
#ضمن "path-util.h"
#ضمن "pidref.h"
#ضمن "process-util.h"
#ضمن "sort-util.h"
#ضمن "stat-util.h"
#ضمن "uid-range.h"
#ضمن "user-util.h"

UIDRange *uid_range_free(UIDRange *range) {
        إذا (!range)
                أرجع عدم;

        حرر(range->entries);
        أرجع mfree(range);
}

ساكن bool uid_range_مدخل_intersect(ثابت UIDRange_مدخل *a, ثابت UIDRange_مدخل *b) {
        أكد(a);
        أكد(b);

        أرجع a->شروع <= b->شروع + b->nr && a->شروع + a->nr >= b->شروع;
}

ساكن صحيح uid_range_مدخل_compare(ثابت UIDRange_مدخل *a, ثابت UIDRange_مدخل *b) {
        صحيح r;

        أكد(a);
        أكد(b);

        r = CMP(a->شروع, b->شروع);
        إذا (r != 0)
                أرجع r;

        أرجع CMP(a->nr, b->nr);
}

ساكن فراغ uid_range_coalesce(UIDRange *range) {
        أكد(range);

        إذا (range->n_entries <= 0)
                أرجع;

        typesafe_qsort(range->entries, range->n_entries, uid_range_مدخل_compare);

        لكل (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_مدخل *x = range->entries + i;

                لكل (size_t j = i + 1; j < range->n_entries; j++) {
                        UIDRange_مدخل *y = range->entries + j;
                        uid_t begin, انتها;

                        إذا (!uid_range_مدخل_intersect(x, y))
                                اكسر;

                        begin = MIN(x->شروع, y->شروع);

                        /* Silence static analyzers, overflow is prevented by uid_range_add_internal() */
                        أكد(x->شروع <= UINT32_MAX - x->nr);
                        أكد(y->شروع <= UINT32_MAX - y->nr);
                        انتها = MAX(x->شروع + x->nr, y->شروع + y->nr);

                        x->شروع = begin;
                        x->nr = انتها - begin;

                        إذا (range->n_entries > j + 1)
                                memmove(y, y + 1, حجم(UIDRange_مدخل) * (range->n_entries - j - 1));

                        /* Silence static analyzers, n_entries > 0 since j < n_entries holds in the loop condition */
                        أكد(range->n_entries > 0);
                        range->n_entries--;

                        /* Silence static analyzers, j cannot be 0 here since it starts at i + 1, i.e. >= 1 */
                        أكد(j > 0);
                        j--;
                }
        }
}

صحيح uid_range_جمع_کن_داخلی(UIDRange **range, uid_t شروع, uid_t nr, bool coalesce) {
        _cleanup_(uid_range_freep) UIDRange *range_new = عدم;
        UIDRange *p;

        أكد(range);

        إذا (nr <= 0)
                أرجع 0;

        إذا (شروع > UINT32_MAX - nr) /* overflow check */
                أرجع -ERANGE;

        إذا (*range)
                p = *range;
        وإلا {
                range_new = new0(UIDRange, 1);
                إذا (!range_new)
                        أرجع -ENOMEM;

                p = range_new;
        }

        إذا (!GREEDY_REALLOC(p->entries, p->n_entries + 1))
                أرجع -ENOMEM;

        p->entries[p->n_entries++] = (UIDRange_مدخل) {
                .شروع = شروع,
                .nr = nr,
        };

        إذا (coalesce)
                uid_range_coalesce(p);

        TAKE_PTR(range_new);
        *range = p;

        أرجع 0;
}

صحيح uid_range_جمع_کن_str_پر(UIDRange **range, ثابت حرف *s, bool coalesce) {
        uid_t شروع, انتها;
        صحيح r;

        أكد(range);
        أكد(s);

        r = parse_uid_range(s, &شروع, &انتها);
        إذا (r < 0)
                أرجع r;

        أرجع uid_range_جمع_کن_داخلی(range, شروع, انتها - شروع + 1, coalesce);
}

صحيح uid_range_بعدی_پایینی(ثابت UIDRange *range, uid_t *uid) {
        uid_t closest = UID_INVALID, candidate;

        أكد(range);
        أكد(uid);

        إذا (*uid == 0)
                أرجع -EBUSY;

        candidate = *uid - 1;

        لكل (size_t i = 0; i < range->n_entries; i++) {
                uid_t begin, انتها;

                begin = range->entries[i].شروع;
                انتها = range->entries[i].شروع + range->entries[i].nr - 1;

                إذا (candidate >= begin && candidate <= انتها) {
                        *uid = candidate;
                        أرجع 1;
                }

                إذا (انتها < candidate)
                        closest = انتها;
        }

        إذا (closest == UID_INVALID)
                أرجع -EBUSY;

        *uid = closest;
        أرجع 1;
}

bool uid_range_covers(ثابت UIDRange *range, uid_t شروع, uid_t nr) {
        إذا (nr == 0) /* empty range? always covered... */
                أرجع true;

        إذا (شروع > UINT32_MAX - nr) /* range overflows? definitely not covered... */
                أرجع false;

        إذا (!range)
                أرجع false;

        FOREACH_ARRAY(i, range->entries, range->n_entries)
                إذا (شروع >= i->شروع &&
                    شروع + nr <= i->شروع + i->nr)
                        أرجع true;

        أرجع false;
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_پایه, uid_t *ret_لغزش, uid_t *ret_range) {
        uid_t uid_پایه, uid_لغزش, uid_range;
        صحيح r;

        أكد(f);

        errno = 0;
        r = fscanf(f, UID_FMT " " UID_FMT " " UID_FMT "\n", &uid_پایه, &uid_لغزش, &uid_range);
        إذا (r == EOF)
                أرجع errno_or_else(ENOMSG);
        أكد(r >= 0);
        إذا (r != 3)
                أرجع -EBADMSG;
        إذا (uid_range <= 0)
                أرجع -EBADMSG;

        إذا (ret_پایه)
                *ret_پایه = uid_پایه;
        إذا (ret_لغزش)
                *ret_لغزش = uid_لغزش;
        إذا (ret_range)
                *ret_range = uid_range;

        أرجع 0;
}

بیعلامت uid_range_اندازه(ثابت UIDRange *range) {
        إذا (!range)
                أرجع 0;

        بیعلامت n = 0;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n += e->nr;

        أرجع n;
}

bool uid_range_is_خالی(ثابت UIDRange *range) {

        إذا (!range)
                أرجع true;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                إذا (e->nr > 0)
                        أرجع false;

        أرجع true;
}

صحيح uid_range_بارگیری_userns_پر(ثابت حرف *مسیر, UIDRange_Userns_نما نما, bool coalesce, UIDRange **ret) {
        _cleanup_(uid_range_freep) UIDRange *range = عدم;
        _cleanup_fclose_ FILE *f = عدم;
        صحيح r;

        /* If 'path' is NULL loads the UID range of the userns namespace we run. Otherwise load the data from
         * the specified file (which can be either uid_map or gid_map, in case caller needs to deal with GID
         * maps).
         *
         * To simplify things this will modify the passed array in case of later failure. */

        أكد(نما >= 0);
        أكد(نما < UID_RANGE_USERNS_نما_MAX);
        أكد(ret);

        إذا (!مسیر)
                مسیر = درون_مجموعه(نما, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "/proc/self/uid_map" : "/proc/self/gid_map";

        f = fopen(مسیر, "re");
        إذا (!f) {
                r = -errno;

                إذا (r == -ENOENT && مسیر_startswith(مسیر, "/proc/"))
                        أرجع proc_mounted() > 0 ? -EOPNOTSUPP : -ENOSYS;

                أرجع r;
        }

        range = new0(UIDRange, 1);
        إذا (!range)
                أرجع -ENOMEM;

        لكل (;;) {
                uid_t uid_پایه, uid_لغزش, uid_range;

                r = uid_map_read_one(f, &uid_پایه, &uid_لغزش, &uid_range);
                إذا (r == -ENOMSG)
                        اكسر;
                إذا (r < 0)
                        أرجع r;

                r = uid_range_جمع_کن_داخلی(
                                &range,
                                درون_مجموعه(نما, UID_RANGE_USERNS_INSIDE, GID_RANGE_USERNS_INSIDE) ? uid_پایه : uid_لغزش,
                                uid_range,
                                /* coalesce= */ false);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (coalesce)
                uid_range_coalesce(range);

        *ret = TAKE_PTR(range);
        أرجع 0;
}

صحيح uid_range_بارگیری_userns_by_fd_پر(صحيح userns_fd, UIDRange_Userns_نما نما, bool coalesce, UIDRange **ret) {
        _cleanup_(pidref_انجامشده_sigkill_wait) PidRef pidref = PIDREF_NULL;
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(نما >= 0);
        أكد(نما < UID_RANGE_USERNS_نما_MAX);
        أكد(ret);

        r = is_our_namespace(userns_fd, NAMESPACE_USER);
        إذا (r < 0)
                أرجع r;
        إذا (r > 0)
                أرجع uid_range_بارگیری_userns_پر(/* path= */ عدم, نما, coalesce, ret);

        r = userns_enter_and_pin(userns_fd, &pidref);
        إذا (r < 0)
                أرجع r;

        ثابت حرف *p = procfs_file_alloca(
                        pidref.pid,
                        درون_مجموعه(نما, UID_RANGE_USERNS_INSIDE, UID_RANGE_USERNS_OUTSIDE) ? "uid_map" : "gid_map");

        أرجع uid_range_بارگیری_userns_پر(p, نما, coalesce, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t شروع, uid_t nr) {

        إذا (!range)
                أرجع false;

        /* Avoid overflow */
        إذا (شروع > UINT32_MAX - nr)
                nr = UINT32_MAX - شروع;

        إذا (nr == 0)
                أرجع false;

        FOREACH_ARRAY(مدخل, range->entries, range->n_entries)
                إذا (شروع < مدخل->شروع + مدخل->nr &&
                    شروع + nr >= مدخل->شروع)
                        أرجع true;

        أرجع false;
}

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max) {
        أكد(range);

        إذا (min > max)
                أرجع -EINVAL;

        size_t t = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries) {
                uid_t مدخل_انتها = e->شروع + e->nr; /* one past the last UID in entry */

                /* Skip entries completely outside [min, max] */
                إذا (مدخل_انتها <= min || e->شروع > max)
                        استمر;

                /* Trim the entry to fit within [min, max] */
                uid_t new_شروع = MAX(e->شروع, min);
                /* entry_end is exclusive, avoid overflow when max == UINT32_MAX */
                uid_t new_انتها = مدخل_انتها <= max ? مدخل_انتها : max + 1;
                أكد(new_انتها > new_شروع);

                range->entries[t++] = (UIDRange_مدخل) {
                        .شروع = new_شروع,
                        .nr = new_انتها - new_شروع,
                };
        }

        range->n_entries = t;

        أرجع 0;
}

صحيح uid_range_افراز(UIDRange *range, uid_t اندازه) {
        أكد(range);
        أكد(اندازه > 0);

        /* Partitions the UID range entries into buckets of the given size. Any entry larger than the given
         * size will be partitioned into multiple entries, each of the given size. Any leftover UIDs in the
         * entry are dropped. Any entries smaller than the given size are also dropped. */

        /* Count how many entries we'll need after partitioning */
        size_t n_new_entries = 0;
        FOREACH_ARRAY(e, range->entries, range->n_entries)
                n_new_entries += e->nr / اندازه;

        إذا (n_new_entries == 0) {
                range->n_entries = 0;
                أرجع 0;
        }

        إذا (n_new_entries > range->n_entries && !GREEDY_REALLOC(range->entries, n_new_entries))
                أرجع -ENOMEM;

        /* Compact in place: drop entries that contribute zero partitions (nr < size). This forward pass
         * reads each entry once and only writes to lower-or-equal indices, so it cannot alias an unread
         * source entry. */
        size_t n_src = 0;
        لكل (size_t i = 0; i < range->n_entries; i++)
                إذا (range->entries[i].nr >= اندازه)
                        range->entries[n_src++] = range->entries[i];

        /* Pre-compaction guarantees every surviving entry contributes at least one partition slot, so the
         * write cursor t stays ahead of the read index. */
        size_t t = n_new_entries;
        لكل (size_t i = n_src; i > 0; i--) {
                UIDRange_مدخل *e = range->entries + i - 1;
                بیعلامت n_parts = e->nr / اندازه;

                لكل (بیعلامت j = n_parts; j > 0; j--)
                        range->entries[--t] = (UIDRange_مدخل) {
                                .شروع = e->شروع + (j - 1) * اندازه,
                                .nr = اندازه,
                        };
        }

        range->n_entries = n_new_entries;

        أرجع 0;
}

صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret) {
        أكد(ret);

        إذا (!range) {
                *ret = عدم;
                أرجع 0;
        }

        _cleanup_(uid_range_freep) UIDRange *copy = new0(UIDRange, 1);
        إذا (!copy)
                أرجع -ENOMEM;

        إذا (range->n_entries > 0) {
                copy->entries = newdup(UIDRange_مدخل, range->entries, range->n_entries);
                إذا (!copy->entries)
                        أرجع -ENOMEM;

                copy->n_entries = range->n_entries;
        }

        *ret = TAKE_PTR(copy);
        أرجع 0;
}

صحيح uid_range_remove(UIDRange *range, uid_t شروع, uid_t اندازه) {
        أكد(range);

        إذا (اندازه == 0)
                أرجع 0;

        uid_t انتها = شروع + اندازه; /* one past the last UID to remove */

        لكل (size_t i = 0; i < range->n_entries; i++) {
                UIDRange_مدخل *e = range->entries + i;
                uid_t مدخل_انتها = e->شروع + e->nr;

                /* No overlap */
                إذا (مدخل_انتها <= شروع || e->شروع >= انتها)
                        استمر;

                /* Check if this removal splits the entry into two parts */
                إذا (e->شروع < شروع && مدخل_انتها > انتها) {
                        /* Need to split: grow the array first */
                        إذا (!GREEDY_REALLOC(range->entries, range->n_entries + 1))
                                أرجع -ENOMEM;

                        /* Re-fetch pointer after potential realloc */
                        e = range->entries + i;
                        مدخل_انتها = e->شروع + e->nr;

                        /* Shift everything after this entry to make room */
                        memmove(range->entries + i + 2, range->entries + i + 1,
                                (range->n_entries - i - 1) * حجم(UIDRange_مدخل));
                        range->n_entries++;

                        /* First part: before the removed range */
                        range->entries[i] = (UIDRange_مدخل) {
                                .شروع = e->شروع,
                                .nr = شروع - e->شروع,
                        };

                        /* Second part: after the removed range */
                        range->entries[i + 1] = (UIDRange_مدخل) {
                                .شروع = انتها,
                                .nr = مدخل_انتها - انتها,
                        };

                        /* Skip the newly inserted entry */
                        i++;
                        استمر;
                }

                /* Removal covers the entire entry */
                إذا (شروع <= e->شروع && انتها >= مدخل_انتها) {
                        memmove(e, e + 1, (range->n_entries - i - 1) * حجم(UIDRange_مدخل));
                        range->n_entries--;
                        i--;
                        استمر;
                }

                /* Removal trims the start of the entry */
                إذا (شروع <= e->شروع && انتها > e->شروع) {
                        e->nr = مدخل_انتها - انتها;
                        e->شروع = انتها;
                        استمر;
                }

                /* Removal trims the end of the entry */
                إذا (شروع < مدخل_انتها && انتها >= مدخل_انتها) {
                        e->nr = شروع - e->شروع;
                        استمر;
                }
        }

        أرجع 0;
}

صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret) {
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

        لكل (size_t i = 0; i < uid_range_entries(outside); i++)
                أكد(outside->entries[i].nr == inside->entries[i].nr);

        لكل (size_t i = 0; i < uid_range_entries(outside); i++) {
                ثابت UIDRange_مدخل *e = outside->entries + i;

                إذا (uid < e->شروع || uid >= e->شروع + e->nr)
                        استمر;

                *ret = inside->entries[i].شروع + uid - e->شروع;
                أرجع 0;
        }

        أرجع -ESRCH;
}

صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRange_Userns_نما نما, uid_t uid, uid_t *ret) {
        صحيح r;

        أكد(userns_fd >= 0);
        أكد(درون_مجموعه(نما, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        _cleanup_(uid_range_freep) UIDRange *outside_range = عدم;
        r = uid_range_بارگیری_userns_by_fd_پر(userns_fd, نما, /* coalesce= */ false, &outside_range);
        إذا (r < 0)
                أرجع r;

        نما = نما == UID_RANGE_USERNS_OUTSIDE ? UID_RANGE_USERNS_INSIDE : GID_RANGE_USERNS_INSIDE;

        _cleanup_(uid_range_freep) UIDRange *inside_range = عدم;
        r = uid_range_بارگیری_userns_by_fd_پر(userns_fd, نما, /* coalesce= */ false, &inside_range);
        إذا (r < 0)
                أرجع r;

        أرجع uid_range_translate(outside_range, inside_range, uid, ret);
}

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b) {
        إذا (a == b)
                أرجع true;

        إذا (!a || !b)
                أرجع false;

        إذا (a->n_entries != b->n_entries)
                أرجع false;

        لكل (size_t i = 0; i < a->n_entries; i++) {
                إذا (a->entries[i].شروع != b->entries[i].شروع)
                        أرجع false;
                إذا (a->entries[i].nr != b->entries[i].nr)
                        أرجع false;
        }

        أرجع true;
}

صحيح uid_map_جستجو_ریشه(pid_t pid, UIDRange_Userns_نما نما, uid_t *ret) {
        صحيح r;

        أكد(pid_is_valid(pid));
        أكد(درون_مجموعه(نما, UID_RANGE_USERNS_OUTSIDE, GID_RANGE_USERNS_OUTSIDE));

        ثابت حرف *p = procfs_file_alloca(pid, نما == UID_RANGE_USERNS_OUTSIDE ? "uid_map" : "gid_map");
        _cleanup_fclose_ FILE *f = fopen(p, "re");
        إذا (!f) {
                إذا (errno != ENOENT)
                        أرجع -errno;

                r = proc_mounted();
                إذا (r < 0)
                        أرجع -ENOENT; /* original error, if we can't determine /proc/ state */

                أرجع r ? -ENOPKG : -ENOSYS;
        }

        لكل (;;) {
                uid_t uid_پایه = UID_INVALID, uid_لغزش = UID_INVALID;

                r = uid_map_read_one(f, &uid_پایه, &uid_لغزش, /* ret_range= */ عدم);
                إذا (r < 0)
                        أرجع r;

                إذا (uid_پایه == 0) {
                        إذا (ret)
                                *ret = uid_لغزش;
                        أرجع 0;
                }
        }
}

uid_t uid_range_پایه(ثابت UIDRange *range) {
        /* Returns the lowest UID in the range (notw that elements are sorted, hence we just need to look at
         * the first one that is populated. */

        إذا (uid_range_is_خالی(range))
                أرجع UID_INVALID;

        FOREACH_ARRAY(e, range->entries, range->n_entries)
                إذا (e->nr > 0)
                        أرجع e->شروع;

        أرجع UID_INVALID;
}
