/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_ಬೇರು_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_REBOOT_ಮೂಲಕ_KEXEC                (UINT64_C(1) << 1)
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_ಗಣ_ಮೇಲೆ  (UINT64_C(1) << 3)
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ಬಹಿರಂಗ (SD_LOGIND_ಬೇರು_CHECK_INHIBITORS|SD_LOGIND_REBOOT_ಮೂಲಕ_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_ಗಣ_ಮೇಲೆ|SD_LOGIND_SKIP_INHIBITORS)
#ವ್ಯಾಖ್ಯಾನಿಸು SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ಬಹಿರಂಗ|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *id) ಶುದ್ಧ;

bool logind_running(ಶೂನ್ಯ);
