/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "assert-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
स्थिर अक्षर *log_abort_संदेश = रिक्त;

स्थिर शून्य log_assert(
        पूर्णांक level,
        स्थिरांक अक्षर *text,
        स्थिरांक अक्षर *file,
        पूर्णांक ओळ,
        स्थिरांक अक्षर *func,
        स्थिरांक अक्षर *format) {

        स्थिर अक्षर buffer[LINE_MAX];

        जर (_likely_(LOG_PRI(level) > log_get_max_level()))
                परत;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (शून्य) snprintf(buffer, आकार buffer, format, text, file, ओळ, func);
        REENABLE_WARNING;

        log_abort_संदेश = buffer;

        log_dispatch_internal(level, 0, file, ओळ, func, रिक्त, रिक्त, रिक्त, रिक्त, buffer);
}

_noreturn_ शून्य log_assert_failed(स्थिरांक अक्षर *text, स्थिरांक अक्षर *file, पूर्णांक ओळ, स्थिरांक अक्षर *func) {
        log_assert(LOG_CRIT, text, file, ओळ, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        पात();
}

_noreturn_ शून्य log_assert_failed_unreachable(स्थिरांक अक्षर *file, पूर्णांक ओळ, स्थिरांक अक्षर *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, ओळ, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        पात();
}

शून्य log_assert_failed_return(स्थिरांक अक्षर *text, स्थिरांक अक्षर *file, पूर्णांक ओळ, स्थिरांक अक्षर *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        जर (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, ओळ, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, ओळ, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
