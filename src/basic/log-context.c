/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <threads.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "env-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "log-context.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ saved_log_context_enabled = -1;
സ്ഥിതം thread_local LIST_HEAD(LogContext, _log_context) = ശൂന്യ;
സ്ഥിതം thread_local size_t log_context_അക്കം_fields = 0;

bool log_context_enabled(ശൂന്യം) {
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (log_get_max_level() == LOG_DEBUG)
                തിരികെ true;

        എങ്കിൽ (saved_log_context_enabled >= 0)
                തിരികെ saved_log_context_enabled;

        r = secure_getenv_bool("SYSTEMD_ENABLE_LOG_CONTEXT");
        എങ്കിൽ (r < 0 && r != -ENXIO)
                log_debug_errno(r, "Failed to parse $SYSTEMD_ENABLE_LOG_CONTEXT, ignoring: %m");

        saved_log_context_enabled = r > 0;

        തിരികെ saved_log_context_enabled;
}

സ്ഥിതം LogContext* log_context_attach(LogContext *c) {
        പ്രതിജ്ഞാ(c);

        log_context_അക്കം_fields += strv_length(c->fields);
        log_context_അക്കം_fields += c->n_നിവേശം_iovec;
        log_context_അക്കം_fields += !!c->താക്കോൽ;

        തിരികെ LIST_PREPEND(ll, _log_context, c);
}

സ്ഥിതം LogContext* log_context_detach(LogContext *c) {
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        പ്രതിജ്ഞാ(log_context_അക്കം_fields >= strv_length(c->fields) + c->n_നിവേശം_iovec +!!c->താക്കോൽ);
        log_context_അക്കം_fields -= strv_length(c->fields);
        log_context_അക്കം_fields -= c->n_നിവേശം_iovec;
        log_context_അക്കം_fields -= !!c->താക്കോൽ;

        LIST_REMOVE(ll, _log_context, c);
        തിരികെ ശൂന്യ;
}

LogContext* log_context_new(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം) {
        പ്രതിജ്ഞാ(താക്കോൽ);
        പ്രതിജ്ഞാ(endswith(താക്കോൽ, "="));

        എങ്കിൽ (!മൂല്യം)
                തിരികെ ശൂന്യ;

        LIST_FOREACH(ll, i, _log_context)
                എങ്കിൽ (i->താക്കോൽ == താക്കോൽ && i->മൂല്യം == മൂല്യം)
                        തിരികെ log_context_ref(i);

        LogContext *c = new(LogContext, 1);
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        *c = (LogContext) {
                .n_ref = 1,
                .താക്കോൽ = (അക്ഷരം *) താക്കോൽ,
                .മൂല്യം = (അക്ഷരം *) മൂല്യം,
        };

        തിരികെ log_context_attach(c);
}

LogContext* log_context_new_strv(അക്ഷരം **fields, bool owned) {
        എങ്കിൽ (!fields)
                തിരികെ ശൂന്യ;

        LIST_FOREACH(ll, i, _log_context)
                എങ്കിൽ (i->fields == fields) {
                        പ്രതിജ്ഞാ(!owned);
                        തിരികെ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        *c = (LogContext) {
                .n_ref = 1,
                .fields = fields,
                .owned = owned,
        };

        തിരികെ log_context_attach(c);
}

LogContext* log_context_new_iov(ഘടന iovec *നിവേശം_iovec, size_t n_നിവേശം_iovec, bool owned) {
        എങ്കിൽ (!നിവേശം_iovec || n_നിവേശം_iovec == 0)
                തിരികെ ശൂന്യ;

        LIST_FOREACH(ll, i, _log_context)
                എങ്കിൽ (i->നിവേശം_iovec == നിവേശം_iovec && i->n_നിവേശം_iovec == n_നിവേശം_iovec) {
                        പ്രതിജ്ഞാ(!owned);
                        തിരികെ log_context_ref(i);
                }

        LogContext *c = new(LogContext, 1);
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        *c = (LogContext) {
                .n_ref = 1,
                .നിവേശം_iovec = നിവേശം_iovec,
                .n_നിവേശം_iovec = n_നിവേശം_iovec,
                .owned = owned,
        };

        തിരികെ log_context_attach(c);
}

സ്ഥിതം LogContext* log_context_free(LogContext *c) {
        എങ്കിൽ (!c)
                തിരികെ ശൂന്യ;

        log_context_detach(c);

        എങ്കിൽ (c->owned) {
                strv_free(c->fields);
                iovec_array_free(c->നിവേശം_iovec, c->n_നിവേശം_iovec);
                സ്വതന്ത്രം(c->താക്കോൽ);
                സ്വതന്ത്രം(c->മൂല്യം);
        }

        തിരികെ mfree(c);
}

DEFINE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context, log_context_free);

LogContext* log_context_new_strv_consume(അക്ഷരം **fields) {
        LogContext *c = log_context_new_strv(fields, /* owned= */ true);
        എങ്കിൽ (!c)
                strv_free(fields);

        തിരികെ c;
}

LogContext* log_context_new_iov_consume(ഘടന iovec *നിവേശം_iovec, size_t n_നിവേശം_iovec) {
        LogContext *c = log_context_new_iov(നിവേശം_iovec, n_നിവേശം_iovec, /* owned= */ true);
        എങ്കിൽ (!c)
                iovec_array_free(നിവേശം_iovec, n_നിവേശം_iovec);

        തിരികെ c;
}

LogContext* log_context_head(ശൂന്യം) {
        തിരികെ _log_context;
}

size_t log_context_അക്കം_contexts(ശൂന്യം) {
        size_t n = 0;

        LIST_FOREACH(ll, c, _log_context)
                n++;

        തിരികെ n;
}

size_t log_context_അക്കം_fields_2(ശൂന്യം) {
        തിരികെ log_context_അക്കം_fields;
}

ശൂന്യം log_context_swap(LogContext **log_context, size_t *അക്കം_fields) {
        പ്രതിജ്ഞാ(log_context);
        പ്രതിജ്ഞാ(അക്കം_fields);

        SWAP_TWO(_log_context, *log_context);
        SWAP_TWO(log_context_അക്കം_fields, *അക്കം_fields);
}

ശൂന്യം _reset_log_level(പൂർണ്ണസംഖ്യ *saved_log_level) {
        പ്രതിജ്ഞാ(saved_log_level);

        log_set_max_level(*saved_log_level);
}
