/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ "runtime-scope.h"
#అన్తర్భావ "string-table.h"

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి runtime_scope_పట్టిక[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "system",
        [RUNTIME_SCOPE_USER]   = "user",
        [RUNTIME_SCOPE_GLOBAL] = "global",
};

DEFINE_STRING_పట్టిక_LOOKUP(runtime_scope, RuntimeScope);

స్థిర స్థిరరాశి అక్షరం* స్థిరరాశి runtime_scope_cmdline_option_పట్టిక[_RUNTIME_SCOPE_MAX] = {
        [RUNTIME_SCOPE_SYSTEM] = "--system",
        [RUNTIME_SCOPE_USER]   = "--user",
        [RUNTIME_SCOPE_GLOBAL] = "--global",
};

DEFINE_STRING_పట్టిక_LOOKUP_గమ్యం_STRING(runtime_scope_cmdline_option, RuntimeScope);
