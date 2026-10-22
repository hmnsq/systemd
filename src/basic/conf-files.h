/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన ConfFilesFlags {
        CONF_FILES_EXECUTABLE               = 1 << 0, /* inode must be marked executable */
        CONF_FILES_REGULAR                  = 1 << 1, /* inode must be regular file */
        CONF_FILES_DIRECTORY                = 1 << 2, /* inode must be directory */
        CONF_FILES_BASENAME                 = 1 << 3, /* only return basename of file, not full path */
        CONF_FILES_FILTER_MASKED_BY_SYMLINK = 1 << 4, /* implement /dev/null symlink based masking */
        CONF_FILES_FILTER_MASKED_BY_ఖాళీలు   = 1 << 5, /* implement masking by empty file */
        CONF_FILES_FILTER_MASKED            = CONF_FILES_FILTER_MASKED_BY_SYMLINK | CONF_FILES_FILTER_MASKED_BY_ఖాళీలు,
        CONF_FILES_TRUNCATE_SUFFIX          = 1 << 6, /* truncate specified suffix from return filename or path */
        CONF_FILES_WARN                     = 1 << 7, /* warn on some errors */
        CONF_FILES_DONT_ఉపసర్గ_మూలం         = 1 << 8, /* don't prefix the specified root path to the resulting paths */
} ConfFilesFlags;

రకనిర్వచనం నిర్మాణం ConfFile {
        అక్షరం *filename;      /* filename of the discovered file (i.e. without any directory prefix) */
        అక్షరం *ఫలితం;        /* full path to the file (with the directory prefix fully resolved, but the filename part left as is) */
        అక్షరం *original_మార్గం; /* full path to the file (original – non-resolved – directory prefix + filename part left as is) */
        అక్షరం *resolved_మార్గం; /* fully resolved path to the file with both directory prefix and filename fully resolved */
        పూర్ణాంకం fd;              /* O_PATH fd to resolved_path, -EBADF if resolved_path does not exist */
        నిర్మాణం stat st;      /* stat of the file. */
} ConfFile;

ConfFile* conf_file_free(ConfFile *c);
DEFINE_TRIVIAL_CLEANUP_FUNC(ConfFile*, conf_file_free);
శూన్య conf_file_free_array(ConfFile **array, size_t n);

పూర్ణాంకం conf_file_new_at(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, పూర్ణాంకం rfd, ConfFilesFlags flags, ConfFile **ret);
పూర్ణాంకం conf_file_new(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, ConfFile **ret);

పూర్ణాంకం conf_files_list(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir);
పూర్ణాంకం conf_files_list_at(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir);
పూర్ణాంకం conf_files_list_strv(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం* స్థిరరాశి* dirs);
పూర్ణాంకం conf_files_list_strv_at(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం * స్థిరరాశి *dirs);
పూర్ణాంకం conf_files_list_nulstr(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs);
పూర్ణాంకం conf_files_list_nulstr_at(అక్షరం ***ret, స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs);

పూర్ణాంకం conf_files_list_నిండిన(స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir, ConfFile ***ret_files, size_t *ret_n_files);
పూర్ణాంకం conf_files_list_at_నిండిన(స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dir, ConfFile ***ret_files, size_t *ret_n_files);
పూర్ణాంకం conf_files_list_strv_నిండిన(స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం * స్థిరరాశి *dirs, ConfFile ***ret_files, size_t *ret_n_files);
పూర్ణాంకం conf_files_list_strv_at_నిండిన(స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం * స్థిరరాశి *dirs, ConfFile ***ret_files, size_t *ret_n_files);
పూర్ణాంకం conf_files_list_nulstr_నిండిన(స్థిరరాశి అక్షరం *suffix, స్థిరరాశి అక్షరం *మూలం, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs, ConfFile ***ret_files, size_t *ret_n_files);
పూర్ణాంకం conf_files_list_nulstr_at_నిండిన(స్థిరరాశి అక్షరం *suffix, పూర్ణాంకం rfd, ConfFilesFlags flags, స్థిరరాశి అక్షరం *dirs, ConfFile ***ret_files, size_t *ret_n_files);

పూర్ణాంకం conf_files_list_with_replacement(
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం **config_dirs,
                స్థిరరాశి అక్షరం *replacement,
                అక్షరం ***ret_files,
                అక్షరం **ret_inserted);
పూర్ణాంకం conf_files_list_dropins(
                అక్షరం ***ret,
                స్థిరరాశి అక్షరం *dropin_dirname,
                స్థిరరాశి అక్షరం *మూలం,
                పూర్ణాంకం మూలం_fd,
                ConfFilesFlags flags,
                స్థిరరాశి అక్షరం * స్థిరరాశి *dirs);

రకనిర్వచనం పూర్ణాంకం parse_పంక్తి_t(
                స్థిరరాశి అక్షరం *fname,
                గుర్తులేని పంక్తి,
                స్థిరరాశి అక్షరం *బఫరు,
                bool *invalid_config,
                శూన్య *userdata);

పూర్ణాంకం conf_file_read(
                స్థిరరాశి అక్షరం *మూలం,
                స్థిరరాశి అక్షరం **config_dirs,
                స్థిరరాశి అక్షరం *fn,
                parse_పంక్తి_t parse_పంక్తి,
                శూన్య *userdata,
                bool ignore_enoent,
                bool *invalid_config);
