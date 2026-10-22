/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

صحيح getxattr_at_malloc(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags, حرف **ret, size_t *ret_size);
ساكن مضمن صحيح getxattr_malloc(ثابت حرف *path, ثابت حرف *name, حرف **ret, size_t *ret_size) {
        أرجع getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
ساكن مضمن صحيح lgetxattr_malloc(ثابت حرف *path, ثابت حرف *name, حرف **ret, size_t *ret_size) {
        أرجع getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
ساكن مضمن صحيح fgetxattr_malloc(صحيح fd, ثابت حرف *name, حرف **ret, size_t *ret_size) {
        أرجع getxattr_at_malloc(fd, عدم, name, AT_EMPTY_PATH, ret, ret_size);
}

صحيح getxattr_at_bool(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags);
صحيح getxattr_at_strv(صحيح fd, ثابت حرف *path, ثابت حرف *name, صحيح at_flags, حرف ***ret_strv);

صحيح listxattr_at_malloc(صحيح fd, ثابت حرف *path, صحيح at_flags, حرف **ret);
ساكن مضمن صحيح listxattr_malloc(ثابت حرف *path, حرف **ret) {
        أرجع listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
ساكن مضمن صحيح llistxattr_malloc(ثابت حرف *path, حرف **ret) {
        أرجع listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
ساكن مضمن صحيح flistxattr_malloc(صحيح fd, حرف **ret) {
        أرجع listxattr_at_malloc(fd, عدم, AT_EMPTY_PATH, ret);
}

صحيح xsetxattr_full(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *درج,
                size_t size,
                صحيح xattr_flags);
ساكن مضمن صحيح xsetxattr(
                صحيح fd,
                ثابت حرف *path,
                صحيح at_flags,
                ثابت حرف *name,
                ثابت حرف *درج) {
        أرجع xsetxattr_full(fd, path, at_flags, name, درج, SIZE_MAX, 0);
}

صحيح xsetxattr_strv(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name, حرف * ثابت *l);

صحيح xremovexattr(صحيح fd, ثابت حرف *path, صحيح at_flags, ثابت حرف *name);

صحيح fd_setcrtime(صحيح fd, usec_t usec);
صحيح getcrtime_at(صحيح fd, ثابت حرف *path, صحيح at_flags, usec_t *ret);
ساكن مضمن صحيح fd_getcrtime(صحيح fd, usec_t *ret) {
        أرجع getcrtime_at(fd, عدم, 0, ret);
}

bool xattr_is_acl(ثابت حرف *name);
bool xattr_is_selinux(ثابت حرف *name);
