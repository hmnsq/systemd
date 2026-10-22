/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "assert-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
സ്ഥിതം അക്ഷരം *log_abort_സന്ദേശം = ശൂന്യ;

സ്ഥിതം ശൂന്യം log_assert(
        പൂർണ്ണസംഖ്യ level,
        സ്ഥിരം അക്ഷരം *text,
        സ്ഥിരം അക്ഷരം *file,
        പൂർണ്ണസംഖ്യ വരി,
        സ്ഥിരം അക്ഷരം *func,
        സ്ഥിരം അക്ഷരം *format) {

        സ്ഥിതം അക്ഷരം buffer[LINE_MAX];

        എങ്കിൽ (_likely_(LOG_PRI(level) > log_get_max_level()))
                തിരികെ;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (ശൂന്യം) snprintf(buffer, വലുപ്പം buffer, format, text, file, വരി, func);
        REENABLE_WARNING;

        log_abort_സന്ദേശം = buffer;

        log_dispatch_internal(level, 0, file, വരി, func, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, buffer);
}

_noreturn_ ശൂന്യം log_assert_failed(സ്ഥിരം അക്ഷരം *text, സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func) {
        log_assert(LOG_CRIT, text, file, വരി, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ ശൂന്യം log_assert_failed_unreachable(സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, വരി, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

ശൂന്യം log_assert_failed_return(സ്ഥിരം അക്ഷരം *text, സ്ഥിരം അക്ഷരം *file, പൂർണ്ണസംഖ്യ വരി, സ്ഥിരം അക്ഷരം *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        എങ്കിൽ (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, വരി, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, വരി, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
