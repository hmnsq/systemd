/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <threads.h>

#ضمن "alloc-util.h"
#ضمن "env-util.h"
#ضمن "iovec-util.h"
#ضمن "log.h"
#ضمن "log-context.h"
#ضمن "string-util.h"
#ضمن "strv.h"

ساكن صحيح saved_log_context_enabled = -1;
ساكن thread_local LIST_HEAD(LogContext, _log_context) = عدم;
ساكن thread_local size_t log_context_رقم_fields = 0;

bool log_context_enabled(فراغ) {
        صحيح r;

        إذا (log_get_max_level() == LOG_DEBUG)
                أرجع true;

        إذا (saved_log_context_enabled >= 0)
                أرجع saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        إذا (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        أرجع saved_log_context_enabled;
}

ساكن LogContext* log_context_attach(LogContext *c) {
        أكد(c);

        log_context_رقم_fields += strv_length(c->fields);
        log_context_رقم_fields += c->n_مدخل_iovec;
        log_context_رقم_fields += !!c->مفتاح;

        أرجع LIST_PREPEND(ll, _log_context, c);
}

ساكن LogContext* log_context_detach(LogContext *c) {
        إذا (!c)
                أرجع عدم;

        أكد(log_context_رقم_fields >= strv_length(c->fields) + c->n_مدخل_iovec +!!c->مفتاح);
        log_context_رقم_fields -= strv_length(c->fields);
        log_context_رقم_fields -= c->n_مدخل_iovec;
        log_context_رقم_fields -= !!c->مفتاح;

        LIST_REMOVE(ll, _log_context, c);
        أرجع عدم;
}

LogContext* log_context_new(ثابت حرف *مفتاح, ثابت حرف *قيمة) {
        أكد(مفتاح);
        أكد(endswith(مفتاح, "="));

        إذا (!قيمة)
                أرجع عدم;

        LIST_FOREACH(ll, i, _log_context)
                إذا (i->مفتاح == مفتاح && i->قيمة == قيمة)
                        أرجع log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        إذا (!c)
                أرجع عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .مفتاح = (حرف *) مفتاح,
                .قيمة = (حرف *) قيمة,
        };

        أرجع log_context_attach(c);
}

LogContext* log_context_new_strv(حرف **fields, bool owned) {
        إذا (!fields)
                أرجع عدم;

        LIST_FOREACH(ll, i, _log_context)
                إذا (i->fields == fields) {
                        أكد(!owned);
                        أرجع log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        إذا (!c)
                أرجع عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        أرجع log_context_attach(c);
}

LogContext* log_context_new_iov(بنية iovec *مدخل_iovec, size_t n_مدخل_iovec, bool owned) {
        إذا (!مدخل_iovec || n_مدخل_iovec == 0)
                أرجع عدم;

        LIST_FOREACH(ll, i, _log_context)
                إذا (i->مدخل_iovec == مدخل_iovec && i->n_مدخل_iovec == n_مدخل_iovec) {
                        أكد(!owned);
                        أرجع log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        إذا (!c)
                أرجع عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .مدخل_iovec = مدخل_iovec,
                .n_مدخل_iovec = n_مدخل_iovec,
                .owned = owned,
        };

        أرجع log_context_attach(c);
}

ساكن LogContext* log_context_free(LogContext *c) {
        إذا (!c)
                أرجع عدم;

        log_context_detach(c);

        إذا (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->مدخل_iovec, c->n_مدخل_iovec);
                حرر(c->مفتاح);
                حرر(c->قيمة);
        }

        أرجع mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(حرف **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        إذا (!c)
                strv_free(fields);

        أرجع c;
}

LogContext* log_context_new_iov_consume(بنية iovec *مدخل_iovec, size_t n_مدخل_iovec) {
        LogContext *c = log_context_new_iov(مدخل_iovec, n_مدخل_iovec, /* owned= */ true);
        إذا (!c)
                iovec_array_free(مدخل_iovec, n_مدخل_iovec);

        أرجع c;
}

LogContext* log_context_head(فراغ) {
        أرجع _log_context;
}

size_t log_context_رقم_contexts(فراغ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        أرجع n;
}

size_t log_context_رقم_fields_2(فراغ) {
        أرجع log_context_رقم_fields;
}

فراغ log_context_swap(LogContext **log_context, size_t *رقم_fields) {
        أكد(log_context);
        أكد(رقم_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_رقم_fields, *رقم_fields);
}

فراغ _reset_log_level(صحيح *saved_log_level) {
        أكد(saved_log_level);

        log_set_max_level(*saved_log_level);
}
