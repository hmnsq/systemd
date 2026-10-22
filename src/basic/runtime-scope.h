/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_పట్టిక_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(runtime_scope_cmdline_option, RuntimeScope);

స్థిర అంతఃవరుస mode_t runtime_scope_గమ్యం_socket_బహుళకం(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        ఎంపిక (scope) {
        స్థితి RUNTIME_SCOPE_SYSTEM:
                తిరిగి 0666;

        స్థితి RUNTIME_SCOPE_USER:
                తిరిగి 0600;

        అప్రమేయ:
                తిరిగి బహుళకం_INVALID;
        }
}
