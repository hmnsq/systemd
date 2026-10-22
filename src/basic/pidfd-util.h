/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

số_nguyên pidfd_get_namespace(số_nguyên fd, unsigned long ns_type_cmd);

số_nguyên pidfd_get_info(số_nguyên fd, struct pidfd_info *info);
số_nguyên pidfd_info_mask_is_supported(uint64_t mask);

số_nguyên pidfd_get_pid(số_nguyên fd, pid_t *ret);
số_nguyên pidfd_verify_pid(số_nguyên pidfd, pid_t pid);

số_nguyên pidfd_get_ppid(số_nguyên fd, pid_t *ret);
số_nguyên pidfd_get_uid(số_nguyên fd, uid_t *ret);
số_nguyên pidfd_get_cgroupid(số_nguyên fd, uint64_t *ret);

số_nguyên pidfd_get_inode_id_impl(số_nguyên fd, uint64_t *ret);
số_nguyên pidfd_get_inode_id(số_nguyên fd, uint64_t *ret);
số_nguyên pidfd_get_inode_id_self_cached(uint64_t *ret);

số_nguyên pidfd_check_pidfs(số_nguyên pid_fd);
