/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

صحیح getxattr_at_malloc(صحیح fd, ثابت حرف *راستہ, ثابت حرف *name, صحیح at_flags, حرف **ret, size_t *ret_سائز);
ساکن اندرونی صحیح getxattr_malloc(ثابت حرف *راستہ, ثابت حرف *name, حرف **ret, size_t *ret_سائز) {
        واپس getxattr_at_malloc(AT_FDCWD, راستہ, name, AT_SYMLINK_FOLLOW, ret, ret_سائز);
}
ساکن اندرونی صحیح lgetxattr_malloc(ثابت حرف *راستہ, ثابت حرف *name, حرف **ret, size_t *ret_سائز) {
        واپس getxattr_at_malloc(AT_FDCWD, راستہ, name, 0, ret, ret_سائز);
}
ساکن اندرونی صحیح fgetxattr_malloc(صحیح fd, ثابت حرف *name, حرف **ret, size_t *ret_سائز) {
        واپس getxattr_at_malloc(fd, تہی, name, AT_خالی_خانے_راستہ, ret, ret_سائز);
}

صحیح getxattr_at_bool(صحیح fd, ثابت حرف *راستہ, ثابت حرف *name, صحیح at_flags);
صحیح getxattr_at_strv(صحیح fd, ثابت حرف *راستہ, ثابت حرف *name, صحیح at_flags, حرف ***ret_strv);

صحیح listxattr_at_malloc(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, حرف **ret);
ساکن اندرونی صحیح listxattr_malloc(ثابت حرف *راستہ, حرف **ret) {
        واپس listxattr_at_malloc(AT_FDCWD, راستہ, AT_SYMLINK_FOLLOW, ret);
}
ساکن اندرونی صحیح llistxattr_malloc(ثابت حرف *راستہ, حرف **ret) {
        واپس listxattr_at_malloc(AT_FDCWD, راستہ, 0, ret);
}
ساکن اندرونی صحیح flistxattr_malloc(صحیح fd, حرف **ret) {
        واپس listxattr_at_malloc(fd, تہی, AT_خالی_خانے_راستہ, ret);
}

صحیح xsetxattr_بھرا(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                ثابت حرف *name,
                ثابت حرف *قدر,
                size_t سائز,
                صحیح xattr_flags);
ساکن اندرونی صحیح xsetxattr(
                صحیح fd,
                ثابت حرف *راستہ,
                صحیح at_flags,
                ثابت حرف *name,
                ثابت حرف *قدر) {
        واپس xsetxattr_بھرا(fd, راستہ, at_flags, name, قدر, SIZE_MAX, 0);
}

صحیح xsetxattr_strv(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, ثابت حرف *name, حرف * ثابت *l);

صحیح xremovexattr(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, ثابت حرف *name);

صحیح fd_setcrtime(صحیح fd, usec_t usec);
صحیح getcrtime_at(صحیح fd, ثابت حرف *راستہ, صحیح at_flags, usec_t *ret);
ساکن اندرونی صحیح fd_getcrtime(صحیح fd, usec_t *ret) {
        واپس getcrtime_at(fd, تہی, 0, ret);
}

bool xattr_is_acl(ثابت حرف *name);
bool xattr_is_selinux(ثابت حرف *name);
