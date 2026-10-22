/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

صحيح getxattr_at_malloc(صحيح fd, ثابت حرف *لار, ثابت حرف *name, صحيح at_flags, حرف **ret, size_t *ret_اندازه);
ساكن مضمن صحيح getxattr_malloc(ثابت حرف *لار, ثابت حرف *name, حرف **ret, size_t *ret_اندازه) {
        أرجع getxattr_at_malloc(AT_FDCWD, لار, name, AT_SYMLINK_FOLLOW, ret, ret_اندازه);
}
ساكن مضمن صحيح lgetxattr_malloc(ثابت حرف *لار, ثابت حرف *name, حرف **ret, size_t *ret_اندازه) {
        أرجع getxattr_at_malloc(AT_FDCWD, لار, name, 0, ret, ret_اندازه);
}
ساكن مضمن صحيح fgetxattr_malloc(صحيح fd, ثابت حرف *name, حرف **ret, size_t *ret_اندازه) {
        أرجع getxattr_at_malloc(fd, عدم, name, AT_تشه_لار, ret, ret_اندازه);
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *لار, ثابت حرف *name, صحيح at_flags);
صحيح getxattr_at_strv(صحيح fd, ثابت حرف *لار, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv);

صحيح listxattr_at_malloc(صحيح fd, ثابت حرف *لار, صحيح at_flags, حرف **ret);
ساكن مضمن صحيح listxattr_malloc(ثابت حرف *لار, حرف **ret) {
        أرجع listxattr_at_malloc(AT_FDCWD, لار, AT_SYMLINK_FOLLOW, ret);
}
ساكن مضمن صحيح llistxattr_malloc(ثابت حرف *لار, حرف **ret) {
        أرجع listxattr_at_malloc(AT_FDCWD, لار, 0, ret);
}
ساكن مضمن صحيح flistxattr_malloc(صحيح fd, حرف **ret) {
        أرجع listxattr_at_malloc(fd, عدم, AT_تشه_لار, ret);
}

صحيح xsetxattr_ډک(
                صحيح fd,
                ثابت حرف *لار,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *ارزښت,
                size_t اندازه,
                صحيح xattr_flags);
ساكن مضمن صحيح xsetxattr(
                صحيح fd,
                ثابت حرف *لار,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *ارزښت) {
        أرجع xsetxattr_ډک(fd, لار, at_flags, name, ارزښت, SIZE_MAX, 0);
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *لار, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l);

صحيح xremovexattr(صحيح fd, ثابت حرف *لار, صحيح at_flags, ثابت حرف *name);

صحيح fd_setcrtime(صحيح fd, usec_t usec);
صحيح getcrtime_at(صحيح fd, ثابت حرف *لار, صحيح at_flags, usec_t *ret);
ساكن مضمن صحيح fd_getcrtime(صحيح fd, usec_t *ret) {
        أرجع getcrtime_at(fd, عدم, 0, ret);
}

bool xattr_is_acl(ثابت حرف *name);
bool xattr_is_selinux(ثابت حرف *name);
