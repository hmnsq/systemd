/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_TABLE_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_TABLE_LOOKUP_TO_STRING(runtime_scope_cmdline_option, RuntimeScope);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ mode_t runtime_scope_to_socket_mode(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        ବାଛ (scope) {
        ସ୍ଥିତି RUNTIME_SCOPE_SYSTEM:
                ଫେରନ୍ତୁ 0666;

        ସ୍ଥିତି RUNTIME_SCOPE_USER:
                ଫେରନ୍ତୁ 0600;

        ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                ଫେରନ୍ତୁ MODE_INVALID;
        }
}
