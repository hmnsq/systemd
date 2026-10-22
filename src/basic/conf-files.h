/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব "forward.h"

প্রকারলেখ গণনা ConfFilesFlags {
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

প্রকারলেখ কাঠামো ConfFile {
        অক্ষর *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        অক্ষর *ফলাফল;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        অক্ষর *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        অক্ষর *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        পূর্ণসংখ্যা fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        কাঠামো stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
শূন্য conf_file_free_array(ConfFile **array, size_t n);

পূর্ণসংখ্যা conf_file_new_at(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ConfFile **ret);
পূর্ণসংখ্যা conf_file_new(ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ConfFile **ret);

পূর্ণসংখ্যা conf_files_list(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর *dir);
পূর্ণসংখ্যা conf_files_list_at(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর *dir);
পূর্ণসংখ্যা conf_files_list_strv(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর* ধ্রুবক* dirs);
পূর্ণসংখ্যা conf_files_list_strv_at(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর * ধ্রুবক *dirs);
পূর্ণসংখ্যা conf_files_list_nulstr(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর *dirs);
পূর্ণসংখ্যা conf_files_list_nulstr_at(অক্ষর ***ret, ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর *dirs);

পূর্ণসংখ্যা conf_files_list_full(ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর *dir, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণসংখ্যা conf_files_list_at_full(ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর *dir, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণসংখ্যা conf_files_list_strv_full(ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর * ধ্রুবক *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণসংখ্যা conf_files_list_strv_at_full(ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর * ধ্রুবক *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণসংখ্যা conf_files_list_nulstr_full(ধ্রুবক অক্ষর *suffix, ধ্রুবক অক্ষর *মূল, ConfFilesFlags flags, ধ্রুবক অক্ষর *dirs, ConfFile ***ret_files, size_t *ret_n_files);
পূর্ণসংখ্যা conf_files_list_nulstr_at_full(ধ্রুবক অক্ষর *suffix, পূর্ণসংখ্যা rfd, ConfFilesFlags flags, ধ্রুবক অক্ষর *dirs, ConfFile ***ret_files, size_t *ret_n_files);

পূর্ণসংখ্যা conf_files_list_with_replacement(
                ধ্রুবক অক্ষর *মূল,
                অক্ষর **config_dirs,
                ধ্রুবক অক্ষর *replacement,
                অক্ষর ***ret_files,
                অক্ষর **ret_inserted);
পূর্ণসংখ্যা conf_files_list_dropins(
                অক্ষর ***ret,
                ধ্রুবক অক্ষর *dropin_dirname,
                ধ্রুবক অক্ষর *মূল,
                পূর্ণসংখ্যা মূল_fd,
                ConfFilesFlags flags,
                ধ্রুবক অক্ষর * ধ্রুবক *dirs);

প্রকারলেখ পূর্ণসংখ্যা parse_সারি_t(
                ধ্রুবক অক্ষর *fname,
                unsigned সারি,
                ধ্রুবক অক্ষর *বাফার,
                bool *invalid_config,
                শূন্য *userdata);

পূর্ণসংখ্যা conf_file_read(
                ধ্রুবক অক্ষর *মূল,
                ধ্রুবক অক্ষর **config_dirs,
                ধ্রুবক অক্ষর *fn,
                parse_সারি_t parse_সারি,
                শূন্য *userdata,
                bool ignore_enoent,
                bool *invalid_config);
