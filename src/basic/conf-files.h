/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "forward.h"

प्रकारव्याख्या गणना ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_मूळ         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

प्रकारव्याख्या संरचना ConfFile {
        अक्षर *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        अक्षर *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        अक्षर *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        अक्षर *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        पूर्णांक fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        संरचना stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
शून्य conf_file_free_array(ConfFile **array, माप_प्रकार n);

पूर्णांक conf_file_new_at(स्थिरांक अक्षर *path, स्थिरांक अक्षर *मूळ, पूर्णांक rfd, ConfFilesFlags flags, ConfFile **ret);
पूर्णांक conf_file_new(स्थिरांक अक्षर *path, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, ConfFile **ret);

पूर्णांक conf_files_list(अक्षर ***ret, स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dir);
पूर्णांक conf_files_list_at(अक्षर ***ret, स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dir);
पूर्णांक conf_files_list_strv(अक्षर ***ret, स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर* स्थिरांक* dirs);
पूर्णांक conf_files_list_strv_at(अक्षर ***ret, स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर * स्थिरांक *dirs);
पूर्णांक conf_files_list_nulstr(अक्षर ***ret, स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dirs);
पूर्णांक conf_files_list_nulstr_at(अक्षर ***ret, स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dirs);

पूर्णांक conf_files_list_full(स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_at_full(स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_strv_full(स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर * स्थिरांक *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_strv_at_full(स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर * स्थिरांक *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_nulstr_full(स्थिरांक अक्षर *suffix, स्थिरांक अक्षर *मूळ, ConfFilesFlags flags, स्थिरांक अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_nulstr_at_full(स्थिरांक अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिरांक अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);

पूर्णांक conf_files_list_with_replacement(
                स्थिरांक अक्षर *मूळ,
                अक्षर **config_dirs,
                स्थिरांक अक्षर *replacement,
                अक्षर ***ret_files,
                अक्षर **ret_inserted);
पूर्णांक conf_files_list_dropins(
                अक्षर ***ret,
                स्थिरांक अक्षर *dropin_dirname,
                स्थिरांक अक्षर *मूळ,
                पूर्णांक मूळ_fd,
                ConfFilesFlags flags,
                स्थिरांक अक्षर * स्थिरांक *dirs);

प्रकारव्याख्या पूर्णांक parse_ओळ_t(
                स्थिरांक अक्षर *fname,
                अचिन्हित ओळ,
                स्थिरांक अक्षर *buffer,
                bool *invalid_config,
                शून्य *userdata);

पूर्णांक conf_file_read(
                स्थिरांक अक्षर *मूळ,
                स्थिरांक अक्षर **config_dirs,
                स्थिरांक अक्षर *fn,
                parse_ओळ_t parse_ओळ,
                शून्य *userdata,
                bool ignore_enoent,
                bool *invalid_config);
