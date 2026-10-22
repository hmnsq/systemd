/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

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
        अचिह्नित n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        वर्ण **fields;
        संरचना iovec *निवेशः_iovec;
        size_t n_निवेशः_iovec;
        वर्ण *कुञ्चिका;
        वर्ण *मानम्;
        bool owned;
        LIST_FIELDS(संरचना LogContext, ll);
} LogContext;

bool log_context_enabled(शून्य);

LogContext* log_context_new(ध्रुव वर्ण *कुञ्चिका, ध्रुव वर्ण *मानम्);
LogContext* log_context_new_strv(वर्ण **fields, bool owned);
LogContext* log_context_new_iov(संरचना iovec *निवेशः_iovec, size_t n_निवेशः_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(वर्ण **fields);
LogContext* log_context_new_iov_consume(संरचना iovec *निवेशः_iovec, size_t n_निवेशः_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_शीर्ष(शून्य);
/* Returns the number of attached log context objects. */
size_t log_context_अङ्कः_contexts(शून्य);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_अङ्कः_fields_2(शून्य);

शून्य log_context_swap(LogContext **log_context, size_t *अङ्कः_fields);

शून्य _reset_log_level(पूर्णाङ्क *saved_log_level);

#परिभाषय LOG_CONTEXT_समुच्चयः_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) अप्रयुक्तम् पूर्णाङ्क l = log_समुच्चयः_max_level(level);

#परिभाषय LOG_CONTEXT_समुच्चयः_LOG_LEVEL_2(level) \
        LOG_CONTEXT_समुच्चयः_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#परिभाषय LOG_CONTEXT_प्रक्षिप(...) \
        LOG_CONTEXT_प्रक्षिप_STRV(STRV_MAKE(__VA_ARGS__))

#परिभाषय LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_प्रक्षिप(snprintf_ok((वर्ण[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#परिभाषय LOG_CONTEXT_प्रक्षिप_कुञ्चिका_मानम्(कुञ्चिका, मानम्, c) \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new(कुञ्चिका, मानम्);

#परिभाषय LOG_CONTEXT_प्रक्षिप_कुञ्चिका_मानम्_2(कुञ्चिका, मानम्) \
        LOG_CONTEXT_प्रक्षिप_कुञ्चिका_मानम्(कुञ्चिका, मानम्, UNIQ_T(c, UNIQ))

#परिभाषय LOG_CONTEXT_प्रक्षिप_STRV_2(strv, c) \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#परिभाषय LOG_CONTEXT_प्रक्षिप_STRV(strv) \
        LOG_CONTEXT_प्रक्षिप_STRV_2(strv, UNIQ_T(c, UNIQ))

#परिभाषय LOG_CONTEXT_प्रक्षिप_IOV(निवेशः_iovec, n_निवेशः_iovec, c) \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(निवेशः_iovec, n_निवेशः_iovec, /* owned= */ false);

#परिभाषय LOG_CONTEXT_प्रक्षिप_IOV_2(निवेशः_iovec, n_निवेशः_iovec) \
        LOG_CONTEXT_प्रक्षिप_IOV(निवेशः_iovec, n_निवेशः_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#परिभाषय _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        अप्रयुक्तम् _cleanup_strv_free_ strv = strv_new(s);                                                \
        यदि (!strv)                                                                                      \
                मुक्त(s);                                                                                \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#परिभाषय LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#परिभाषय _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#परिभाषय LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#परिभाषय _LOG_CONTEXT_CONSUME_IOV(निवेशः_iovec, n_निवेशः_iovec, c) \
        अप्रयुक्तम् _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(निवेशः_iovec, n_निवेशः_iovec);

#परिभाषय LOG_CONTEXT_CONSUME_IOV(निवेशः_iovec, n_निवेशः_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(निवेशः_iovec, n_निवेशः_iovec, UNIQ_T(c, UNIQ))
