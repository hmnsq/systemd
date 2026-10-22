/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"

تعريفنوع بنية UIDRange_مدخل {
        uid_t شروع, nr;
} UIDRange_مدخل;

تعريفنوع بنية UIDRange {
        UIDRange_مدخل *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحيح uid_range_جمع_کن_داخلی(UIDRange **range, uid_t شروع, uid_t nr, bool coalesce);
ساكن مضمن صحيح uid_range_جمع_کن(UIDRange **range, uid_t شروع, uid_t nr) {
        أرجع uid_range_جمع_کن_داخلی(range, شروع, nr, true);
}
صحيح uid_range_جمع_کن_str_پر(UIDRange **range, ثابت حرف *s, bool coalesce);
ساكن مضمن صحيح uid_range_جمع_کن_str(UIDRange **range, ثابت حرف *s) {
        أرجع uid_range_جمع_کن_str_پر(range, s, true);
}

صحيح uid_range_بعدی_پایینی(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t شروع, uid_t nr);
ساكن مضمن bool uid_range_شامل_است(ثابت UIDRange *range, uid_t uid) {
        أرجع uid_range_covers(range, uid, 1);
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_پایه, uid_t *ret_لغزش, uid_t *ret_range);

ساكن مضمن size_t uid_range_entries(ثابت UIDRange *range) {
        أرجع range ? range->n_entries : 0;
}

بیعلامت uid_range_اندازه(ثابت UIDRange *range) خالص;
bool uid_range_is_خالی(ثابت UIDRange *range) خالص;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعريفنوع تعداد UIDRange_Userns_نما {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_نما_MAX,
        UID_RANGE_USERNS_نما_INVALID = -EINVAL,
} UIDRange_Userns_نما;

صحيح uid_range_بارگیری_userns_پر(ثابت حرف *مسیر, UIDRange_Userns_نما نما, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_بارگیری_userns(ثابت حرف *مسیر, UIDRange_Userns_نما نما, UIDRange **ret) {
        أرجع uid_range_بارگیری_userns_پر(مسیر, نما, true, ret);
}
صحيح uid_range_بارگیری_userns_by_fd_پر(صحيح userns_fd, UIDRange_Userns_نما نما, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_بارگیری_userns_by_fd(صحيح userns_fd, UIDRange_Userns_نما نما, UIDRange **ret) {
        أرجع uid_range_بارگیری_userns_by_fd_پر(userns_fd, نما, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t شروع, uid_t nr);

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحيح uid_range_افراز(UIDRange *range, uid_t اندازه);
صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحيح uid_range_remove(UIDRange *range, uid_t شروع, uid_t اندازه);
صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRange_Userns_نما نما, uid_t uid, uid_t *ret);

صحيح uid_map_جستجو_ریشه(pid_t pid, UIDRange_Userns_نما نما, uid_t *ret);

uid_t uid_range_پایه(ثابت UIDRange *range);
