/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>

#அந்தர்பாவ "assert-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
நிலையான எழுத்து *log_abort_செய்தி = வெற்று;

நிலையான வெறுமை log_assert(
        முழு level,
        மாறிலி எழுத்து *text,
        மாறிலி எழுத்து *file,
        முழு வரி,
        மாறிலி எழுத்து *func,
        மாறிலி எழுத்து *format) {

        நிலையான எழுத்து buffer[LINE_MAX];

        எனில் (_likely_(LOG_PRI(level) > log_get_max_level()))
                திரும்பு;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (வெறுமை) snprintf(buffer, அளவு buffer, format, text, file, வரி, func);
        REENABLE_WARNING;

        log_abort_செய்தி = buffer;

        log_dispatch_internal(level, 0, file, வரி, func, வெற்று, வெற்று, வெற்று, வெற்று, buffer);
}

_noreturn_ வெறுமை log_assert_failed(மாறிலி எழுத்து *text, மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func) {
        log_assert(LOG_CRIT, text, file, வரி, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ வெறுமை log_assert_failed_unreachable(மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, வரி, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

வெறுமை log_assert_failed_return(மாறிலி எழுத்து *text, மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        எனில் (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, வரி, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, வரி, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
