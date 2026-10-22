/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <sys/pidfd.h>          /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಙ್ಕ pidfd_get_namespace(ಪೂರ್ಣಾಙ್ಕ fd, unsigned long ns_type_cmd);

ಪೂರ್ಣಾಙ್ಕ pidfd_get_info(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ pidfd_info *info);
ಪೂರ್ಣಾಙ್ಕ pidfd_info_mask_is_supported(uint64_t mask);

ಪೂರ್ಣಾಙ್ಕ pidfd_get_pid(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidfd_verify_pid(ಪೂರ್ಣಾಙ್ಕ pidfd, pid_t pid);

ಪೂರ್ಣಾಙ್ಕ pidfd_get_ppid(ಪೂರ್ಣಾಙ್ಕ fd, pid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidfd_get_uid(ಪೂರ್ಣಾಙ್ಕ fd, uid_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidfd_get_cgroupid(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id_impl(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಙ್ಕ pidfd_get_inode_id_self_cached(uint64_t *ret);

ಪೂರ್ಣಾಙ್ಕ pidfd_check_pidfs(ಪೂರ್ಣಾಙ್ಕ pid_fd);
