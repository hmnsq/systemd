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
ساكن thread_local size_t log_context_لمب_fields = 0;

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

        log_context_لمب_fields += strv_length(c->fields);
        log_context_لمب_fields += c->n_شغرو_iovec;
        log_context_لمب_fields += !!c->مبودي;

        أرجع LIST_PREPEND(ll, _log_context, c);
}

ساكن LogContext* log_context_detach(LogContext *c) {
        إذا (!c)
                أرجع عدم;

        أكد(log_context_لمب_fields >= strv_length(c->fields) + c->n_شغرو_iovec +!!c->مبودي);
        log_context_لمب_fields -= strv_length(c->fields);
        log_context_لمب_fields -= c->n_شغرو_iovec;
        log_context_لمب_fields -= !!c->مبودي;

        LIST_REMOVE(ll, _log_context, c);
        أرجع عدم;
}

LogContext* log_context_new(ثابت حرف *مبودي, ثابت حرف *درج) {
        أكد(مبودي);
        أكد(endswith(مبودي, "="));

        إذا (!درج)
                أرجع عدم;

        LIST_FOREACH(ll, i, _log_context)
                إذا (i->مبودي == مبودي && i->درج == درج)
                        أرجع log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        إذا (!c)
                أرجع عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .مبودي = (حرف *) مبودي,
                .درج = (حرف *) درج,
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

LogContext* log_context_new_iov(بنية iovec *شغرو_iovec, size_t n_شغرو_iovec, bool owned) {
        إذا (!شغرو_iovec || n_شغرو_iovec == 0)
                أرجع عدم;

        LIST_FOREACH(ll, i, _log_context)
                إذا (i->شغرو_iovec == شغرو_iovec && i->n_شغرو_iovec == n_شغرو_iovec) {
                        أكد(!owned);
                        أرجع log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        إذا (!c)
                أرجع عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .شغرو_iovec = شغرو_iovec,
                .n_شغرو_iovec = n_شغرو_iovec,
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
                iovec_array_free(c->شغرو_iovec, c->n_شغرو_iovec);
                حرر(c->مبودي);
                حرر(c->درج);
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

LogContext* log_context_new_iov_consume(بنية iovec *شغرو_iovec, size_t n_شغرو_iovec) {
        LogContext *c = log_context_new_iov(شغرو_iovec, n_شغرو_iovec, /* owned= */ true);
        إذا (!c)
                iovec_array_free(شغرو_iovec, n_شغرو_iovec);

        أرجع c;
}

LogContext* log_context_head(فراغ) {
        أرجع _log_context;
}

size_t log_context_لمب_contexts(فراغ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        أرجع n;
}

size_t log_context_لمب_fields_2(فراغ) {
        أرجع log_context_لمب_fields;
}

فراغ log_context_swap(LogContext **log_context, size_t *لمب_fields) {
        أكد(log_context);
        أكد(لمب_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_لمب_fields, *لمب_fields);
}

فراغ _reset_log_level(صحيح *saved_log_level) {
        أكد(saved_log_level);

        log_set_max_level(*saved_log_level);
}
