/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>

#include "alloc-util.h"
#include "env-util.h"
#include "iovec-util.h"
#include "log.h"
#include "log-context.h"
#include "string-util.h"
#include "strv.h"

static 정수 saved_log_context_enabled = -1;
static thread_local LIST_HEAD(LogContext, _log_context) = NULL;
static thread_local size_t _log_context_num_fields = 0;

bool log_context_enabled(void) {
        정수 r;

        만약 (log_get_max_level() == LOG_DEBUG)
                반환 true;

        만약 (saved_log_context_enabled >= 0)
                반환 saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        만약 (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        반환 saved_log_context_enabled;
}

static LogContext* log_context_attach(LogContext *c) {
        assert(c);

        _log_context_num_fields += strv_length(c->fields);
        _log_context_num_fields += c->n_input_iovec;
        _log_context_num_fields += !!c->key;

        반환 LIST_PREPEND(ll, _log_context, c);
}

static LogContext* log_context_detach(LogContext *c) {
        만약 (!c)
                반환 NULL;

        assert(_log_context_num_fields >= strv_length(c->fields) + c->n_input_iovec +!!c->key);
        _log_context_num_fields -= strv_length(c->fields);
        _log_context_num_fields -= c->n_input_iovec;
        _log_context_num_fields -= !!c->key;

        LIST_REMOVE(ll, _log_context, c);
        반환 NULL;
}

LogContext* log_context_new(const char *key, const char *value) {
        assert(key);
        assert(endswith(key, "="));

        만약 (!value)
                반환 NULL;

        LIST_FOREACH(ll, i, _log_context)
                만약 (i->key == key && i->value == value)
                        반환 log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        만약 (!c)
                반환 NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .key = (char *) key,
                .value = (char *) value,
        };

        반환 log_context_attach(c);
}

LogContext* log_context_new_strv(char **fields, bool owned) {
        만약 (!fields)
                반환 NULL;

        LIST_FOREACH(ll, i, _log_context)
                만약 (i->fields == fields) {
                        assert(!owned);
                        반환 log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        만약 (!c)
                반환 NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        반환 log_context_attach(c);
}

LogContext* log_context_new_iov(struct iovec *input_iovec, size_t n_input_iovec, bool owned) {
        만약 (!input_iovec || n_input_iovec == 0)
                반환 NULL;

        LIST_FOREACH(ll, i, _log_context)
                만약 (i->input_iovec == input_iovec && i->n_input_iovec == n_input_iovec) {
                        assert(!owned);
                        반환 log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        만약 (!c)
                반환 NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .input_iovec = input_iovec,
                .n_input_iovec = n_input_iovec,
                .owned = owned,
        };

        반환 log_context_attach(c);
}

static LogContext* log_context_free(LogContext *c) {
        만약 (!c)
                반환 NULL;

        log_context_detach(c);

        만약 (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->input_iovec, c->n_input_iovec);
                free(c->key);
                free(c->value);
        }

        반환 mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(char **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        만약 (!c)
                strv_free(fields);

        반환 c;
}

LogContext* log_context_new_iov_consume(struct iovec *input_iovec, size_t n_input_iovec) {
        LogContext *c = log_context_new_iov(input_iovec, n_input_iovec, /* owned= */ true);
        만약 (!c)
                iovec_array_free(input_iovec, n_input_iovec);

        반환 c;
}

LogContext* log_context_head(void) {
        반환 _log_context;
}

size_t log_context_num_contexts(void) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        반환 n;
}

size_t log_context_num_fields(void) {
        반환 _log_context_num_fields;
}

void log_context_swap(LogContext **log_context, size_t *num_fields) {
        assert(log_context);
        assert(num_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(_log_context_num_fields, *num_fields);
}

void _reset_log_level(정수 *saved_log_level) {
        assert(saved_log_level);

        log_set_max_level(*saved_log_level);
}
