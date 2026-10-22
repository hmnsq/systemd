/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ "forward.h"

প্রকারনির্দেশ গণনা ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_মূল         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

প্রকারনির্দেশ সংরচনা ConfFile {
        ৱর্ণ *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        ৱর্ণ *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        ৱর্ণ *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        ৱর্ণ *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        পূর্ণাঙ্ক fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        সংরচনা stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
শূন্য conf_file_free_array(ConfFile **array, size_t n);

পূর্ণাঙ্ক conf_file_new_at(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *মূল, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ConfFile **ret);
পূর্ণাঙ্ক conf_file_new(ধ্রুৱ ৱর্ণ *path, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ConfFile **ret);

পূর্ণাঙ্ক conf_files_list(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir);
পূর্ণাঙ্ক conf_files_list_at(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir);
পূর্ণাঙ্ক conf_files_list_strv(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ* ধ্রুৱ* dirs);
পূর্ণাঙ্ক conf_files_list_strv_at(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs);
পূর্ণাঙ্ক conf_files_list_nulstr(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs);
পূর্ণাঙ্ক conf_files_list_nulstr_at(ৱর্ণ ***ret, ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs);

পূর্ণাঙ্ক conf_files_list_full(ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণাঙ্ক conf_files_list_at_full(ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dir, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণাঙ্ক conf_files_list_strv_full(ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণাঙ্ক conf_files_list_strv_at_full(ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণাঙ্ক conf_files_list_nulstr_full(ধ্রুৱ ৱর্ণ *suffix, ধ্রুৱ ৱর্ণ *মূল, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণাঙ্ক conf_files_list_nulstr_at_full(ধ্রুৱ ৱর্ণ *suffix, পূর্ণাঙ্ক rfd, ConfFilesFlags flags, ধ্রুৱ ৱর্ণ *dirs, ConfFile ***ret_files, size_t *ret_n_files);

পূর্ণাঙ্ক conf_files_list_with_replacement(
                ধ্রুৱ ৱর্ণ *মূল,
                ৱর্ণ **config_dirs,
                ধ্রুৱ ৱর্ণ *replacement,
                ৱর্ণ ***ret_files,
                ৱর্ণ **ret_inserted);
পূর্ণাঙ্ক conf_files_list_dropins(
                ৱর্ণ ***ret,
                ধ্রুৱ ৱর্ণ *dropin_dirname,
                ধ্রুৱ ৱর্ণ *মূল,
                পূর্ণাঙ্ক মূল_fd,
                ConfFilesFlags flags,
                ধ্রুৱ ৱর্ণ * ধ্রুৱ *dirs);

প্রকারনির্দেশ পূর্ণাঙ্ক parse_সাৰি_t(
                ধ্রুৱ ৱর্ণ *fname,
                unsigned সাৰি,
                ধ্রুৱ ৱর্ণ *buffer,
                bool *invalid_config,
                শূন্য *userdata);

পূর্ণাঙ্ক conf_file_read(
                ধ্রুৱ ৱর্ণ *মূল,
                ধ্রুৱ ৱর্ণ **config_dirs,
                ধ্রুৱ ৱর্ণ *fn,
                parse_সাৰি_t parse_সাৰি,
                শূন্য *userdata,
                bool ignore_enoent,
                bool *invalid_config);
