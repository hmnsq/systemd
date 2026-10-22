/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

#تعریف_کرو SD_LOGIND_جڑ_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#تعریف_کرو SD_LOGIND_REBOOT_بذریعہ_KEXEC                (UINT64_C(1) << 1)
#تعریف_کرو SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#تعریف_کرو SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سیٹ_اوپر  (UINT64_C(1) << 3)
#تعریف_کرو SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#تعریف_کرو SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#تعریف_کرو SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عوامی (SD_LOGIND_جڑ_CHECK_INHIBITORS|SD_LOGIND_REBOOT_بذریعہ_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_سیٹ_اوپر|SD_LOGIND_SKIP_INHIBITORS)
#تعریف_کرو SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_عوامی|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ثابت حرف *id) خالص;

bool logind_running(خالی);
