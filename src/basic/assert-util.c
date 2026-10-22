/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <stdlib.h>

#شامل "assert-util.h"
#شامل "errno-util.h"
#شامل "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ایستا نویسه *log_abort_پیام = پوچ;

ایستا تهی log_assert(
        صحیح level,
        ثابت نویسه *text,
        ثابت نویسه *file,
        صحیح سطر,
        ثابت نویسه *func,
        ثابت نویسه *format) {

        ایستا نویسه buffer[LINE_MAX];

        اگر (_likely_(LOG_PRI(level) > log_get_max_level()))
                بازگشت;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (تهی) snprintf(buffer, اندازه buffer, format, text, file, سطر, func);
        REENABLE_WARNING;

        log_abort_پیام = buffer;

        log_dispatch_internal(level, 0, file, سطر, func, پوچ, پوچ, پوچ, پوچ, buffer);
}

_noreturn_ تهی log_assert_failed(ثابت نویسه *text, ثابت نویسه *file, صحیح سطر, ثابت نویسه *func) {
        log_assert(LOG_CRIT, text, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ تهی log_assert_failed_unreachable(ثابت نویسه *file, صحیح سطر, ثابت نویسه *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, سطر, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

تهی log_assert_failed_return(ثابت نویسه *text, ثابت نویسه *file, صحیح سطر, ثابت نویسه *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        اگر (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, سطر, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, سطر, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
