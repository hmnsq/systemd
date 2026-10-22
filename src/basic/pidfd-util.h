/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <sys/pidfd.h>          /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

முழு pidfd_get_namespace(முழு fd, unsigned long ns_type_cmd);

முழு pidfd_get_info(முழு fd, கட்டமைப்பு pidfd_info *info);
முழு pidfd_info_mask_is_supported(uint64_t mask);

முழு pidfd_get_pid(முழு fd, pid_t *ret);
முழு pidfd_verify_pid(முழு pidfd, pid_t pid);

முழு pidfd_get_ppid(முழு fd, pid_t *ret);
முழு pidfd_get_uid(முழு fd, uid_t *ret);
முழு pidfd_get_cgroupid(முழு fd, uint64_t *ret);

முழு pidfd_get_inode_id_impl(முழு fd, uint64_t *ret);
முழு pidfd_get_inode_id(முழு fd, uint64_t *ret);
முழு pidfd_get_inode_id_self_cached(uint64_t *ret);

முழு pidfd_check_pidfs(முழு pid_fd);
