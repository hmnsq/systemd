/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_ಖಾಲಿಗಳು   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_ಖಾಲಿಗಳು,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_ಪೂರ್ವಪ್ರತ್ಯಯ_ಬೇರು         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ConfFile {
        ಅಕ್ಷರ *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        ಅಕ್ಷರ *ಫಲಿತಾಂಶ;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        ಅಕ್ಷರ *original_ಮಾರ್ಗ; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        ಅಕ್ಷರ *resolved_ಮಾರ್ಗ; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        ಪೂರ್ಣಾಂಕ fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ರಚನೆ stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
ಶೂನ್ಯ conf_file_free_array(ConfFile **array, size_t n);

ಪೂರ್ಣಾಂಕ conf_file_new_at(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ConfFile **ret);
ಪೂರ್ಣಾಂಕ conf_file_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ConfFile **ret);

ಪೂರ್ಣಾಂಕ conf_files_list(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir);
ಪೂರ್ಣಾಂಕ conf_files_list_at(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir);
ಪೂರ್ಣಾಂಕ conf_files_list_strv(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ* dirs);
ಪೂರ್ಣಾಂಕ conf_files_list_strv_at(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs);
ಪೂರ್ಣಾಂಕ conf_files_list_nulstr(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs);
ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_at(ಅಕ್ಷರ ***ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs);

ಪೂರ್ಣಾಂಕ conf_files_list_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಂಕ conf_files_list_at_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಂಕ conf_files_list_strv_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಂಕ conf_files_list_strv_at_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ಪೂರ್ಣಾಂಕ conf_files_list_nulstr_at_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *suffix, ಪೂರ್ಣಾಂಕ rfd, ConfFilesFlags flags, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dirs, ConfFile ***ret_files, size_t *ret_n_files);

ಪೂರ್ಣಾಂಕ conf_files_list_with_replacement(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಅಕ್ಷರ **config_dirs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *replacement,
                ಅಕ್ಷರ ***ret_files,
                ಅಕ್ಷರ **ret_inserted);
ಪೂರ್ಣಾಂಕ conf_files_list_dropins(
                ಅಕ್ಷರ ***ret,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dropin_dirname,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಪೂರ್ಣಾಂಕ ಬೇರು_fd,
                ConfFilesFlags flags,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ *dirs);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ parse_ಸಾಲು_t(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname,
                ಚಿಹ್ನೆರಹಿತ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್,
                bool *invalid_config,
                ಶೂನ್ಯ *userdata);

ಪೂರ್ಣಾಂಕ conf_file_read(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **config_dirs,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn,
                parse_ಸಾಲು_t parse_ಸಾಲು,
                ಶೂನ್ಯ *userdata,
                bool ignore_enoent,
                bool *invalid_config);
