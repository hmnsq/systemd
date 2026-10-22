/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

#परिभाषय SD_LOGIND_मूलम्_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#परिभाषय SD_LOGIND_REBOOT_द्वारा_KEXEC                (UINT64_C(1) << 1)
#परिभाषय SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#परिभाषय SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_समुच्चयः_ऊर्ध्वम्  (UINT64_C(1) << 3)
#परिभाषय SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#परिभाषय SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#परिभाषय SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_लौकिकम् (SD_LOGIND_मूलम्_CHECK_INHIBITORS|SD_LOGIND_REBOOT_द्वारा_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_समुच्चयः_ऊर्ध्वम्|SD_LOGIND_SKIP_INHIBITORS)
#परिभाषय SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_लौकिकम्|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ध्रुव वर्ण *id) विशुद्धम्;

bool logind_running(शून्य);
