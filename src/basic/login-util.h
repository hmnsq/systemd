/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

#ضمن "forward.h"

#تعریف_کړه SD_LOGIND_ریښه_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#تعریف_کړه SD_LOGIND_REBOOT_له_لارې_KEXEC                (UINT64_C(1) << 1)
#تعریف_کړه SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#تعریف_کړه SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سېټ_پورته  (UINT64_C(1) << 3)
#تعریف_کړه SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#تعریف_کړه SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#تعریف_کړه SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عامه (SD_LOGIND_ریښه_CHECK_INHIBITORS|SD_LOGIND_REBOOT_له_لارې_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سېټ_پورته|SD_LOGIND_SKIP_INHIBITORS)
#تعریف_کړه SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عامه|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ثابت حرف *id) خالص;

bool logind_running(فراغ);
