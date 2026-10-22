/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅକ୍ଷର **ret, size_t *ret_size);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_malloc(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret, size_t *ret_size) {
        ଫେରନ୍ତୁ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା lgetxattr_malloc(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret, size_t *ret_size) {
        ଫେରନ୍ତୁ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fgetxattr_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret, size_t *ret_size) {
        ଫେରନ୍ତୁ getxattr_at_malloc(fd, ଖାଲି, name, AT_EMPTY_PATH, ret, ret_size);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_bool(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getxattr_at_strv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅକ୍ଷର ***ret_strv);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା listxattr_at_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅକ୍ଷର **ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା listxattr_malloc(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା llistxattr_malloc(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା flistxattr_malloc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅକ୍ଷର **ret) {
        ଫେରନ୍ତୁ listxattr_at_malloc(fd, ଖାଲି, AT_EMPTY_PATH, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xsetxattr_full(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                size_t size,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା xattr_flags);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା xsetxattr(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd,
                ଅଚଳ ଅକ୍ଷର *path,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags,
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ଫେରନ୍ତୁ xsetxattr_full(fd, path, at_flags, name, ମୂଲ୍ୟ, SIZE_MAX, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xsetxattr_strv(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର * ଅଚଳ *l);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା xremovexattr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, ଅଚଳ ଅକ୍ଷର *name);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_setcrtime(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, usec_t usec);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା getcrtime_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_flags, usec_t *ret);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_getcrtime(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, usec_t *ret) {
        ଫେରନ୍ତୁ getcrtime_at(fd, ଖାଲି, 0, ret);
}

bool xattr_is_acl(ଅଚଳ ଅକ୍ଷର *name);
bool xattr_is_selinux(ଅଚଳ ଅକ୍ଷର *name);
