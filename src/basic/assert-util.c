/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "assert-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
स्थिर वर्ण *log_abort_सन्देशः = रिक्त;

स्थिर शून्य log_assert(
        पूर्णाङ्क level,
        ध्रुव वर्ण *text,
        ध्रुव वर्ण *file,
        पूर्णाङ्क पङ्क्तिः,
        ध्रुव वर्ण *func,
        ध्रुव वर्ण *format) {

        स्थिर वर्ण buffer[LINE_MAX];

        जइ (_likely_(LOG_PRI(level) > log_get_max_level()))
                प्रत्यागम;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (शून्य) snprintf(buffer, परिमाण buffer, format, text, file, पङ्क्तिः, func);
        REENABLE_WARNING;

        log_abort_सन्देशः = buffer;

        log_dispatch_internal(level, 0, file, पङ्क्तिः, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

_noreturn_ शून्य log_assert_failed(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *func) {
        log_assert(LOG_CRIT, text, file, पङ्क्तिः, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ शून्य log_assert_failed_unreachable(ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, पङ्क्तिः, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

शून्य log_assert_failed_return(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क पङ्क्तिः, ध्रुव वर्ण *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        जइ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, पङ्क्तिः, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, पङ्क्तिः, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
