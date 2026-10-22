/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/stat.h>

#ضمن "forward.h"

تعريفنوع تعداد ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_يىلتىز         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

تعريفنوع بنية ConfFile {
        حرف *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        حرف *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        حرف *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        حرف *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        صحيح fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        بنية stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
فراغ conf_file_free_array(ConfFile **array, size_t n);

صحيح conf_file_new_at(ثابت حرف *path, ثابت حرف *يىلتىز, صحيح rfd, ConfFilesFlags flags, ConfFile **ret);
صحيح conf_file_new(ثابت حرف *path, ثابت حرف *يىلتىز, ConfFilesFlags flags, ConfFile **ret);

صحيح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف *dir);
صحيح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir);
صحيح conf_files_list_strv(حرف ***ret, ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف* ثابت* dirs);
صحيح conf_files_list_strv_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف * ثابت *dirs);
صحيح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف *dirs);
صحيح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs);

صحيح conf_files_list_full(ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحيح conf_files_list_at_full(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحيح conf_files_list_strv_full(ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحيح conf_files_list_strv_at_full(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحيح conf_files_list_nulstr_full(ثابت حرف *suffix, ثابت حرف *يىلتىز, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحيح conf_files_list_nulstr_at_full(ثابت حرف *suffix, صحيح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files);

صحيح conf_files_list_with_replacement(
                ثابت حرف *يىلتىز,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted);
صحيح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *يىلتىز,
                صحيح يىلتىز_fd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs);

تعريفنوع صحيح parse_قۇر_t(
                ثابت حرف *fname,
                unsigned قۇر,
                ثابت حرف *buffer,
                bool *invalid_config,
                فراغ *userdata);

صحيح conf_file_read(
                ثابت حرف *يىلتىز,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_قۇر_t parse_قۇر,
                فراغ *userdata,
                bool ignore_enoent,
                bool *invalid_config);
