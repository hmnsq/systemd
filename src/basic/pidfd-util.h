/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

ціле pidfd_get_namespace(ціле fd, unsigned long ns_type_cmd);

ціле pidfd_get_info(ціле fd, struct pidfd_info *info);
ціле pidfd_info_mask_is_supported(uint64_t mask);

ціле pidfd_get_pid(ціле fd, pid_t *ret);
ціле pidfd_verify_pid(ціле pidfd, pid_t pid);

ціле pidfd_get_ppid(ціле fd, pid_t *ret);
ціле pidfd_get_uid(ціле fd, uid_t *ret);
ціле pidfd_get_cgroupid(ціле fd, uint64_t *ret);

ціле pidfd_get_inode_id_impl(ціле fd, uint64_t *ret);
ціле pidfd_get_inode_id(ціле fd, uint64_t *ret);
ціле pidfd_get_inode_id_self_cached(uint64_t *ret);

ціле pidfd_check_pidfs(ціле pid_fd);
