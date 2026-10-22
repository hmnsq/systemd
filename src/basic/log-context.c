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
ساكن thread_local size_t _log_context_num_fields = 0;

bool log_context_enabled(فراغ) {
        صحيح r;

        ܐܢ (log_get_max_level() == LOG_DEBUG)
                ܦܢܐ true;

        ܐܢ (saved_log_context_enabled >= 0)
                ܦܢܐ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ܐܢ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ܦܢܐ saved_log_context_enabled;
}

ساكن LogContext* log_context_attach(LogContext *c) {
        أكد(c);

        _log_context_num_fields += strv_length(c->fields);
        _log_context_num_fields += c->n_input_iovec;
        _log_context_num_fields += !!c->key;

        ܦܢܐ LIST_PREPEND(ll, _log_context, c);
}

ساكن LogContext* log_context_detach(LogContext *c) {
        ܐܢ (!c)
                ܦܢܐ عدم;

        أكد(_log_context_num_fields >= strv_length(c->fields) + c->n_input_iovec +!!c->key);
        _log_context_num_fields -= strv_length(c->fields);
        _log_context_num_fields -= c->n_input_iovec;
        _log_context_num_fields -= !!c->key;

        LIST_REMOVE(ll, _log_context, c);
        ܦܢܐ عدم;
}

LogContext* log_context_new(ثابت حرف *key, ثابت حرف *value) {
        أكد(key);
        أكد(endswith(key, "="));

        ܐܢ (!value)
                ܦܢܐ عدم;

        LIST_FOREACH(ll, i, _log_context)
                ܐܢ (i->key == key && i->value == value)
                        ܦܢܐ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ܐܢ (!c)
                ܦܢܐ عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .key = (حرف *) key,
                .value = (حرف *) value,
        };

        ܦܢܐ log_context_attach(c);
}

LogContext* log_context_new_strv(حرف **fields, bool owned) {
        ܐܢ (!fields)
                ܦܢܐ عدم;

        LIST_FOREACH(ll, i, _log_context)
                ܐܢ (i->fields == fields) {
                        أكد(!owned);
                        ܦܢܐ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ܐܢ (!c)
                ܦܢܐ عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ܦܢܐ log_context_attach(c);
}

LogContext* log_context_new_iov(بنية iovec *input_iovec, size_t n_input_iovec, bool owned) {
        ܐܢ (!input_iovec || n_input_iovec == 0)
                ܦܢܐ عدم;

        LIST_FOREACH(ll, i, _log_context)
                ܐܢ (i->input_iovec == input_iovec && i->n_input_iovec == n_input_iovec) {
                        أكد(!owned);
                        ܦܢܐ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ܐܢ (!c)
                ܦܢܐ عدم;

        *c = (LogContext) {
                .n_ref = 1,
                .input_iovec = input_iovec,
                .n_input_iovec = n_input_iovec,
                .owned = owned,
        };

        ܦܢܐ log_context_attach(c);
}

ساكن LogContext* log_context_free(LogContext *c) {
        ܐܢ (!c)
                ܦܢܐ عدم;

        log_context_detach(c);

        ܐܢ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->input_iovec, c->n_input_iovec);
                حرر(c->key);
                حرر(c->value);
        }

        ܦܢܐ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(حرف **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ܐܢ (!c)
                strv_free(fields);

        ܦܢܐ c;
}

LogContext* log_context_new_iov_consume(بنية iovec *input_iovec, size_t n_input_iovec) {
        LogContext *c = log_context_new_iov(input_iovec, n_input_iovec, /* owned= */ true);
        ܐܢ (!c)
                iovec_array_free(input_iovec, n_input_iovec);

        ܦܢܐ c;
}

LogContext* log_context_head(فراغ) {
        ܦܢܐ _log_context;
}

size_t log_context_num_contexts(فراغ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ܦܢܐ n;
}

size_t log_context_num_fields(فراغ) {
        ܦܢܐ _log_context_num_fields;
}

فراغ log_context_swap(LogContext **log_context, size_t *num_fields) {
        أكد(log_context);
        أكد(num_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(_log_context_num_fields, *num_fields);
}

فراغ _reset_log_level(صحيح *saved_log_level) {
        أكد(saved_log_level);

        log_set_max_level(*saved_log_level);
}
