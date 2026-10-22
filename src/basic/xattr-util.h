/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণাঙ্ক getxattr_at_malloc(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক at_flags, বর্ণ **ret, size_t *ret_size);
স্থির অন্তঃস্থ পূর্ণাঙ্ক getxattr_malloc(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, বর্ণ **ret, size_t *ret_size) {
        প্রত্যাগম getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক lgetxattr_malloc(ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, বর্ণ **ret, size_t *ret_size) {
        প্রত্যাগম getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক fgetxattr_malloc(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *name, বর্ণ **ret, size_t *ret_size) {
        প্রত্যাগম getxattr_at_malloc(fd, রিক্ত, name, AT_EMPTY_PATH, ret, ret_size);
}

পূর্ণাঙ্ক getxattr_at_bool(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক at_flags);
পূর্ণাঙ্ক getxattr_at_strv(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *name, পূর্ণাঙ্ক at_flags, বর্ণ ***ret_strv);

পূর্ণাঙ্ক listxattr_at_malloc(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, বর্ণ **ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক listxattr_malloc(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        প্রত্যাগম listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক llistxattr_malloc(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        প্রত্যাগম listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
স্থির অন্তঃস্থ পূর্ণাঙ্ক flistxattr_malloc(পূর্ণাঙ্ক fd, বর্ণ **ret) {
        প্রত্যাগম listxattr_at_malloc(fd, রিক্ত, AT_EMPTY_PATH, ret);
}

পূর্ণাঙ্ক xsetxattr_full(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                ধ্রুব বর্ণ *name,
                ধ্রুব বর্ণ *মান,
                size_t size,
                পূর্ণাঙ্ক xattr_flags);
স্থির অন্তঃস্থ পূর্ণাঙ্ক xsetxattr(
                পূর্ণাঙ্ক fd,
                ধ্রুব বর্ণ *path,
                পূর্ণাঙ্ক at_flags,
                ধ্রুব বর্ণ *name,
                ধ্রুব বর্ণ *মান) {
        প্রত্যাগম xsetxattr_full(fd, path, at_flags, name, মান, SIZE_MAX, 0);
}

পূর্ণাঙ্ক xsetxattr_strv(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, ধ্রুব বর্ণ *name, বর্ণ * ধ্রুব *l);

পূর্ণাঙ্ক xremovexattr(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, ধ্রুব বর্ণ *name);

পূর্ণাঙ্ক fd_setcrtime(পূর্ণাঙ্ক fd, usec_t usec);
পূর্ণাঙ্ক getcrtime_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, পূর্ণাঙ্ক at_flags, usec_t *ret);
স্থির অন্তঃস্থ পূর্ণাঙ্ক fd_getcrtime(পূর্ণাঙ্ক fd, usec_t *ret) {
        প্রত্যাগম getcrtime_at(fd, রিক্ত, 0, ret);
}

bool xattr_is_acl(ধ্রুব বর্ণ *name);
bool xattr_is_selinux(ধ্রুব বর্ণ *name);
