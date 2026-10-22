/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        ವಿಕಲ್ಪ (scope) {
        ಪಕ್ಷ RUNTIME_SCOPE_SYSTEM:
                ಪ್ರತ್ಯಾಗಮ 0666;

        ಪಕ್ಷ RUNTIME_SCOPE_USER:
                ಪ್ರತ್ಯಾಗಮ 0600;

        ಸ್ವಭಾವ:
                ಪ್ರತ್ಯಾಗಮ MODE_INVALID;
        }
}
