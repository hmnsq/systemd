/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <stdio.h>
#समावेश <stdlib.h>

#समावेश "assert-util.h"
#समावेश "errno-util.h"
#समावेश "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
अटल अक्षर *log_abort_संदेश = शून्य;

अटल व्योम log_assert(
        पूर्णांक level,
        स्थिर अक्षर *text,
        स्थिर अक्षर *file,
        पूर्णांक पंक्ति,
        स्थिर अक्षर *func,
        स्थिर अक्षर *format) {

        अटल अक्षर बफर[LINE_MAX];

        अगर (_likely_(LOG_PRI(level) > log_get_max_level()))
                वापस;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (व्योम) snprintf(बफर, माप बफर, format, text, file, पंक्ति, func);
        REENABLE_WARNING;

        log_abort_संदेश = बफर;

        log_dispatch_internal(level, 0, file, पंक्ति, func, शून्य, शून्य, शून्य, शून्य, बफर);
}

_noreturn_ व्योम log_assert_failed(स्थिर अक्षर *text, स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func) {
        log_assert(LOG_CRIT, text, file, पंक्ति, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        पात();
}

_noreturn_ व्योम log_assert_failed_unreachable(स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, पंक्ति, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        पात();
}

व्योम log_assert_failed_return(स्थिर अक्षर *text, स्थिर अक्षर *file, पूर्णांक पंक्ति, स्थिर अक्षर *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        अगर (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, पंक्ति, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, पंक्ति, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
