/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <threads.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "env-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "log-context.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "strv.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା saved_log_context_enabled = -1;
ସ୍ଥିର thread_local LIST_HEAD(LogContext, _log_context) = ଖାଲି;
ସ୍ଥିର thread_local size_t log_context_ଅଙ୍କ_fields = 0;

bool log_context_enabled(ଶୂନ୍ୟ) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (log_get_max_level() == LOG_DEBUG)
                ଫେରନ୍ତୁ true;

        ଯଦି (saved_log_context_enabled >= 0)
                ଫେରନ୍ତୁ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        ଯଦି (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        ଫେରନ୍ତୁ saved_log_context_enabled;
}

ସ୍ଥିର LogContext* log_context_attach(LogContext *c) {
        ପ୍ରତିଜ୍ଞା(c);

        log_context_ଅଙ୍କ_fields += strv_length(c->fields);
        log_context_ଅଙ୍କ_fields += c->n_ନିବେଶ_iovec;
        log_context_ଅଙ୍କ_fields += !!c->ଚାବି;

        ଫେରନ୍ତୁ LIST_PREPEND(ll, _log_context, c);
}

ସ୍ଥିର LogContext* log_context_detach(LogContext *c) {
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        ପ୍ରତିଜ୍ଞା(log_context_ଅଙ୍କ_fields >= strv_length(c->fields) + c->n_ନିବେଶ_iovec +!!c->ଚାବି);
        log_context_ଅଙ୍କ_fields -= strv_length(c->fields);
        log_context_ଅଙ୍କ_fields -= c->n_ନିବେଶ_iovec;
        log_context_ଅଙ୍କ_fields -= !!c->ଚାବି;

        LIST_REMOVE(ll, _log_context, c);
        ଫେରନ୍ତୁ ଖାଲି;
}

LogContext* log_context_new(ଅଚଳ ଅକ୍ଷର *ଚାବି, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ) {
        ପ୍ରତିଜ୍ଞା(ଚାବି);
        ପ୍ରତିଜ୍ଞା(endswith(ଚାବି, "="));

        ଯଦି (!ମୂଲ୍ୟ)
                ଫେରନ୍ତୁ ଖାଲି;

        LIST_FOREACH(ll, i, _log_context)
                ଯଦି (i->ଚାବି == ଚାବି && i->ମୂଲ୍ୟ == ମୂଲ୍ୟ)
                        ଫେରନ୍ତୁ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        *c = (LogContext) {
                .n_ref = 1,
                .ଚାବି = (ଅକ୍ଷର *) ଚାବି,
                .ମୂଲ୍ୟ = (ଅକ୍ଷର *) ମୂଲ୍ୟ,
        };

        ଫେରନ୍ତୁ log_context_attach(c);
}

LogContext* log_context_new_strv(ଅକ୍ଷର **fields, bool owned) {
        ଯଦି (!fields)
                ଫେରନ୍ତୁ ଖାଲି;

        LIST_FOREACH(ll, i, _log_context)
                ଯଦି (i->fields == fields) {
                        ପ୍ରତିଜ୍ଞା(!owned);
                        ଫେରନ୍ତୁ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        ଫେରନ୍ତୁ log_context_attach(c);
}

LogContext* log_context_new_iov(ସଂରଚନା iovec *ନିବେଶ_iovec, size_t n_ନିବେଶ_iovec, bool owned) {
        ଯଦି (!ନିବେଶ_iovec || n_ନିବେଶ_iovec == 0)
                ଫେରନ୍ତୁ ଖାଲି;

        LIST_FOREACH(ll, i, _log_context)
                ଯଦି (i->ନିବେଶ_iovec == ନିବେଶ_iovec && i->n_ନିବେଶ_iovec == n_ନିବେଶ_iovec) {
                        ପ୍ରତିଜ୍ଞା(!owned);
                        ଫେରନ୍ତୁ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        *c = (LogContext) {
                .n_ref = 1,
                .ନିବେଶ_iovec = ନିବେଶ_iovec,
                .n_ନିବେଶ_iovec = n_ନିବେଶ_iovec,
                .owned = owned,
        };

        ଫେରନ୍ତୁ log_context_attach(c);
}

ସ୍ଥିର LogContext* log_context_free(LogContext *c) {
        ଯଦି (!c)
                ଫେରନ୍ତୁ ଖାଲି;

        log_context_detach(c);

        ଯଦି (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->ନିବେଶ_iovec, c->n_ନିବେଶ_iovec);
                ମୁକ୍ତ(c->ଚାବି);
                ମୁକ୍ତ(c->ମୂଲ୍ୟ);
        }

        ଫେରନ୍ତୁ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(ଅକ୍ଷର **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        ଯଦି (!c)
                strv_free(fields);

        ଫେରନ୍ତୁ c;
}

LogContext* log_context_new_iov_consume(ସଂରଚନା iovec *ନିବେଶ_iovec, size_t n_ନିବେଶ_iovec) {
        LogContext *c = log_context_new_iov(ନିବେଶ_iovec, n_ନିବେଶ_iovec, /* owned= */ true);
        ଯଦି (!c)
                iovec_array_free(ନିବେଶ_iovec, n_ନିବେଶ_iovec);

        ଫେରନ୍ତୁ c;
}

LogContext* log_context_head(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ _log_context;
}

size_t log_context_ଅଙ୍କ_contexts(ଶୂନ୍ୟ) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        ଫେରନ୍ତୁ n;
}

size_t log_context_ଅଙ୍କ_fields_2(ଶୂନ୍ୟ) {
        ଫେରନ୍ତୁ log_context_ଅଙ୍କ_fields;
}

ଶୂନ୍ୟ log_context_swap(LogContext **log_context, size_t *ଅଙ୍କ_fields) {
        ପ୍ରତିଜ୍ଞା(log_context);
        ପ୍ରତିଜ୍ଞା(ଅଙ୍କ_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_ଅଙ୍କ_fields, *ଅଙ୍କ_fields);
}

ଶୂନ୍ୟ _reset_log_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *saved_log_level) {
        ପ୍ରତିଜ୍ଞା(saved_log_level);

        log_set_max_level(*saved_log_level);
}
