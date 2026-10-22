/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <threads.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "env-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "log-context.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

நிலையான முழு saved_log_context_enabled = -1;
நிலையான thread_local LIST_HEAD(LogContext, _log_context) = வெற்று;
நிலையான thread_local size_t log_context_இலக்கம்_fields = 0;

bool log_context_enabled(வெறுமை) {
        முழு r;

        எனில் (log_get_max_level() == LOG_DEBUG)
                திரும்பு true;

        எனில் (saved_log_context_enabled >= 0)
                திரும்பு saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        எனில் (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        திரும்பு saved_log_context_enabled;
}

நிலையான LogContext* log_context_attach(LogContext *c) {
        ப்ரதிஜ்ஞா(c);

        log_context_இலக்கம்_fields += strv_length(c->fields);
        log_context_இலக்கம்_fields += c->n_உள்ளீடு_iovec;
        log_context_இலக்கம்_fields += !!c->சாவி;

        திரும்பு LIST_PREPEND(ll, _log_context, c);
}

நிலையான LogContext* log_context_detach(LogContext *c) {
        எனில் (!c)
                திரும்பு வெற்று;

        ப்ரதிஜ்ஞா(log_context_இலக்கம்_fields >= strv_length(c->fields) + c->n_உள்ளீடு_iovec +!!c->சாவி);
        log_context_இலக்கம்_fields -= strv_length(c->fields);
        log_context_இலக்கம்_fields -= c->n_உள்ளீடு_iovec;
        log_context_இலக்கம்_fields -= !!c->சாவி;

        LIST_REMOVE(ll, _log_context, c);
        திரும்பு வெற்று;
}

LogContext* log_context_new(மாறிலி எழுத்து *சாவி, மாறிலி எழுத்து *மதிப்பு) {
        ப்ரதிஜ்ஞா(சாவி);
        ப்ரதிஜ்ஞா(endswith(சாவி, "="));

        எனில் (!மதிப்பு)
                திரும்பு வெற்று;

        LIST_FOREACH(ll, i, _log_context)
                எனில் (i->சாவி == சாவி && i->மதிப்பு == மதிப்பு)
                        திரும்பு log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        எனில் (!c)
                திரும்பு வெற்று;

        *c = (LogContext) {
                .n_ref = 1,
                .சாவி = (எழுத்து *) சாவி,
                .மதிப்பு = (எழுத்து *) மதிப்பு,
        };

        திரும்பு log_context_attach(c);
}

LogContext* log_context_new_strv(எழுத்து **fields, bool owned) {
        எனில் (!fields)
                திரும்பு வெற்று;

        LIST_FOREACH(ll, i, _log_context)
                எனில் (i->fields == fields) {
                        ப்ரதிஜ்ஞா(!owned);
                        திரும்பு log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        எனில் (!c)
                திரும்பு வெற்று;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        திரும்பு log_context_attach(c);
}

LogContext* log_context_new_iov(கட்டமைப்பு iovec *உள்ளீடு_iovec, size_t n_உள்ளீடு_iovec, bool owned) {
        எனில் (!உள்ளீடு_iovec || n_உள்ளீடு_iovec == 0)
                திரும்பு வெற்று;

        LIST_FOREACH(ll, i, _log_context)
                எனில் (i->உள்ளீடு_iovec == உள்ளீடு_iovec && i->n_உள்ளீடு_iovec == n_உள்ளீடு_iovec) {
                        ப்ரதிஜ்ஞா(!owned);
                        திரும்பு log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        எனில் (!c)
                திரும்பு வெற்று;

        *c = (LogContext) {
                .n_ref = 1,
                .உள்ளீடு_iovec = உள்ளீடு_iovec,
                .n_உள்ளீடு_iovec = n_உள்ளீடு_iovec,
                .owned = owned,
        };

        திரும்பு log_context_attach(c);
}

நிலையான LogContext* log_context_free(LogContext *c) {
        எனில் (!c)
                திரும்பு வெற்று;

        log_context_detach(c);

        எனில் (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->உள்ளீடு_iovec, c->n_உள்ளீடு_iovec);
                விடுவி(c->சாவி);
                விடுவி(c->மதிப்பு);
        }

        திரும்பு mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(எழுத்து **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        எனில் (!c)
                strv_free(fields);

        திரும்பு c;
}

LogContext* log_context_new_iov_consume(கட்டமைப்பு iovec *உள்ளீடு_iovec, size_t n_உள்ளீடு_iovec) {
        LogContext *c = log_context_new_iov(உள்ளீடு_iovec, n_உள்ளீடு_iovec, /* owned= */ true);
        எனில் (!c)
                iovec_array_free(உள்ளீடு_iovec, n_உள்ளீடு_iovec);

        திரும்பு c;
}

LogContext* log_context_head(வெறுமை) {
        திரும்பு _log_context;
}

size_t log_context_இலக்கம்_contexts(வெறுமை) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        திரும்பு n;
}

size_t log_context_இலக்கம்_fields_2(வெறுமை) {
        திரும்பு log_context_இலக்கம்_fields;
}

வெறுமை log_context_swap(LogContext **log_context, size_t *இலக்கம்_fields) {
        ப்ரதிஜ்ஞா(log_context);
        ப்ரதிஜ்ஞா(இலக்கம்_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_இலக்கம்_fields, *இலக்கம்_fields);
}

வெறுமை _reset_log_level(முழு *saved_log_level) {
        ப்ரதிஜ்ஞா(saved_log_level);

        log_set_max_level(*saved_log_level);
}
