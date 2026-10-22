/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <sys/pidfd.h>          /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪೂರ್ಣಾಂಕ pidfd_get_namespace(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ns_type_cmd);

ಪೂರ್ಣಾಂಕ pidfd_get_info(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ pidfd_info *info);
ಪೂರ್ಣಾಂಕ pidfd_info_ಮುಖವಾಡ_is_supported(uint64_t ಮುಖವಾಡ_3);

ಪೂರ್ಣಾಂಕ pidfd_get_pid(ಪೂರ್ಣಾಂಕ fd, pid_t *ret);
ಪೂರ್ಣಾಂಕ pidfd_verify_pid(ಪೂರ್ಣಾಂಕ pidfd, pid_t pid);

ಪೂರ್ಣಾಂಕ pidfd_get_ppid(ಪೂರ್ಣಾಂಕ fd, pid_t *ret);
ಪೂರ್ಣಾಂಕ pidfd_get_uid(ಪೂರ್ಣಾಂಕ fd, uid_t *ret);
ಪೂರ್ಣಾಂಕ pidfd_get_cgroupid(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);

ಪೂರ್ಣಾಂಕ pidfd_get_inode_id_impl(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಂಕ pidfd_get_inode_id(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret);
ಪೂರ್ಣಾಂಕ pidfd_get_inode_id_self_cached(uint64_t *ret);

ಪೂರ್ಣಾಂಕ pidfd_check_pidfs(ಪೂರ್ಣಾಂಕ pid_fd);
