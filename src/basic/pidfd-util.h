/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/pidfd.h>          /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

पूर्णाङ्क pidfd_get_namespace(पूर्णाङ्क fd, अचिह्नित दीर्घ ns_type_cmd);

पूर्णाङ्क pidfd_get_info(पूर्णाङ्क fd, संरचना pidfd_info *info);
पूर्णाङ्क pidfd_info_आवरणम्_is_supported(uint64_t आवरणम्_3);

पूर्णाङ्क pidfd_get_pid(पूर्णाङ्क fd, pid_t *ret);
पूर्णाङ्क pidfd_verify_pid(पूर्णाङ्क pidfd, pid_t pid);

पूर्णाङ्क pidfd_get_ppid(पूर्णाङ्क fd, pid_t *ret);
पूर्णाङ्क pidfd_get_uid(पूर्णाङ्क fd, uid_t *ret);
पूर्णाङ्क pidfd_get_cgroupid(पूर्णाङ्क fd, uint64_t *ret);

पूर्णाङ्क pidfd_get_inode_id_impl(पूर्णाङ्क fd, uint64_t *ret);
पूर्णाङ्क pidfd_get_inode_id(पूर्णाङ्क fd, uint64_t *ret);
पूर्णाङ्क pidfd_get_inode_id_self_cached(uint64_t *ret);

पूर्णाङ्क pidfd_check_pidfs(पूर्णाङ्क pid_fd);
