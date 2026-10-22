/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/stat.h>

#समावेश "forward.h"

प्रकार क्रमागत ConfFilesFlags {
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

प्रकार काष्ठा ConfFile {
        अक्षर *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        अक्षर *परिणाम;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        अक्षर *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        अक्षर *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        पूर्णांक fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        काष्ठा stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
व्योम conf_file_free_array(ConfFile **array, माप_प्रकार n);

पूर्णांक conf_file_new_at(स्थिर अक्षर *path, स्थिर अक्षर *मूल, पूर्णांक rfd, ConfFilesFlags flags, ConfFile **ret);
पूर्णांक conf_file_new(स्थिर अक्षर *path, स्थिर अक्षर *मूल, ConfFilesFlags flags, ConfFile **ret);

पूर्णांक conf_files_list(अक्षर ***ret, स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dir);
पूर्णांक conf_files_list_at(अक्षर ***ret, स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dir);
पूर्णांक conf_files_list_strv(अक्षर ***ret, स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर* स्थिर* dirs);
पूर्णांक conf_files_list_strv_at(अक्षर ***ret, स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर * स्थिर *dirs);
पूर्णांक conf_files_list_nulstr(अक्षर ***ret, स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dirs);
पूर्णांक conf_files_list_nulstr_at(अक्षर ***ret, स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dirs);

पूर्णांक conf_files_list_full(स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_at_full(स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dir, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_strv_full(स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर * स्थिर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_strv_at_full(स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर * स्थिर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_nulstr_full(स्थिर अक्षर *suffix, स्थिर अक्षर *मूल, ConfFilesFlags flags, स्थिर अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);
पूर्णांक conf_files_list_nulstr_at_full(स्थिर अक्षर *suffix, पूर्णांक rfd, ConfFilesFlags flags, स्थिर अक्षर *dirs, ConfFile ***ret_files, माप_प्रकार *ret_n_files);

पूर्णांक conf_files_list_with_replacement(
                स्थिर अक्षर *मूल,
                अक्षर **config_dirs,
                स्थिर अक्षर *replacement,
                अक्षर ***ret_files,
                अक्षर **ret_inserted);
पूर्णांक conf_files_list_dropins(
                अक्षर ***ret,
                स्थिर अक्षर *dropin_dirname,
                स्थिर अक्षर *मूल,
                पूर्णांक मूल_fd,
                ConfFilesFlags flags,
                स्थिर अक्षर * स्थिर *dirs);

प्रकार पूर्णांक parse_पंक्ति_t(
                स्थिर अक्षर *fname,
                अचिन्हित पंक्ति,
                स्थिर अक्षर *बफर,
                bool *invalid_config,
                व्योम *userdata);

पूर्णांक conf_file_read(
                स्थिर अक्षर *मूल,
                स्थिर अक्षर **config_dirs,
                स्थिर अक्षर *fn,
                parse_पंक्ति_t parse_पंक्ति,
                व्योम *userdata,
                bool ignore_enoent,
                bool *invalid_config);
