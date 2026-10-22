/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <threads.h>

#شامل "alloc-util.h"
#شامل "env-util.h"
#شامل "iovec-util.h"
#شامل "log.h"
#شامل "log-context.h"
#شامل "string-util.h"
#شامل "strv.h"

ایستا صحیح saved_log_context_enabled = -1;
ایستا thread_local LIST_HEAD(LogContext, _log_context) = پوچ;
ایستا thread_local size_t log_context_رقم_fields = 0;

bool log_context_enabled(تهی) {
        صحیح r;

        اگر (log_get_max_level() == LOG_DEBUG)
                بازگشت true;

        اگر (saved_log_context_enabled >= 0)
                بازگشت saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        اگر (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        بازگشت saved_log_context_enabled;
}

ایستا LogContext* log_context_attach(LogContext *c) {
        أكد(c);

        log_context_رقم_fields += strv_length(c->fields);
        log_context_رقم_fields += c->n_درون_داد_iovec;
        log_context_رقم_fields += !!c->کلید;

        بازگشت LIST_PREPEND(ll, _log_context, c);
}

ایستا LogContext* log_context_detach(LogContext *c) {
        اگر (!c)
                بازگشت پوچ;

        أكد(log_context_رقم_fields >= strv_length(c->fields) + c->n_درون_داد_iovec +!!c->کلید);
        log_context_رقم_fields -= strv_length(c->fields);
        log_context_رقم_fields -= c->n_درون_داد_iovec;
        log_context_رقم_fields -= !!c->کلید;

        LIST_REMOVE(ll, _log_context, c);
        بازگشت پوچ;
}

LogContext* log_context_new(ثابت نویسه *کلید, ثابت نویسه *مقدار) {
        أكد(کلید);
        أكد(endswith(کلید, "="));

        اگر (!مقدار)
                بازگشت پوچ;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->کلید == کلید && i->مقدار == مقدار)
                        بازگشت log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                بازگشت پوچ;

        *c = (LogContext) {
                .n_ref = 1,
                .کلید = (نویسه *) کلید,
                .مقدار = (نویسه *) مقدار,
        };

        بازگشت log_context_attach(c);
}

LogContext* log_context_new_strv(نویسه **fields, bool owned) {
        اگر (!fields)
                بازگشت پوچ;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->fields == fields) {
                        أكد(!owned);
                        بازگشت log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                بازگشت پوچ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        بازگشت log_context_attach(c);
}

LogContext* log_context_new_iov(ساختار iovec *درون_داد_iovec, size_t n_درون_داد_iovec, bool owned) {
        اگر (!درون_داد_iovec || n_درون_داد_iovec == 0)
                بازگشت پوچ;

        LIST_FOREACH(ll, i, _log_context)
                اگر (i->درون_داد_iovec == درون_داد_iovec && i->n_درون_داد_iovec == n_درون_داد_iovec) {
                        أكد(!owned);
                        بازگشت log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        اگر (!c)
                بازگشت پوچ;

        *c = (LogContext) {
                .n_ref = 1,
                .درون_داد_iovec = درون_داد_iovec,
                .n_درون_داد_iovec = n_درون_داد_iovec,
                .owned = owned,
        };

        بازگشت log_context_attach(c);
}

ایستا LogContext* log_context_free(LogContext *c) {
        اگر (!c)
                بازگشت پوچ;

        log_context_detach(c);

        اگر (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->درون_داد_iovec, c->n_درون_داد_iovec);
                آزاد(c->کلید);
                آزاد(c->مقدار);
        }

        بازگشت mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(نویسه **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        اگر (!c)
                strv_free(fields);

        بازگشت c;
}

LogContext* log_context_new_iov_consume(ساختار iovec *درون_داد_iovec, size_t n_درون_داد_iovec) {
        LogContext *c = log_context_new_iov(درون_داد_iovec, n_درون_داد_iovec, /* owned= */ true);
        اگر (!c)
                iovec_array_free(درون_داد_iovec, n_درون_داد_iovec);

        بازگشت c;
}

LogContext* log_context_head(تهی) {
        بازگشت _log_context;
}

size_t log_context_رقم_contexts(تهی) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        بازگشت n;
}

size_t log_context_رقم_fields_2(تهی) {
        بازگشت log_context_رقم_fields;
}

تهی log_context_swap(LogContext **log_context, size_t *رقم_fields) {
        أكد(log_context);
        أكد(رقم_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_رقم_fields, *رقم_fields);
}

تهی _reset_log_level(صحیح *saved_log_level) {
        أكد(saved_log_level);

        log_set_max_level(*saved_log_level);
}
