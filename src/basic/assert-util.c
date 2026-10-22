/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <stdlib.h>

#ضمن "assert-util.h"
#ضمن "errno-util.h"
#ضمن "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ساكن حرف *log_abort_پیغام = عدم;

ساكن فراغ log_assert(
        صحيح level,
        ثابت حرف *text,
        ثابت حرف *file,
        صحيح سطر,
        ثابت حرف *func,
        ثابت حرف *format) {

        ساكن حرف buffer[LINE_MAX];

        إذا (_likely_(LOG_PRI(level) > log_get_max_level()))
                أرجع;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (فراغ) snprintf(buffer, حجم buffer, format, text, file, سطر, func);
        REENABLE_WARNING;

        log_abort_پیغام = buffer;

        log_dispatch_internal(level, 0, file, سطر, func, عدم, عدم, عدم, عدم, buffer);
}

_noreturn_ فراغ log_assert_failed(ثابت حرف *text, ثابت حرف *file, صحيح سطر, ثابت حرف *func) {
        log_assert(LOG_CRIT, text, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ فراغ log_assert_failed_unreachable(ثابت حرف *file, صحيح سطر, ثابت حرف *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, سطر, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

فراغ log_assert_failed_return(ثابت حرف *text, ثابت حرف *file, صحيح سطر, ثابت حرف *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        إذا (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, سطر, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
