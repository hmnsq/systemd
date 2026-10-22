/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "runtime-scope.h"
#अन्तर्भाव "string-table.h"

स्थिर ध्रुव वर्ण* ध्रुव runtime_scope_सारणी[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "system",
        [RUNTIME_SCOPE_USER]   = "user",
        [RUNTIME_SCOPE_GLOBAL] = "global",
};

DEFINE_STRING_सारणी_LOOKUP(runtime_scope, RuntimeScope);

स्थिर ध्रुव वर्ण* ध्रुव runtime_scope_cmdline_option_सारणी[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "--system",
        [RUNTIME_SCOPE_USER]   = "--user",
        [RUNTIME_SCOPE_GLOBAL] = "--global",
};

DEFINE_STRING_सारणी_LOOKUP_गन्तव्यम्_STRING(runtime_scope_cmdline_option, RuntimeScope);
