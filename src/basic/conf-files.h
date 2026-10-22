/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_ਖਾਲੀ_ਥਾਵਾਂ   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_ਖਾਲੀ_ਥਾਵਾਂ,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_ਅਗੇਤਰ_ਜੜ੍ਹ         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ConfFile {
        ਅੱਖਰ *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        ਅੱਖਰ *ਨਤੀਜਾ;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        ਅੱਖਰ *original_ਰਸਤਾ; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        ਅੱਖਰ *resolved_ਰਸਤਾ; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        ਪੂਰਨ fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ਬਣਤਰ stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
ਖਾਲੀ conf_file_free_array(ConfFile **array, size_t n);

ਪੂਰਨ conf_file_new_at(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਪੂਰਨ rfd, ConfFilesFlags flags, ConfFile **ret);
ਪੂਰਨ conf_file_new(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ConfFile **ret);

ਪੂਰਨ conf_files_list(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir);
ਪੂਰਨ conf_files_list_at(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir);
ਪੂਰਨ conf_files_list_strv(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ* ਅਚਲ* dirs);
ਪੂਰਨ conf_files_list_strv_at(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs);
ਪੂਰਨ conf_files_list_nulstr(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs);
ਪੂਰਨ conf_files_list_nulstr_at(ਅੱਖਰ ***ret, ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs);

ਪੂਰਨ conf_files_list_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ਪੂਰਨ conf_files_list_at_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dir, ConfFile ***ret_files, size_t *ret_n_files);
ਪੂਰਨ conf_files_list_strv_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ਪੂਰਨ conf_files_list_strv_at_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ਪੂਰਨ conf_files_list_nulstr_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ਪੂਰਨ conf_files_list_nulstr_at_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *suffix, ਪੂਰਨ rfd, ConfFilesFlags flags, ਅਚਲ ਅੱਖਰ *dirs, ConfFile ***ret_files, size_t *ret_n_files);

ਪੂਰਨ conf_files_list_with_replacement(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ **config_dirs,
                ਅਚਲ ਅੱਖਰ *replacement,
                ਅੱਖਰ ***ret_files,
                ਅੱਖਰ **ret_inserted);
ਪੂਰਨ conf_files_list_dropins(
                ਅੱਖਰ ***ret,
                ਅਚਲ ਅੱਖਰ *dropin_dirname,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਪੂਰਨ ਜੜ੍ਹ_fd,
                ConfFilesFlags flags,
                ਅਚਲ ਅੱਖਰ * ਅਚਲ *dirs);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ parse_ਸਤਰ_t(
                ਅਚਲ ਅੱਖਰ *fname,
                ਬਿਨਾਨਿਸ਼ਾਨੀ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *ਬਫਰ,
                bool *invalid_config,
                ਖਾਲੀ *userdata);

ਪੂਰਨ conf_file_read(
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅਚਲ ਅੱਖਰ **config_dirs,
                ਅਚਲ ਅੱਖਰ *fn,
                parse_ਸਤਰ_t parse_ਸਤਰ,
                ਖਾਲੀ *userdata,
                bool ignore_enoent,
                bool *invalid_config);
