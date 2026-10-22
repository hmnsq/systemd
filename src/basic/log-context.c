/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <threads.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "env-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "log-context.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"

স্থির পূর্ণাঙ্ক saved_log_context_enabled = -1;
স্থির thread_local LIST_HEAD(LogContext, _log_context) = রিক্ত;
স্থির thread_local size_t log_context_অঙ্ক_fields = 0;

bool log_context_enabled(শূন্য) {
        পূর্ণাঙ্ক r;

        যদি (log_get_max_level() == LOG_DEBUG)
                প্রত্যাগম true;

        যদি (saved_log_context_enabled >= 0)
                প্রত্যাগম saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        যদি (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        প্রত্যাগম saved_log_context_enabled;
}

স্থির LogContext* log_context_attach(LogContext *c) {
        প্রতিজ্ঞা(c);

        log_context_অঙ্ক_fields += strv_length(c->fields);
        log_context_অঙ্ক_fields += c->n_প্রবেশ_iovec;
        log_context_অঙ্ক_fields += !!c->চাবি;

        প্রত্যাগম LIST_PREPEND(ll, _log_context, c);
}

স্থির LogContext* log_context_detach(LogContext *c) {
        যদি (!c)
                প্রত্যাগম রিক্ত;

        প্রতিজ্ঞা(log_context_অঙ্ক_fields >= strv_length(c->fields) + c->n_প্রবেশ_iovec +!!c->চাবি);
        log_context_অঙ্ক_fields -= strv_length(c->fields);
        log_context_অঙ্ক_fields -= c->n_প্রবেশ_iovec;
        log_context_অঙ্ক_fields -= !!c->চাবি;

        LIST_REMOVE(ll, _log_context, c);
        প্রত্যাগম রিক্ত;
}

LogContext* log_context_new(ধ্রুব বর্ণ *চাবি, ধ্রুব বর্ণ *মান) {
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(endswith(চাবি, "="));

        যদি (!মান)
                প্রত্যাগম রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->চাবি == চাবি && i->মান == মান)
                        প্রত্যাগম log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                প্রত্যাগম রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .চাবি = (বর্ণ *) চাবি,
                .মান = (বর্ণ *) মান,
        };

        প্রত্যাগম log_context_attach(c);
}

LogContext* log_context_new_strv(বর্ণ **fields, bool owned) {
        যদি (!fields)
                প্রত্যাগম রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->fields == fields) {
                        প্রতিজ্ঞা(!owned);
                        প্রত্যাগম log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                প্রত্যাগম রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        প্রত্যাগম log_context_attach(c);
}

LogContext* log_context_new_iov(সংরচনা iovec *প্রবেশ_iovec, size_t n_প্রবেশ_iovec, bool owned) {
        যদি (!প্রবেশ_iovec || n_প্রবেশ_iovec == 0)
                প্রত্যাগম রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->প্রবেশ_iovec == প্রবেশ_iovec && i->n_প্রবেশ_iovec == n_প্রবেশ_iovec) {
                        প্রতিজ্ঞা(!owned);
                        প্রত্যাগম log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                প্রত্যাগম রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .প্রবেশ_iovec = প্রবেশ_iovec,
                .n_প্রবেশ_iovec = n_প্রবেশ_iovec,
                .owned = owned,
        };

        প্রত্যাগম log_context_attach(c);
}

স্থির LogContext* log_context_free(LogContext *c) {
        যদি (!c)
                প্রত্যাগম রিক্ত;

        log_context_detach(c);

        যদি (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->প্রবেশ_iovec, c->n_প্রবেশ_iovec);
                মুক্ত(c->চাবি);
                মুক্ত(c->মান);
        }

        প্রত্যাগম mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(বর্ণ **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        যদি (!c)
                strv_free(fields);

        প্রত্যাগম c;
}

LogContext* log_context_new_iov_consume(সংরচনা iovec *প্রবেশ_iovec, size_t n_প্রবেশ_iovec) {
        LogContext *c = log_context_new_iov(প্রবেশ_iovec, n_প্রবেশ_iovec, /* owned= */ true);
        যদি (!c)
                iovec_array_free(প্রবেশ_iovec, n_প্রবেশ_iovec);

        প্রত্যাগম c;
}

LogContext* log_context_head(শূন্য) {
        প্রত্যাগম _log_context;
}

size_t log_context_অঙ্ক_contexts(শূন্য) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        প্রত্যাগম n;
}

size_t log_context_অঙ্ক_fields_2(শূন্য) {
        প্রত্যাগম log_context_অঙ্ক_fields;
}

শূন্য log_context_swap(LogContext **log_context, size_t *অঙ্ক_fields) {
        প্রতিজ্ঞা(log_context);
        প্রতিজ্ঞা(অঙ্ক_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_অঙ্ক_fields, *অঙ্ক_fields);
}

শূন্য _reset_log_level(পূর্ণাঙ্ক *saved_log_level) {
        প্রতিজ্ঞা(saved_log_level);

        log_set_max_level(*saved_log_level);
}
