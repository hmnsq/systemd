/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

정수 pidfd_get_namespace(정수 fd, unsigned long ns_type_cmd);

정수 pidfd_get_info(정수 fd, struct pidfd_info *info);
정수 pidfd_info_mask_is_supported(uint64_t mask);

정수 pidfd_get_pid(정수 fd, pid_t *ret);
정수 pidfd_verify_pid(정수 pidfd, pid_t pid);

정수 pidfd_get_ppid(정수 fd, pid_t *ret);
정수 pidfd_get_uid(정수 fd, uid_t *ret);
정수 pidfd_get_cgroupid(정수 fd, uint64_t *ret);

정수 pidfd_get_inode_id_impl(정수 fd, uint64_t *ret);
정수 pidfd_get_inode_id(정수 fd, uint64_t *ret);
정수 pidfd_get_inode_id_self_cached(uint64_t *ret);

정수 pidfd_check_pidfs(정수 pid_fd);
