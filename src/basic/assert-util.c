/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdio.h>
#include <stdlib.h>

#include "assert-util.h"
#include "errno-util.h"
#include "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
static var_na *log_abort_msg = NULL;

static void log_assert(
        ཧྲིལ level,
        const var_na *text,
        const var_na *file,
        ཧྲིལ line,
        const var_na *func,
        const var_na *format) {

        static var_na buffer[LINE_MAX];

        གལ་སྲིད (_likely_(LOG_PRI(level) > log_get_max_level()))
                ལོག;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (void) snprintf(buffer, parimaa_na buffer, format, text, file, line, func);
        REENABLE_WARNING;

        log_abort_msg = buffer;

        log_dispatch_internal(level, 0, file, line, func, NULL, NULL, NULL, NULL, buffer);
}

_noreturn_ void log_assert_failed(const var_na *text, const var_na *file, ཧྲིལ line, const var_na *func) {
        log_assert(LOG_CRIT, text, file, line, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ void log_assert_failed_unreachable(const var_na *file, ཧྲིལ line, const var_na *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, line, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

void log_assert_failed_return(const var_na *text, const var_na *file, ཧྲིལ line, const var_na *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        གལ་སྲིད (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, line, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, line, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
