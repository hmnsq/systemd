/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_મૂળ         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

પ્રકારવ્યાખ્યા સંરચના ConfFile {
        અક્ષર *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        અક્ષર *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        અક્ષર *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        અક્ષર *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        પૂર્ણાંક fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        સંરચના stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
શૂન્ય conf_file_free_array(ConfFile **array, size_t n);

પૂર્ણાંક conf_file_new_at(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, પૂર્ણાંક rfd, ConfFilesFlags flags, ConfFile **ret);
પૂર્ણાંક conf_file_new(અચલ અક્ષર *path, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, ConfFile **ret);

પૂર્ણાંક conf_files_list(અક્ષર ***ret, અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dir);
પૂર્ણાંક conf_files_list_at(અક્ષર ***ret, અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dir);
પૂર્ણાંક conf_files_list_strv(અક્ષર ***ret, અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર* અચલ* dirs);
પૂર્ણાંક conf_files_list_strv_at(અક્ષર ***ret, અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર * અચલ *dirs);
પૂર્ણાંક conf_files_list_nulstr(અક્ષર ***ret, અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dirs);
પૂર્ણાંક conf_files_list_nulstr_at(અક્ષર ***ret, અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dirs);

પૂર્ણાંક conf_files_list_full(અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dir, ConfFile ***ret_files, size_t *ret_n_files);
પૂર્ણાંક conf_files_list_at_full(અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dir, ConfFile ***ret_files, size_t *ret_n_files);
પૂર્ણાંક conf_files_list_strv_full(અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર * અચલ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
પૂર્ણાંક conf_files_list_strv_at_full(અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર * અચલ *dirs, ConfFile ***ret_files, size_t *ret_n_files);
પૂર્ણાંક conf_files_list_nulstr_full(અચલ અક્ષર *suffix, અચલ અક્ષર *મૂળ, ConfFilesFlags flags, અચલ અક્ષર *dirs, ConfFile ***ret_files, size_t *ret_n_files);
પૂર્ણાંક conf_files_list_nulstr_at_full(અચલ અક્ષર *suffix, પૂર્ણાંક rfd, ConfFilesFlags flags, અચલ અક્ષર *dirs, ConfFile ***ret_files, size_t *ret_n_files);

પૂર્ણાંક conf_files_list_with_replacement(
                અચલ અક્ષર *મૂળ,
                અક્ષર **config_dirs,
                અચલ અક્ષર *replacement,
                અક્ષર ***ret_files,
                અક્ષર **ret_inserted);
પૂર્ણાંક conf_files_list_dropins(
                અક્ષર ***ret,
                અચલ અક્ષર *dropin_dirname,
                અચલ અક્ષર *મૂળ,
                પૂર્ણાંક મૂળ_fd,
                ConfFilesFlags flags,
                અચલ અક્ષર * અચલ *dirs);

પ્રકારવ્યાખ્યા પૂર્ણાંક parse_લીટી_t(
                અચલ અક્ષર *fname,
                unsigned લીટી,
                અચલ અક્ષર *buffer,
                bool *invalid_config,
                શૂન્ય *userdata);

પૂર્ણાંક conf_file_read(
                અચલ અક્ષર *મૂળ,
                અચલ અક્ષર **config_dirs,
                અચલ અક્ષર *fn,
                parse_લીટી_t parse_લીટી,
                શૂન્ય *userdata,
                bool ignore_enoent,
                bool *invalid_config);
