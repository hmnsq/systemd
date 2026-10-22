/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <sys/pidfd.h>          /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

પૂર્ણાંક pidfd_get_namespace(પૂર્ણાંક fd, unsigned long ns_type_cmd);

પૂર્ણાંક pidfd_get_info(પૂર્ણાંક fd, સંરચના pidfd_info *info);
પૂર્ણાંક pidfd_info_mask_is_supported(uint64_t mask);

પૂર્ણાંક pidfd_get_pid(પૂર્ણાંક fd, pid_t *ret);
પૂર્ણાંક pidfd_verify_pid(પૂર્ણાંક pidfd, pid_t pid);

પૂર્ણાંક pidfd_get_ppid(પૂર્ણાંક fd, pid_t *ret);
પૂર્ણાંક pidfd_get_uid(પૂર્ણાંક fd, uid_t *ret);
પૂર્ણાંક pidfd_get_cgroupid(પૂર્ણાંક fd, uint64_t *ret);

પૂર્ણાંક pidfd_get_inode_id_impl(પૂર્ણાંક fd, uint64_t *ret);
પૂર્ણાંક pidfd_get_inode_id(પૂર્ણાંક fd, uint64_t *ret);
પૂર્ણાંક pidfd_get_inode_id_self_cached(uint64_t *ret);

પૂર્ણાંક pidfd_check_pidfs(પૂર્ણાંક pid_fd);
