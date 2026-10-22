/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ ChaseFlags {
        CHASE_PREFIX_വേര്        = 1 << 0,  /* The specified path will be prefixed by the specified root before beginning the iteration */
        CHASE_NONEXISTENT        = 1 << 1,  /* It's OK if the path doesn't actually exist. */
        CHASE_NO_AUTOFS          = 1 << 2,  /* Return -EREMOTE if autofs mount point found */
        CHASE_TRIGGER_AUTOFS     = 1 << 3,  /* Use open_tree() rather than openat() to trigger autofs mount */
        CHASE_SAFE               = 1 << 4,  /* Return -EPERM if we ever traverse from unprivileged to privileged files or directories */
        CHASE_TRAIL_SLASH        = 1 << 5,  /* Any trailing slash will be preserved */
        CHASE_STEP               = 1 << 6,  /* Just execute a single step of the normalization */
        CHASE_NOFOLLOW           = 1 << 7,  /* Do not follow the path's right-most component. With ret_fd, when the path's
                                             * right-most component refers to symlink, return O_PATH fd of the symlink. */
        CHASE_WARN               = 1 << 8,  /* Emit an appropriate warning when an error is encountered.
                                             * Note: this may do an NSS lookup, hence this flag cannot be used in PID 1. */
        CHASE_PROHIBIT_SYMLINKS  = 1 << 9,  /* Refuse all symlinks */
        CHASE_PARENT             = 1 << 10, /* Chase the parent directory of the given path. Note that the
                                             * full path is still stored in ret_path and only the returned
                                             * file descriptor will point to the parent directory. Note that
                                             * the result path is the root or '.', then the file descriptor
                                             * also points to the result path even if this flag is set.
                                             * When this specified, chase() will succeed with 1 even if the
                                             * file points to the last path component does not exist. */
        CHASE_MKDIR_0755         = 1 << 11, /* Create any missing directories in the given path. */
        CHASE_EXTRACT_FILENAME   = 1 << 12, /* Only return the last component of the resolved path */
        CHASE_MUST_BE_DIRECTORY  = 1 << 13, /* Fail if returned inode fd is not a dir */
        CHASE_MUST_BE_REGULAR    = 1 << 14, /* Fail if returned inode fd is not a regular file */
        CHASE_MUST_BE_SOCKET     = 1 << 15, /* Fail if returned inode fd is not a socket */
} ChaseFlags;

പൂർണ്ണസംഖ്യ statx_unsafe_transition(സ്ഥിരം ഘടന statx *a, സ്ഥിരം ഘടന statx *b);
bool stat_unsafe_transition(സ്ഥിരം ഘടന stat *a, സ്ഥിരം ഘടന stat *b);

/* How many iterations to execute before returning -ELOOP */
#define CHASE_MAX 128U

പൂർണ്ണസംഖ്യ chase(സ്ഥിരം അക്ഷരം *path_with_prefix, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags flags, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);

പൂർണ്ണസംഖ്യ chaseat_prefix_വേര്(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret);
പൂർണ്ണസംഖ്യ chase_extract_filename(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret);

പൂർണ്ണസംഖ്യ chase_and_open(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ open_flags, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_opendir(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_path, DIR **ret_dir);
പൂർണ്ണസംഖ്യ chase_and_stat(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_path, ഘടന stat *ret_stat);
പൂർണ്ണസംഖ്യ chase_and_access(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ access_mode, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_fopen_unlocked(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, സ്ഥിരം അക്ഷരം *open_flags, അക്ഷരം **ret_path, FILE **ret_file);
പൂർണ്ണസംഖ്യ chase_and_unlink(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ unlink_flags, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_open_parent(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *വേര്, ChaseFlags chase_flags, അക്ഷരം **ret_filename);

പൂർണ്ണസംഖ്യ chaseat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags flags, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd);

പൂർണ്ണസംഖ്യ chase_and_openat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ open_flags, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_opendirat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_path, DIR **ret_dir);
പൂർണ്ണസംഖ്യ chase_and_statat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_path, ഘടന stat *ret_stat);
പൂർണ്ണസംഖ്യ chase_and_accessat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ access_mode, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_fopenat_unlocked(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, സ്ഥിരം അക്ഷരം *open_flags, അക്ഷരം **ret_path, FILE **ret_file);
പൂർണ്ണസംഖ്യ chase_and_unlinkat(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, പൂർണ്ണസംഖ്യ unlink_flags, അക്ഷരം **ret_path);
പൂർണ്ണസംഖ്യ chase_and_open_parent_at(പൂർണ്ണസംഖ്യ വേര്_fd, പൂർണ്ണസംഖ്യ dir_fd, സ്ഥിരം അക്ഷരം *path, ChaseFlags chase_flags, അക്ഷരം **ret_filename);
