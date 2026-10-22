/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"
#अन्तर्भाव "list.h"

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

प्रकारनिर्देश संरचना LogContext {
        अचिन्हित n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        वर्ण **fields;
        संरचना iovec *निवेश_iovec;
        माप_प्रकार n_निवेश_iovec;
        वर्ण *कुंजी;
        वर्ण *मूल्य;
        bool owned;
        LIST_FIELDS(संरचना LogContext, ll);
} LogContext;

bool log_context_enabled(शून्य);

LogContext* log_context_new(ध्रुव वर्ण *कुंजी, ध्रुव वर्ण *मूल्य);
LogContext* log_context_new_strv(वर्ण **fields, bool owned);
LogContext* log_context_new_iov(संरचना iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(वर्ण **fields);
LogContext* log_context_new_iov_consume(संरचना iovec *निवेश_iovec, माप_प्रकार n_निवेश_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_head(शून्य);
/* Returns the number of attached log context objects. */
माप_प्रकार log_context_अंक_contexts(शून्य);
/* Returns the number of fields in all attached log contexts. */
माप_प्रकार log_context_अंक_fields_2(शून्य);

शून्य log_context_swap(LogContext **log_context, माप_प्रकार *अंक_fields);

शून्य _reset_log_level(पूर्णाङ्क *saved_log_level);

#define _LOG_CONTEXT_SET_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) _unused_ पूर्णाङ्क l = log_set_max_level(level);

#define LOG_CONTEXT_SET_LOG_LEVEL(level) \
        _LOG_CONTEXT_SET_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#define LOG_CONTEXT_धकेलो(...) \
        LOG_CONTEXT_धकेलो_STRV(STRV_MAKE(__VA_ARGS__))

#define LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_धकेलो(snprintf_ok((वर्ण[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#define LOG_CONTEXT_धकेलो_कुंजी_मूल्य(कुंजी, मूल्य, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new(कुंजी, मूल्य);

#define LOG_CONTEXT_धकेलो_कुंजी_मूल्य_2(कुंजी, मूल्य) \
        LOG_CONTEXT_धकेलो_कुंजी_मूल्य(कुंजी, मूल्य, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_धकेलो_STRV_2(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#define LOG_CONTEXT_धकेलो_STRV(strv) \
        LOG_CONTEXT_धकेलो_STRV_2(strv, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_धकेलो_IOV(निवेश_iovec, n_निवेश_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(निवेश_iovec, n_निवेश_iovec, /* owned= */ false);

#define LOG_CONTEXT_धकेलो_IOV_2(निवेश_iovec, n_निवेश_iovec) \
        LOG_CONTEXT_धकेलो_IOV(निवेश_iovec, n_निवेश_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#define _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        _unused_ _cleanup_strv_free_ strv = strv_new(s);                                                \
        यदि (!strv)                                                                                      \
                मुक्त(s);                                                                                \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#define LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#define _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#define LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#define _LOG_CONTEXT_CONSUME_IOV(निवेश_iovec, n_निवेश_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(निवेश_iovec, n_निवेश_iovec);

#define LOG_CONTEXT_CONSUME_IOV(निवेश_iovec, n_निवेश_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(निवेश_iovec, n_निवेश_iovec, UNIQ_T(c, UNIQ))
