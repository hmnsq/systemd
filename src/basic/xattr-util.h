/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা getxattr_at_malloc(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা at_flags, অক্ষর **ret, size_t *ret_আকার);
স্থির অন্তঃরেখ পূর্ণসংখ্যা getxattr_malloc(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, অক্ষর **ret, size_t *ret_আকার) {
        ফেরত getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_আকার);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা lgetxattr_malloc(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, অক্ষর **ret, size_t *ret_আকার) {
        ফেরত getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_আকার);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা fgetxattr_malloc(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *name, অক্ষর **ret, size_t *ret_আকার) {
        ফেরত getxattr_at_malloc(fd, ফাঁকা, name, AT_EMPTY_PATH, ret, ret_আকার);
}

পূর্ণসংখ্যা getxattr_at_bool(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা at_flags);
পূর্ণসংখ্যা getxattr_at_strv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *name, পূর্ণসংখ্যা at_flags, অক্ষর ***ret_strv);

পূর্ণসংখ্যা listxattr_at_malloc(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, অক্ষর **ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা listxattr_malloc(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        ফেরত listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা llistxattr_malloc(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        ফেরত listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
স্থির অন্তঃরেখ পূর্ণসংখ্যা flistxattr_malloc(পূর্ণসংখ্যা fd, অক্ষর **ret) {
        ফেরত listxattr_at_malloc(fd, ফাঁকা, AT_EMPTY_PATH, ret);
}

পূর্ণসংখ্যা xsetxattr_full(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *মান,
                size_t আকার_2,
                পূর্ণসংখ্যা xattr_flags);
স্থির অন্তঃরেখ পূর্ণসংখ্যা xsetxattr(
                পূর্ণসংখ্যা fd,
                ধ্রুবক অক্ষর *path,
                পূর্ণসংখ্যা at_flags,
                ধ্রুবক অক্ষর *name,
                ধ্রুবক অক্ষর *মান) {
        ফেরত xsetxattr_full(fd, path, at_flags, name, মান, SIZE_MAX, 0);
}

পূর্ণসংখ্যা xsetxattr_strv(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, ধ্রুবক অক্ষর *name, অক্ষর * ধ্রুবক *l);

পূর্ণসংখ্যা xremovexattr(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, ধ্রুবক অক্ষর *name);

পূর্ণসংখ্যা fd_setcrtime(পূর্ণসংখ্যা fd, usec_t usec);
পূর্ণসংখ্যা getcrtime_at(পূর্ণসংখ্যা fd, ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা at_flags, usec_t *ret);
স্থির অন্তঃরেখ পূর্ণসংখ্যা fd_getcrtime(পূর্ণসংখ্যা fd, usec_t *ret) {
        ফেরত getcrtime_at(fd, ফাঁকা, 0, ret);
}

bool xattr_is_acl(ধ্রুবক অক্ষর *name);
bool xattr_is_selinux(ধ্রুবক অক্ষর *name);
