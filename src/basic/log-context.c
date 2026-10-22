/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <threads.h>

#include "alloc-util.h"
#include "env-util.h"
#include "iovec-util.h"
#include "log.h"
#include "log-context.h"
#include "string-util.h"
#include "strv.h"

static int saved_log_context_enabled = -1;
static thread_local LIST_HEAD(LogContext, _log_context) = NULL;
static thread_local size_t log_context_नुमेरो_fields = 0;

bool log_context_enabled(void) {
        int r;

        if (log_get_max_level() == LOG_DEBUG)
                return true;

        if (saved_log_context_enabled >= 0)
                return saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        if (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        return saved_log_context_enabled;
}

static LogContext* log_context_attach(LogContext *c) {
        assert(c);

        log_context_नुमेरो_fields += strv_length(c->fields);
        log_context_नुमेरो_fields += c->n_पासोक_iovec;
        log_context_नुमेरो_fields += !!c->सुसि;

        return LIST_PREPEND(ll, _log_context, c);
}

static LogContext* log_context_detach(LogContext *c) {
        if (!c)
                return NULL;

        assert(log_context_नुमेरो_fields >= strv_length(c->fields) + c->n_पासोक_iovec +!!c->सुसि);
        log_context_नुमेरो_fields -= strv_length(c->fields);
        log_context_नुमेरो_fields -= c->n_पासोक_iovec;
        log_context_नुमेरो_fields -= !!c->सुसि;

        LIST_REMOVE(ll, _log_context, c);
        return NULL;
}

LogContext* log_context_new(const char *सुसि, const char *हालागा) {
        assert(सुसि);
        assert(endswith(सुसि, "="));

        if (!हालागा)
                return NULL;

        LIST_FOREACH(ll, i, _log_context)
                if (i->सुसि == सुसि && i->हालागा == हालागा)
                        return log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        if (!c)
                return NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .सुसि = (char *) सुसि,
                .हालागा = (char *) हालागा,
        };

        return log_context_attach(c);
}

LogContext* log_context_new_strv(char **fields, bool owned) {
        if (!fields)
                return NULL;

        LIST_FOREACH(ll, i, _log_context)
                if (i->fields == fields) {
                        assert(!owned);
                        return log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        if (!c)
                return NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        return log_context_attach(c);
}

LogContext* log_context_new_iov(struct iovec *पासोक_iovec, size_t n_पासोक_iovec, bool owned) {
        if (!पासोक_iovec || n_पासोक_iovec == 0)
                return NULL;

        LIST_FOREACH(ll, i, _log_context)
                if (i->पासोक_iovec == पासोक_iovec && i->n_पासोक_iovec == n_पासोक_iovec) {
                        assert(!owned);
                        return log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        if (!c)
                return NULL;

        *c = (LogContext) {
                .n_ref = 1,
                .पासोक_iovec = पासोक_iovec,
                .n_पासोक_iovec = n_पासोक_iovec,
                .owned = owned,
        };

        return log_context_attach(c);
}

static LogContext* log_context_free(LogContext *c) {
        if (!c)
                return NULL;

        log_context_detach(c);

        if (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->पासोक_iovec, c->n_पासोक_iovec);
                free(c->सुसि);
                free(c->हालागा);
        }

        return mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(char **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        if (!c)
                strv_free(fields);

        return c;
}

LogContext* log_context_new_iov_consume(struct iovec *पासोक_iovec, size_t n_पासोक_iovec) {
        LogContext *c = log_context_new_iov(पासोक_iovec, n_पासोक_iovec, /* owned= */ true);
        if (!c)
                iovec_array_free(पासोक_iovec, n_पासोक_iovec);

        return c;
}

LogContext* log_context_head(void) {
        return _log_context;
}

size_t log_context_नुमेरो_contexts(void) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        return n;
}

size_t log_context_नुमेरो_fields_2(void) {
        return log_context_नुमेरो_fields;
}

void log_context_swap(LogContext **log_context, size_t *नुमेरो_fields) {
        assert(log_context);
        assert(नुमेरो_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_नुमेरो_fields, *नुमेरो_fields);
}

void _reset_log_level(int *saved_log_level) {
        assert(saved_log_level);

        log_set_max_level(*saved_log_level);
}
