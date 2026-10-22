/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>

#include "alloc-util.h"
#include "env-util.h"
#include "iovec-util.h"
#include "log.h"
#include "log-context.h"
#include "string-util.h"
#include "strv.h"

static цео saved_log_context_enabled = -1;
static thread_local LIST_HEAD(LogContext, _log_context) = NULL;
static thread_local size_t _log_context_num_fields = 0;

bool log_context_enabled(void) {
        цео r;

        ако (log_get_max_level() == LOG_DEBUG)
                врати true;

        ако (saved_log_context_enabled >= 0)
                врати saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ако (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        врати saved_log_context_enabled;
}

static LogContext* log_context_attach(LogContext *c) {
        assert(c);

        _log_context_num_fields += strv_length(c->fields);
        _log_context_num_fields += c->n_input_iovec;
        _log_context_num_fields += !!c->key;

        врати LIST_PREPEND(ll, _log_context, c);
}

static LogContext* log_context_detach(LogContext *c) {
        ако (!c)
                врати NULL;

        assert(_log_context_num_fields >= strv_length(c->fields) + c->n_input_iovec +!!c->key);
        _log_context_num_fields -= strv_length(c->fields);
        _log_context_num_fields -= c->n_input_iovec;
        _log_context_num_fields -= !!c->key;

        LIST_REMOVE(ll, _log_context, c);
        врати NULL;
}

LogContext* log_context_new(const char *key, const char *value) {
        assert(key);
        assert(endswith(key, "="));

        ако (!value)
                врати NULL;

        LIST_FOREACH(ll, i, _log_context)
                ако (i->key == key && i->value == value)
                        врати log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ако (!c)
                врати NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .key = (char *) key,
                .value = (char *) value,
        };

        врати log_context_attach(c);
}

LogContext* log_context_new_strv(char **fields, bool owned) {
        ако (!fields)
                врати NULL;

        LIST_FOREACH(ll, i, _log_context)
                ако (i->fields == fields) {
                        assert(!owned);
                        врати log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ако (!c)
                врати NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        врати log_context_attach(c);
}

LogContext* log_context_new_iov(struct iovec *input_iovec, size_t n_input_iovec, bool owned) {
        ако (!input_iovec || n_input_iovec == 0)
                врати NULL;

        LIST_FOREACH(ll, i, _log_context)
                ако (i->input_iovec == input_iovec && i->n_input_iovec == n_input_iovec) {
                        assert(!owned);
                        врати log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ако (!c)
                врати NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .input_iovec = input_iovec,
                .n_input_iovec = n_input_iovec,
                .owned = owned,
        };

        врати log_context_attach(c);
}

static LogContext* log_context_free(LogContext *c) {
        ако (!c)
                врати NULL;

        log_context_detach(c);

        ако (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->input_iovec, c->n_input_iovec);
                free(c->key);
                free(c->value);
        }

        врати mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(char **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ако (!c)
                strv_free(fields);

        врати c;
}

LogContext* log_context_new_iov_consume(struct iovec *input_iovec, size_t n_input_iovec) {
        LogContext *c = log_context_new_iov(input_iovec, n_input_iovec, /* owned= */ true);
        ако (!c)
                iovec_array_free(input_iovec, n_input_iovec);

        врати c;
}

LogContext* log_context_head(void) {
        врати _log_context;
}

size_t log_context_num_contexts(void) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        врати n;
}

size_t log_context_num_fields(void) {
        врати _log_context_num_fields;
}

void log_context_swap(LogContext **log_context, size_t *num_fields) {
        assert(log_context);
        assert(num_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(_log_context_num_fields, *num_fields);
}

void _reset_log_level(цео *saved_log_level) {
        assert(saved_log_level);

        log_set_max_level(*saved_log_level);
}
