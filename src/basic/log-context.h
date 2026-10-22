/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"
#ಅನ್ತರ್ಭಾವ "list.h"

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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ LogContext {
        unsigned n_ref;
        /* Depending on which destructor is used (log_context_free() or log_context_detach()) the memory
         * referenced by this is freed or not */
        ವರ್ಣ **fields;
        ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec;
        size_t n_ಒಳಹರಿವು_iovec;
        ವರ್ಣ *ಕೀಲಿ;
        ವರ್ಣ *ಮೌಲ್ಯ;
        bool owned;
        LIST_FIELDS(ಸಂರಚನಾ LogContext, ll);
} LogContext;

bool log_context_enabled(ಶೂನ್ಯ);

LogContext* log_context_new(ಧ್ರುವ ವರ್ಣ *ಕೀಲಿ, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ);
LogContext* log_context_new_strv(ವರ್ಣ **fields, bool owned);
LogContext* log_context_new_iov(ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec, bool owned);

/* Same as log_context_new(), but frees the given fields strv/iovec on failure. */
LogContext* log_context_new_strv_consume(ವರ್ಣ **fields);
LogContext* log_context_new_iov_consume(ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec, size_t n_ಒಳಹರಿವು_iovec);

DECLARE_TRIVIAL_REF_UNREF_FUNC(LogContext, log_context);

DEFINE_TRIVIAL_CLEANUP_FUNC(LogContext*, log_context_unref);

/* Returns the head of the log context list. */
LogContext* log_context_head(ಶೂನ್ಯ);
/* Returns the number of attached log context objects. */
size_t log_context_ಅಂಕೆ_contexts(ಶೂನ್ಯ);
/* Returns the number of fields in all attached log contexts. */
size_t log_context_ಅಂಕೆ_fields_2(ಶೂನ್ಯ);

ಶೂನ್ಯ log_context_swap(LogContext **log_context, size_t *ಅಂಕೆ_fields);

ಶೂನ್ಯ _reset_log_level(ಪೂರ್ಣಾಙ್ಕ *saved_log_level);

#define _LOG_CONTEXT_SET_LOG_LEVEL(level, l) \
        _cleanup_(_reset_log_level) _unused_ ಪೂರ್ಣಾಙ್ಕ l = log_set_max_level(level);

#define LOG_CONTEXT_SET_LOG_LEVEL(level) \
        _LOG_CONTEXT_SET_LOG_LEVEL(level, UNIQ_T(l, UNIQ))

#define LOG_CONTEXT_ತಳ್ಳು(...) \
        LOG_CONTEXT_ತಳ್ಳು_STRV(STRV_MAKE(__VA_ARGS__))

#define LOG_CONTEXT_PUSHF(...) \
        LOG_CONTEXT_ತಳ್ಳು(snprintf_ok((ವರ್ಣ[LINE_MAX]) {}, LINE_MAX, __VA_ARGS__))

#define LOG_CONTEXT_ತಳ್ಳು_ಕೀಲಿ_ಮೌಲ್ಯ(ಕೀಲಿ, ಮೌಲ್ಯ, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new(ಕೀಲಿ, ಮೌಲ್ಯ);

#define LOG_CONTEXT_ತಳ್ಳು_ಕೀಲಿ_ಮೌಲ್ಯ_2(ಕೀಲಿ, ಮೌಲ್ಯ) \
        LOG_CONTEXT_ತಳ್ಳು_ಕೀಲಿ_ಮೌಲ್ಯ(ಕೀಲಿ, ಮೌಲ್ಯ, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ತಳ್ಳು_STRV_2(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv(strv, /* owned= */ false);

#define LOG_CONTEXT_ತಳ್ಳು_STRV(strv) \
        LOG_CONTEXT_ತಳ್ಳು_STRV_2(strv, UNIQ_T(c, UNIQ))

#define LOG_CONTEXT_ತಳ್ಳು_IOV(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, /* owned= */ false);

#define LOG_CONTEXT_ತಳ್ಳು_IOV_2(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec) \
        LOG_CONTEXT_ತಳ್ಳು_IOV(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, UNIQ_T(c, UNIQ))

/* LOG_CONTEXT_CONSUME_STR()/LOG_CONTEXT_CONSUME_STRV()/LOG_CONTEXT_CONSUME_IOV() are identical to
* LOG_CONTEXT_PUSH_STR()/LOG_CONTEXT_PUSH_STRV()/LOG_CONTEXT_PUSH_IOV() except they take ownership of the
* given str/strv argument.
*/

#define _LOG_CONTEXT_CONSUME_STR(s, c, strv) \
        _unused_ _cleanup_strv_free_ strv = strv_new(s);                                                \
        ಯದಿ (!strv)                                                                                      \
                ಮುಕ್ತ(s);                                                                                \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(TAKE_PTR(strv))

#define LOG_CONTEXT_CONSUME_STR(s) \
        _LOG_CONTEXT_CONSUME_STR(s, UNIQ_T(c, UNIQ), UNIQ_T(sv, UNIQ))

#define _LOG_CONTEXT_CONSUME_STRV(strv, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_strv_consume(strv);

#define LOG_CONTEXT_CONSUME_STRV(strv) \
        _LOG_CONTEXT_CONSUME_STRV(strv, UNIQ_T(c, UNIQ))

#define _LOG_CONTEXT_CONSUME_IOV(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, c) \
        _unused_ _cleanup_(log_context_unrefp) LogContext *c = log_context_new_iov_consume(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec);

#define LOG_CONTEXT_CONSUME_IOV(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec) \
        _LOG_CONTEXT_CONSUME_IOV(ಒಳಹರಿವು_iovec, n_ಒಳಹರಿವು_iovec, UNIQ_T(c, UNIQ))
