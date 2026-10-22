/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना RuntimeScope {
        RUNTIME_SCOPE_SYSTEM,           /* for the system */
        RUNTIME_SCOPE_USER,             /* for a user */
        RUNTIME_SCOPE_GLOBAL,           /* for all users */
        _RUNTIME_SCOPE_MAX,
        _RUNTIME_SCOPE_INVALID = -EINVAL,
} RuntimeScope;

DECLARE_STRING_सारणी_LOOKUP(runtime_scope, RuntimeScope);
DECLARE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(runtime_scope_cmdline_option, RuntimeScope);

स्थिर अन्तःस्थ mode_t runtime_scope_गन्तव्यम्_socket_बहुलकम्(RuntimeScope scope) {
        /* Returns the right socket mode to use for binding AF_UNIX sockets intended for the specified
         * scope. If system mode is selected the whole system can connect to it, if user mode is selected
         * only the user can connect to it. */

        विकल्प (scope) {
        पक्ष RUNTIME_SCOPE_SYSTEM:
                प्रत्यागम 0666;

        पक्ष RUNTIME_SCOPE_USER:
                प्रत्यागम 0600;

        स्वभाव:
                प्रत्यागम बहुलकम्_INVALID;
        }
}
