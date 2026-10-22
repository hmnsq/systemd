/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

整数 pidfd_get_namespace(整数 fd, unsigned long ns_type_cmd);

整数 pidfd_get_info(整数 fd, struct pidfd_info *info);
整数 pidfd_info_mask_is_supported(uint64_t mask);

整数 pidfd_get_pid(整数 fd, pid_t *ret);
整数 pidfd_verify_pid(整数 pidfd, pid_t pid);

整数 pidfd_get_ppid(整数 fd, pid_t *ret);
整数 pidfd_get_uid(整数 fd, uid_t *ret);
整数 pidfd_get_cgroupid(整数 fd, uint64_t *ret);

整数 pidfd_get_inode_id_impl(整数 fd, uint64_t *ret);
整数 pidfd_get_inode_id(整数 fd, uint64_t *ret);
整数 pidfd_get_inode_id_self_cached(uint64_t *ret);

整数 pidfd_check_pidfs(整数 pid_fd);
