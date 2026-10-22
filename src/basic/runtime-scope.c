/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "runtime-scope.h"
#شامل "string-table.h"

ساکن ثابت حرف* ثابت runtime_scope_جدول[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "system",
        [RUNTIME_SCOPE_USER]   = "user",
        [RUNTIME_SCOPE_GLOBAL] = "global",
};

DEFINE_STRING_جدول_LOOKUP(runtime_scope, RuntimeScope);

ساکن ثابت حرف* ثابت runtime_scope_cmdline_option_جدول[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "--system",
        [RUNTIME_SCOPE_USER]   = "--user",
        [RUNTIME_SCOPE_GLOBAL] = "--global",
};

DEFINE_STRING_جدول_LOOKUP_مقصد_STRING(runtime_scope_cmdline_option, RuntimeScope);
