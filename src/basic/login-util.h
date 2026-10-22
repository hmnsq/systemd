/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

#నిర్వచించు SD_LOGIND_మూలం_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#నిర్వచించు SD_LOGIND_REBOOT_ద్వారా_KEXEC                (UINT64_C(1) << 1)
#నిర్వచించు SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#నిర్వచించు SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_సమితి_పైకి  (UINT64_C(1) << 3)
#నిర్వచించు SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#నిర్వచించు SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#నిర్వచించు SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ప్రజా (SD_LOGIND_మూలం_CHECK_INHIBITORS|SD_LOGIND_REBOOT_ద్వారా_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_సమితి_పైకి|SD_LOGIND_SKIP_INHIBITORS)
#నిర్వచించు SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ప్రజా|SD_LOGIND_INTERACTIVE)

bool session_id_valid(స్థిరరాశి అక్షరం *id) శుద్ధ;

bool logind_running(శూన్య);
