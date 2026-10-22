/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ getxattr_at_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ at_flags, അക്ഷരം **ret, size_t *ret_size);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ getxattr_malloc(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret, size_t *ret_size) {
        തിരികെ getxattr_at_malloc(AT_FDCWD, path, name, AT_SYMLINK_FOLLOW, ret, ret_size);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ lgetxattr_malloc(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret, size_t *ret_size) {
        തിരികെ getxattr_at_malloc(AT_FDCWD, path, name, 0, ret, ret_size);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fgetxattr_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret, size_t *ret_size) {
        തിരികെ getxattr_at_malloc(fd, ശൂന്യ, name, AT_EMPTY_PATH, ret, ret_size);
}

പൂർണ്ണസംഖ്യ getxattr_at_bool(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ at_flags);
പൂർണ്ണസംഖ്യ getxattr_at_strv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *name, പൂർണ്ണസംഖ്യ at_flags, അക്ഷരം ***ret_strv);

പൂർണ്ണസംഖ്യ listxattr_at_malloc(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, അക്ഷരം **ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ listxattr_malloc(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        തിരികെ listxattr_at_malloc(AT_FDCWD, path, AT_SYMLINK_FOLLOW, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ llistxattr_malloc(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        തിരികെ listxattr_at_malloc(AT_FDCWD, path, 0, ret);
}
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ flistxattr_malloc(പൂർണ്ണസംഖ്യ fd, അക്ഷരം **ret) {
        തിരികെ listxattr_at_malloc(fd, ശൂന്യ, AT_EMPTY_PATH, ret);
}

പൂർണ്ണസംഖ്യ xsetxattr_full(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *മൂല്യം,
                size_t size,
                പൂർണ്ണസംഖ്യ xattr_flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ xsetxattr(
                പൂർണ്ണസംഖ്യ fd,
                സ്ഥിരം അക്ഷരം *path,
                പൂർണ്ണസംഖ്യ at_flags,
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *മൂല്യം) {
        തിരികെ xsetxattr_full(fd, path, at_flags, name, മൂല്യം, SIZE_MAX, 0);
}

പൂർണ്ണസംഖ്യ xsetxattr_strv(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, സ്ഥിരം അക്ഷരം *name, അക്ഷരം * സ്ഥിരം *l);

പൂർണ്ണസംഖ്യ xremovexattr(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, സ്ഥിരം അക്ഷരം *name);

പൂർണ്ണസംഖ്യ fd_setcrtime(പൂർണ്ണസംഖ്യ fd, usec_t usec);
പൂർണ്ണസംഖ്യ getcrtime_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ at_flags, usec_t *ret);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fd_getcrtime(പൂർണ്ണസംഖ്യ fd, usec_t *ret) {
        തിരികെ getcrtime_at(fd, ശൂന്യ, 0, ret);
}

bool xattr_is_acl(സ്ഥിരം അക്ഷരം *name);
bool xattr_is_selinux(സ്ഥിരം അക്ഷരം *name);
