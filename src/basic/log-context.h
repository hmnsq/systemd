/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাৱ "forward.h"
#অন্তর্ভাৱ "list.h"

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

প্রকারনির্দেশ সংরচনা LogContext {
        unsigned n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        ৱর্ণ **fields;
        সংরচনা iovec *প্ৰবেশ_iovec;
        size_t n_প্ৰবেশ_iovec;
        ৱর্ণ *চাবি;
        ৱর্ণ *মান;
        bool owned;
        LIST_FIELDS(সংরচনা LogContext, ll);
} LogContext;

bool log_context_enabled(শূন্য);

LogContext* log_context_new(ধ্রুৱ ৱর্ণ *চাবি, ধ্রুৱ ৱর্ণ *মান);
LogContext* log_context_new_strv(ৱর্ণ **fields, bool owned);
LogContext* log_context_new_iov(সংরচনা iovec *প্ৰবেশ_iovec, size_t n_প্ৰবেশ_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(ৱর্ণ **fields);
LogContext* log_context_new_iov_consume(সংরচনা iovec *প্ৰবেশ_iovec, size_t n_প্ৰবেশ_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_head(শূন্য);
/* Returns the number of attached log context objects. */
size_t log_context_অঙ্ক_contexts(শূন্য);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_অঙ্ক_fields_2(শূন্য);

শূন্য log_context_swap(LogContext **log_context, size_t *অঙ্ক_fields);

শূন্য _reset_log_level(পূর্ণাঙ্ক *saved_log_level);

#define _LOG_CONTEXT_SET_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) _unused_ পূর্ণাঙ্ক l = log_set_max_level(level);

#define LOG_CONTEXT_SET_LOG_LEVEL(level) \
        _LOG_CONTEXT_SET_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#define LOG_CONTEXT_ঠেলো(...) \
        LOG_CONTEXT_ঠেলো_STRV(STRV_MAKE(__VA_ARGS__))

#define LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_ঠেলো(snprintf_ok((ৱর্ণ[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#define LOG_CONTEXT_ঠেলো_চাবি_মান(চাবি, মান, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new(চাবি, মান);

#define LOG_CONTEXT_ঠেলো_চাবি_মান_2(চাবি, মান) \
        LOG_CONTEXT_ঠেলো_চাবি_মান(চাবি, মান, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ঠেলো_STRV_2(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#define LOG_CONTEXT_ঠেলো_STRV(strv) \
        LOG_CONTEXT_ঠেলো_STRV_2(strv, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ঠেলো_IOV(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, /* owned= */ false);

#define LOG_CONTEXT_ঠেলো_IOV_2(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec) \
        LOG_CONTEXT_ঠেলো_IOV(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#define _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        _unused_ _cleanup_strv_free_ strv = strv_new(s);                                                \
        যদি (!strv)                                                                                      \
                মুক্ত(s);                                                                                \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#define LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#define _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#define LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#define _LOG_CONTEXT_CONSUME_IOV(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec);

#define LOG_CONTEXT_CONSUME_IOV(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(প্ৰবেশ_iovec, n_প্ৰবেশ_iovec, UNIQ_T(c, UNIQ))
