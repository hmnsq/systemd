/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક fopen_temporary_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path);
સ્થિર અંતઃરેખ પૂર્ણાંક fopen_temporary(અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path) {
        પાછા fopen_temporary_at(AT_FDCWD, path, ret_file, ret_path);
}

પૂર્ણાંક fopen_temporary_child_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path);
સ્થિર અંતઃરેખ પૂર્ણાંક fopen_temporary_child(અચલ અક્ષર *path, FILE **ret_file, અક્ષર **ret_path) {
        પાછા fopen_temporary_child_at(AT_FDCWD, path, ret_file, ret_path);
}

પૂર્ણાંક mkostemp_safe(અક્ષર *pattern);
પૂર્ણાંક fmkostemp_safe(અક્ષર *pattern, અચલ અક્ષર *mode, FILE**_f);

શૂન્ય unlink_tempfilep(અક્ષર (*p)[]);

પૂર્ણાંક tempfn_xxxxxx(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret);
પૂર્ણાંક tempfn_random(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret);
પૂર્ણાંક tempfn_random_child(અચલ અક્ષર *p, અચલ અક્ષર *extra, અક્ષર **ret);

પૂર્ણાંક open_tmpfile_unlinkable(અચલ અક્ષર *directory, પૂર્ણાંક flags);
પૂર્ણાંક open_tmpfile_linkable_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path);
સ્થિર અંતઃરેખ પૂર્ણાંક open_tmpfile_linkable(અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path) {
        પાછા open_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path);
}
પૂર્ણાંક fopen_tmpfile_linkable_at(પૂર્ણાંક dir_fd, અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path, FILE **ret_file);
સ્થિર અંતઃરેખ પૂર્ણાંક fopen_tmpfile_linkable(અચલ અક્ષર *target, પૂર્ણાંક flags, અક્ષર **ret_path, FILE **ret_file) {
        પાછા fopen_tmpfile_linkable_at(AT_FDCWD, target, flags, ret_path, ret_file);
}

પ્રકારવ્યાખ્યા ગણના LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

પૂર્ણાંક link_tmpfile_at(પૂર્ણાંક fd, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags);
સ્થિર અંતઃરેખ પૂર્ણાંક link_tmpfile(પૂર્ણાંક fd, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags) {
        પાછા link_tmpfile_at(fd, AT_FDCWD, path, target, flags);
}
પૂર્ણાંક flink_tmpfile_at(FILE *f, પૂર્ણાંક dir_fd, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags);
સ્થિર અંતઃરેખ પૂર્ણાંક flink_tmpfile(FILE *f, અચલ અક્ષર *path, અચલ અક્ષર *target, LinkTmpfileFlags flags) {
        પાછા flink_tmpfile_at(f, AT_FDCWD, path, target, flags);
}

પૂર્ણાંક mkdtemp_malloc(અચલ અક્ષર *template, અક્ષર **ret);
પૂર્ણાંક mkdtemp_open(અચલ અક્ષર *template, પૂર્ણાંક flags, અક્ષર **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
સંરચના cleanup_tmpfile_data {
        પૂર્ણાંક *dir_fd;
        અક્ષર **filename;
};
શૂન્ય cleanup_tmpfile_data_done(સંરચના cleanup_tmpfile_data *d);
#define _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        _unused_ _cleanup_(cleanup_tmpfile_data_done) સંરચના cleanup_tmpfile_data UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#define CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
