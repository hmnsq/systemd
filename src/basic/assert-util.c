/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "assert-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "log.h"

/* Akin to glibc's __abort_msg; which is private and we hence cannot
 * use here. */
স্থির বর্ণ *log_abort_বার্তা = রিক্ত;

স্থির শূন্য log_assert(
        পূর্ণাঙ্ক level,
        ধ্রুব বর্ণ *text,
        ধ্রুব বর্ণ *file,
        পূর্ণাঙ্ক সারি,
        ধ্রুব বর্ণ *func,
        ধ্রুব বর্ণ *format) {

        স্থির বর্ণ buffer[LINE_MAX];

        যদি (_likely_(LOG_PRI(level) > log_get_max_level()))
                প্রত্যাগম;

        DISABLE_WARNING_FORMAT_NONLITERAL;
        (শূন্য) snprintf(buffer, পরিমাণ buffer, format, text, file, সারি, func);
        REENABLE_WARNING;

        log_abort_বার্তা = buffer;

        log_dispatch_internal(level, 0, file, সারি, func, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buffer);
}

_noreturn_ শূন্য log_assert_failed(ধ্রুব বর্ণ *text, ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *func) {
        log_assert(LOG_CRIT, text, file, সারি, func,
                   "Assertion '%s' failed at %s:%u, function %s(). Aborting.");
        abort();
}

_noreturn_ শূন্য log_assert_failed_unreachable(ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *func) {
        log_assert(LOG_CRIT, "Code should not be reached", file, সারি, func,
                   "%s at %s:%u, function %s(). Aborting. 💥");
        abort();
}

শূন্য log_assert_failed_return(ধ্রুব বর্ণ *text, ধ্রুব বর্ণ *file, পূর্ণাঙ্ক সারি, ধ্রুব বর্ণ *func) {
        /* log_get_assert_return_is_critical is a weak symbol. It may be NULL. */
        যদি (log_get_assert_return_is_critical && log_get_assert_return_is_critical())
                log_assert_failed(text, file, সারি, func);

        PROTECT_ERRNO;
        log_assert(LOG_DEBUG, text, file, সারি, func,
                   "Assertion '%s' failed at %s:%u, function %s(), ignoring.");
}
