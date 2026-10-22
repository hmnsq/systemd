/* SPDX-License-Identifier: LGPL-2.1-or-later */
#لارښوونه once

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
        بینښه n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        حرف **fields;
        بنية iovec *ننوت_iovec;
        size_t n_ننوت_iovec;
        حرف *کلی;
        حرف *ارزښت;
        bool owned;
        LIST_FIELDS(بنية LogContext, ll);
} LogContext;

bool log_context_enabled(فراغ);

LogContext* log_context_new(ثابت حرف *کلی, ثابت حرف *ارزښت);
LogContext* log_context_new_strv(حرف **fields, bool owned);
LogContext* log_context_new_iov(بنية iovec *ننوت_iovec, size_t n_ننوت_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(حرف **fields);
LogContext* log_context_new_iov_consume(بنية iovec *ننوت_iovec, size_t n_ننوت_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_سر(فراغ);
/* Returns the number of attached log context objects. */
size_t log_context_عدد_contexts(فراغ);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_عدد_fields_2(فراغ);

فراغ log_context_بدلون(LogContext **log_context, size_t *عدد_fields);

فراغ _reset_log_level(صحيح *saved_log_level);

#تعریف_کړه LOG_CONTEXT_سېټ_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) ناکارول_شوی صحيح l = log_سېټ_max_level(level);

#تعریف_کړه LOG_CONTEXT_سېټ_LOG_LEVEL_2(level) \
        LOG_CONTEXT_سېټ_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#تعریف_کړه LOG_CONTEXT_ټېل(...) \
        LOG_CONTEXT_ټېل_STRV(STRV_MAKE(__VA_ARGS__))

#تعریف_کړه LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_ټېل(snprintf_ok((حرف[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#تعریف_کړه LOG_CONTEXT_ټېل_کلی_ارزښت(کلی, ارزښت, c) \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new(کلی, ارزښت);

#تعریف_کړه LOG_CONTEXT_ټېل_کلی_ارزښت_2(کلی, ارزښت) \
        LOG_CONTEXT_ټېل_کلی_ارزښت(کلی, ارزښت, UNIQ_T(c, UNIQ))

#تعریف_کړه LOG_CONTEXT_ټېل_STRV_2(strv, c) \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#تعریف_کړه LOG_CONTEXT_ټېل_STRV(strv) \
        LOG_CONTEXT_ټېل_STRV_2(strv, UNIQ_T(c, UNIQ))

#تعریف_کړه LOG_CONTEXT_ټېل_IOV(ننوت_iovec, n_ننوت_iovec, c) \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(ننوت_iovec, n_ننوت_iovec, /* owned= */ false);

#تعریف_کړه LOG_CONTEXT_ټېل_IOV_2(ننوت_iovec, n_ننوت_iovec) \
        LOG_CONTEXT_ټېل_IOV(ننوت_iovec, n_ننوت_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#تعریف_کړه _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        ناکارول_شوی _cleanup_strv_free_ strv = strv_new(s);                                                \
        إذا (!strv)                                                                                      \
                حرر(s);                                                                                \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#تعریف_کړه LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#تعریف_کړه _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#تعریف_کړه LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#تعریف_کړه _LOG_CONTEXT_CONSUME_IOV(ننوت_iovec, n_ننوت_iovec, c) \
        ناکارول_شوی _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(ننوت_iovec, n_ننوت_iovec);

#تعریف_کړه LOG_CONTEXT_CONSUME_IOV(ننوت_iovec, n_ننوت_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(ننوت_iovec, n_ننوت_iovec, UNIQ_T(c, UNIQ))
