/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ <sys/pidfd.h>          /* IWYU pragma: export */

#অন্তর্ভাৱ "forward.h"

পূর্ণাঙ্ক pidfd_get_namespace(পূর্ণাঙ্ক fd, unsigned long ns_type_cmd);

পূর্ণাঙ্ক pidfd_get_info(পূর্ণাঙ্ক fd, সংরচনা pidfd_info *info);
পূর্ণাঙ্ক pidfd_info_mask_is_supported(uint64_t mask);

পূর্ণাঙ্ক pidfd_get_pid(পূর্ণাঙ্ক fd, pid_t *ret);
পূর্ণাঙ্ক pidfd_verify_pid(পূর্ণাঙ্ক pidfd, pid_t pid);

পূর্ণাঙ্ক pidfd_get_ppid(পূর্ণাঙ্ক fd, pid_t *ret);
পূর্ণাঙ্ক pidfd_get_uid(পূর্ণাঙ্ক fd, uid_t *ret);
পূর্ণাঙ্ক pidfd_get_cgroupid(পূর্ণাঙ্ক fd, uint64_t *ret);

পূর্ণাঙ্ক pidfd_get_inode_id_impl(পূর্ণাঙ্ক fd, uint64_t *ret);
পূর্ণাঙ্ক pidfd_get_inode_id(পূর্ণাঙ্ক fd, uint64_t *ret);
পূর্ণাঙ্ক pidfd_get_inode_id_self_cached(uint64_t *ret);

পূর্ণাঙ্ক pidfd_check_pidfs(পূর্ণাঙ্ক pid_fd);
