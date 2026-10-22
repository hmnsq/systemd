/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

typedef ga_nanaa RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

static anta_hstha mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        switch (scope) {
        pak_sa RUNTIME_SCOPE_SYSTEM:
                ត្រឡប់ 0666;

        pak_sa RUNTIME_SCOPE_USER:
                ត្រឡប់ 0600;

        default:
                ត្រឡប់ MODE_INVALID;
        }
}
