/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <threads.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "env-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "log-context.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

స్థిర పూర్ణాంకం saved_log_context_enabled = -1;
స్థిర thread_local LIST_తల(LogContext, _log_context) = ఖాళీ;
స్థిర thread_local size_t log_context_అంకె_fields = 0;

bool log_context_enabled(శూన్య) {
        పూర్ణాంకం r;

        ఒకవేళ (log_get_max_level() == LOG_DEBUG)
                తిరిగి true;

        ఒకవేళ (saved_log_context_enabled >= 0)
                తిరిగి saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ఒకవేళ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        తిరిగి saved_log_context_enabled;
}

స్థిర LogContext* log_context_attach(LogContext *c) {
        ప్రతిజ్ఞా(c);

        log_context_అంకె_fields += strv_నిడివి(c->fields);
        log_context_అంకె_fields += c->n_ఆదానం_iovec;
        log_context_అంకె_fields += !!c->కీలకం;

        తిరిగి LIST_PREPEND(ll, _log_context, c);
}

స్థిర LogContext* log_context_detach(LogContext *c) {
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        ప్రతిజ్ఞా(log_context_అంకె_fields >= strv_నిడివి(c->fields) + c->n_ఆదానం_iovec +!!c->కీలకం);
        log_context_అంకె_fields -= strv_నిడివి(c->fields);
        log_context_అంకె_fields -= c->n_ఆదానం_iovec;
        log_context_అంకె_fields -= !!c->కీలకం;

        LIST_REMOVE(ll, _log_context, c);
        తిరిగి ఖాళీ;
}

LogContext* log_context_new(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ) {
        ప్రతిజ్ఞా(కీలకం);
        ప్రతిజ్ఞా(endswith(కీలకం, "="));

        ఒకవేళ (!విలువ)
                తిరిగి ఖాళీ;

        LIST_FOREACH(ll, i, _log_context)
                ఒకవేళ (i->కీలకం == కీలకం && i->విలువ == విలువ)
                        తిరిగి log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        *c = (LogContext) {
                .n_ref = 1,
                .కీలకం = (అక్షరం *) కీలకం,
                .విలువ = (అక్షరం *) విలువ,
        };

        తిరిగి log_context_attach(c);
}

LogContext* log_context_new_strv(అక్షరం **fields, bool owned) {
        ఒకవేళ (!fields)
                తిరిగి ఖాళీ;

        LIST_FOREACH(ll, i, _log_context)
                ఒకవేళ (i->fields == fields) {
                        ప్రతిజ్ఞా(!owned);
                        తిరిగి log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        తిరిగి log_context_attach(c);
}

LogContext* log_context_new_iov(నిర్మాణం iovec *ఆదానం_iovec, size_t n_ఆదానం_iovec, bool owned) {
        ఒకవేళ (!ఆదానం_iovec || n_ఆదానం_iovec == 0)
                తిరిగి ఖాళీ;

        LIST_FOREACH(ll, i, _log_context)
                ఒకవేళ (i->ఆదానం_iovec == ఆదానం_iovec && i->n_ఆదానం_iovec == n_ఆదానం_iovec) {
                        ప్రతిజ్ఞా(!owned);
                        తిరిగి log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        *c = (LogContext) {
                .n_ref = 1,
                .ఆదానం_iovec = ఆదానం_iovec,
                .n_ఆదానం_iovec = n_ఆదానం_iovec,
                .owned = owned,
        };

        తిరిగి log_context_attach(c);
}

స్థిర LogContext* log_context_free(LogContext *c) {
        ఒకవేళ (!c)
                తిరిగి ఖాళీ;

        log_context_detach(c);

        ఒకవేళ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->ఆదానం_iovec, c->n_ఆదానం_iovec);
                విడుదల(c->కీలకం);
                విడుదల(c->విలువ);
        }

        తిరిగి mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(అక్షరం **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ఒకవేళ (!c)
                strv_free(fields);

        తిరిగి c;
}

LogContext* log_context_new_iov_consume(నిర్మాణం iovec *ఆదానం_iovec, size_t n_ఆదానం_iovec) {
        LogContext *c = log_context_new_iov(ఆదానం_iovec, n_ఆదానం_iovec, /* owned= */ true);
        ఒకవేళ (!c)
                iovec_array_free(ఆదానం_iovec, n_ఆదానం_iovec);

        తిరిగి c;
}

LogContext* log_context_తల(శూన్య) {
        తిరిగి _log_context;
}

size_t log_context_అంకె_contexts(శూన్య) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        తిరిగి n;
}

size_t log_context_అంకె_fields_2(శూన్య) {
        తిరిగి log_context_అంకె_fields;
}

శూన్య log_context_మార్పిడి(LogContext **log_context, size_t *అంకె_fields) {
        ప్రతిజ్ఞా(log_context);
        ప్రతిజ్ఞా(అంకె_fields);

        మార్పిడి_TWO(_log_context, *log_context);
        మార్పిడి_TWO(log_context_అంకె_fields, *అంకె_fields);
}

శూన్య _reset_log_level(పూర్ణాంకం *saved_log_level) {
        ప్రతిజ్ఞా(saved_log_level);

        log_సమితి_max_level(*saved_log_level);
}
