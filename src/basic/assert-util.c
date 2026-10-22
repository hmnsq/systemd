/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>

#شامل "assert-util.h"
#شامل "errno-util.h"
#شامل "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ساکن حرف *log_abort_پیغام = تہی;

ساکن خالی log_assert(
        صحیح level,
        ثابت حرف *متن,
        ثابت حرف *file,
        صحیح سطر,
        ثابت حرف *func,
        ثابت حرف *format) {

        ساکن حرف بفر[LINE_MAX];

        اگر (_likely_(LOG_PRI(level) > log_get_max_level()))
                واپس;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (خالی) snprintf(بفر, حجم بفر, format, متن, file, سطر, func);
        REENABLE_WARNING;

        log_abort_پیغام = بفر;

        log_dispatch_اندرونی(level, 0, file, سطر, func, تہی, تہی, تہی, تہی, بفر);
}

_noreturn_ خالی log_assert_failed(ثابت حرف *متن, ثابت حرف *file, صحیح سطر, ثابت حرف *func) {
        log_assert(LOG_CRIT, متن, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ خالی log_assert_failed_unreachable(ثابت حرف *file, صحیح سطر, ثابت حرف *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, سطر, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

خالی log_assert_failed_return(ثابت حرف *متن, ثابت حرف *file, صحیح سطر, ثابت حرف *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        اگر (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(متن, file, سطر, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, متن, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
