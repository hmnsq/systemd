/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <sys/pidfd.h>          /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

പൂർണ്ണസംഖ്യ pidfd_get_namespace(പൂർണ്ണസംഖ്യ fd, unsigned long ns_type_cmd);

പൂർണ്ണസംഖ്യ pidfd_get_info(പൂർണ്ണസംഖ്യ fd, ഘടന pidfd_info *info);
പൂർണ്ണസംഖ്യ pidfd_info_mask_is_supported(uint64_t mask);

പൂർണ്ണസംഖ്യ pidfd_get_pid(പൂർണ്ണസംഖ്യ fd, pid_t *ret);
പൂർണ്ണസംഖ്യ pidfd_verify_pid(പൂർണ്ണസംഖ്യ pidfd, pid_t pid);

പൂർണ്ണസംഖ്യ pidfd_get_ppid(പൂർണ്ണസംഖ്യ fd, pid_t *ret);
പൂർണ്ണസംഖ്യ pidfd_get_uid(പൂർണ്ണസംഖ്യ fd, uid_t *ret);
പൂർണ്ണസംഖ്യ pidfd_get_cgroupid(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);

പൂർണ്ണസംഖ്യ pidfd_get_inode_id_impl(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);
പൂർണ്ണസംഖ്യ pidfd_get_inode_id(പൂർണ്ണസംഖ്യ fd, uint64_t *ret);
പൂർണ്ണസംഖ്യ pidfd_get_inode_id_self_cached(uint64_t *ret);

പൂർണ്ണസംഖ്യ pidfd_check_pidfs(പൂർണ്ണസംഖ്യ pid_fd);
