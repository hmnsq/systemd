/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>

#ಅನ್ತರ್ಭಾವ "assert-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
ಸ್ಥಿರ ಅಕ್ಷರ *log_abort_ಸಂದೇಶ = ಖಾಲಿ;

ಸ್ಥಿರ ಶೂನ್ಯ log_assert(
        ಪೂರ್ಣಾಂಕ level,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
        ಪೂರ್ಣಾಂಕ ಸಾಲು,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format) {

        ಸ್ಥಿರ ಅಕ್ಷರ ಬಫರ್[LINE_MAX];

        ಆದರೆ (_likely_(LOG_PRI(level) > log_get_max_level()))
                ಹಿಂತಿರುಗಿ;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (ಶೂನ್ಯ) snprintf(ಬಫರ್, ಗಾತ್ರ ಬಫರ್, format, ಪಠ್ಯ, file, ಸಾಲು, func);
        REENABLE_WARNING;

        log_abort_ಸಂದೇಶ = ಬಫರ್;

        log_dispatch_ಆಂತರಿಕ(level, 0, file, ಸಾಲು, func, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಬಫರ್);
}

_noreturn_ ಶೂನ್ಯ log_assert_failed(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func) {
        log_assert(LOG_CRIT, ಪಠ್ಯ, file, ಸಾಲು, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ ಶೂನ್ಯ log_assert_failed_unreachable(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, ಸಾಲು, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

ಶೂನ್ಯ log_assert_failed_return(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪಠ್ಯ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file, ಪೂರ್ಣಾಂಕ ಸಾಲು, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        ಆದರೆ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(ಪಠ್ಯ, file, ಸಾಲು, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, ಪಠ್ಯ, file, ಸಾಲು, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
