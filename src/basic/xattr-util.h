/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક getxattr_at_malloc(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *name, પૂર્ણાંક at_flags, અક્ષર **ret, size_t *ret_size);
સ્થિર અંતઃરેખ પૂર્ણાંક getxattr_malloc(અચલ અક્ષર *path, અચલ અક્ષર *name, અક્ષર **ret, size_t *ret_size) {
        પાછા getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
સ્થિર અંતઃરેખ પૂર્ણાંક lgetxattr_malloc(અચલ અક્ષર *path, અચલ અક્ષર *name, અક્ષર **ret, size_t *ret_size) {
        પાછા getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
સ્થિર અંતઃરેખ પૂર્ણાંક fgetxattr_malloc(પૂર્ણાંક fd, અચલ અક્ષર *name, અક્ષર **ret, size_t *ret_size) {
        પાછા getxattr_at_malloc(fd, ખાલી, name, AT_EMPTY_PATH, ret, ret_size);
}

પૂર્ણાંક getxattr_at_bool(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *name, પૂર્ણાંક at_flags);
પૂર્ણાંક getxattr_at_strv(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *name, પૂર્ણાંક at_flags, અક્ષર ***ret_strv);

પૂર્ણાંક listxattr_at_malloc(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અક્ષર **ret);
સ્થિર અંતઃરેખ પૂર્ણાંક listxattr_malloc(અચલ અક્ષર *path, અક્ષર **ret) {
        પાછા listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક llistxattr_malloc(અચલ અક્ષર *path, અક્ષર **ret) {
        પાછા listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
સ્થિર અંતઃરેખ પૂર્ણાંક flistxattr_malloc(પૂર્ણાંક fd, અક્ષર **ret) {
        પાછા listxattr_at_malloc(fd, ખાલી, AT_EMPTY_PATH, ret);
}

પૂર્ણાંક xsetxattr_full(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                અચલ અક્ષર *name,
                અચલ અક્ષર *કિંમત,
                size_t size,
                પૂર્ણાંક xattr_flags);
સ્થિર અંતઃરેખ પૂર્ણાંક xsetxattr(
                પૂર્ણાંક fd,
                અચલ અક્ષર *path,
                પૂર્ણાંક at_flags,
                અચલ અક્ષર *name,
                અચલ અક્ષર *કિંમત) {
        પાછા xsetxattr_full(fd, path, at_flags, name, કિંમત, SIZE_MAX, 0);
}

પૂર્ણાંક xsetxattr_strv(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અચલ અક્ષર *name, અક્ષર * અચલ *l);

પૂર્ણાંક xremovexattr(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, અચલ અક્ષર *name);

પૂર્ણાંક fd_setcrtime(પૂર્ણાંક fd, usec_t usec);
પૂર્ણાંક getcrtime_at(પૂર્ણાંક fd, અચલ અક્ષર *path, પૂર્ણાંક at_flags, usec_t *ret);
સ્થિર અંતઃરેખ પૂર્ણાંક fd_getcrtime(પૂર્ણાંક fd, usec_t *ret) {
        પાછા getcrtime_at(fd, ખાલી, 0, ret);
}

bool xattr_is_acl(અચલ અક્ષર *name);
bool xattr_is_selinux(અચલ અક્ષર *name);
