/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <sys/pidfd.h>          /* IWYU pragma: export */

#समावेश "forward.h"

पूर्णांक pidfd_get_namespace(पूर्णांक fd, अचिन्हित दीर्घ ns_type_cmd);

पूर्णांक pidfd_get_info(पूर्णांक fd, काष्ठा pidfd_info *info);
पूर्णांक pidfd_info_mask_is_supported(uint64_t mask);

पूर्णांक pidfd_get_pid(पूर्णांक fd, pid_t *ret);
पूर्णांक pidfd_verify_pid(पूर्णांक pidfd, pid_t pid);

पूर्णांक pidfd_get_ppid(पूर्णांक fd, pid_t *ret);
पूर्णांक pidfd_get_uid(पूर्णांक fd, uid_t *ret);
पूर्णांक pidfd_get_cgroupid(पूर्णांक fd, uint64_t *ret);

पूर्णांक pidfd_get_inode_id_impl(पूर्णांक fd, uint64_t *ret);
पूर्णांक pidfd_get_inode_id(पूर्णांक fd, uint64_t *ret);
पूर्णांक pidfd_get_inode_id_self_cached(uint64_t *ret);

पूर्णांक pidfd_check_pidfs(पूर्णांक pid_fd);
