/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <stdlib.h>

#અન્તર્ભાવ "assert-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
સ્થિર અક્ષર *log_abort_સંદેશ = ખાલી;

સ્થિર શૂન્ય log_assert(
        પૂર્ણાંક level,
        અચલ અક્ષર *text,
        અચલ અક્ષર *file,
        પૂર્ણાંક લીટી,
        અચલ અક્ષર *func,
        અચલ અક્ષર *format) {

        સ્થિર અક્ષર buffer[LINE_MAX];

        જો (_likely_(LOG_PRI(level) > log_get_max_level()))
                પાછા;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (શૂન્ય) snprintf(buffer, કદ buffer, format, text, file, લીટી, func);
        REENABLE_WARNING;

        log_abort_સંદેશ = buffer;

        log_dispatch_internal(level, 0, file, લીટી, func, ખાલી, ખાલી, ખાલી, ખાલી, buffer);
}

_noreturn_ શૂન્ય log_assert_failed(અચલ અક્ષર *text, અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func) {
        log_assert(LOG_CRIT, text, file, લીટી, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ શૂન્ય log_assert_failed_unreachable(અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, લીટી, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

શૂન્ય log_assert_failed_return(અચલ અક્ષર *text, અચલ અક્ષર *file, પૂર્ણાંક લીટી, અચલ અક્ષર *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        જો (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, લીટી, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, લીટી, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
