/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_मूल         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

प्रकारनिर्देश संरचना ConfFile {
        वर्ण *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        वर्ण *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        वर्ण *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        वर्ण *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        पूर्णाङ्क fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        संरचना stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
शून्य conf_file_free_array(ConfFile **array, size_t n);

पूर्णाङ्क conf_file_new_at(ध्रुव वर्ण *path, ध्रुव वर्ण *मूल, पूर्णाङ्क rfd, ConfFilesFlags flags, ConfFile **ret);
पूर्णाङ्क conf_file_new(ध्रुव वर्ण *path, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ConfFile **ret);

पूर्णाङ्क conf_files_list(वर्ण ***ret, ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण *dir);
पूर्णाङ्क conf_files_list_at(वर्ण ***ret, ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dir);
पूर्णाङ्क conf_files_list_strv(वर्ण ***ret, ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण* ध्रुव* dirs);
पूर्णाङ्क conf_files_list_strv_at(वर्ण ***ret, ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण * ध्रुव *dirs);
पूर्णाङ्क conf_files_list_nulstr(वर्ण ***ret, ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण *dirs);
पूर्णाङ्क conf_files_list_nulstr_at(वर्ण ***ret, ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dirs);

पूर्णाङ्क conf_files_list_full(ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण *dir, ConfFile ***ret_files, size_t *ret_n_files);
पूर्णाङ्क conf_files_list_at_full(ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dir, ConfFile ***ret_files, size_t *ret_n_files);
पूर्णाङ्क conf_files_list_strv_full(ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण * ध्रुव *dirs, ConfFile ***ret_files, size_t *ret_n_files);
पूर्णाङ्क conf_files_list_strv_at_full(ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण * ध्रुव *dirs, ConfFile ***ret_files, size_t *ret_n_files);
पूर्णाङ्क conf_files_list_nulstr_full(ध्रुव वर्ण *suffix, ध्रुव वर्ण *मूल, ConfFilesFlags flags, ध्रुव वर्ण *dirs, ConfFile ***ret_files, size_t *ret_n_files);
पूर्णाङ्क conf_files_list_nulstr_at_full(ध्रुव वर्ण *suffix, पूर्णाङ्क rfd, ConfFilesFlags flags, ध्रुव वर्ण *dirs, ConfFile ***ret_files, size_t *ret_n_files);

पूर्णाङ्क conf_files_list_with_replacement(
                ध्रुव वर्ण *मूल,
                वर्ण **config_dirs,
                ध्रुव वर्ण *replacement,
                वर्ण ***ret_files,
                वर्ण **ret_inserted);
पूर्णाङ्क conf_files_list_dropins(
                वर्ण ***ret,
                ध्रुव वर्ण *dropin_dirname,
                ध्रुव वर्ण *मूल,
                पूर्णाङ्क मूल_fd,
                ConfFilesFlags flags,
                ध्रुव वर्ण * ध्रुव *dirs);

प्रकारनिर्देश पूर्णाङ्क parse_पंक्ति_t(
                ध्रुव वर्ण *fname,
                unsigned पंक्ति,
                ध्रुव वर्ण *buffer,
                bool *invalid_config,
                शून्य *userdata);

पूर्णाङ्क conf_file_read(
                ध्रुव वर्ण *मूल,
                ध्रुव वर्ण **config_dirs,
                ध्रुव वर्ण *fn,
                parse_पंक्ति_t parse_पंक्ति,
                शून्य *userdata,
                bool ignore_enoent,
                bool *invalid_config);
