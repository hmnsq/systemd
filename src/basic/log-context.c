/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "env-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log-context.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

ਸਥਿਰ ਪੂਰਨ saved_log_context_enabled = -1;
ਸਥਿਰ thread_local LIST_ਸਿਰ(LogContext, _log_context) = ਨਿਰਾ;
ਸਥਿਰ thread_local size_t log_context_ਅੰਕ_fields = 0;

bool log_context_enabled(ਖਾਲੀ) {
        ਪੂਰਨ r;

        ਜੇ (log_get_max_level() == LOG_DEBUG)
                ਵਾਪਸ true;

        ਜੇ (saved_log_context_enabled >= 0)
                ਵਾਪਸ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ਜੇ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ਵਾਪਸ saved_log_context_enabled;
}

ਸਥਿਰ LogContext* log_context_attach(LogContext *c) {
        ਪ੍ਰਤਿਜ੍ਞਾ(c);

        log_context_ਅੰਕ_fields += strv_ਲੰਮਾਈ(c->fields);
        log_context_ਅੰਕ_fields += c->n_ਨਿਵੇਸ਼_iovec;
        log_context_ਅੰਕ_fields += !!c->ਕੁੰਜੀ;

        ਵਾਪਸ LIST_PREPEND(ll, _log_context, c);
}

ਸਥਿਰ LogContext* log_context_detach(LogContext *c) {
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(log_context_ਅੰਕ_fields >= strv_ਲੰਮਾਈ(c->fields) + c->n_ਨਿਵੇਸ਼_iovec +!!c->ਕੁੰਜੀ);
        log_context_ਅੰਕ_fields -= strv_ਲੰਮਾਈ(c->fields);
        log_context_ਅੰਕ_fields -= c->n_ਨਿਵੇਸ਼_iovec;
        log_context_ਅੰਕ_fields -= !!c->ਕੁੰਜੀ;

        LIST_REMOVE(ll, _log_context, c);
        ਵਾਪਸ ਨਿਰਾ;
}

LogContext* log_context_new(ਅਚਲ ਅੱਖਰ *ਕੁੰਜੀ, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਕੁੰਜੀ);
        ਪ੍ਰਤਿਜ੍ਞਾ(endswith(ਕੁੰਜੀ, "="));

        ਜੇ (!ਮੁੱਲ)
                ਵਾਪਸ ਨਿਰਾ;

        LIST_FOREACH(ll, i, _log_context)
                ਜੇ (i->ਕੁੰਜੀ == ਕੁੰਜੀ && i->ਮੁੱਲ == ਮੁੱਲ)
                        ਵਾਪਸ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        *c = (LogContext) {
                .n_ref = 1,
                .ਕੁੰਜੀ = (ਅੱਖਰ *) ਕੁੰਜੀ,
                .ਮੁੱਲ = (ਅੱਖਰ *) ਮੁੱਲ,
        };

        ਵਾਪਸ log_context_attach(c);
}

LogContext* log_context_new_strv(ਅੱਖਰ **fields, bool owned) {
        ਜੇ (!fields)
                ਵਾਪਸ ਨਿਰਾ;

        LIST_FOREACH(ll, i, _log_context)
                ਜੇ (i->fields == fields) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(!owned);
                        ਵਾਪਸ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ਵਾਪਸ log_context_attach(c);
}

LogContext* log_context_new_iov(ਬਣਤਰ iovec *ਨਿਵੇਸ਼_iovec, size_t n_ਨਿਵੇਸ਼_iovec, bool owned) {
        ਜੇ (!ਨਿਵੇਸ਼_iovec || n_ਨਿਵੇਸ਼_iovec == 0)
                ਵਾਪਸ ਨਿਰਾ;

        LIST_FOREACH(ll, i, _log_context)
                ਜੇ (i->ਨਿਵੇਸ਼_iovec == ਨਿਵੇਸ਼_iovec && i->n_ਨਿਵੇਸ਼_iovec == n_ਨਿਵੇਸ਼_iovec) {
                        ਪ੍ਰਤਿਜ੍ਞਾ(!owned);
                        ਵਾਪਸ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        *c = (LogContext) {
                .n_ref = 1,
                .ਨਿਵੇਸ਼_iovec = ਨਿਵੇਸ਼_iovec,
                .n_ਨਿਵੇਸ਼_iovec = n_ਨਿਵੇਸ਼_iovec,
                .owned = owned,
        };

        ਵਾਪਸ log_context_attach(c);
}

ਸਥਿਰ LogContext* log_context_free(LogContext *c) {
        ਜੇ (!c)
                ਵਾਪਸ ਨਿਰਾ;

        log_context_detach(c);

        ਜੇ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->ਨਿਵੇਸ਼_iovec, c->n_ਨਿਵੇਸ਼_iovec);
                ਮੁਕਤ(c->ਕੁੰਜੀ);
                ਮੁਕਤ(c->ਮੁੱਲ);
        }

        ਵਾਪਸ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(ਅੱਖਰ **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ਜੇ (!c)
                strv_free(fields);

        ਵਾਪਸ c;
}

LogContext* log_context_new_iov_consume(ਬਣਤਰ iovec *ਨਿਵੇਸ਼_iovec, size_t n_ਨਿਵੇਸ਼_iovec) {
        LogContext *c = log_context_new_iov(ਨਿਵੇਸ਼_iovec, n_ਨਿਵੇਸ਼_iovec, /* owned= */ true);
        ਜੇ (!c)
                iovec_array_free(ਨਿਵੇਸ਼_iovec, n_ਨਿਵੇਸ਼_iovec);

        ਵਾਪਸ c;
}

LogContext* log_context_ਸਿਰ(ਖਾਲੀ) {
        ਵਾਪਸ _log_context;
}

size_t log_context_ਅੰਕ_contexts(ਖਾਲੀ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ਵਾਪਸ n;
}

size_t log_context_ਅੰਕ_fields_2(ਖਾਲੀ) {
        ਵਾਪਸ log_context_ਅੰਕ_fields;
}

ਖਾਲੀ log_context_ਅਦਲਾ_ਬਦਲੀ(LogContext **log_context, size_t *ਅੰਕ_fields) {
        ਪ੍ਰਤਿਜ੍ਞਾ(log_context);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅੰਕ_fields);

        ਅਦਲਾ_ਬਦਲੀ_TWO(_log_context, *log_context);
        ਅਦਲਾ_ਬਦਲੀ_TWO(log_context_ਅੰਕ_fields, *ਅੰਕ_fields);
}

ਖਾਲੀ _reset_log_level(ਪੂਰਨ *saved_log_level) {
        ਪ੍ਰਤਿਜ੍ਞਾ(saved_log_level);

        log_ਸੈੱਟ_max_level(*saved_log_level);
}
