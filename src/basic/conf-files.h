/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/stat.h>

#شامل "forward.h"

تعریفنوع شمارش ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_ریشه         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

تعریفنوع ساختار ConfFile {
        نویسه *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        نویسه *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        نویسه *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        نویسه *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        صحیح fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ساختار stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
تهی conf_file_free_array(ConfFile **array, size_t n);

صحیح conf_file_new_at(ثابت نویسه *path, ثابت نویسه *ریشه, صحیح rfd, ConfFilesFlags flags, ConfFile **ret);
صحیح conf_file_new(ثابت نویسه *path, ثابت نویسه *ریشه, ConfFilesFlags flags, ConfFile **ret);

صحیح conf_files_list(نویسه ***ret, ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dir);
صحیح conf_files_list_at(نویسه ***ret, ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dir);
صحیح conf_files_list_strv(نویسه ***ret, ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه* ثابت* dirs);
صحیح conf_files_list_strv_at(نویسه ***ret, ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه * ثابت *dirs);
صحیح conf_files_list_nulstr(نویسه ***ret, ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dirs);
صحیح conf_files_list_nulstr_at(نویسه ***ret, ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dirs);

صحیح conf_files_list_full(ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_at_full(ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_strv_full(ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_strv_at_full(ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_nulstr_full(ثابت نویسه *suffix, ثابت نویسه *ریشه, ConfFilesFlags flags, ثابت نویسه *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_nulstr_at_full(ثابت نویسه *suffix, صحیح rfd, ConfFilesFlags flags, ثابت نویسه *dirs, ConfFile ***ret_files, size_t *ret_n_files);

صحیح conf_files_list_with_replacement(
                ثابت نویسه *ریشه,
                نویسه **config_dirs,
                ثابت نویسه *replacement,
                نویسه ***ret_files,
                نویسه **ret_inserted);
صحیح conf_files_list_dropins(
                نویسه ***ret,
                ثابت نویسه *dropin_dirname,
                ثابت نویسه *ریشه,
                صحیح ریشه_fd,
                ConfFilesFlags flags,
                ثابت نویسه * ثابت *dirs);

تعریفنوع صحیح parse_سطر_t(
                ثابت نویسه *fname,
                unsigned سطر,
                ثابت نویسه *buffer,
                bool *invalid_config,
                تهی *userdata);

صحیح conf_file_read(
                ثابت نویسه *ریشه,
                ثابت نویسه **config_dirs,
                ثابت نویسه *fn,
                parse_سطر_t parse_سطر,
                تهی *userdata,
                bool ignore_enoent,
                bool *invalid_config);
