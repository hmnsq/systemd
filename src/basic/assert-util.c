/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "assert-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ਸਥਿਰ ਅੱਖਰ *log_abort_ਸੁਨੇਹਾ = ਨਿਰਾ;

ਸਥਿਰ ਖਾਲੀ log_assert(
        ਪੂਰਨ level,
        ਅਚਲ ਅੱਖਰ *ਪਾਠ,
        ਅਚਲ ਅੱਖਰ *file,
        ਪੂਰਨ ਸਤਰ,
        ਅਚਲ ਅੱਖਰ *func,
        ਅਚਲ ਅੱਖਰ *format) {

        ਸਥਿਰ ਅੱਖਰ ਬਫਰ[LINE_MAX];

        ਜੇ (_likely_(LOG_PRI(level) > log_get_max_level()))
                ਵਾਪਸ;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (ਖਾਲੀ) snprintf(ਬਫਰ, ਆਕਾਰ ਬਫਰ, format, ਪਾਠ, file, ਸਤਰ, func);
        REENABLE_WARNING;

        log_abort_ਸੁਨੇਹਾ = ਬਫਰ;

        log_dispatch_ਅੰਦਰੂਨੀ(level, 0, file, ਸਤਰ, func, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਬਫਰ);
}

_noreturn_ ਖਾਲੀ log_assert_failed(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func) {
        log_assert(LOG_CRIT, ਪਾਠ, file, ਸਤਰ, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ ਖਾਲੀ log_assert_failed_unreachable(ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, ਸਤਰ, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

ਖਾਲੀ log_assert_failed_return(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        ਜੇ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(ਪਾਠ, file, ਸਤਰ, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, ਪਾਠ, file, ਸਤਰ, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
