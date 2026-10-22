/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

ຈຳນວນເຕັມ pidfd_get_namespace(ຈຳນວນເຕັມ fd, unsigned long ns_type_cmd);

ຈຳນວນເຕັມ pidfd_get_info(ຈຳນວນເຕັມ fd, sa_nracanaa pidfd_info *info);
ຈຳນວນເຕັມ pidfd_info_mask_is_supported(uint64_t mask);

ຈຳນວນເຕັມ pidfd_get_pid(ຈຳນວນເຕັມ fd, pid_t *ret);
ຈຳນວນເຕັມ pidfd_verify_pid(ຈຳນວນເຕັມ pidfd, pid_t pid);

ຈຳນວນເຕັມ pidfd_get_ppid(ຈຳນວນເຕັມ fd, pid_t *ret);
ຈຳນວນເຕັມ pidfd_get_uid(ຈຳນວນເຕັມ fd, uid_t *ret);
ຈຳນວນເຕັມ pidfd_get_cgroupid(ຈຳນວນເຕັມ fd, uint64_t *ret);

ຈຳນວນເຕັມ pidfd_get_inode_id_impl(ຈຳນວນເຕັມ fd, uint64_t *ret);
ຈຳນວນເຕັມ pidfd_get_inode_id(ຈຳນວນເຕັມ fd, uint64_t *ret);
ຈຳນວນເຕັມ pidfd_get_inode_id_self_cached(uint64_t *ret);

ຈຳນວນເຕັມ pidfd_check_pidfs(ຈຳນວນເຕັມ pid_fd);
