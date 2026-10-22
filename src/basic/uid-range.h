/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

تعريفنوع بنية UIDRange_ثبت {
        uid_t پیل, nr;
} UIDRange_ثبت;

تعريفنوع بنية UIDRange {
        UIDRange_ثبت *entries;
        size_t n_entries;
} UIDRange;

UIDRange* uid_range_free(UIDRange *range);
DEFINE_TRIVIAL_CLEANUP_FUNC(UIDRange*, uid_range_free);

صحيح uid_range_جمع_کړه_دننی(UIDRange **range, uid_t پیل, uid_t nr, bool coalesce);
ساكن مضمن صحيح uid_range_جمع_کړه(UIDRange **range, uid_t پیل, uid_t nr) {
        أرجع uid_range_جمع_کړه_دننی(range, پیل, nr, true);
}
صحيح uid_range_جمع_کړه_str_ډک(UIDRange **range, ثابت حرف *s, bool coalesce);
ساكن مضمن صحيح uid_range_جمع_کړه_str(UIDRange **range, ثابت حرف *s) {
        أرجع uid_range_جمع_کړه_str_ډک(range, s, true);
}

صحيح uid_range_بل_ښکتنی(ثابت UIDRange *range, uid_t *uid);

bool uid_range_covers(ثابت UIDRange *range, uid_t پیل, uid_t nr);
ساكن مضمن bool uid_range_لري(ثابت UIDRange *range, uid_t uid) {
        أرجع uid_range_covers(range, uid, 1);
}

صحيح uid_map_read_one(FILE *f, uid_t *ret_بنسټ, uid_t *ret_ښويدنه, uid_t *ret_range);

ساكن مضمن size_t uid_range_entries(ثابت UIDRange *range) {
        أرجع range ? range->n_entries : 0;
}

بینښه uid_range_اندازه(ثابت UIDRange *range) خالص;
bool uid_range_is_تشه(ثابت UIDRange *range) خالص;

bool uid_range_equal(ثابت UIDRange *a, ثابت UIDRange *b);

تعريفنوع تعداد UIDRange_Userns_ډېرپېښ {
        UID_RANGE_USERNS_INSIDE,
        UID_RANGE_USERNS_OUTSIDE,
        GID_RANGE_USERNS_INSIDE,
        GID_RANGE_USERNS_OUTSIDE,
        UID_RANGE_USERNS_ډېرپېښ_MAX,
        UID_RANGE_USERNS_ډېرپېښ_INVALID = -EINVAL,
} UIDRange_Userns_ډېرپېښ;

صحيح uid_range_بارول_userns_ډک(ثابت حرف *لار, UIDRange_Userns_ډېرپېښ ډېرپېښ, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_بارول_userns(ثابت حرف *لار, UIDRange_Userns_ډېرپېښ ډېرپېښ, UIDRange **ret) {
        أرجع uid_range_بارول_userns_ډک(لار, ډېرپېښ, true, ret);
}
صحيح uid_range_بارول_userns_by_fd_ډک(صحيح userns_fd, UIDRange_Userns_ډېرپېښ ډېرپېښ, bool coalesce, UIDRange **ret);
ساكن مضمن صحيح uid_range_بارول_userns_by_fd(صحيح userns_fd, UIDRange_Userns_ډېرپېښ ډېرپېښ, UIDRange **ret) {
        أرجع uid_range_بارول_userns_by_fd_ډک(userns_fd, ډېرپېښ, true, ret);
}

bool uid_range_overlaps(ثابت UIDRange *range, uid_t پیل, uid_t nr);

صحيح uid_range_clip(UIDRange *range, uid_t min, uid_t max);
صحيح uid_range_برخه_کول(UIDRange *range, uid_t اندازه);
صحيح uid_range_copy(ثابت UIDRange *range, UIDRange **ret);
صحيح uid_range_remove(UIDRange *range, uid_t پیل, uid_t اندازه);
صحيح uid_range_translate(ثابت UIDRange *outside, ثابت UIDRange *inside, uid_t uid, uid_t *ret);
صحيح uid_range_translate_userns_fd(صحيح userns_fd, UIDRange_Userns_ډېرپېښ ډېرپېښ, uid_t uid, uid_t *ret);

صحيح uid_map_پلټنه_ریښه(pid_t pid, UIDRange_Userns_ډېرپېښ ډېرپېښ, uid_t *ret);

uid_t uid_range_بنسټ(ثابت UIDRange *range);
