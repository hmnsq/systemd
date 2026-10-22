/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <sys/pidfd.h>          /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

పూర్ణాంకం pidfd_get_namespace(పూర్ణాంకం fd, గుర్తులేని దీర్ఘ ns_type_cmd);

పూర్ణాంకం pidfd_get_info(పూర్ణాంకం fd, నిర్మాణం pidfd_info *info);
పూర్ణాంకం pidfd_info_ముసుగు_is_supported(uint64_t ముసుగు_3);

పూర్ణాంకం pidfd_get_pid(పూర్ణాంకం fd, pid_t *ret);
పూర్ణాంకం pidfd_verify_pid(పూర్ణాంకం pidfd, pid_t pid);

పూర్ణాంకం pidfd_get_ppid(పూర్ణాంకం fd, pid_t *ret);
పూర్ణాంకం pidfd_get_uid(పూర్ణాంకం fd, uid_t *ret);
పూర్ణాంకం pidfd_get_cgroupid(పూర్ణాంకం fd, uint64_t *ret);

పూర్ణాంకం pidfd_get_inode_id_impl(పూర్ణాంకం fd, uint64_t *ret);
పూర్ణాంకం pidfd_get_inode_id(పూర్ణాంకం fd, uint64_t *ret);
పూర్ణాంకం pidfd_get_inode_id_self_cached(uint64_t *ret);

పూర్ణాంకం pidfd_check_pidfs(పూర్ణాంకం pid_fd);
