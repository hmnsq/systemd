/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <sys/pidfd.h>          /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_namespace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned long ns_type_cmd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା pidfd_info *info);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_info_mask_is_supported(uint64_t mask);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_pid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_verify_pid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd, pid_t pid);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_ppid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, pid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_uid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uid_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_cgroupid(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id_impl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_get_inode_id_self_cached(uint64_t *ret);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pidfd_check_pidfs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା pid_fd);
