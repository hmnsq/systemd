/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <threads.h>

#समावेश "alloc-util.h"
#समावेश "env-util.h"
#समावेश "iovec-util.h"
#समावेश "log.h"
#समावेश "log-context.h"
#समावेश "string-util.h"
#समावेश "strv.h"

अटल पूर्णांक saved_log_context_enabled = -1;
अटल thread_local LIST_शीर्ष(LogContext, _log_context) = शून्य;
अटल thread_local माप_प्रकार log_context_अंक_fields = 0;

bool log_context_enabled(व्योम) {
        पूर्णांक r;

        अगर (log_get_max_level() == LOG_DEBUG)
                वापस true;

        अगर (saved_log_context_enabled >= 0)
                वापस saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        अगर (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        वापस saved_log_context_enabled;
}

अटल LogContext* log_context_attach(LogContext *c) {
        निश्चित(c);

        log_context_अंक_fields += strv_लंबाई(c->fields);
        log_context_अंक_fields += c->n_निवेश_iovec;
        log_context_अंक_fields += !!c->कुंजी;

        वापस LIST_PREPEND(ll, _log_context, c);
}

अटल LogContext* log_context_detach(LogContext *c) {
        अगर (!c)
                वापस शून्य;

        निश्चित(log_context_अंक_fields >= strv_लंबाई(c->fields) + c->n_निवेश_iovec +!!c->कुंजी);
        log_context_अंक_fields -= strv_लंबाई(c->fields);
        log_context_अंक_fields -= c->n_निवेश_iovec;
        log_context_अंक_fields -= !!c->कुंजी;

        LIST_REMOVE(ll, _log_context, c);
        वापस शून्य;
}

LogContext* log_context_new(स्थिर अक्षर *कुंजी, स्थिर अक्षर *मूल्य) {
        निश्चित(कुंजी);
        निश्चित(endswith(कुंजी, "="));

        अगर (!मूल्य)
                वापस शून्य;

        LIST_FOREACH(ll, i, _log_context)
                अगर (i->कुंजी == कुंजी && i->मूल्य == मूल्य)
                        वापस log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        अगर (!c)
                वापस शून्य;

        *c = (LogContext) {
                .n_ref = 1,
                .कुंजी = (अक्षर *) कुंजी,
                .मूल्य = (अक्षर *) मूल्य,
        };

        वापस log_context_attach(c);
}

LogContext* log_context_new_strv(अक्षर **fields, bool owned) {
        अगर (!fields)
                वापस शून्य;

        LIST_FOREACH(ll, i, _log_context)
                अगर (i->fields == fields) {
                        निश्चित(!owned);
                        वापस log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        अगर (!c)
                वापस शून्य;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        वापस log_context_attach(c);
}

LogContext* log_context_new_iov(काष्ठा iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec, bool owned) {
        अगर (!निवेश_iovec || n_निवेश_iovec == 0)
                वापस शून्य;

        LIST_FOREACH(ll, i, _log_context)
                अगर (i->निवेश_iovec == निवेश_iovec && i->n_निवेश_iovec == n_निवेश_iovec) {
                        निश्चित(!owned);
                        वापस log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        अगर (!c)
                वापस शून्य;

        *c = (LogContext) {
                .n_ref = 1,
                .निवेश_iovec = निवेश_iovec,
                .n_निवेश_iovec = n_निवेश_iovec,
                .owned = owned,
        };

        वापस log_context_attach(c);
}

अटल LogContext* log_context_free(LogContext *c) {
        अगर (!c)
                वापस शून्य;

        log_context_detach(c);

        अगर (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->निवेश_iovec, c->n_निवेश_iovec);
                मुक्त(c->कुंजी);
                मुक्त(c->मूल्य);
        }

        वापस mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(अक्षर **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        अगर (!c)
                strv_free(fields);

        वापस c;
}

LogContext* log_context_new_iov_consume(काष्ठा iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec) {
        LogContext *c = log_context_new_iov(निवेश_iovec, n_निवेश_iovec, /* owned= */ true);
        अगर (!c)
                iovec_array_free(निवेश_iovec, n_निवेश_iovec);

        वापस c;
}

LogContext* log_context_शीर्ष(व्योम) {
        वापस _log_context;
}

माप_प्रकार log_context_अंक_contexts(व्योम) {
        माप_प्रकार n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        वापस n;
}

माप_प्रकार log_context_अंक_fields_2(व्योम) {
        वापस log_context_अंक_fields;
}

व्योम log_context_swap(LogContext **log_context, माप_प्रकार *अंक_fields) {
        निश्चित(log_context);
        निश्चित(अंक_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_अंक_fields, *अंक_fields);
}

व्योम _reset_log_level(पूर्णांक *saved_log_level) {
        निश्चित(saved_log_level);

        log_set_max_level(*saved_log_level);
}
