/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

تعريفنوع بنية UIDRange_اندراج {
        uid_t شروع, nr;
} UIDRange_اندراج;

تعريفنوع بنية UIDRange {
        UIDRange_اندراج *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحيح uid_range_جوڑو_اندرونی(UIDRange **range, uid_t شروع, uid_t nr, bool coalesce);
ساكن مضمن صحيح uid_range_جوڑو(UIDRange **range, uid_t شروع, uid_t nr) {
        أرجع uid_range_جوڑو_اندرونی(range, شروع, nr, true);
}
صحيح uid_range_جوڑو_str_بھریا(UIDRange **range, ثابت حرف *s, bool coalesce);
ساكن مضمن صحيح uid_range_جوڑو_str(UIDRange **range, ثابت حرف *s) {
        أرجع uid_range_جوڑو_str_بھریا(range, s, true);
}

صحيح uid_range_اگلا_ہیٹھلا(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t شروع, uid_t nr);
ساكن مضمن bool uid_range_شامل_اے(ثابت UIDRange *range, uid_t uid) {
        أرجع uid_range_covers(range, uid, 1);
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_ادھار, uid_t *ret_کھسکاؤ, uid_t *ret_range);

ساكن مضمن size_t uid_range_entries(ثابت UIDRange *range) {
        أرجع range ? range->n_entries : 0;
}

بنانشانی uid_range_ناپ(ثابت UIDRange *range) خالص;
bool uid_range_is_خالی_تھاواں(ثابت UIDRange *range) خالص;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعريفنوع تعداد UIDRange_Userns_منوال {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_منوال_MAX,
        UID_RANGE_USERNS_منوال_INVALID = -EINVAL,
} UIDRange_Userns_منوال;

صحيح uid_range_لدو_userns_بھریا(ثابت حرف *رستہ, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_لدو_userns(ثابت حرف *رستہ, UIDRange_Userns_منوال منوال, UIDRange **ret) {
        أرجع uid_range_لدو_userns_بھریا(رستہ, منوال, true, ret);
}
صحيح uid_range_لدو_userns_by_fd_بھریا(صحيح userns_fd, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_لدو_userns_by_fd(صحيح userns_fd, UIDRange_Userns_منوال منوال, UIDRange **ret) {
        أرجع uid_range_لدو_userns_by_fd_بھریا(userns_fd, منوال, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t شروع, uid_t nr);

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحيح uid_range_حصے_کرو(UIDRange *range, uid_t ناپ);
صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحيح uid_range_remove(UIDRange *range, uid_t شروع, uid_t ناپ);
صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRange_Userns_منوال منوال, uid_t uid, uid_t *ret);

صحيح uid_map_تلاش_جڑ(pid_t pid, UIDRange_Userns_منوال منوال, uid_t *ret);

uid_t uid_range_ادھار(ثابت UIDRange *range);
