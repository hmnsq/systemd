/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

प्रकार क्रमागत RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

अटल अंतरभूत mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        चयन (scope) {
        हाल RUNTIME_SCOPE_SYSTEM:
                वापस 0666;

        हाल RUNTIME_SCOPE_USER:
                वापस 0600;

        शेष:
                वापस MODE_INVALID;
        }
}
