/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

puur_naa_nka pidfd_get_namespace(puur_naa_nka fd, unsigned long ns_type_cmd);

puur_naa_nka pidfd_get_info(puur_naa_nka fd, sa_nracanaa pidfd_info *info);
puur_naa_nka pidfd_info_mask_is_supported(uint64_t mask);

puur_naa_nka pidfd_get_pid(puur_naa_nka fd, pid_t *ret);
puur_naa_nka pidfd_verify_pid(puur_naa_nka pidfd, pid_t pid);

puur_naa_nka pidfd_get_ppid(puur_naa_nka fd, pid_t *ret);
puur_naa_nka pidfd_get_uid(puur_naa_nka fd, uid_t *ret);
puur_naa_nka pidfd_get_cgroupid(puur_naa_nka fd, uint64_t *ret);

puur_naa_nka pidfd_get_inode_id_impl(puur_naa_nka fd, uint64_t *ret);
puur_naa_nka pidfd_get_inode_id(puur_naa_nka fd, uint64_t *ret);
puur_naa_nka pidfd_get_inode_id_self_cached(uint64_t *ret);

puur_naa_nka pidfd_check_pidfs(puur_naa_nka pid_fd);
