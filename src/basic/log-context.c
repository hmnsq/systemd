/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <threads.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "env-util.h"
#অন্তর্ভাৱ "iovec-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "log-context.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "strv.h"

স্থির পূর্ণাঙ্ক saved_log_context_enabled = -1;
স্থির thread_local LIST_HEAD(LogContext, _log_context) = রিক্ত;
স্থির thread_local size_t log_context_অঙ্ক_fields = 0;

bool log_context_enabled(শূন্য) {
        পূর্ণাঙ্ক r;

        যদি (log_get_max_level() == LOG_DEBUG)
                ঘূৰাই true;

        যদি (saved_log_context_enabled >= 0)
                ঘূৰাই saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        যদি (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ঘূৰাই saved_log_context_enabled;
}

স্থির LogContext* log_context_attach(LogContext *c) {
        প্রতিজ্ঞা(c);

        log_context_অঙ্ক_fields += strv_length(c->fields);
        log_context_অঙ্ক_fields += c->n_প্ৰবেশ_iovec;
        log_context_অঙ্ক_fields += !!c->চাবি;

        ঘূৰাই LIST_PREPEND(ll, _log_context, c);
}

স্থির LogContext* log_context_detach(LogContext *c) {
        যদি (!c)
                ঘূৰাই রিক্ত;

        প্রতিজ্ঞা(log_context_অঙ্ক_fields >= strv_length(c->fields) + c->n_প্ৰবেশ_iovec +!!c->চাবি);
        log_context_অঙ্ক_fields -= strv_length(c->fields);
        log_context_অঙ্ক_fields -= c->n_প্ৰবেশ_iovec;
        log_context_অঙ্ক_fields -= !!c->চাবি;

        LIST_REMOVE(ll, _log_context, c);
        ঘূৰাই রিক্ত;
}

LogContext* log_context_new(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান) {
        প্রতিজ্ঞা(চাবি);
        প্রতিজ্ঞা(endswith(চাবি, "="));

        যদি (!মান)
                ঘূৰাই রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->চাবি == চাবি && i->মান == মান)
                        ঘূৰাই log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ঘূৰাই রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .চাবি = (ৱর্ণ *) চাবি,
                .মান = (ৱর্ণ *) মান,
        };

        ঘূৰাই log_context_attach(c);
}

LogContext* log_context_new_strv(ৱর্ণ **fields, bool owned) {
        যদি (!fields)
                ঘূৰাই রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->fields == fields) {
                        প্রতিজ্ঞা(!owned);
                        ঘূৰাই log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ঘূৰাই রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ঘূৰাই log_context_attach(c);
}

LogContext* log_context_new_iov(সংরচনা iovec *প্ৰবেশ_iovec, size_t n_প্ৰবেশ_iovec, bool owned) {
        যদি (!প্ৰবেশ_iovec || n_প্ৰবেশ_iovec == 0)
                ঘূৰাই রিক্ত;

        LIST_FOREACH(ll, i, _log_context)
                যদি (i->প্ৰবেশ_iovec == প্ৰবেশ_iovec && i->n_প্ৰবেশ_iovec == n_প্ৰবেশ_iovec) {
                        প্রতিজ্ঞা(!owned);
                        ঘূৰাই log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        যদি (!c)
                ঘূৰাই রিক্ত;

        *c = (LogContext) {
                .n_ref = 1,
                .প্ৰবেশ_iovec = প্ৰবেশ_iovec,
                .n_প্ৰবেশ_iovec = n_প্ৰবেশ_iovec,
                .owned = owned,
        };

        ঘূৰাই log_context_attach(c);
}

স্থির LogContext* log_context_free(LogContext *c) {
        যদি (!c)
                ঘূৰাই রিক্ত;

        log_context_detach(c);

        যদি (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->প্ৰবেশ_iovec, c->n_প্ৰবেশ_iovec);
                মুক্ত(c->চাবি);
                মুক্ত(c->মান);
        }

        ঘূৰাই mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(ৱর্ণ **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        যদি (!c)
                strv_free(fields);

        ঘূৰাই c;
}

LogContext* log_context_new_iov_consume(সংরচনা iovec *প্ৰবেশ_iovec, size_t n_প্ৰবেশ_iovec) {
        LogContext *c = log_context_new_iov(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, /* owned= */ true);
        যদি (!c)
                iovec_array_free(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec);

        ঘূৰাই c;
}

LogContext* log_context_head(শূন্য) {
        ঘূৰাই _log_context;
}

size_t log_context_অঙ্ক_contexts(শূন্য) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ঘূৰাই n;
}

size_t log_context_অঙ্ক_fields_2(শূন্য) {
        ঘূৰাই log_context_অঙ্ক_fields;
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
