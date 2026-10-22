/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "assert-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ಸ್ಥಿರ ವರ್ಣ *log_abort_ಸಂದೇಶ = ರಿಕ್ತ;

ಸ್ಥಿರ ಶೂನ್ಯ log_assert(
        ಪೂರ್ಣಾಙ್ಕ level,
        ಧ್ರುವ ವರ್ಣ *text,
        ಧ್ರುವ ವರ್ಣ *file,
        ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
        ಧ್ರುವ ವರ್ಣ *func,
        ಧ್ರುವ ವರ್ಣ *format) {

        ಸ್ಥಿರ ವರ್ಣ buffer[LINE_MAX];

        ಯದಿ (_likely_(LOG_PRI(level) > log_get_max_level()))
                ಪ್ರತ್ಯಾಗಮ;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (ಶೂನ್ಯ) snprintf(buffer, ಪರಿಮಾಣ buffer, format, text, file, ಸಾಲು, func);
        REENABLE_WARNING;

        log_abort_ಸಂದೇಶ = buffer;

        log_dispatch_internal(level, 0, file, ಸಾಲು, func, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buffer);
}

_noreturn_ ಶೂನ್ಯ log_assert_failed(ಧ್ರುವ ವರ್ಣ *text, ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func) {
        log_assert(LOG_CRIT, text, file, ಸಾಲು, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ ಶೂನ್ಯ log_assert_failed_unreachable(ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, ಸಾಲು, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

ಶೂನ್ಯ log_assert_failed_return(ಧ್ರುವ ವರ್ಣ *text, ಧ್ರುವ ವರ್ಣ *file, ಪೂರ್ಣಾಙ್ಕ ಸಾಲು, ಧ್ರುವ ವರ್ಣ *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        ಯದಿ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, ಸಾಲು, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, ಸಾಲು, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
