/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <threads.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "env-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "log-context.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "strv.h"

સ્થિર પૂર્ણાંક saved_log_context_enabled = -1;
સ્થિર thread_local LIST_HEAD(LogContext, _log_context) = ખાલી;
સ્થિર thread_local size_t log_context_અંક_fields = 0;

bool log_context_enabled(શૂન્ય) {
        પૂર્ણાંક r;

        જો (log_get_max_level() == LOG_DEBUG)
                પાછા true;

        જો (saved_log_context_enabled >= 0)
                પાછા saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        જો (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        પાછા saved_log_context_enabled;
}

સ્થિર LogContext* log_context_attach(LogContext *c) {
        પ્રતિજ્ઞા(c);

        log_context_અંક_fields += strv_length(c->fields);
        log_context_અંક_fields += c->n_આગત_iovec;
        log_context_અંક_fields += !!c->ચાવી;

        પાછા LIST_PREPEND(ll, _log_context, c);
}

સ્થિર LogContext* log_context_detach(LogContext *c) {
        જો (!c)
                પાછા ખાલી;

        પ્રતિજ્ઞા(log_context_અંક_fields >= strv_length(c->fields) + c->n_આગત_iovec +!!c->ચાવી);
        log_context_અંક_fields -= strv_length(c->fields);
        log_context_અંક_fields -= c->n_આગત_iovec;
        log_context_અંક_fields -= !!c->ચાવી;

        LIST_REMOVE(ll, _log_context, c);
        પાછા ખાલી;
}

LogContext* log_context_new(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત) {
        પ્રતિજ્ઞા(ચાવી);
        પ્રતિજ્ઞા(endswith(ચાવી, "="));

        જો (!કિંમત)
                પાછા ખાલી;

        LIST_FOREACH(ll, i, _log_context)
                જો (i->ચાવી == ચાવી && i->કિંમત == કિંમત)
                        પાછા log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        જો (!c)
                પાછા ખાલી;

        *c = (LogContext) {
                .n_ref = 1,
                .ચાવી = (અક્ષર *) ચાવી,
                .કિંમત = (અક્ષર *) કિંમત,
        };

        પાછા log_context_attach(c);
}

LogContext* log_context_new_strv(અક્ષર **fields, bool owned) {
        જો (!fields)
                પાછા ખાલી;

        LIST_FOREACH(ll, i, _log_context)
                જો (i->fields == fields) {
                        પ્રતિજ્ઞા(!owned);
                        પાછા log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        જો (!c)
                પાછા ખાલી;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        પાછા log_context_attach(c);
}

LogContext* log_context_new_iov(સંરચના iovec *આગત_iovec, size_t n_આગત_iovec, bool owned) {
        જો (!આગત_iovec || n_આગત_iovec == 0)
                પાછા ખાલી;

        LIST_FOREACH(ll, i, _log_context)
                જો (i->આગત_iovec == આગત_iovec && i->n_આગત_iovec == n_આગત_iovec) {
                        પ્રતિજ્ઞા(!owned);
                        પાછા log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        જો (!c)
                પાછા ખાલી;

        *c = (LogContext) {
                .n_ref = 1,
                .આગત_iovec = આગત_iovec,
                .n_આગત_iovec = n_આગત_iovec,
                .owned = owned,
        };

        પાછા log_context_attach(c);
}

સ્થિર LogContext* log_context_free(LogContext *c) {
        જો (!c)
                પાછા ખાલી;

        log_context_detach(c);

        જો (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->આગત_iovec, c->n_આગત_iovec);
                મુક્ત(c->ચાવી);
                મુક્ત(c->કિંમત);
        }

        પાછા mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(અક્ષર **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        જો (!c)
                strv_free(fields);

        પાછા c;
}

LogContext* log_context_new_iov_consume(સંરચના iovec *આગત_iovec, size_t n_આગત_iovec) {
        LogContext *c = log_context_new_iov(આગત_iovec, n_આગત_iovec, /* owned= */ true);
        જો (!c)
                iovec_array_free(આગત_iovec, n_આગત_iovec);

        પાછા c;
}

LogContext* log_context_head(શૂન્ય) {
        પાછા _log_context;
}

size_t log_context_અંક_contexts(શૂન્ય) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        પાછા n;
}

size_t log_context_અંક_fields_2(શૂન્ય) {
        પાછા log_context_અંક_fields;
}

શૂન્ય log_context_swap(LogContext **log_context, size_t *અંક_fields) {
        પ્રતિજ્ઞા(log_context);
        પ્રતિજ્ઞા(અંક_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_અંક_fields, *અંક_fields);
}

શૂન્ય _reset_log_level(પૂર્ણાંક *saved_log_level) {
        પ્રતિજ્ઞા(saved_log_level);

        log_set_max_level(*saved_log_level);
}
