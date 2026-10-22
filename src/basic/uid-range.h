/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف ساخت UIDRange_اندراج {
        uid_t آغاز, nr;
} UIDRange_اندراج;

قسمتعریف ساخت UIDRange {
        UIDRange_اندراج *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحیح uid_range_جوڑو_اندرونی(UIDRange **range, uid_t آغاز, uid_t nr, bool coalesce);
ساکن اندرونی صحیح uid_range_جوڑو(UIDRange **range, uid_t آغاز, uid_t nr) {
        واپس uid_range_جوڑو_اندرونی(range, آغاز, nr, true);
}
صحیح uid_range_جوڑو_str_بھرا(UIDRange **range, ثابت حرف *s, bool coalesce);
ساکن اندرونی صحیح uid_range_جوڑو_str(UIDRange **range, ثابت حرف *s) {
        واپس uid_range_جوڑو_str_بھرا(range, s, true);
}

صحیح uid_range_اگلا_زیریں(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t آغاز, uid_t nr);
ساکن اندرونی bool uid_range_شامل_ہے(ثابت UIDRange *range, uid_t uid) {
        واپس uid_range_covers(range, uid, 1);
}

صحیح uid_map_read_one(FILE *f, uid_t *ret_بنیاد, uid_t *ret_سرکاؤ, uid_t *ret_range);

ساکن اندرونی size_t uid_range_entries(ثابت UIDRange *range) {
        واپس range ? range->n_entries : 0;
}

غیرعلامتی uid_range_سائز(ثابت UIDRange *range) خالص;
bool uid_range_is_خالی_خانے(ثابت UIDRange *range) خالص;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

قسمتعریف شمار UIDRange_Userns_منوال {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_منوال_MAX,
        UID_RANGE_USERNS_منوال_INVALID = -EINVAL,
} UIDRange_Userns_منوال;

صحیح uid_range_بھرو_userns_بھرا(ثابت حرف *راستہ, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret);
ساکن اندرونی صحیح uid_range_بھرو_userns(ثابت حرف *راستہ, UIDRange_Userns_منوال منوال, UIDRange **ret) {
        واپس uid_range_بھرو_userns_بھرا(راستہ, منوال, true, ret);
}
صحیح uid_range_بھرو_userns_by_fd_بھرا(صحیح userns_fd, UIDRange_Userns_منوال منوال, bool coalesce, UIDRange **ret);
ساکن اندرونی صحیح uid_range_بھرو_userns_by_fd(صحیح userns_fd, UIDRange_Userns_منوال منوال, UIDRange **ret) {
        واپس uid_range_بھرو_userns_by_fd_بھرا(userns_fd, منوال, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t آغاز, uid_t nr);

صحیح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحیح uid_range_تقسیم(UIDRange *range, uid_t سائز);
صحیح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحیح uid_range_remove(UIDRange *range, uid_t آغاز, uid_t سائز);
صحیح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحیح uid_range_translate_userns_fd(صحیح userns_fd, UIDRange_Userns_منوال منوال, uid_t uid, uid_t *ret);

صحیح uid_map_تلاش_جڑ(pid_t pid, UIDRange_Userns_منوال منوال, uid_t *ret);

uid_t uid_range_بنیاد(ثابت UIDRange *range);
