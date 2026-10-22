/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

цео pidfd_get_namespace(цео fd, unsigned long ns_type_cmd);

цео pidfd_get_info(цео fd, struct pidfd_info *info);
цео pidfd_info_mask_is_supported(uint64_t mask);

цео pidfd_get_pid(цео fd, pid_t *ret);
цео pidfd_verify_pid(цео pidfd, pid_t pid);

цео pidfd_get_ppid(цео fd, pid_t *ret);
цео pidfd_get_uid(цео fd, uid_t *ret);
цео pidfd_get_cgroupid(цео fd, uint64_t *ret);

цео pidfd_get_inode_id_impl(цео fd, uint64_t *ret);
цео pidfd_get_inode_id(цео fd, uint64_t *ret);
цео pidfd_get_inode_id_self_cached(uint64_t *ret);

цео pidfd_check_pidfs(цео pid_fd);
