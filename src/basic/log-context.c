/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>

#include "alloc-util.h"
#include "env-util.h"
#include "iovec-util.h"
#include "log.h"
#include "log-context.h"
#include "string-util.h"
#include "strv.h"

static ຈຳນວນເຕັມ saved_log_context_enabled = -1;
static thread_local LIST_HEAD(LogContext, _log_context) = NULL;
static thread_local size_t _log_context_num_fields = 0;

bool log_context_enabled(void) {
        ຈຳນວນເຕັມ r;

        ຖ້າ (log_get_max_level() == LOG_DEBUG)
                ສົ່ງຄືນ true;

        ຖ້າ (saved_log_context_enabled >= 0)
                ສົ່ງຄືນ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ຖ້າ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ສົ່ງຄືນ saved_log_context_enabled;
}

static LogContext* log_context_attach(LogContext *c) {
        assert(c);

        _log_context_num_fields += strv_length(c->fields);
        _log_context_num_fields += c->n_input_iovec;
        _log_context_num_fields += !!c->key;

        ສົ່ງຄືນ LIST_PREPEND(ll, _log_context, c);
}

static LogContext* log_context_detach(LogContext *c) {
        ຖ້າ (!c)
                ສົ່ງຄືນ NULL;

        assert(_log_context_num_fields >= strv_length(c->fields) + c->n_input_iovec +!!c->key);
        _log_context_num_fields -= strv_length(c->fields);
        _log_context_num_fields -= c->n_input_iovec;
        _log_context_num_fields -= !!c->key;

        LIST_REMOVE(ll, _log_context, c);
        ສົ່ງຄືນ NULL;
}

LogContext* log_context_new(const var_na *key, const var_na *value) {
        assert(key);
        assert(endswith(key, "="));

        ຖ້າ (!value)
                ສົ່ງຄືນ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ຖ້າ (i->key == key && i->value == value)
                        ສົ່ງຄືນ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ຖ້າ (!c)
                ສົ່ງຄືນ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .key = (var_na *) key,
                .value = (var_na *) value,
        };

        ສົ່ງຄືນ log_context_attach(c);
}

LogContext* log_context_new_strv(var_na **fields, bool owned) {
        ຖ້າ (!fields)
                ສົ່ງຄືນ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ຖ້າ (i->fields == fields) {
                        assert(!owned);
                        ສົ່ງຄືນ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ຖ້າ (!c)
                ສົ່ງຄືນ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ສົ່ງຄືນ log_context_attach(c);
}

LogContext* log_context_new_iov(sa_nracanaa iovec *input_iovec, size_t n_input_iovec, bool owned) {
        ຖ້າ (!input_iovec || n_input_iovec == 0)
                ສົ່ງຄືນ NULL;

        LIST_FOREACH(ll, i, _log_context)
                ຖ້າ (i->input_iovec == input_iovec && i->n_input_iovec == n_input_iovec) {
                        assert(!owned);
                        ສົ່ງຄືນ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ຖ້າ (!c)
                ສົ່ງຄືນ NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .input_iovec = input_iovec,
                .n_input_iovec = n_input_iovec,
                .owned = owned,
        };

        ສົ່ງຄືນ log_context_attach(c);
}

static LogContext* log_context_free(LogContext *c) {
        ຖ້າ (!c)
                ສົ່ງຄືນ NULL;

        log_context_detach(c);

        ຖ້າ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->input_iovec, c->n_input_iovec);
                free(c->key);
                free(c->value);
        }

        ສົ່ງຄືນ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(var_na **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ຖ້າ (!c)
                strv_free(fields);

        ສົ່ງຄືນ c;
}

LogContext* log_context_new_iov_consume(sa_nracanaa iovec *input_iovec, size_t n_input_iovec) {
        LogContext *c = log_context_new_iov(input_iovec, n_input_iovec, /* owned= */ true);
        ຖ້າ (!c)
                iovec_array_free(input_iovec, n_input_iovec);

        ສົ່ງຄືນ c;
}

LogContext* log_context_head(void) {
        ສົ່ງຄືນ _log_context;
}

size_t log_context_num_contexts(void) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ສົ່ງຄືນ n;
}

size_t log_context_num_fields(void) {
        ສົ່ງຄືນ _log_context_num_fields;
}

void log_context_swap(LogContext **log_context, size_t *num_fields) {
        assert(log_context);
        assert(num_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(_log_context_num_fields, *num_fields);
}

void _reset_log_level(ຈຳນວນເຕັມ *saved_log_level) {
        assert(saved_log_level);

        log_set_max_level(*saved_log_level);
}
