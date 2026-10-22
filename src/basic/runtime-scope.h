/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "forward.h"

تعريفنوع تعداد RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_جدول_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_جدول_LOOKUP_منزل_STRING(runtime_scope_cmdline_option, RuntimeScope);

ساكن مضمن mode_t runtime_scope_منزل_socket_گهڻائي(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        بدل (scope) {
        حالة RUNTIME_SCOPE_SYSTEM:
                أرجع 0666;

        حالة RUNTIME_SCOPE_USER:
                أرجع 0600;

        افتراضي:
                أرجع گهڻائي_INVALID;
        }
}
