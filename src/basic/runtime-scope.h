/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ গণনা RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

স্থির অন্তঃস্থ mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        বিকল্প (scope) {
        পক্ষ RUNTIME_SCOPE_SYSTEM:
                প্রত্যাগম 0666;

        পক্ষ RUNTIME_SCOPE_USER:
                প্রত্যাগম 0600;

        স্বভাব:
                প্রত্যাগম MODE_INVALID;
        }
}
