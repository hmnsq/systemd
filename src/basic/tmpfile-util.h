/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ fopen_temporary_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fopen_temporary(സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path) {
        തിരികെ fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

പൂർണ്ണസംഖ്യ fopen_temporary_child_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fopen_temporary_child(സ്ഥിരം അക്ഷരം *path, FILE **ret_file, അക്ഷരം **ret_path) {
        തിരികെ fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

പൂർണ്ണസംഖ്യ mkostemp_safe(അക്ഷരം *pattern);
പൂർണ്ണസംഖ്യ fmkostemp_safe(അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *mode, FILE**_f);

ശൂന്യം unlink_tempfilep(അക്ഷരം (*p)[]);

പൂർണ്ണസംഖ്യ tempfn_xxxxxx(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ tempfn_random(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ tempfn_random_child(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *extra, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ open_tmpfile_unlinkable(സ്ഥിരം അക്ഷരം *directory, പൂർണ്ണസംഖ്യ flags);
പൂർണ്ണസംഖ്യ open_tmpfile_linkable_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ open_tmpfile_linkable(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path) {
        തിരികെ open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
പൂർണ്ണസംഖ്യ fopen_tmpfile_linkable_at(പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path, FILE **ret_file);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ fopen_tmpfile_linkable(സ്ഥിരം അക്ഷരം *target, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret_path, FILE **ret_file) {
        തിരികെ fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

തരനിർവചനം എണ്ണൽ LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

പൂർണ്ണസംഖ്യ link_tmpfile_at(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ link_tmpfile(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags) {
        തിരികെ link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
പൂർണ്ണസംഖ്യ flink_tmpfile_at(FILE *f, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ flink_tmpfile(FILE *f, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *target, LinkTmpfileFlags flags) {
        തിരികെ flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

പൂർണ്ണസംഖ്യ mkdtemp_malloc(സ്ഥിരം അക്ഷരം *template, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ mkdtemp_open(സ്ഥിരം അക്ഷരം *template, പൂർണ്ണസംഖ്യ flags, അക്ഷരം **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
ഘടന cleanup_tmpfile_data {
        പൂർണ്ണസംഖ്യ *dir_fd;
        അക്ഷരം **filename;
};
ശൂന്യം cleanup_tmpfile_data_done(ഘടന cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) ഘടന cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
