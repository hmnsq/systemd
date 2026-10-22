/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل "forward.h"

صحیح getxattr_at_malloc(صحیح fd, ثابت نویسه *path, ثابت نویسه *name, صحیح at_flags, نویسه **ret, size_t *ret_size);
ایستا درونخطی صحیح getxattr_malloc(ثابت نویسه *path, ثابت نویسه *name, نویسه **ret, size_t *ret_size) {
        بازگشت getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
ایستا درونخطی صحیح lgetxattr_malloc(ثابت نویسه *path, ثابت نویسه *name, نویسه **ret, size_t *ret_size) {
        بازگشت getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
ایستا درونخطی صحیح fgetxattr_malloc(صحیح fd, ثابت نویسه *name, نویسه **ret, size_t *ret_size) {
        بازگشت getxattr_at_malloc(fd, پوچ, name, AT_EMPTY_PATH, ret, ret_size);
}

صحیح getxattr_at_bool(صحیح fd, ثابت نویسه *path, ثابت نویسه *name, صحیح at_flags);
صحیح getxattr_at_strv(صحیح fd, ثابت نویسه *path, ثابت نویسه *name, صحیح at_flags, نویسه ***ret_strv);

صحیح listxattr_at_malloc(صحیح fd, ثابت نویسه *path, صحیح at_flags, نویسه **ret);
ایستا درونخطی صحیح listxattr_malloc(ثابت نویسه *path, نویسه **ret) {
        بازگشت listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
ایستا درونخطی صحیح llistxattr_malloc(ثابت نویسه *path, نویسه **ret) {
        بازگشت listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
ایستا درونخطی صحیح flistxattr_malloc(صحیح fd, نویسه **ret) {
        بازگشت listxattr_at_malloc(fd, پوچ, AT_EMPTY_PATH, ret);
}

صحیح xsetxattr_full(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                ثابت نویسه *name,
                ثابت نویسه *مقدار,
                size_t size,
                صحیح xattr_flags);
ایستا درونخطی صحیح xsetxattr(
                صحیح fd,
                ثابت نویسه *path,
                صحیح at_flags,
                ثابت نویسه *name,
                ثابت نویسه *مقدار) {
        بازگشت xsetxattr_full(fd, path, at_flags, name, مقدار, SIZE_MAX, 0);
}

صحیح xsetxattr_strv(صحیح fd, ثابت نویسه *path, صحیح at_flags, ثابت نویسه *name, نویسه * ثابت *l);

صحیح xremovexattr(صحیح fd, ثابت نویسه *path, صحیح at_flags, ثابت نویسه *name);

صحیح fd_setcrtime(صحیح fd, usec_t usec);
صحیح getcrtime_at(صحیح fd, ثابت نویسه *path, صحیح at_flags, usec_t *ret);
ایستا درونخطی صحیح fd_getcrtime(صحیح fd, usec_t *ret) {
        بازگشت getcrtime_at(fd, پوچ, 0, ret);
}

bool xattr_is_acl(ثابت نویسه *name);
bool xattr_is_selinux(ثابت نویسه *name);
