/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "assert-util.h"
#ضمن "errno-util.h"
#ضمن "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ساكن حرف *log_abort_نياپو = عدم;

ساكن فراغ log_assert(
        صحيح level,
        ثابت حرف *متن,
        ثابت حرف *file,
        صحيح سٽ,
        ثابت حرف *func,
        ثابت حرف *format) {

        ساكن حرف بفر[LINE_MAX];

        إذا (_likely_(LOG_PRI(level) > log_get_max_level()))
                أرجع;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (فراغ) snprintf(بفر, حجم بفر, format, متن, file, سٽ, func);
        REENABLE_WARNING;

        log_abort_نياپو = بفر;

        log_dispatch_اندروني(level, 0, file, سٽ, func, عدم, عدم, عدم, عدم, بفر);
}

_noreturn_ فراغ log_assert_failed(ثابت حرف *متن, ثابت حرف *file, صحيح سٽ, ثابت حرف *func) {
        log_assert(LOG_CRIT, متن, file, سٽ, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ فراغ log_assert_failed_unreachable(ثابت حرف *file, صحيح سٽ, ثابت حرف *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, سٽ, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

فراغ log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحيح سٽ, ثابت حرف *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        إذا (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(متن, file, سٽ, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, متن, file, سٽ, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
