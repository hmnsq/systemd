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
स्थिर thread_local माप_प्रकार log_context_अङ्क_fields = 0;

bool log_context_enabled(शून्य) {
        पूर्णाङ्क r;

        यदि (log_get_max_level() == LOG_DEBUG)
                फर्काऊ true;

        यदि (saved_log_context_enabled >= 0)
                फर्काऊ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        यदि (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        फर्काऊ saved_log_context_enabled;
}

स्थिर LogContext* log_context_attach(LogContext *c) {
        प्रतिज्ञा(c);

        log_context_अङ्क_fields += strv_length(c->fields);
        log_context_अङ्क_fields += c->n_आगत_iovec;
        log_context_अङ्क_fields += !!c->साँचो;

        फर्काऊ LIST_PREPEND(ll, _log_context, c);
}

स्थिर LogContext* log_context_detach(LogContext *c) {
        यदि (!c)
                फर्काऊ रिक्त;

        प्रतिज्ञा(log_context_अङ्क_fields >= strv_length(c->fields) + c->n_आगत_iovec +!!c->साँचो);
        log_context_अङ्क_fields -= strv_length(c->fields);
        log_context_अङ्क_fields -= c->n_आगत_iovec;
        log_context_अङ्क_fields -= !!c->साँचो;

        LIST_REMOVE(ll, _log_context, c);
        फर्काऊ रिक्त;
}

LogContext* log_context_new(अचर वर्ण *साँचो, अचर वर्ण *मान) {
        प्रतिज्ञा(साँचो);
        प्रतिज्ञा(endswith(साँचो, "="));

        यदि (!मान)
                फर्काऊ रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->साँचो == साँचो && i->मान == मान)
                        फर्काऊ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                फर्काऊ रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .साँचो = (वर्ण *) साँचो,
                .मान = (वर्ण *) मान,
        };

        फर्काऊ log_context_attach(c);
}

LogContext* log_context_new_strv(वर्ण **fields, bool owned) {
        यदि (!fields)
                फर्काऊ रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->fields == fields) {
                        प्रतिज्ञा(!owned);
                        फर्काऊ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                फर्काऊ रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        फर्काऊ log_context_attach(c);
}

LogContext* log_context_new_iov(संरचना iovec *आगत_iovec, माप_प्रकार n_आगत_iovec, bool owned) {
        यदि (!आगत_iovec || n_आगत_iovec == 0)
                फर्काऊ रिक्त;

        LIST_FOREACH(ll, i, _log_context)
                यदि (i->आगत_iovec == आगत_iovec && i->n_आगत_iovec == n_आगत_iovec) {
                        प्रतिज्ञा(!owned);
                        फर्काऊ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        यदि (!c)
                फर्काऊ रिक्त;

        *c = (LogContext) {
                .n_ref = 1,
                .आगत_iovec = आगत_iovec,
                .n_आगत_iovec = n_आगत_iovec,
                .owned = owned,
        };

        फर्काऊ log_context_attach(c);
}

स्थिर LogContext* log_context_free(LogContext *c) {
        यदि (!c)
                फर्काऊ रिक्त;

        log_context_detach(c);

        यदि (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->आगत_iovec, c->n_आगत_iovec);
                मुक्त(c->साँचो);
                मुक्त(c->मान);
        }

        फर्काऊ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(वर्ण **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        यदि (!c)
                strv_free(fields);

        फर्काऊ c;
}

LogContext* log_context_new_iov_consume(संरचना iovec *आगत_iovec, माप_प्रकार n_आगत_iovec) {
        LogContext *c = log_context_new_iov(आगत_iovec, n_आगत_iovec, /* owned= */ true);
        यदि (!c)
                iovec_array_free(आगत_iovec, n_आगत_iovec);

        फर्काऊ c;
}

LogContext* log_context_head(शून्य) {
        फर्काऊ _log_context;
}

माप_प्रकार log_context_अङ्क_contexts(शून्य) {
        माप_प्रकार n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        फर्काऊ n;
}

माप_प्रकार log_context_अङ्क_fields_2(शून्य) {
        फर्काऊ log_context_अङ्क_fields;
}

शून्य log_context_swap(LogContext **log_context, माप_प्रकार *अङ्क_fields) {
        प्रतिज्ञा(log_context);
        प्रतिज्ञा(अङ्क_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_अङ्क_fields, *अङ्क_fields);
}

शून्य _reset_log_level(पूर्णाङ्क *saved_log_level) {
        प्रतिज्ञा(saved_log_level);

        log_set_max_level(*saved_log_level);
}
