/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "forward.h"
#ضمن "list.h"

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

تعريفنوع بنية LogContext {
        بیعلامت n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        حرف **fields;
        بنية iovec *درون_داد_iovec;
        size_t n_درون_داد_iovec;
        حرف *کلید;
        حرف *مقدار;
        bool owned;
        LIST_FIELDS(بنية LogContext, ll);
} LogContext;

bool log_context_enabled(فراغ);

LogContext* log_context_new(ثابت حرف *کلید, ثابت حرف *مقدار);
LogContext* log_context_new_strv(حرف **fields, bool owned);
LogContext* log_context_new_iov(بنية iovec *درون_داد_iovec, size_t n_درون_داد_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(حرف **fields);
LogContext* log_context_new_iov_consume(بنية iovec *درون_داد_iovec, size_t n_درون_داد_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_سر(فراغ);
/* Returns the number of attached log context objects. */
size_t log_context_رقم_contexts(فراغ);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_رقم_fields_2(فراغ);

فراغ log_context_جابجا(LogContext **log_context, size_t *رقم_fields);

فراغ _reset_log_level(صحيح *saved_log_level);

#تعریف_کن LOG_CONTEXT_مجموعه_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) استفادهنشده صحيح l = log_مجموعه_max_level(level);

#تعریف_کن LOG_CONTEXT_مجموعه_LOG_LEVEL_2(level) \
        LOG_CONTEXT_مجموعه_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#تعریف_کن LOG_CONTEXT_افزودن(...) \
        LOG_CONTEXT_افزودن_STRV(STRV_MAKE(__VA_ARGS__))

#تعریف_کن LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_افزودن(snprintf_ok((حرف[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#تعریف_کن LOG_CONTEXT_افزودن_کلید_مقدار(کلید, مقدار, c) \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new(کلید, مقدار);

#تعریف_کن LOG_CONTEXT_افزودن_کلید_مقدار_2(کلید, مقدار) \
        LOG_CONTEXT_افزودن_کلید_مقدار(کلید, مقدار, UNIQ_T(c, UNIQ))

#تعریف_کن LOG_CONTEXT_افزودن_STRV_2(strv, c) \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#تعریف_کن LOG_CONTEXT_افزودن_STRV(strv) \
        LOG_CONTEXT_افزودن_STRV_2(strv, UNIQ_T(c, UNIQ))

#تعریف_کن LOG_CONTEXT_افزودن_IOV(درون_داد_iovec, n_درون_داد_iovec, c) \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(درون_داد_iovec, n_درون_داد_iovec, /* owned= */ false);

#تعریف_کن LOG_CONTEXT_افزودن_IOV_2(درون_داد_iovec, n_درون_داد_iovec) \
        LOG_CONTEXT_افزودن_IOV(درون_داد_iovec, n_درون_داد_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#تعریف_کن _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        استفادهنشده _cleanup_strv_free_ strv = strv_new(s);                                                \
        إذا (!strv)                                                                                      \
                حرر(s);                                                                                \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#تعریف_کن LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#تعریف_کن _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#تعریف_کن LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#تعریف_کن _LOG_CONTEXT_CONSUME_IOV(درون_داد_iovec, n_درون_داد_iovec, c) \
        استفادهنشده _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(درون_داد_iovec, n_درون_داد_iovec);

#تعریف_کن LOG_CONTEXT_CONSUME_IOV(درون_داد_iovec, n_درون_داد_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(درون_داد_iovec, n_درون_داد_iovec, UNIQ_T(c, UNIQ))
