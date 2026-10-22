/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

நிலையான உள்வரி mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        தேர்வு (scope) {
        நிலை RUNTIME_SCOPE_SYSTEM:
                திரும்பு 0666;

        நிலை RUNTIME_SCOPE_USER:
                திரும்பு 0600;

        இயல்பு:
                திரும்பு MODE_INVALID;
        }
}
