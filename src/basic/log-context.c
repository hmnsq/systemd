/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <threads.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "env-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "log-context.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

स्थिर पूर्णाङ्क saved_log_context_enabled = -1;
स्थिर thread_local LIST_HEAD(LogContext, _log_context) = रिक्त;
स्थिर thread_local माप_प्रकार log_context_अंक_fields = 0;

bool log_context_enabled(शून्य) {
        पूर्णाङ्क r;

        यदि (log_get_max_level() == LOG_DEBUG)
                प्रत्यागम true;

        यदि (saved_log_context_enabled >= 0)
                प्रत्यागम saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        यदि (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        प्रत्यागम saved_log_context_enabled;
}

स्थिर LogContext* log_context_attach(LogContext *c) {
        प्रतिज्ञा(c);

        log_context_अंक_fields += strv_length(c->fields);
        log_context_अंक_fields += c->n_निवेश_iovec;
        log_context_अंक_fields += !!c->कुंजी;

        प्रत्यागम LIST_PREPEND(ll, _log_context, c);
}

स्थिर LogContext* log_context_detach(LogContext *c) {
        यदि (!c)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(log_context_अंक_fields >= strv_length(c->fields) + c->n_निवेश_iovec +!!c->कुंजी);
        log_context_अंक_fields -= strv_length(c->fields);
        log_context_अंक_fields -= c->n_निवेश_iovec;
        log_context_अंक_fields -= !!c->कुंजी;

        LIST_REMOVE(ll, _log_context, c);
        प्रत्यागम रिक्त;
}

LogContext* log_context_new(ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य) {
        प्रतिज्ञा(कुंजी);
        प्रतिज्ञा(endswith(कुंजी, "="));

        यदि (!मूल्य)
                प्रत्यागम रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->कुंजी == कुंजी && i->मूल्य == मूल्य)
                        प्रत्यागम log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                प्रत्यागम रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .कुंजी = (वर्ण *) कुंजी,
                .मूल्य = (वर्ण *) मूल्य,
        };

        प्रत्यागम log_context_attach(c);
}

LogContext* log_context_new_strv(वर्ण **fields, bool owned) {
        यदि (!fields)
                प्रत्यागम रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->fields == fields) {
                        प्रतिज्ञा(!owned);
                        प्रत्यागम log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                प्रत्यागम रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        प्रत्यागम log_context_attach(c);
}

LogContext* log_context_new_iov(संरचना iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec, bool owned) {
        यदि (!निवेश_iovec || n_निवेश_iovec == 0)
                प्रत्यागम रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->निवेश_iovec == निवेश_iovec && i->n_निवेश_iovec == n_निवेश_iovec) {
                        प्रतिज्ञा(!owned);
                        प्रत्यागम log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                प्रत्यागम रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .निवेश_iovec = निवेश_iovec,
                .n_निवेश_iovec = n_निवेश_iovec,
                .owned = owned,
        };

        प्रत्यागम log_context_attach(c);
}

स्थिर LogContext* log_context_free(LogContext *c) {
        यदि (!c)
                प्रत्यागम रिक्त;

        log_context_detach(c);

        यदि (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->निवेश_iovec, c->n_निवेश_iovec);
                मुक्त(c->कुंजी);
                मुक्त(c->मूल्य);
        }

        प्रत्यागम mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(वर्ण **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        यदि (!c)
                strv_free(fields);

        प्रत्यागम c;
}

LogContext* log_context_new_iov_consume(संरचना iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec) {
        LogContext *c = log_context_new_iov(निवेश_iovec, n_निवेश_iovec, /* owned= */ true);
        यदि (!c)
                iovec_array_free(निवेश_iovec, n_निवेश_iovec);

        प्रत्यागम c;
}

LogContext* log_context_head(शून्य) {
        प्रत्यागम _log_context;
}

माप_प्रकार log_context_अंक_contexts(शून्य) {
        माप_प्रकार n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        प्रत्यागम n;
}

माप_प्रकार log_context_अंक_fields_2(शून्य) {
        प्रत्यागम log_context_अंक_fields;
}

शून्य log_context_swap(LogContext **log_context, माप_प्रकार *अंक_fields) {
        प्रतिज्ञा(log_context);
        प्रतिज्ञा(अंक_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_अंक_fields, *अंक_fields);
}

शून्य _reset_log_level(पूर्णाङ्क *saved_log_level) {
        प्रतिज्ञा(saved_log_level);

        log_set_max_level(*saved_log_level);
}
