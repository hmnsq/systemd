/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "log-context.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ saved_log_context_enabled = -1;
ಸ್ಥಿರ thread_local LIST_HEAD(LogContext, _log_context) = ರಿಕ್ತ;
ಸ್ಥಿರ thread_local size_t log_context_ಅಂಕೆ_fields = 0;

bool log_context_enabled(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (log_get_max_level() == LOG_DEBUG)
                ಪ್ರತ್ಯಾಗಮ true;

        ಯದಿ (saved_log_context_enabled >= 0)
                ಪ್ರತ್ಯಾಗಮ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ಯದಿ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ಪ್ರತ್ಯಾಗಮ saved_log_context_enabled;
}

ಸ್ಥಿರ LogContext* log_context_attach(LogContext *c) {
        ಪ್ರತಿಜ್ಞಾ(c);

        log_context_ಅಂಕೆ_fields += strv_length(c->fields);
        log_context_ಅಂಕೆ_fields += c->n_ಒಳಹರಿವು_iovec;
        log_context_ಅಂಕೆ_fields += !!c->ಕೀಲಿ;

        ಪ್ರತ್ಯಾಗಮ LIST_PREPEND(ll, _log_context, c);
}

ಸ್ಥಿರ LogContext* log_context_detach(LogContext *c) {
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(log_context_ಅಂಕೆ_fields >= strv_length(c->fields) + c->n_ಒಳಹರಿವು_iovec +!!c->ಕೀಲಿ);
        log_context_ಅಂಕೆ_fields -= strv_length(c->fields);
        log_context_ಅಂಕೆ_fields -= c->n_ಒಳಹರಿವು_iovec;
        log_context_ಅಂಕೆ_fields -= !!c->ಕೀಲಿ;

        LIST_REMOVE(ll, _log_context, c);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

LogContext* log_context_new(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(endswith(ಕೀಲಿ, "="));

        ಯದಿ (!ಮೌಲ್ಯ)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        LIST_FOREACH(ll, i, _log_context)
                ಯದಿ (i->ಕೀಲಿ == ಕೀಲಿ && i->ಮೌಲ್ಯ == ಮೌಲ್ಯ)
                        ಪ್ರತ್ಯಾಗಮ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *c = (LogContext) {
                .n_ref = 1,
                .ಕೀಲಿ = (ವರ್ಣ *) ಕೀಲಿ,
                .ಮೌಲ್ಯ = (ವರ್ಣ *) ಮೌಲ್ಯ,
        };

        ಪ್ರತ್ಯಾಗಮ log_context_attach(c);
}

LogContext* log_context_new_strv(ವರ್ಣ **fields, bool owned) {
        ಯದಿ (!fields)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        LIST_FOREACH(ll, i, _log_context)
                ಯದಿ (i->fields == fields) {
                        ಪ್ರತಿಜ್ಞಾ(!owned);
                        ಪ್ರತ್ಯಾಗಮ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ಪ್ರತ್ಯಾಗಮ log_context_attach(c);
}

LogContext* log_context_new_iov(ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec, bool owned) {
        ಯದಿ (!ಒಳಹರಿವು_iovec || n_ಒಳಹರಿವು_iovec == 0)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        LIST_FOREACH(ll, i, _log_context)
                ಯದಿ (i->ಒಳಹರಿವು_iovec == ಒಳಹರಿವು_iovec && i->n_ಒಳಹರಿವು_iovec == n_ಒಳಹರಿವು_iovec) {
                        ಪ್ರತಿಜ್ಞಾ(!owned);
                        ಪ್ರತ್ಯಾಗಮ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        *c = (LogContext) {
                .n_ref = 1,
                .ಒಳಹರಿವು_iovec = ಒಳಹರಿವು_iovec,
                .n_ಒಳಹರಿವು_iovec = n_ಒಳಹರಿವು_iovec,
                .owned = owned,
        };

        ಪ್ರತ್ಯಾಗಮ log_context_attach(c);
}

ಸ್ಥಿರ LogContext* log_context_free(LogContext *c) {
        ಯದಿ (!c)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        log_context_detach(c);

        ಯದಿ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->ಒಳಹರಿವು_iovec, c->n_ಒಳಹರಿವು_iovec);
                ಮುಕ್ತ(c->ಕೀಲಿ);
                ಮುಕ್ತ(c->ಮೌಲ್ಯ);
        }

        ಪ್ರತ್ಯಾಗಮ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(ವರ್ಣ **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ಯದಿ (!c)
                strv_free(fields);

        ಪ್ರತ್ಯಾಗಮ c;
}

LogContext* log_context_new_iov_consume(ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec) {
        LogContext *c = log_context_new_iov(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, /* owned= */ true);
        ಯದಿ (!c)
                iovec_array_free(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec);

        ಪ್ರತ್ಯಾಗಮ c;
}

LogContext* log_context_head(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ _log_context;
}

size_t log_context_ಅಂಕೆ_contexts(ಶೂನ್ಯ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ಪ್ರತ್ಯಾಗಮ n;
}

size_t log_context_ಅಂಕೆ_fields_2(ಶೂನ್ಯ) {
        ಪ್ರತ್ಯಾಗಮ log_context_ಅಂಕೆ_fields;
}

ಶೂನ್ಯ log_context_swap(LogContext **log_context, size_t *ಅಂಕೆ_fields) {
        ಪ್ರತಿಜ್ಞಾ(log_context);
        ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_ಅಂಕೆ_fields, *ಅಂಕೆ_fields);
}

ಶೂನ್ಯ _reset_log_level(ಪೂರ್ಣಾಙ್ಕ *saved_log_level) {
        ಪ್ರತಿಜ್ಞಾ(saved_log_level);

        log_set_max_level(*saved_log_level);
}
