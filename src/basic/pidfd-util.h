/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <sys/pidfd.h>          /* IWYU pragma: export */

#include "forward.h"

පූර්ණ_සංඛ්‍යා pidfd_get_namespace(පූර්ණ_සංඛ්‍යා fd, unsigned long ns_type_cmd);

පූර්ණ_සංඛ්‍යා pidfd_get_info(පූර්ණ_සංඛ්‍යා fd, sa_nracanaa pidfd_info *info);
පූර්ණ_සංඛ්‍යා pidfd_info_mask_is_supported(uint64_t mask);

පූර්ණ_සංඛ්‍යා pidfd_get_pid(පූර්ණ_සංඛ්‍යා fd, pid_t *ret);
පූර්ණ_සංඛ්‍යා pidfd_verify_pid(පූර්ණ_සංඛ්‍යා pidfd, pid_t pid);

පූර්ණ_සංඛ්‍යා pidfd_get_ppid(පූර්ණ_සංඛ්‍යා fd, pid_t *ret);
පූර්ණ_සංඛ්‍යා pidfd_get_uid(පූර්ණ_සංඛ්‍යා fd, uid_t *ret);
පූර්ණ_සංඛ්‍යා pidfd_get_cgroupid(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret);

පූර්ණ_සංඛ්‍යා pidfd_get_inode_id_impl(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret);
පූර්ණ_සංඛ්‍යා pidfd_get_inode_id(පූර්ණ_සංඛ්‍යා fd, uint64_t *ret);
පූර්ණ_සංඛ්‍යා pidfd_get_inode_id_self_cached(uint64_t *ret);

පූර්ණ_සංඛ්‍යා pidfd_check_pidfs(පූර්ණ_සංඛ්‍යා pid_fd);
