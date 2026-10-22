/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_ಬೇರು         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ ConfFile {
        ವರ್ಣ *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        ವರ್ಣ *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        ವರ್ಣ *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        ವರ್ಣ *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        ಪೂರ್ಣಾಙ್ಕ fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ಸಂರಚನಾ stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
ಶೂನ್ಯ conf_file_free_array(ConfFile **array, size_t n);

ಪೂರ್ಣಾಙ್ಕ conf_file_new_at(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ConfFile **ret);
ಪೂರ್ಣಾಙ್ಕ conf_file_new(ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ConfFile **ret);

ಪೂರ್ಣಾಙ್ಕ conf_files_list(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_at(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ* ಧ್ರುವ* dirs);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_at(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_at(ವರ್ಣ ***ret, ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs);

ಪೂರ್ಣಾಙ್ಕ conf_files_list_full(ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_at_full(ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_full(ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_strv_at_full(ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_full(ಧ್ರುವ ವರ್ಣ *suffix, ಧ್ರುವ ವರ್ಣ *ಬೇರು, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_nulstr_at_full(ಧ್ರುವ ವರ್ಣ *suffix, ಪೂರ್ಣಾಙ್ಕ rfd, ConfFilesFlags flags, ಧ್ರುವ ವರ್ಣ *dirs, ConfFile ***ret_files, size_t *ret_n_files);

ಪೂರ್ಣಾಙ್ಕ conf_files_list_with_replacement(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ವರ್ಣ **config_dirs,
                ಧ್ರುವ ವರ್ಣ *replacement,
                ವರ್ಣ ***ret_files,
                ವರ್ಣ **ret_inserted);
ಪೂರ್ಣಾಙ್ಕ conf_files_list_dropins(
                ವರ್ಣ ***ret,
                ಧ್ರುವ ವರ್ಣ *dropin_dirname,
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಪೂರ್ಣಾಙ್ಕ ಬೇರು_fd,
                ConfFilesFlags flags,
                ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ *dirs);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ parse_ಸಾಲು_t(
                ಧ್ರುವ ವರ್ಣ *fname,
                unsigned ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *buffer,
                bool *invalid_config,
                ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಙ್ಕ conf_file_read(
                ಧ್ರುವ ವರ್ಣ *ಬೇರು,
                ಧ್ರುವ ವರ್ಣ **config_dirs,
                ಧ್ರುವ ವರ್ಣ *fn,
                parse_ಸಾಲು_t parse_ಸಾಲು,
                ಶೂನ್ಯ *userdata,
                bool ignore_enoent,
                bool *invalid_config);
