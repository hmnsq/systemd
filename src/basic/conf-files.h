/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_EMPTY   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_EMPTY,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_PREFIX_വേര്         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

തരനിർവചനം ഘടന ConfFile {
        അക്ഷരം *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        അക്ഷരം *result;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        അക്ഷരം *original_path; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        അക്ഷരം *resolved_path; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        പൂർണ്ണസംഖ്യ fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        ഘടന stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
ശൂന്യം conf_file_free_array(ConfFile **array, size_t n);

പൂർണ്ണസംഖ്യ conf_file_new_at(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, ConfFile **ret);
പൂർണ്ണസംഖ്യ conf_file_new(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, ConfFile **ret);

പൂർണ്ണസംഖ്യ conf_files_list(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir);
പൂർണ്ണസംഖ്യ conf_files_list_at(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir);
പൂർണ്ണസംഖ്യ conf_files_list_strv(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം* സ്ഥിരം* dirs);
പൂർണ്ണസംഖ്യ conf_files_list_strv_at(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs);
പൂർണ്ണസംഖ്യ conf_files_list_nulstr(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs);
പൂർണ്ണസംഖ്യ conf_files_list_nulstr_at(അക്ഷരം ***ret, സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs);

പൂർണ്ണസംഖ്യ conf_files_list_full(സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir, ConfFile ***ret_files, size_t *ret_n_files);
പൂർണ്ണസംഖ്യ conf_files_list_at_full(സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dir, ConfFile ***ret_files, size_t *ret_n_files);
പൂർണ്ണസംഖ്യ conf_files_list_strv_full(സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs, ConfFile ***ret_files, size_t *ret_n_files);
പൂർണ്ണസംഖ്യ conf_files_list_strv_at_full(സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs, ConfFile ***ret_files, size_t *ret_n_files);
പൂർണ്ണസംഖ്യ conf_files_list_nulstr_full(സ്ഥിരം അക്ഷരം *suffix, സ്ഥിരം അക്ഷരം *വേര്, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs, ConfFile ***ret_files, size_t *ret_n_files);
പൂർണ്ണസംഖ്യ conf_files_list_nulstr_at_full(സ്ഥിരം അക്ഷരം *suffix, പൂർണ്ണസംഖ്യ rfd, ConfFilesFlags flags, സ്ഥിരം അക്ഷരം *dirs, ConfFile ***ret_files, size_t *ret_n_files);

പൂർണ്ണസംഖ്യ conf_files_list_with_replacement(
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം **config_dirs,
                സ്ഥിരം അക്ഷരം *replacement,
                അക്ഷരം ***ret_files,
                അക്ഷരം **ret_inserted);
പൂർണ്ണസംഖ്യ conf_files_list_dropins(
                അക്ഷരം ***ret,
                സ്ഥിരം അക്ഷരം *dropin_dirname,
                സ്ഥിരം അക്ഷരം *വേര്,
                പൂർണ്ണസംഖ്യ വേര്_fd,
                ConfFilesFlags flags,
                സ്ഥിരം അക്ഷരം * സ്ഥിരം *dirs);

തരനിർവചനം പൂർണ്ണസംഖ്യ parse_വരി_t(
                സ്ഥിരം അക്ഷരം *fname,
                unsigned വരി,
                സ്ഥിരം അക്ഷരം *buffer,
                bool *invalid_config,
                ശൂന്യം *userdata);

പൂർണ്ണസംഖ്യ conf_file_read(
                സ്ഥിരം അക്ഷരം *വേര്,
                സ്ഥിരം അക്ഷരം **config_dirs,
                സ്ഥിരം അക്ഷരം *fn,
                parse_വരി_t parse_വരി,
                ശൂന്യം *userdata,
                bool ignore_enoent,
                bool *invalid_config);
