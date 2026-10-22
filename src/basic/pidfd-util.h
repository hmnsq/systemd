/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/pidfd.h>          /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਪੂਰਨ pidfd_get_namespace(ਪੂਰਨ fd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ns_type_cmd);

ਪੂਰਨ pidfd_get_info(ਪੂਰਨ fd, ਬਣਤਰ pidfd_info *info);
ਪੂਰਨ pidfd_info_ਨਕਾਬ_is_supported(uint64_t ਨਕਾਬ_3);

ਪੂਰਨ pidfd_get_pid(ਪੂਰਨ fd, pid_t *ret);
ਪੂਰਨ pidfd_verify_pid(ਪੂਰਨ pidfd, pid_t pid);

ਪੂਰਨ pidfd_get_ppid(ਪੂਰਨ fd, pid_t *ret);
ਪੂਰਨ pidfd_get_uid(ਪੂਰਨ fd, uid_t *ret);
ਪੂਰਨ pidfd_get_cgroupid(ਪੂਰਨ fd, uint64_t *ret);

ਪੂਰਨ pidfd_get_inode_id_impl(ਪੂਰਨ fd, uint64_t *ret);
ਪੂਰਨ pidfd_get_inode_id(ਪੂਰਨ fd, uint64_t *ret);
ਪੂਰਨ pidfd_get_inode_id_self_cached(uint64_t *ret);

ਪੂਰਨ pidfd_check_pidfs(ਪੂਰਨ pid_fd);
