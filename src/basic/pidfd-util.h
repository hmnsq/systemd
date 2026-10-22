/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <sys/pidfd.h>          /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

পূর্ণসংখ্যা pidfd_get_namespace(পূর্ণসংখ্যা fd, unsigned long ns_type_cmd);

পূর্ণসংখ্যা pidfd_get_info(পূর্ণসংখ্যা fd, কাঠামো pidfd_info *info);
পূর্ণসংখ্যা pidfd_info_mask_is_supported(uint64_t mask);

পূর্ণসংখ্যা pidfd_get_pid(পূর্ণসংখ্যা fd, pid_t *ret);
পূর্ণসংখ্যা pidfd_verify_pid(পূর্ণসংখ্যা pidfd, pid_t pid);

পূর্ণসংখ্যা pidfd_get_ppid(পূর্ণসংখ্যা fd, pid_t *ret);
পূর্ণসংখ্যা pidfd_get_uid(পূর্ণসংখ্যা fd, uid_t *ret);
পূর্ণসংখ্যা pidfd_get_cgroupid(পূর্ণসংখ্যা fd, uint64_t *ret);

পূর্ণসংখ্যা pidfd_get_inode_id_impl(পূর্ণসংখ্যা fd, uint64_t *ret);
পূর্ণসংখ্যা pidfd_get_inode_id(পূর্ণসংখ্যা fd, uint64_t *ret);
পূর্ণসংখ্যা pidfd_get_inode_id_self_cached(uint64_t *ret);

পূর্ণসংখ্যা pidfd_check_pidfs(পূর্ণসংখ্যা pid_fd);
