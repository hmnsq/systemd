/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

قسمتعریف شمار RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_جدول_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_جدول_LOOKUP_مقصد_STRING(runtime_scope_cmdline_option, RuntimeScope);

ساکن اندرونی mode_t runtime_scope_مقصد_socket_منوال(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        انتخاب (scope) {
        حالت RUNTIME_SCOPE_SYSTEM:
                واپس 0666;

        حالت RUNTIME_SCOPE_USER:
                واپس 0600;

        طےشدہ:
                واپس منوال_INVALID;
        }
}
