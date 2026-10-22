/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_ମୂଳ         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା ConfFile {
        ଅକ୍ଷର *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        ଅକ୍ଷର *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        ଅକ୍ଷର *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        ଅକ୍ଷର *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ସଂରଚନା stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
ଶୂନ୍ୟ conf_file_free_array(ConfFile **array, size_t n);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_new_at(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ConfFile **ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_new(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ConfFile **ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_at(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର* ଅଚଳ* dirs);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_at(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_at(ଅକ୍ଷର ***ret, ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_full(ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir, ConfFile ***ret_files, size_t *ret_n_files);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_at_full(ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dir, ConfFile ***ret_files, size_t *ret_n_files);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_full(ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_strv_at_full(ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_full(ଅଚଳ ଅକ୍ଷର *suffix, ଅଚଳ ଅକ୍ଷର *ମୂଳ, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs, ConfFile ***ret_files, size_t *ret_n_files);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_nulstr_at_full(ଅଚଳ ଅକ୍ଷର *suffix, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rfd, ConfFilesFlags flags, ଅଚଳ ଅକ୍ଷର *dirs, ConfFile ***ret_files, size_t *ret_n_files);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_with_replacement(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅକ୍ଷର **config_dirs,
                ଅଚଳ ଅକ୍ଷର *replacement,
                ଅକ୍ଷର ***ret_files,
                ଅକ୍ଷର **ret_inserted);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_files_list_dropins(
                ଅକ୍ଷର ***ret,
                ଅଚଳ ଅକ୍ଷର *dropin_dirname,
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ମୂଳ_fd,
                ConfFilesFlags flags,
                ଅଚଳ ଅକ୍ଷର * ଅଚଳ *dirs);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା parse_ଧାଡ଼ି_t(
                ଅଚଳ ଅକ୍ଷର *fname,
                unsigned ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *buffer,
                bool *invalid_config,
                ଶୂନ୍ୟ *userdata);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା conf_file_read(
                ଅଚଳ ଅକ୍ଷର *ମୂଳ,
                ଅଚଳ ଅକ୍ଷର **config_dirs,
                ଅଚଳ ଅକ୍ଷର *fn,
                parse_ଧାଡ଼ି_t parse_ଧାଡ଼ି,
                ଶୂନ୍ୟ *userdata,
                bool ignore_enoent,
                bool *invalid_config);
