/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ getxattr_at_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ at_flags, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ getxattr_malloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ getxattr_at_malloc(AT_FDCWD, ਰਸਤਾ, name, AT_SYMLINK_FOLLOW, ret, ret_ਮਾਪ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ lgetxattr_malloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ getxattr_at_malloc(AT_FDCWD, ਰਸਤਾ, name, 0, ret, ret_ਮਾਪ);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fgetxattr_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ **ret, size_t *ret_ਮਾਪ) {
        ਵਾਪਸ getxattr_at_malloc(fd, ਨਿਰਾ, name, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, ret, ret_ਮਾਪ);
}

ਪੂਰਨ getxattr_at_bool(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ at_flags);
ਪੂਰਨ getxattr_at_strv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *name, ਪੂਰਨ at_flags, ਅੱਖਰ ***ret_strv);

ਪੂਰਨ listxattr_at_malloc(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅੱਖਰ **ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ listxattr_malloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਵਾਪਸ listxattr_at_malloc(AT_FDCWD, ਰਸਤਾ, AT_SYMLINK_FOLLOW, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ llistxattr_malloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਵਾਪਸ listxattr_at_malloc(AT_FDCWD, ਰਸਤਾ, 0, ret);
}
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ flistxattr_malloc(ਪੂਰਨ fd, ਅੱਖਰ **ret) {
        ਵਾਪਸ listxattr_at_malloc(fd, ਨਿਰਾ, AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, ret);
}

ਪੂਰਨ xsetxattr_ਭਰਿਆ(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                size_t ਮਾਪ,
                ਪੂਰਨ xattr_flags);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ xsetxattr(
                ਪੂਰਨ fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਪੂਰਨ at_flags,
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਵਾਪਸ xsetxattr_ਭਰਿਆ(fd, ਰਸਤਾ, at_flags, name, ਮੁੱਲ, SIZE_MAX, 0);
}

ਪੂਰਨ xsetxattr_strv(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅਚਲ ਅੱਖਰ *name, ਅੱਖਰ * ਅਚਲ *l);

ਪੂਰਨ xremovexattr(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, ਅਚਲ ਅੱਖਰ *name);

ਪੂਰਨ fd_setcrtime(ਪੂਰਨ fd, usec_t usec);
ਪੂਰਨ getcrtime_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ at_flags, usec_t *ret);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ fd_getcrtime(ਪੂਰਨ fd, usec_t *ret) {
        ਵਾਪਸ getcrtime_at(fd, ਨਿਰਾ, 0, ret);
}

bool xattr_is_acl(ਅਚਲ ਅੱਖਰ *name);
bool xattr_is_selinux(ਅਚਲ ਅੱਖਰ *name);
