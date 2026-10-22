/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <threads.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "iovec-util.h"
#شامل "log.h"
#شامل "log-context.h"
#شامل "string-util.h"
#شامل "strv.h"

ساکن صحیح saved_log_context_enabled = -1;
ساکن thread_local LIST_سر(LogContext, _log_context) = تہی;
ساکن thread_local size_t log_context_عدد_fields = 0;

bool log_context_enabled(خالی) {
        صحیح r;

        اگر (log_get_max_level() == LOG_DEBUG)
                واپس true;

        اگر (saved_log_context_enabled >= 0)
                واپس saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        اگر (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        واپس saved_log_context_enabled;
}

ساکن LogContext* log_context_attach(LogContext *c) {
        أكد(c);

        log_context_عدد_fields += strv_لمبائی(c->fields);
        log_context_عدد_fields += c->n_داخلہ_iovec;
        log_context_عدد_fields += !!c->کلید;

        واپس LIST_PREPEND(ll, _log_context, c);
}

ساکن LogContext* log_context_detach(LogContext *c) {
        اگر (!c)
                واپس تہی;

        أكد(log_context_عدد_fields >= strv_لمبائی(c->fields) + c->n_داخلہ_iovec +!!c->کلید);
        log_context_عدد_fields -= strv_لمبائی(c->fields);
        log_context_عدد_fields -= c->n_داخلہ_iovec;
        log_context_عدد_fields -= !!c->کلید;

        LIST_REMOVE(ll, _log_context, c);
        واپس تہی;
}

LogContext* log_context_new(ثابت حرف *کلید, ثابت حرف *قدر) {
        أكد(کلید);
        أكد(endswith(کلید, "="));

        اگر (!قدر)
                واپس تہی;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->کلید == کلید && i->قدر == قدر)
                        واپس log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                واپس تہی;

        *c = (LogContext) {
                .n_ref = 1,
                .کلید = (حرف *) کلید,
                .قدر = (حرف *) قدر,
        };

        واپس log_context_attach(c);
}

LogContext* log_context_new_strv(حرف **fields, bool owned) {
        اگر (!fields)
                واپس تہی;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->fields == fields) {
                        أكد(!owned);
                        واپس log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                واپس تہی;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        واپس log_context_attach(c);
}

LogContext* log_context_new_iov(ساخت iovec *داخلہ_iovec, size_t n_داخلہ_iovec, bool owned) {
        اگر (!داخلہ_iovec || n_داخلہ_iovec == 0)
                واپس تہی;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->داخلہ_iovec == داخلہ_iovec && i->n_داخلہ_iovec == n_داخلہ_iovec) {
                        أكد(!owned);
                        واپس log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                واپس تہی;

        *c = (LogContext) {
                .n_ref = 1,
                .داخلہ_iovec = داخلہ_iovec,
                .n_داخلہ_iovec = n_داخلہ_iovec,
                .owned = owned,
        };

        واپس log_context_attach(c);
}

ساکن LogContext* log_context_free(LogContext *c) {
        اگر (!c)
                واپس تہی;

        log_context_detach(c);

        اگر (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->داخلہ_iovec, c->n_داخلہ_iovec);
                آزاد(c->کلید);
                آزاد(c->قدر);
        }

        واپس mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(حرف **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        اگر (!c)
                strv_free(fields);

        واپس c;
}

LogContext* log_context_new_iov_consume(ساخت iovec *داخلہ_iovec, size_t n_داخلہ_iovec) {
        LogContext *c = log_context_new_iov(داخلہ_iovec, n_داخلہ_iovec, /* owned= */ true);
        اگر (!c)
                iovec_array_free(داخلہ_iovec, n_داخلہ_iovec);

        واپس c;
}

LogContext* log_context_سر(خالی) {
        واپس _log_context;
}

size_t log_context_عدد_contexts(خالی) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        واپس n;
}

size_t log_context_عدد_fields_2(خالی) {
        واپس log_context_عدد_fields;
}

خالی log_context_تبادلہ(LogContext **log_context, size_t *عدد_fields) {
        أكد(log_context);
        أكد(عدد_fields);

        تبادلہ_TWO(_log_context, *log_context);
        تبادلہ_TWO(log_context_عدد_fields, *عدد_fields);
}

خالی _reset_log_level(صحیح *saved_log_level) {
        أكد(saved_log_level);

        log_سیٹ_max_level(*saved_log_level);
}
