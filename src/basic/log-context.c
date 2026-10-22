/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>

#include "alloc-util.h"
#include "env-util.h"
#include "iovec-util.h"
#include "log.h"
#include "log-context.h"
#include "string-util.h"
#include "strv.h"

static ចំនួនគត់ saved_log_context_enabled = -1;
static thread_local LIST_HEAD(LogContext, _log_context) = NULL;
static thread_local size_t _log_context_num_fields = 0;

bool log_context_enabled(void) {
        ចំនួនគត់ r;

        ប្រសិនបើ (log_get_max_level() == LOG_DEBUG)
                ត្រឡប់ true;

        ប្រសិនបើ (saved_log_context_enabled >= 0)
                ត្រឡប់ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ប្រសិនបើ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ត្រឡប់ saved_log_context_enabled;
}

static LogContext* log_context_attach(LogContext *c) {
        assert(c);

        _log_context_num_fields += strv_length(c->fields);
        _log_context_num_fields += c->n_input_iovec;
        _log_context_num_fields += !!c->key;

        ត្រឡប់ LIST_PREPEND(ll, _log_context, c);
}

static LogContext* log_context_detach(LogContext *c) {
        ប្រសិនបើ (!c)
                ត្រឡប់ NULL;

        assert(_log_context_num_fields >= strv_length(c->fields) + c->n_input_iovec +!!c->key);
        _log_context_num_fields -= strv_length(c->fields);
        _log_context_num_fields -= c->n_input_iovec;
        _log_context_num_fields -= !!c->key;

        LIST_REMOVE(ll, _log_context, c);
        ត្រឡប់ NULL;
}

LogContext* log_context_new(const var_na *key, const var_na *value) {
        assert(key);
        assert(endswith(key, "="));

        ប្រសិនបើ (!value)
                ត្រឡប់ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ប្រសិនបើ (i->key == key && i->value == value)
                        ត្រឡប់ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ប្រសិនបើ (!c)
                ត្រឡប់ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .key = (var_na *) key,
                .value = (var_na *) value,
        };

        ត្រឡប់ log_context_attach(c);
}

LogContext* log_context_new_strv(var_na **fields, bool owned) {
        ប្រសិនបើ (!fields)
                ត្រឡប់ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ប្រសិនបើ (i->fields == fields) {
                        assert(!owned);
                        ត្រឡប់ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ប្រសិនបើ (!c)
                ត្រឡប់ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ត្រឡប់ log_context_attach(c);
}

LogContext* log_context_new_iov(sa_nracanaa iovec *input_iovec, size_t n_input_iovec, bool owned) {
        ប្រសិនបើ (!input_iovec || n_input_iovec == 0)
                ត្រឡប់ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ប្រសិនបើ (i->input_iovec == input_iovec && i->n_input_iovec == n_input_iovec) {
                        assert(!owned);
                        ត្រឡប់ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ប្រសិនបើ (!c)
                ត្រឡប់ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .input_iovec = input_iovec,
                .n_input_iovec = n_input_iovec,
                .owned = owned,
        };

        ត្រឡប់ log_context_attach(c);
}

static LogContext* log_context_free(LogContext *c) {
        ប្រសិនបើ (!c)
                ត្រឡប់ NULL;

        log_context_detach(c);

        ប្រសិនបើ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->input_iovec, c->n_input_iovec);
                free(c->key);
                free(c->value);
        }

        ត្រឡប់ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(var_na **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ប្រសិនបើ (!c)
                strv_free(fields);

        ត្រឡប់ c;
}

LogContext* log_context_new_iov_consume(sa_nracanaa iovec *input_iovec, size_t n_input_iovec) {
        LogContext *c = log_context_new_iov(input_iovec, n_input_iovec, /* owned= */ true);
        ប្រសិនបើ (!c)
                iovec_array_free(input_iovec, n_input_iovec);

        ត្រឡប់ c;
}

LogContext* log_context_head(void) {
        ត្រឡប់ _log_context;
}

size_t log_context_num_contexts(void) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ត្រឡប់ n;
}

size_t log_context_num_fields(void) {
        ត្រឡប់ _log_context_num_fields;
}

void log_context_swap(LogContext **log_context, size_t *num_fields) {
        assert(log_context);
        assert(num_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(_log_context_num_fields, *num_fields);
}

void _reset_log_level(ចំនួនគត់ *saved_log_level) {
        assert(saved_log_level);

        log_set_max_level(*saved_log_level);
}
