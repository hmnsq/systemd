/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"
#അന്തര്ഭാവ "list.h"

/*
 * The log context allows attaching extra metadata to log messages written to the journal via log.h. We keep
 * track of a thread local log context onto which we can push extra metadata fields that should be logged.
 *
 * LOG_CONTEXT_PUSH() will add the provided field to the log context and will remove it again when the
 * current block ends. LOG_CONTEXT_PUSH_STRV() will do the same but for all fields in the given strv.
 * LOG_CONTEXT_PUSHF() is like LOG_CONTEXT_PUSH() but takes a format string and arguments.
 *
 * Using the macros is as simple as putting them anywhere inside a block to add a field to all following log
 * messages logged from inside that block.
 *
 * void myfunction(...) {
 *         ...
 *
 *         LOG_CONTEXT_PUSHF("MYMETADATA=%s", "abc");
 *
 *         // Every journal message logged will now have the MYMETADATA=abc
 *         // field included.
 * }
 *
 * One special case to note is async code, where we use callbacks that are invoked to continue processing
 * when some event occurs. For async code, there's usually an associated "userdata" struct containing all the
 * information associated with the async operation. In this "userdata" struct, we can store a log context
 * allocated with log_context_new() and freed with log_context_free(). We can then add and remove fields to
 * the `fields` member of the log context object and all those fields will be logged along with each log
 * message.
 */

തരനിർവചനം ഘടന LogContext {
        unsigned n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        അക്ഷരം **fields;
        ഘടന iovec *നിവേശം_iovec;
        size_t n_നിവേശം_iovec;
        അക്ഷരം *താക്കോൽ;
        അക്ഷരം *മൂല്യം;
        bool owned;
        LIST_FIELDS(ഘടന LogContext, ll);
} LogContext;

bool log_context_enabled(ശൂന്യം);

LogContext* log_context_new(സ്ഥിരം അക്ഷരം *താക്കോൽ, സ്ഥിരം അക്ഷരം *മൂല്യം);
LogContext* log_context_new_strv(അക്ഷരം **fields, bool owned);
LogContext* log_context_new_iov(ഘടന iovec *നിവേശം_iovec, size_t n_നിവേശം_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(അക്ഷരം **fields);
LogContext* log_context_new_iov_consume(ഘടന iovec *നിവേശം_iovec, size_t n_നിവേശം_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_head(ശൂന്യം);
/* Returns the number of attached log context objects. */
size_t log_context_അക്കം_contexts(ശൂന്യം);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_അക്കം_fields_2(ശൂന്യം);

ശൂന്യം log_context_swap(LogContext **log_context, size_t *അക്കം_fields);

ശൂന്യം _reset_log_level(പൂർണ്ണസംഖ്യ *saved_log_level);

#define _LOG_CONTEXT_SET_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) _unused_ പൂർണ്ണസംഖ്യ l = log_set_max_level(level);

#define LOG_CONTEXT_SET_LOG_LEVEL(level) \
        _LOG_CONTEXT_SET_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#define LOG_CONTEXT_തള്ളുക(...) \
        LOG_CONTEXT_തള്ളുക_STRV(STRV_MAKE(__VA_ARGS__))

#define LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_തള്ളുക(snprintf_ok((അക്ഷരം[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#define LOG_CONTEXT_തള്ളുക_താക്കോൽ_മൂല്യം(താക്കോൽ, മൂല്യം, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new(താക്കോൽ, മൂല്യം);

#define LOG_CONTEXT_തള്ളുക_താക്കോൽ_മൂല്യം_2(താക്കോൽ, മൂല്യം) \
        LOG_CONTEXT_തള്ളുക_താക്കോൽ_മൂല്യം(താക്കോൽ, മൂല്യം, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_തള്ളുക_STRV_2(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#define LOG_CONTEXT_തള്ളുക_STRV(strv) \
        LOG_CONTEXT_തള്ളുക_STRV_2(strv, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_തള്ളുക_IOV(നിവേശം_iovec, n_നിവേശം_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(നിവേശം_iovec, n_നിവേശം_iovec, /* owned= */ false);

#define LOG_CONTEXT_തള്ളുക_IOV_2(നിവേശം_iovec, n_നിവേശം_iovec) \
        LOG_CONTEXT_തള്ളുക_IOV(നിവേശം_iovec, n_നിവേശം_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#define _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        _unused_ _cleanup_strv_free_ strv = strv_new(s);                                                \
        എങ്കിൽ (!strv)                                                                                      \
                സ്വതന്ത്രം(s);                                                                                \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#define LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#define _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#define LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#define _LOG_CONTEXT_CONSUME_IOV(നിവേശം_iovec, n_നിവേശം_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(നിവേശം_iovec, n_നിവേശം_iovec);

#define LOG_CONTEXT_CONSUME_IOV(നിവേശം_iovec, n_നിവേശം_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(നിവേശം_iovec, n_നിവേശം_iovec, UNIQ_T(c, UNIQ))
