/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "runtime-scope.h"
#ಅನ್ತರ್ಭಾವ "string-table.h"

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ runtime_scope_ಕೋಷ್ಟಕ[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "system",
        [RUNTIME_SCOPE_USER]   = "user",
        [RUNTIME_SCOPE_GLOBAL] = "global",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP(runtime_scope, RuntimeScope);

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* ಸ್ಥಿರಾಂಕ runtime_scope_cmdline_option_ಕೋಷ್ಟಕ[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "--system",
        [RUNTIME_SCOPE_USER]   = "--user",
        [RUNTIME_SCOPE_GLOBAL] = "--global",
};

DEFINE_STRING_ಕೋಷ್ಟಕ_LOOKUP_ಗಮ್ಯ_STRING(runtime_scope_cmdline_option, RuntimeScope);
