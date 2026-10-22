/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <sys/stat.h>

#شامل "forward.h"

قسمتعریف شمار ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_خالی_خانے   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_خالی_خانے,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_سابقہ_جڑ         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

قسمتعریف ساخت ConfFile {
        حرف *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        حرف *نتیجہ;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        حرف *original_راستہ; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        حرف *resolved_راستہ; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        صحیح fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ساخت stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
خالی conf_file_free_array(ConfFile **array, size_t n);

صحیح conf_file_new_at(ثابت حرف *راستہ, ثابت حرف *جڑ, صحیح rfd, ConfFilesFlags flags, ConfFile **ret);
صحیح conf_file_new(ثابت حرف *راستہ, ثابت حرف *جڑ, ConfFilesFlags flags, ConfFile **ret);

صحیح conf_files_list(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir);
صحیح conf_files_list_at(حرف ***ret, ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dir);
صحیح conf_files_list_strv(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف* ثابت* dirs);
صحیح conf_files_list_strv_at(حرف ***ret, ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف * ثابت *dirs);
صحیح conf_files_list_nulstr(حرف ***ret, ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs);
صحیح conf_files_list_nulstr_at(حرف ***ret, ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dirs);

صحیح conf_files_list_بھرا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_at_بھرا(ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dir, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_strv_بھرا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_strv_at_بھرا(ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف * ثابت *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_nulstr_بھرا(ثابت حرف *suffix, ثابت حرف *جڑ, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files);
صحیح conf_files_list_nulstr_at_بھرا(ثابت حرف *suffix, صحیح rfd, ConfFilesFlags flags, ثابت حرف *dirs, ConfFile ***ret_files, size_t *ret_n_files);

صحیح conf_files_list_with_replacement(
                ثابت حرف *جڑ,
                حرف **config_dirs,
                ثابت حرف *replacement,
                حرف ***ret_files,
                حرف **ret_inserted);
صحیح conf_files_list_dropins(
                حرف ***ret,
                ثابت حرف *dropin_dirname,
                ثابت حرف *جڑ,
                صحیح جڑ_fd,
                ConfFilesFlags flags,
                ثابت حرف * ثابت *dirs);

قسمتعریف صحیح parse_سطر_t(
                ثابت حرف *fname,
                غیرعلامتی سطر,
                ثابت حرف *بفر,
                bool *invalid_config,
                خالی *userdata);

صحیح conf_file_read(
                ثابت حرف *جڑ,
                ثابت حرف **config_dirs,
                ثابت حرف *fn,
                parse_سطر_t parse_سطر,
                خالی *userdata,
                bool ignore_enoent,
                bool *invalid_config);
