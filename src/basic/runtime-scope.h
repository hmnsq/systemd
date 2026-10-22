/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(runtime_scope_cmdline_option, RuntimeScope);

ਸਥਿਰ ਸਤਰਅੰਦਰ mode_t runtime_scope_ਮੰਜ਼ਿਲ_socket_ਬਹੁਲਕ(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        ਚੋਣ (scope) {
        ਹਾਲਤ RUNTIME_SCOPE_SYSTEM:
                ਵਾਪਸ 0666;

        ਹਾਲਤ RUNTIME_SCOPE_USER:
                ਵਾਪਸ 0600;

        ਮੂਲ:
                ਵਾਪਸ ਬਹੁਲਕ_INVALID;
        }
}
