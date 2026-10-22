/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

#घोषणा SD_LOGIND_मूल_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#घोषणा SD_LOGIND_REBOOT_VIA_KEXEC                (UINT64_C(1) << 1)
#घोषणा SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#घोषणा SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_SET_UP  (UINT64_C(1) << 3)
#घोषणा SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#घोषणा SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#घोषणा SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_PUBLIC (SD_LOGIND_मूल_CHECK_INHIBITORS|SD_LOGIND_REBOOT_VIA_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_SET_UP|SD_LOGIND_SKIP_INHIBITORS)
#घोषणा SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_PUBLIC|SD_LOGIND_INTERACTIVE)

bool session_id_valid(स्थिर अक्षर *id) _pure_;

bool logind_running(व्योम);
