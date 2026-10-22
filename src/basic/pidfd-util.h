/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

ចំនួនគត់ pidfd_get_namespace(ចំនួនគត់ fd, unsigned long ns_type_cmd);

ចំនួនគត់ pidfd_get_info(ចំនួនគត់ fd, sa_nracanaa pidfd_info *info);
ចំនួនគត់ pidfd_info_mask_is_supported(uint64_t mask);

ចំនួនគត់ pidfd_get_pid(ចំនួនគត់ fd, pid_t *ret);
ចំនួនគត់ pidfd_verify_pid(ចំនួនគត់ pidfd, pid_t pid);

ចំនួនគត់ pidfd_get_ppid(ចំនួនគត់ fd, pid_t *ret);
ចំនួនគត់ pidfd_get_uid(ចំនួនគត់ fd, uid_t *ret);
ចំនួនគត់ pidfd_get_cgroupid(ចំនួនគត់ fd, uint64_t *ret);

ចំនួនគត់ pidfd_get_inode_id_impl(ចំនួនគត់ fd, uint64_t *ret);
ចំនួនគត់ pidfd_get_inode_id(ចំនួនគត់ fd, uint64_t *ret);
ចំនួនគត់ pidfd_get_inode_id_self_cached(uint64_t *ret);

ចំនួនគត់ pidfd_check_pidfs(ចំនួនគត់ pid_fd);
