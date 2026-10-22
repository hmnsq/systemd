/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

#وصف_ڪريو SD_LOGIND_پاڙ_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#وصف_ڪريو SD_LOGIND_REBOOT_ذريعي_KEXEC                (UINT64_C(1) << 1)
#وصف_ڪريو SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#وصف_ڪريو SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سيٽ_مٿي  (UINT64_C(1) << 3)
#وصف_ڪريو SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#وصف_ڪريو SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#وصف_ڪريو SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عوامي (SD_LOGIND_پاڙ_CHECK_INHIBITORS|SD_LOGIND_REBOOT_ذريعي_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سيٽ_مٿي|SD_LOGIND_SKIP_INHIBITORS)
#وصف_ڪريو SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عوامي|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ثابت حرف *id) خالص;

bool logind_running(فراغ);
