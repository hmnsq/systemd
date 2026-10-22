/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"
#અન્તર્ભાવ "list.h"

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

પ્રકારવ્યાખ્યા સંરચના LogContext {
        unsigned n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        અક્ષર **fields;
        સંરચના iovec *આગત_iovec;
        size_t n_આગત_iovec;
        અક્ષર *ચાવી;
        અક્ષર *કિંમત;
        bool owned;
        LIST_FIELDS(સંરચના LogContext, ll);
} LogContext;

bool log_context_enabled(શૂન્ય);

LogContext* log_context_new(અચલ અક્ષર *ચાવી, અચલ અક્ષર *કિંમત);
LogContext* log_context_new_strv(અક્ષર **fields, bool owned);
LogContext* log_context_new_iov(સંરચના iovec *આગત_iovec, size_t n_આગત_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(અક્ષર **fields);
LogContext* log_context_new_iov_consume(સંરચના iovec *આગત_iovec, size_t n_આગત_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_head(શૂન્ય);
/* Returns the number of attached log context objects. */
size_t log_context_અંક_contexts(શૂન્ય);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_અંક_fields_2(શૂન્ય);

શૂન્ય log_context_swap(LogContext **log_context, size_t *અંક_fields);

શૂન્ય _reset_log_level(પૂર્ણાંક *saved_log_level);

#define _LOG_CONTEXT_SET_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) _unused_ પૂર્ણાંક l = log_set_max_level(level);

#define LOG_CONTEXT_SET_LOG_LEVEL(level) \
        _LOG_CONTEXT_SET_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#define LOG_CONTEXT_ધકેલો(...) \
        LOG_CONTEXT_ધકેલો_STRV(STRV_MAKE(__VA_ARGS__))

#define LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_ધકેલો(snprintf_ok((અક્ષર[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#define LOG_CONTEXT_ધકેલો_ચાવી_કિંમત(ચાવી, કિંમત, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new(ચાવી, કિંમત);

#define LOG_CONTEXT_ધકેલો_ચાવી_કિંમત_2(ચાવી, કિંમત) \
        LOG_CONTEXT_ધકેલો_ચાવી_કિંમત(ચાવી, કિંમત, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ધકેલો_STRV_2(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#define LOG_CONTEXT_ધકેલો_STRV(strv) \
        LOG_CONTEXT_ધકેલો_STRV_2(strv, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ધકેલો_IOV(આગત_iovec, n_આગત_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(આગત_iovec, n_આગત_iovec, /* owned= */ false);

#define LOG_CONTEXT_ધકેલો_IOV_2(આગત_iovec, n_આગત_iovec) \
        LOG_CONTEXT_ધકેલો_IOV(આગત_iovec, n_આગત_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#define _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        _unused_ _cleanup_strv_free_ strv = strv_new(s);                                                \
        જો (!strv)                                                                                      \
                મુક્ત(s);                                                                                \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#define LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#define _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#define LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#define _LOG_CONTEXT_CONSUME_IOV(આગત_iovec, n_આગત_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(આગત_iovec, n_આગત_iovec);

#define LOG_CONTEXT_CONSUME_IOV(આગત_iovec, n_આગત_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(આગત_iovec, n_આગત_iovec, UNIQ_T(c, UNIQ))
