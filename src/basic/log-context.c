/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <threads.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "env-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "log-context.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "strv.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ saved_log_context_enabled = -1;
ಸ್ಥಿರ thread_local LIST_ತಲೆ(LogContext, _log_context) = ಖಾಲಿ;
ಸ್ಥಿರ thread_local size_t log_context_ಅಂಕೆ_fields = 0;

bool log_context_enabled(ಶೂನ್ಯ) {
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (log_get_max_level() == LOG_DEBUG)
                ಹಿಂತಿರುಗಿ true;

        ಆದರೆ (saved_log_context_enabled >= 0)
                ಹಿಂತಿರುಗಿ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ಆದರೆ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ಹಿಂತಿರುಗಿ saved_log_context_enabled;
}

ಸ್ಥಿರ LogContext* log_context_attach(LogContext *c) {
        ಪ್ರತಿಜ್ಞಾ(c);

        log_context_ಅಂಕೆ_fields += strv_ಉದ್ದಳತೆ(c->fields);
        log_context_ಅಂಕೆ_fields += c->n_ಒಳಹರಿವು_iovec;
        log_context_ಅಂಕೆ_fields += !!c->ಕೀಲಿ;

        ಹಿಂತಿರುಗಿ LIST_PREPEND(ll, _log_context, c);
}

ಸ್ಥಿರ LogContext* log_context_detach(LogContext *c) {
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(log_context_ಅಂಕೆ_fields >= strv_ಉದ್ದಳತೆ(c->fields) + c->n_ಒಳಹರಿವು_iovec +!!c->ಕೀಲಿ);
        log_context_ಅಂಕೆ_fields -= strv_ಉದ್ದಳತೆ(c->fields);
        log_context_ಅಂಕೆ_fields -= c->n_ಒಳಹರಿವು_iovec;
        log_context_ಅಂಕೆ_fields -= !!c->ಕೀಲಿ;

        LIST_REMOVE(ll, _log_context, c);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

LogContext* log_context_new(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೀಲಿ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ) {
        ಪ್ರತಿಜ್ಞಾ(ಕೀಲಿ);
        ಪ್ರತಿಜ್ಞಾ(endswith(ಕೀಲಿ, "="));

        ಆದರೆ (!ಮೌಲ್ಯ)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        LIST_FOREACH(ll, i, _log_context)
                ಆದರೆ (i->ಕೀಲಿ == ಕೀಲಿ && i->ಮೌಲ್ಯ == ಮೌಲ್ಯ)
                        ಹಿಂತಿರುಗಿ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *c = (LogContext) {
                .n_ref = 1,
                .ಕೀಲಿ = (ಅಕ್ಷರ *) ಕೀಲಿ,
                .ಮೌಲ್ಯ = (ಅಕ್ಷರ *) ಮೌಲ್ಯ,
        };

        ಹಿಂತಿರುಗಿ log_context_attach(c);
}

LogContext* log_context_new_strv(ಅಕ್ಷರ **fields, bool owned) {
        ಆದರೆ (!fields)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        LIST_FOREACH(ll, i, _log_context)
                ಆದರೆ (i->fields == fields) {
                        ಪ್ರತಿಜ್ಞಾ(!owned);
                        ಹಿಂತಿರುಗಿ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ಹಿಂತಿರುಗಿ log_context_attach(c);
}

LogContext* log_context_new_iov(ರಚನೆ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec, bool owned) {
        ಆದರೆ (!ಒಳಹರಿವು_iovec || n_ಒಳಹರಿವು_iovec == 0)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        LIST_FOREACH(ll, i, _log_context)
                ಆದರೆ (i->ಒಳಹರಿವು_iovec == ಒಳಹರಿವು_iovec && i->n_ಒಳಹರಿವು_iovec == n_ಒಳಹರಿವು_iovec) {
                        ಪ್ರತಿಜ್ಞಾ(!owned);
                        ಹಿಂತಿರುಗಿ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        *c = (LogContext) {
                .n_ref = 1,
                .ಒಳಹರಿವು_iovec = ಒಳಹರಿವು_iovec,
                .n_ಒಳಹರಿವು_iovec = n_ಒಳಹರಿವು_iovec,
                .owned = owned,
        };

        ಹಿಂತಿರುಗಿ log_context_attach(c);
}

ಸ್ಥಿರ LogContext* log_context_free(LogContext *c) {
        ಆದರೆ (!c)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        log_context_detach(c);

        ಆದರೆ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->ಒಳಹರಿವು_iovec, c->n_ಒಳಹರಿವು_iovec);
                ಬಿಡುಗಡೆ(c->ಕೀಲಿ);
                ಬಿಡುಗಡೆ(c->ಮೌಲ್ಯ);
        }

        ಹಿಂತಿರುಗಿ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(ಅಕ್ಷರ **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ಆದರೆ (!c)
                strv_free(fields);

        ಹಿಂತಿರುಗಿ c;
}

LogContext* log_context_new_iov_consume(ರಚನೆ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec) {
        LogContext *c = log_context_new_iov(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, /* owned= */ true);
        ಆದರೆ (!c)
                iovec_array_free(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec);

        ಹಿಂತಿರುಗಿ c;
}

LogContext* log_context_ತಲೆ(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ _log_context;
}

size_t log_context_ಅಂಕೆ_contexts(ಶೂನ್ಯ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ಹಿಂತಿರುಗಿ n;
}

size_t log_context_ಅಂಕೆ_fields_2(ಶೂನ್ಯ) {
        ಹಿಂತಿರುಗಿ log_context_ಅಂಕೆ_fields;
}

ಶೂನ್ಯ log_context_ಅದಲುಬದಲು(LogContext **log_context, size_t *ಅಂಕೆ_fields) {
        ಪ್ರತಿಜ್ಞಾ(log_context);
        ಪ್ರತಿಜ್ಞಾ(ಅಂಕೆ_fields);

        ಅದಲುಬದಲು_TWO(_log_context, *log_context);
        ಅದಲುಬದಲು_TWO(log_context_ಅಂಕೆ_fields, *ಅಂಕೆ_fields);
}

ಶೂನ್ಯ _reset_log_level(ಪೂರ್ಣಾಂಕ *saved_log_level) {
        ಪ್ರತಿಜ್ಞಾ(saved_log_level);

        log_ಗಣ_max_level(*saved_log_level);
}
