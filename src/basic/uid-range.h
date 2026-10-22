/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع بنية UIDRange_داخلا {
        uid_t شروعات, nr;
} UIDRange_داخلا;

تعريفنوع بنية UIDRange {
        UIDRange_داخلا *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحيح uid_range_جوڙيو_اندروني(UIDRange **range, uid_t شروعات, uid_t nr, bool coalesce);
ساكن مضمن صحيح uid_range_جوڙيو(UIDRange **range, uid_t شروعات, uid_t nr) {
        أرجع uid_range_جوڙيو_اندروني(range, شروعات, nr, true);
}
صحيح uid_range_جوڙيو_str_ڀريل(UIDRange **range, ثابت حرف *s, bool coalesce);
ساكن مضمن صحيح uid_range_جوڙيو_str(UIDRange **range, ثابت حرف *s) {
        أرجع uid_range_جوڙيو_str_ڀريل(range, s, true);
}

صحيح uid_range_ايندڙ_هيٺاهون(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t شروعات, uid_t nr);
ساكن مضمن bool uid_range_شامل_آهي(ثابت UIDRange *range, uid_t uid) {
        أرجع uid_range_covers(range, uid, 1);
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_بنياد, uid_t *ret_سرڪايو, uid_t *ret_range);

ساكن مضمن size_t uid_range_entries(ثابت UIDRange *range) {
        أرجع range ? range->n_entries : 0;
}

بنانشان uid_range_سائيز(ثابت UIDRange *range) خالص;
bool uid_range_is_خالي_جايون(ثابت UIDRange *range) خالص;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعريفنوع تعداد UIDRange_Userns_گهڻائي {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_گهڻائي_MAX,
        UID_RANGE_USERNS_گهڻائي_INVALID = -EINVAL,
} UIDRange_Userns_گهڻائي;

صحيح uid_range_ڀريو_userns_ڀريل(ثابت حرف *رستو, UIDRange_Userns_گهڻائي گهڻائي, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_ڀريو_userns(ثابت حرف *رستو, UIDRange_Userns_گهڻائي گهڻائي, UIDRange **ret) {
        أرجع uid_range_ڀريو_userns_ڀريل(رستو, گهڻائي, true, ret);
}
صحيح uid_range_ڀريو_userns_by_fd_ڀريل(صحيح userns_fd, UIDRange_Userns_گهڻائي گهڻائي, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_ڀريو_userns_by_fd(صحيح userns_fd, UIDRange_Userns_گهڻائي گهڻائي, UIDRange **ret) {
        أرجع uid_range_ڀريو_userns_by_fd_ڀريل(userns_fd, گهڻائي, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t شروعات, uid_t nr);

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحيح uid_range_حصا_ڪريو(UIDRange *range, uid_t سائيز);
صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحيح uid_range_remove(UIDRange *range, uid_t شروعات, uid_t سائيز);
صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRange_Userns_گهڻائي گهڻائي, uid_t uid, uid_t *ret);

صحيح uid_map_ڳولا_پاڙ(pid_t pid, UIDRange_Userns_گهڻائي گهڻائي, uid_t *ret);

uid_t uid_range_بنياد(ثابت UIDRange *range);
