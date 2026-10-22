/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "runtime-scope.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-table.h"

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ runtime_scope_ਸਾਰਣੀ[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "system",
        [RUNTIME_SCOPE_USER]   = "user",
        [RUNTIME_SCOPE_GLOBAL] = "global",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP(runtime_scope, RuntimeScope);

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* ਅਚਲ runtime_scope_cmdline_option_ਸਾਰਣੀ[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "--system",
        [RUNTIME_SCOPE_USER]   = "--user",
        [RUNTIME_SCOPE_GLOBAL] = "--global",
};

DEFINE_STRING_ਸਾਰਣੀ_LOOKUP_ਮੰਜ਼ਿਲ_STRING(runtime_scope_cmdline_option, RuntimeScope);
