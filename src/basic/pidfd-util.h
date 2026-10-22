/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <sys/pidfd.h>          /* IWYU pragma: export */

#ضمن "forward.h"

صحيح pidfd_get_namespace(صحيح fd, unsigned long ns_type_cmd);

صحيح pidfd_get_info(صحيح fd, بنية pidfd_info *info);
صحيح pidfd_info_mask_is_supported(uint64_t mask);

صحيح pidfd_get_pid(صحيح fd, pid_t *ret);
صحيح pidfd_verify_pid(صحيح pidfd, pid_t pid);

صحيح pidfd_get_ppid(صحيح fd, pid_t *ret);
صحيح pidfd_get_uid(صحيح fd, uid_t *ret);
صحيح pidfd_get_cgroupid(صحيح fd, uint64_t *ret);

صحيح pidfd_get_inode_id_impl(صحيح fd, uint64_t *ret);
صحيح pidfd_get_inode_id(صحيح fd, uint64_t *ret);
صحيح pidfd_get_inode_id_self_cached(uint64_t *ret);

صحيح pidfd_check_pidfs(صحيح pid_fd);
