/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

முழு getxattr_at_malloc(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, முழு at_flags, எழுத்து **ret, size_t *ret_size);
நிலையான உள்வரி முழு getxattr_malloc(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, எழுத்து **ret, size_t *ret_size) {
        திரும்பு getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
நிலையான உள்வரி முழு lgetxattr_malloc(மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, எழுத்து **ret, size_t *ret_size) {
        திரும்பு getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
நிலையான உள்வரி முழு fgetxattr_malloc(முழு fd, மாறிலி எழுத்து *name, எழுத்து **ret, size_t *ret_size) {
        திரும்பு getxattr_at_malloc(fd, வெற்று, name, AT_EMPTY_PATH, ret, ret_size);
}

முழு getxattr_at_bool(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, முழு at_flags);
முழு getxattr_at_strv(முழு fd, மாறிலி எழுத்து *path, மாறிலி எழுத்து *name, முழு at_flags, எழுத்து ***ret_strv);

முழு listxattr_at_malloc(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, எழுத்து **ret);
நிலையான உள்வரி முழு listxattr_malloc(மாறிலி எழுத்து *path, எழுத்து **ret) {
        திரும்பு listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
நிலையான உள்வரி முழு llistxattr_malloc(மாறிலி எழுத்து *path, எழுத்து **ret) {
        திரும்பு listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
நிலையான உள்வரி முழு flistxattr_malloc(முழு fd, எழுத்து **ret) {
        திரும்பு listxattr_at_malloc(fd, வெற்று, AT_EMPTY_PATH, ret);
}

முழு xsetxattr_full(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *மதிப்பு,
                size_t size,
                முழு xattr_flags);
நிலையான உள்வரி முழு xsetxattr(
                முழு fd,
                மாறிலி எழுத்து *path,
                முழு at_flags,
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *மதிப்பு) {
        திரும்பு xsetxattr_full(fd, path, at_flags, name, மதிப்பு, SIZE_MAX, 0);
}

முழு xsetxattr_strv(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, மாறிலி எழுத்து *name, எழுத்து * மாறிலி *l);

முழு xremovexattr(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, மாறிலி எழுத்து *name);

முழு fd_setcrtime(முழு fd, usec_t usec);
முழு getcrtime_at(முழு fd, மாறிலி எழுத்து *path, முழு at_flags, usec_t *ret);
நிலையான உள்வரி முழு fd_getcrtime(முழு fd, usec_t *ret) {
        திரும்பு getcrtime_at(fd, வெற்று, 0, ret);
}

bool xattr_is_acl(மாறிலி எழுத்து *name);
bool xattr_is_selinux(மாறிலி எழுத்து *name);
