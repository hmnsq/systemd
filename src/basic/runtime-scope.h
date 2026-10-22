/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(runtime_scope_cmdline_option, RuntimeScope);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ mode_t runtime_scope_ಗಮ್ಯ_socket_ಬಹುಲಕ(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        ಆಯ್ಕೆ (scope) {
        ಸ್ಥಿತಿ RUNTIME_SCOPE_SYSTEM:
                ಹಿಂತಿರುಗಿ 0666;

        ಸ್ಥಿತಿ RUNTIME_SCOPE_USER:
                ಹಿಂತಿರುಗಿ 0600;

        ಪೂರ್ವನಿಯೋಜಿತ:
                ಹಿಂತಿರುಗಿ ಬಹುಲಕ_INVALID;
        }
}
