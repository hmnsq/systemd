/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "list.h"

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

రకనిర్వచనం నిర్మాణం LogContext {
        గుర్తులేని n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        అక్షరం **fields;
        నిర్మాణం iovec *ఆదానం_iovec;
        size_t n_ఆదానం_iovec;
        అక్షరం *కీలకం;
        అక్షరం *విలువ;
        bool owned;
        LIST_FIELDS(నిర్మాణం LogContext, ll);
} LogContext;

bool log_context_enabled(శూన్య);

LogContext* log_context_new(స్థిరరాశి అక్షరం *కీలకం, స్థిరరాశి అక్షరం *విలువ);
LogContext* log_context_new_strv(అక్షరం **fields, bool owned);
LogContext* log_context_new_iov(నిర్మాణం iovec *ఆదానం_iovec, size_t n_ఆదానం_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(అక్షరం **fields);
LogContext* log_context_new_iov_consume(నిర్మాణం iovec *ఆదానం_iovec, size_t n_ఆదానం_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_తల(శూన్య);
/* Returns the number of attached log context objects. */
size_t log_context_అంకె_contexts(శూన్య);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_అంకె_fields_2(శూన్య);

శూన్య log_context_మార్పిడి(LogContext **log_context, size_t *అంకె_fields);

శూన్య _reset_log_level(పూర్ణాంకం *saved_log_level);

#నిర్వచించు LOG_CONTEXT_సమితి_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) వాడని పూర్ణాంకం l = log_సమితి_max_level(level);

#నిర్వచించు LOG_CONTEXT_సమితి_LOG_LEVEL_2(level) \
        LOG_CONTEXT_సమితి_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#నిర్వచించు LOG_CONTEXT_నెట్టు(...) \
        LOG_CONTEXT_నెట్టు_STRV(STRV_MAKE(__VA_ARGS__))

#నిర్వచించు LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_నెట్టు(snprintf_ok((అక్షరం[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#నిర్వచించు LOG_CONTEXT_నెట్టు_కీలకం_విలువ(కీలకం, విలువ, c) \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new(కీలకం, విలువ);

#నిర్వచించు LOG_CONTEXT_నెట్టు_కీలకం_విలువ_2(కీలకం, విలువ) \
        LOG_CONTEXT_నెట్టు_కీలకం_విలువ(కీలకం, విలువ, UNIQ_T(c, UNIQ))

#నిర్వచించు LOG_CONTEXT_నెట్టు_STRV_2(strv, c) \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#నిర్వచించు LOG_CONTEXT_నెట్టు_STRV(strv) \
        LOG_CONTEXT_నెట్టు_STRV_2(strv, UNIQ_T(c, UNIQ))

#నిర్వచించు LOG_CONTEXT_నెట్టు_IOV(ఆదానం_iovec, n_ఆదానం_iovec, c) \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(ఆదానం_iovec, n_ఆదానం_iovec, /* owned= */ false);

#నిర్వచించు LOG_CONTEXT_నెట్టు_IOV_2(ఆదానం_iovec, n_ఆదానం_iovec) \
        LOG_CONTEXT_నెట్టు_IOV(ఆదానం_iovec, n_ఆదానం_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#నిర్వచించు _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        వాడని _cleanup_strv_free_ strv = strv_new(s);                                                \
        ఒకవేళ (!strv)                                                                                      \
                విడుదల(s);                                                                                \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#నిర్వచించు LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#నిర్వచించు _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#నిర్వచించు LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#నిర్వచించు _LOG_CONTEXT_CONSUME_IOV(ఆదానం_iovec, n_ఆదానం_iovec, c) \
        వాడని _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(ఆదానం_iovec, n_ఆదానం_iovec);

#నిర్వచించు LOG_CONTEXT_CONSUME_IOV(ఆదానం_iovec, n_ఆదానం_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(ఆదానం_iovec, n_ఆదానం_iovec, UNIQ_T(c, UNIQ))
