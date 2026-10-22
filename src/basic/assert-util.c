/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "assert-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
स्थिर वर्ण *log_abort_संदेश = रिक्त;

स्थिर शून्य log_assert(
        पूर्णाङ्क level,
        ध्रुव वर्ण *text,
        ध्रुव वर्ण *file,
        पूर्णाङ्क पंक्ति,
        ध्रुव वर्ण *func,
        ध्रुव वर्ण *format) {

        स्थिर वर्ण buffer[LINE_MAX];

        यदि (_likely_(LOG_PRI(level) > log_get_max_level()))
                प्रत्यागम;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (शून्य) snprintf(buffer, परिमाण buffer, format, text, file, पंक्ति, func);
        REENABLE_WARNING;

        log_abort_संदेश = buffer;

        log_dispatch_internal(level, 0, file, पंक्ति, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

_noreturn_ शून्य log_assert_failed(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *func) {
        log_assert(LOG_CRIT, text, file, पंक्ति, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        पात();
}

_noreturn_ शून्य log_assert_failed_unreachable(ध्रुव वर्ण *file, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, पंक्ति, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        पात();
}

शून्य log_assert_failed_return(ध्रुव वर्ण *text, ध्रुव वर्ण *file, पूर्णाङ्क पंक्ति, ध्रुव वर्ण *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        यदि (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, पंक्ति, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, पंक्ति, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
