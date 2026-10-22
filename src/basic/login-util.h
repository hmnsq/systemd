/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_ਜੜ੍ਹ_CHECK_INHIBITORS           (UINT64_C(1) << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_REBOOT_ਰਾਹੀਂ_KEXEC                (UINT64_C(1) << 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_SOFT_REBOOT                     (UINT64_C(1) << 2)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_ਸੈੱਟ_ਉੱਪਰ  (UINT64_C(1) << 3)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_SKIP_INHIBITORS                 (UINT64_C(1) << 4)

/* For internal use only */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_INTERACTIVE                     (UINT64_C(1) << 63)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ਜਨਤਕ (SD_LOGIND_ਜੜ੍ਹ_CHECK_INHIBITORS|SD_LOGIND_REBOOT_ਰਾਹੀਂ_KEXEC|SD_LOGIND_SOFT_REBOOT|SD_LOGIND_SOFT_REBOOT_IF_NEXTROOT_ਸੈੱਟ_ਉੱਪਰ|SD_LOGIND_SKIP_INHIBITORS)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ALL    (SD_LOGIND_SHUTDOWN_AND_SLEEP_FLAGS_ਜਨਤਕ|SD_LOGIND_INTERACTIVE)

bool session_id_valid(ਅਚਲ ਅੱਖਰ *id) ਸ਼ੁੱਧ;

bool logind_running(ਖਾਲੀ);
