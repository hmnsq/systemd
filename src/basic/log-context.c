/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <threads.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "log-context.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

স্থির পূর্ণসংখ্যা saved_log_context_enabled = -1;
স্থির thread_local LIST_শীর্ষ(LogContext, _log_context) = ফাঁকা;
স্থির thread_local size_t log_context_অঙ্ক_fields = 0;

bool log_context_enabled(শূন্য) {
        পূর্ণসংখ্যা r;

        যদি (log_get_max_level() == LOG_DEBUG)
                ফেরত true;

        যদি (saved_log_context_enabled >= 0)
                ফেরত saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        যদি (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ফেরত saved_log_context_enabled;
}

স্থির LogContext* log_context_attach(LogContext *c) {
        প্রতিজ্ঞা(c);

        log_context_অঙ্ক_fields += strv_দৈর্ঘ্য(c->fields);
        log_context_অঙ্ক_fields += c->n_প্রবেশ_iovec;
        log_context_অঙ্ক_fields += !!c->চাবি;

        ফেরত LIST_PREPEND(ll, _log_context, c);
}

স্থির LogContext* log_context_detach(LogContext *c) {
        যদি (!c)
                ফেরত ফাঁকা;

        প্রতিজ্ঞা(log_context_অঙ্ক_fields >= strv_দৈর্ঘ্য(c->fields) + c->n_প্রবেশ_iovec +!!c->চাবি);
        log_context_অঙ্ক_fields -= strv_দৈর্ঘ্য(c->fields);
        log_context_অঙ্ক_fields -= c->n_প্রবেশ_iovec;
        log_context_অঙ্ক_fields -= !!c->চাবি;

        LIST_REMOVE(ll, _log_context, c);
        ফেরত ফাঁকা;
}

LogContext* log_context_new(ধ্রুবক অক্ষর *চাবি, ধ্রুবক অক্ষর *মান) {
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(endswith(চাবি, "="));

        যদি (!মান)
                ফেরত ফাঁকা;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->চাবি == চাবি && i->মান == মান)
                        ফেরত log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ফেরত ফাঁকা;

        *c = (LogContext) {
                .n_ref = 1,
                .চাবি = (অক্ষর *) চাবি,
                .মান = (অক্ষর *) মান,
        };

        ফেরত log_context_attach(c);
}

LogContext* log_context_new_strv(অক্ষর **fields, bool owned) {
        যদি (!fields)
                ফেরত ফাঁকা;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->fields == fields) {
                        প্রতিজ্ঞা(!owned);
                        ফেরত log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ফেরত ফাঁকা;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ফেরত log_context_attach(c);
}

LogContext* log_context_new_iov(কাঠামো iovec *প্রবেশ_iovec, size_t n_প্রবেশ_iovec, bool owned) {
        যদি (!প্রবেশ_iovec || n_প্রবেশ_iovec == 0)
                ফেরত ফাঁকা;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->প্রবেশ_iovec == প্রবেশ_iovec && i->n_প্রবেশ_iovec == n_প্রবেশ_iovec) {
                        প্রতিজ্ঞা(!owned);
                        ফেরত log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ফেরত ফাঁকা;

        *c = (LogContext) {
                .n_ref = 1,
                .প্রবেশ_iovec = প্রবেশ_iovec,
                .n_প্রবেশ_iovec = n_প্রবেশ_iovec,
                .owned = owned,
        };

        ফেরত log_context_attach(c);
}

স্থির LogContext* log_context_free(LogContext *c) {
        যদি (!c)
                ফেরত ফাঁকা;

        log_context_detach(c);

        যদি (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->প্রবেশ_iovec, c->n_প্রবেশ_iovec);
                মুক্ত(c->চাবি);
                মুক্ত(c->মান);
        }

        ফেরত mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(অক্ষর **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        যদি (!c)
                strv_free(fields);

        ফেরত c;
}

LogContext* log_context_new_iov_consume(কাঠামো iovec *প্রবেশ_iovec, size_t n_প্রবেশ_iovec) {
        LogContext *c = log_context_new_iov(প্রবেশ_iovec, n_প্রবেশ_iovec, /* owned= */ true);
        যদি (!c)
                iovec_array_free(প্রবেশ_iovec, n_প্রবেশ_iovec);

        ফেরত c;
}

LogContext* log_context_শীর্ষ(শূন্য) {
        ফেরত _log_context;
}

size_t log_context_অঙ্ক_contexts(শূন্য) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ফেরত n;
}

size_t log_context_অঙ্ক_fields_2(শূন্য) {
        ফেরত log_context_অঙ্ক_fields;
}

শূন্য log_context_swap(LogContext **log_context, size_t *অঙ্ক_fields) {
        প্রতিজ্ঞা(log_context);
        প্রতিজ্ঞা(অঙ্ক_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_অঙ্ক_fields, *অঙ্ক_fields);
}

শূন্য _reset_log_level(পূর্ণসংখ্যা *saved_log_level) {
        প্রতিজ্ঞা(saved_log_level);

        log_set_max_level(*saved_log_level);
}
