/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_வேர்         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

வகைவரை கட்டமைப்பு ConfFile {
        எழுத்து *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        எழுத்து *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        எழுத்து *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        எழுத்து *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        முழு fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        கட்டமைப்பு stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
வெறுமை conf_file_free_array(ConfFile **array, size_t n);

முழு conf_file_new_at(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், முழு rfd, ConfFilesFlags flags, ConfFile **ret);
முழு conf_file_new(மாறிலி எழுத்து *path, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, ConfFile **ret);

முழு conf_files_list(எழுத்து ***ret, மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dir);
முழு conf_files_list_at(எழுத்து ***ret, மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dir);
முழு conf_files_list_strv(எழுத்து ***ret, மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து* மாறிலி* dirs);
முழு conf_files_list_strv_at(எழுத்து ***ret, மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து * மாறிலி *dirs);
முழு conf_files_list_nulstr(எழுத்து ***ret, மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dirs);
முழு conf_files_list_nulstr_at(எழுத்து ***ret, மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dirs);

முழு conf_files_list_full(மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dir, ConfFile ***ret_files, size_t *ret_n_files);
முழு conf_files_list_at_full(மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dir, ConfFile ***ret_files, size_t *ret_n_files);
முழு conf_files_list_strv_full(மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து * மாறிலி *dirs, ConfFile ***ret_files, size_t *ret_n_files);
முழு conf_files_list_strv_at_full(மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து * மாறிலி *dirs, ConfFile ***ret_files, size_t *ret_n_files);
முழு conf_files_list_nulstr_full(மாறிலி எழுத்து *suffix, மாறிலி எழுத்து *வேர், ConfFilesFlags flags, மாறிலி எழுத்து *dirs, ConfFile ***ret_files, size_t *ret_n_files);
முழு conf_files_list_nulstr_at_full(மாறிலி எழுத்து *suffix, முழு rfd, ConfFilesFlags flags, மாறிலி எழுத்து *dirs, ConfFile ***ret_files, size_t *ret_n_files);

முழு conf_files_list_with_replacement(
                மாறிலி எழுத்து *வேர்,
                எழுத்து **config_dirs,
                மாறிலி எழுத்து *replacement,
                எழுத்து ***ret_files,
                எழுத்து **ret_inserted);
முழு conf_files_list_dropins(
                எழுத்து ***ret,
                மாறிலி எழுத்து *dropin_dirname,
                மாறிலி எழுத்து *வேர்,
                முழு வேர்_fd,
                ConfFilesFlags flags,
                மாறிலி எழுத்து * மாறிலி *dirs);

வகைவரை முழு parse_வரி_t(
                மாறிலி எழுத்து *fname,
                unsigned வரி,
                மாறிலி எழுத்து *buffer,
                bool *invalid_config,
                வெறுமை *userdata);

முழு conf_file_read(
                மாறிலி எழுத்து *வேர்,
                மாறிலி எழுத்து **config_dirs,
                மாறிலி எழுத்து *fn,
                parse_வரி_t parse_வரி,
                வெறுமை *userdata,
                bool ignore_enoent,
                bool *invalid_config);
