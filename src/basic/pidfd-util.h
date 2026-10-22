/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <sys/pidfd.h>          /* IWYU pragma: export */

#شامل "forward.h"

صحیح pidfd_get_namespace(صحیح fd, unsigned long ns_type_cmd);

صحیح pidfd_get_info(صحیح fd, ساختار pidfd_info *info);
صحیح pidfd_info_mask_is_supported(uint64_t mask);

صحیح pidfd_get_pid(صحیح fd, pid_t *ret);
صحیح pidfd_verify_pid(صحیح pidfd, pid_t pid);

صحیح pidfd_get_ppid(صحیح fd, pid_t *ret);
صحیح pidfd_get_uid(صحیح fd, uid_t *ret);
صحیح pidfd_get_cgroupid(صحیح fd, uint64_t *ret);

صحیح pidfd_get_inode_id_impl(صحیح fd, uint64_t *ret);
صحیح pidfd_get_inode_id(صحیح fd, uint64_t *ret);
صحیح pidfd_get_inode_id_self_cached(uint64_t *ret);

صحیح pidfd_check_pidfs(صحیح pid_fd);
