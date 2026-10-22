/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/stat.h>

#include "forward.h"

typedef ga_nanaa ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_ROOT         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

typedef sa_nracanaa ConfFile {
        var_na *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        var_na *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        var_na *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        var_na *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        ຈຳນວນເຕັມ fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        sa_nracanaa stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
void conf_file_free_array(ConfFile **array, size_t n);

ຈຳນວນເຕັມ conf_file_new_at(const var_na *path, const var_na *root, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, ConfFile **ret);
ຈຳນວນເຕັມ conf_file_new(const var_na *path, const var_na *root, ConfFilesFlags flags, ConfFile **ret);

ຈຳນວນເຕັມ conf_files_list(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir);
ຈຳນວນເຕັມ conf_files_list_at(var_na ***ret, const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na *dir);
ຈຳນວນເຕັມ conf_files_list_strv(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na* const* dirs);
ຈຳນວນເຕັມ conf_files_list_strv_at(var_na ***ret, const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na * const *dirs);
ຈຳນວນເຕັມ conf_files_list_nulstr(var_na ***ret, const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs);
ຈຳນວນເຕັມ conf_files_list_nulstr_at(var_na ***ret, const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na *dirs);

ຈຳນວນເຕັມ conf_files_list_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files);
ຈຳນວນເຕັມ conf_files_list_at_full(const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na *dir, ConfFile ***ret_files, size_t *ret_n_files);
ຈຳນວນເຕັມ conf_files_list_strv_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na * const *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ຈຳນວນເຕັມ conf_files_list_strv_at_full(const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na * const *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ຈຳນວນເຕັມ conf_files_list_nulstr_full(const var_na *suffix, const var_na *root, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ຈຳນວນເຕັມ conf_files_list_nulstr_at_full(const var_na *suffix, ຈຳນວນເຕັມ rfd, ConfFilesFlags flags, const var_na *dirs, ConfFile ***ret_files, size_t *ret_n_files);

ຈຳນວນເຕັມ conf_files_list_with_replacement(
                const var_na *root,
                var_na **config_dirs,
                const var_na *replacement,
                var_na ***ret_files,
                var_na **ret_inserted);
ຈຳນວນເຕັມ conf_files_list_dropins(
                var_na ***ret,
                const var_na *dropin_dirname,
                const var_na *root,
                ຈຳນວນເຕັມ root_fd,
                ConfFilesFlags flags,
                const var_na * const *dirs);

typedef ຈຳນວນເຕັມ parse_line_t(
                const var_na *fname,
                unsigned line,
                const var_na *buffer,
                bool *invalid_config,
                void *userdata);

ຈຳນວນເຕັມ conf_file_read(
                const var_na *root,
                const var_na **config_dirs,
                const var_na *fn,
                parse_line_t parse_line,
                void *userdata,
                bool ignore_enoent,
                bool *invalid_config);
