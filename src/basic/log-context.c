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
स्थिर thread_local size_t log_context_अंक_fields = 0;

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
        log_context_अंक_fields += c->n_आदान_iovec;
        log_context_अंक_fields += !!c->किल्ली;

        प्रत्यागम LIST_PREPEND(ll, _log_context, c);
}

स्थिर LogContext* log_context_detach(LogContext *c) {
        यदि (!c)
                प्रत्यागम रिक्त;

        प्रतिज्ञा(log_context_अंक_fields >= strv_length(c->fields) + c->n_आदान_iovec +!!c->किल्ली);
        log_context_अंक_fields -= strv_length(c->fields);
        log_context_अंक_fields -= c->n_आदान_iovec;
        log_context_अंक_fields -= !!c->किल्ली;

        LIST_REMOVE(ll, _log_context, c);
        प्रत्यागम रिक्त;
}

LogContext* log_context_new(ध्रुव वर्ण *किल्ली, ध्रुव वर्ण *मूल्य) {
        प्रतिज्ञा(किल्ली);
        प्रतिज्ञा(endswith(किल्ली, "="));

        यदि (!मूल्य)
                प्रत्यागम रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->किल्ली == किल्ली && i->मूल्य == मूल्य)
                        प्रत्यागम log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                प्रत्यागम रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .किल्ली = (वर्ण *) किल्ली,
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

LogContext* log_context_new_iov(संरचना iovec *आदान_iovec, size_t n_आदान_iovec, bool owned) {
        यदि (!आदान_iovec || n_आदान_iovec == 0)
                प्रत्यागम रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->आदान_iovec == आदान_iovec && i->n_आदान_iovec == n_आदान_iovec) {
                        प्रतिज्ञा(!owned);
                        प्रत्यागम log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                प्रत्यागम रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .आदान_iovec = आदान_iovec,
                .n_आदान_iovec = n_आदान_iovec,
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
                iovec_array_free(c->आदान_iovec, c->n_आदान_iovec);
                मुक्त(c->किल्ली);
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

LogContext* log_context_new_iov_consume(संरचना iovec *आदान_iovec, size_t n_आदान_iovec) {
        LogContext *c = log_context_new_iov(आदान_iovec, n_आदान_iovec, /* owned= */ true);
        यदि (!c)
                iovec_array_free(आदान_iovec, n_आदान_iovec);

        प्रत्यागम c;
}

LogContext* log_context_head(शून्य) {
        प्रत्यागम _log_context;
}

size_t log_context_अंक_contexts(शून्य) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        प्रत्यागम n;
}

size_t log_context_अंक_fields_2(शून्य) {
        प्रत्यागम log_context_अंक_fields;
}

शून्य log_context_swap(LogContext **log_context, size_t *अंक_fields) {
        प्रतिज्ञा(log_context);
        प्रतिज्ञा(अंक_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_अंक_fields, *अंक_fields);
}

शून्य _reset_log_level(पूर्णाङ्क *saved_log_level) {
        प्रतिज्ञा(saved_log_level);

        log_set_max_level(*saved_log_level);
}
