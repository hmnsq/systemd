/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <syslog.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ Log_ਨਿਸ਼ਾਨਾ{
        LOG_ਨਿਸ਼ਾਨਾ_CONSOLE,
        LOG_ਨਿਸ਼ਾਨਾ_KMSG,
        LOG_ਨਿਸ਼ਾਨਾ_JOURNAL,
        LOG_ਨਿਸ਼ਾਨਾ_SYSLOG,
        LOG_ਨਿਸ਼ਾਨਾ_CONSOLE_PREFIXED,
        LOG_ਨਿਸ਼ਾਨਾ_JOURNAL_OR_KMSG,
        LOG_ਨਿਸ਼ਾਨਾ_SYSLOG_OR_KMSG,
        LOG_ਨਿਸ਼ਾਨਾ_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_ਨਿਸ਼ਾਨਾ_NULL,
        LOG_ਨਿਸ਼ਾਨਾ_SINGLE_MAX = LOG_ਨਿਸ਼ਾਨਾ_SYSLOG + 1,
        LOG_ਨਿਸ਼ਾਨਾ_MAX = LOG_ਨਿਸ਼ਾਨਾ_NULL + 1,
        LOG_ਨਿਸ਼ਾਨਾ_INVALID = -EINVAL,
} Log_ਨਿਸ਼ਾਨਾ;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SYNTHETIC_ERRNO(ਅੰਕ)                (ABS(ਅੰਕ) | (1 << 30))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ERRNO_ਮੁੱਲ(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਖਾਲੀ (*log_syntax_callback_t)(ਅਚਲ ਅੱਖਰ *unit, ਪੂਰਨ level, ਖਾਲੀ *userdata);
ਖਾਲੀ ਸੈੱਟ_log_syntax_callback(log_syntax_callback_t cb, ਖਾਲੀ *userdata);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ clear_log_syntax_callback(dummy_t *dummy) {
          ਸੈੱਟ_log_syntax_callback(/* cb= */ ਨਿਰਾ, /* userdata= */ ਨਿਰਾ);
}

DECLARE_STRING_ਸਾਰਣੀ_LOOKUP(log_ਨਿਸ਼ਾਨਾ, Log_ਨਿਸ਼ਾਨਾ);
ਖਾਲੀ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ);
ਖਾਲੀ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_and_open(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ);
ਪੂਰਨ log_ਸੈੱਟ_ਨਿਸ਼ਾਨਾ_from_string(ਅਚਲ ਅੱਖਰ *e);
Log_ਨਿਸ਼ਾਨਾ log_get_ਨਿਸ਼ਾਨਾ(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਖਾਲੀ log_settle_ਨਿਸ਼ਾਨਾ(ਖਾਲੀ);

ਪੂਰਨ log_ਸੈੱਟ_max_level(ਪੂਰਨ level);
ਪੂਰਨ log_ਸੈੱਟ_max_level_from_string(ਅਚਲ ਅੱਖਰ *e);
ਪੂਰਨ log_get_max_level(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਪੂਰਨ log_get_ਨਿਸ਼ਾਨਾ_max_level(Log_ਨਿਸ਼ਾਨਾ ਨਿਸ਼ਾਨਾ);
ਪੂਰਨ log_max_ਪੱਧਰ_ਮੰਜ਼ਿਲ_string(ਪੂਰਨ level, ਅੱਖਰ **ret);

ਖਾਲੀ log_ਸੈੱਟ_facility(ਪੂਰਨ facility);

ਖਾਲੀ log_show_color(bool b);
bool log_get_show_color(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਖਾਲੀ log_show_location(bool b);
bool log_get_show_location(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਖਾਲੀ log_show_ਸਮਾਂ(bool b);
bool log_get_show_ਸਮਾਂ(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਖਾਲੀ log_show_tid(bool b);
bool log_get_show_tid(ਖਾਲੀ) ਸ਼ੁੱਧ;
ਖਾਲੀ log_show_executable(bool b);
bool log_get_show_executable(ਖਾਲੀ) ਸ਼ੁੱਧ;

ਪੂਰਨ log_show_color_from_string(ਅਚਲ ਅੱਖਰ *e);
ਪੂਰਨ log_show_location_from_string(ਅਚਲ ਅੱਖਰ *e);
ਪੂਰਨ log_show_ਸਮਾਂ_from_string(ਅਚਲ ਅੱਖਰ *e);
ਪੂਰਨ log_show_tid_from_string(ਅਚਲ ਅੱਖਰ *e);
ਪੂਰਨ log_show_executable_from_string(ਅਚਲ ਅੱਖਰ *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(ਖਾਲੀ);
ਪੂਰਨ log_open(ਖਾਲੀ);
ਖਾਲੀ log_close(ਖਾਲੀ);
ਖਾਲੀ log_forget_fds(ਖਾਲੀ);

ਖਾਲੀ log_parse_environment_variables(ਖਾਲੀ);
ਖਾਲੀ log_parse_environment(ਖਾਲੀ);

ਪੂਰਨ log_dispatch_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅੱਖਰ *ਬਫਰ);

ਪੂਰਨ log_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) _printf_(6,7);

ਪੂਰਨ log_internalv(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) _printf_(6,0);

ਪੂਰਨ log_object_internalv(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) _printf_(10,0);

ਪੂਰਨ log_object_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *object_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *object,
                ਅਚਲ ਅੱਖਰ *extra_ਖੇਤਰ,
                ਅਚਲ ਅੱਖਰ *extra,
                ਅਚਲ ਅੱਖਰ *format, ...) _printf_(10,11);

ਪੂਰਨ log_struct_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) _sentinel_;

ਪੂਰਨ log_oom_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func);

ਪੂਰਨ log_format_iovec(
                ਬਣਤਰ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *format,
                va_list ap) _printf_(6, 0);

ਪੂਰਨ log_struct_iovec_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਬਣਤਰ iovec *ਨਿਵੇਸ਼_iovec,
                size_t n_ਨਿਵੇਸ਼_iovec);

/* This modifies the buffer passed! */
ਪੂਰਨ log_dump_ਅੰਦਰੂਨੀ(
                ਪੂਰਨ level,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅੱਖਰ *ਬਫਰ);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_dispatch(level, error, ਬਫਰ)                              \
        log_dispatch_ਅੰਦਰੂਨੀ(level, error, PROJECT_FILE, __LINE__, __func__, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਬਫਰ)

/* Logging with level */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ਭਰਿਆ_errno_zerook(level, error, ...)                        \
        ({                                                              \
                ਪੂਰਨ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ਅੰਦਰੂਨੀ(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ਮੁੱਲ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ਜੇ BUILD_ਬਹੁਲਕ_DEVELOPER && !defined(TEST_ਕੋਡ)
#  define ASSERT_NON_ZERO(x) ਪ੍ਰਤਿਜ੍ਞਾ((x) != 0)
#  define ASSERT_UNDERFLOW(x) ਪ੍ਰਤਿਜ੍ਞਾ((x) >= INT_MIN)
#ਨਹੀਂਤਾਂ
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#ਜੇ_ਅੰਤ

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ਭਰਿਆ_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_ਭਰਿਆ_errno_zerook(level, (ਪੂਰਨ)_error, __VA_ARGS__); \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ਭਰਿਆ(level, fmt, ...)                                      \
        ({                                                             \
                ਜੇ (BUILD_ਬਹੁਲਕ_DEVELOPER)                              \
                        ਪ੍ਰਤਿਜ੍ਞਾ(!ਲੜੀ_ਖੋਜ(fmt, "%m"));                    \
                (ਖਾਲੀ) log_ਭਰਿਆ_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

ਪੂਰਨ log_emergency_level(ਖਾਲੀ);

/* Normal logging */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_debug(...)     log_ਭਰਿਆ(LOG_DEBUG,   __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_info(...)      log_ਭਰਿਆ(LOG_INFO,    __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_notice(...)    log_ਭਰਿਆ(LOG_NOTICE,  __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_warning(...)   log_ਭਰਿਆ(LOG_WARNING, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_error(...)     log_ਭਰਿਆ(LOG_ERR,     __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_emergency(...) log_ਭਰਿਆ(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_debug_errno(error, ...)     log_ਭਰਿਆ_errno(LOG_DEBUG,   error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_info_errno(error, ...)      log_ਭਰਿਆ_errno(LOG_INFO,    error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_notice_errno(error, ...)    log_ਭਰਿਆ_errno(LOG_NOTICE,  error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_warning_errno(error, ...)   log_ਭਰਿਆ_errno(LOG_WARNING, error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_error_errno(error, ...)     log_ਭਰਿਆ_errno(LOG_ERR,     error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_emergency_errno(error, ...) log_ਭਰਿਆ_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_once(level, ...)                                             \
        ({                                                               \
                ਜੇ (ONCE)                                                \
                        log_ਭਰਿਆ(level, __VA_ARGS__);                    \
                ਨਹੀਂਤਾਂ ਜੇ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_once_errno(level, error, ...)                                \
        ({                                                               \
                ਪੂਰਨ _err = (error);                                      \
                ਜੇ (ONCE)                                                \
                        _err = log_ਭਰਿਆ_errno(level, _err, __VA_ARGS__); \
                ਨਹੀਂਤਾਂ ਜੇ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ਨਹੀਂਤਾਂ                                                     \
                        _err = -ERRNO_ਮੁੱਲ(_err);                       \
                _err;                                                    \
        })

#ਜੇ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ਨਹੀਂਤਾਂ
#  define log_trace(...)          ਕਰੋ {} ਜਦੋਂ (0)
#  define log_trace_errno(e, ...) (-ERRNO_ਮੁੱਲ(e))
#ਜੇ_ਅੰਤ

/* Structured logging */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_struct_errno(level, error, ...)                             \
        log_struct_ਅੰਦਰੂਨੀ(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_ਅੰਦਰੂਨੀ(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_struct_once(level, ...)                                     \
        ({                                                              \
                ਜੇ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ਨਹੀਂਤਾਂ ਜੇ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_dump(level, ਬਫਰ)                                         \
        log_dump_ਅੰਦਰੂਨੀ(level, 0, PROJECT_FILE, __LINE__, __func__, ਬਫਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_oom_ਭਰਿਆ(level) log_oom_ਅੰਦਰੂਨੀ(level, PROJECT_FILE, __LINE__, __func__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_oom()           log_oom_ਭਰਿਆ(LOG_ERR)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_oom_debug()     log_oom_ਭਰਿਆ(LOG_DEBUG)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_oom_warning()   log_oom_ਭਰਿਆ(LOG_WARNING)

bool log_on_console(ਖਾਲੀ) ਸ਼ੁੱਧ;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ਵਸਤੂ(fmt, ...)    "dummy", ਨਿਰਾ, ##__VA_ARGS__
#ਜਾਂਜੇ LOG_ਸੁਨੇਹਾ_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
ਬਾਹਰੀ bool log_ਸੁਨੇਹਾ_dummy;
#  define LOG_ਵਸਤੂ(fmt, ...) "%.0d" fmt, (log_ਸੁਨੇਹਾ_dummy && ਲਿਖੋ(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ਨਹੀਂਤਾਂ
#  define LOG_ਵਸਤੂ(fmt, ...) fmt, ##__VA_ARGS__
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਸੁਨੇਹਾ(fmt, ...) LOG_ਵਸਤੂ("MESSAGE=" fmt, ##__VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਸੁਨੇਹਾ_ID(id)    LOG_ਵਸਤੂ("MESSAGE_ID=" id)

ਖਾਲੀ log_received_signal(ਪੂਰਨ level, ਅਚਲ ਬਣਤਰ signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
ਖਾਲੀ log_ਸੈੱਟ_upgrade_syslog_ਮੰਜ਼ਿਲ_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
ਖਾਲੀ log_ਸੈੱਟ_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
ਖਾਲੀ log_ਸੈੱਟ_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
ਖਾਲੀ log_ਸੈੱਟ_prohibit_ipc(bool b);

ਪੂਰਨ log_dup_console(ਖਾਲੀ);

ਪੂਰਨ log_syntax_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਪੂਰਨ level,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਪੂਰਨ error,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *format, ...) _printf_(9, 10);

ਪੂਰਨ log_syntax_invalid_utf8_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਪੂਰਨ level,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *rvalue);

ਪੂਰਨ log_syntax_parse_error_ਅੰਦਰੂਨੀ(
                ਅਚਲ ਅੱਖਰ *unit,
                ਅਚਲ ਅੱਖਰ *config_file,
                ਬਿਨਾਨਿਸ਼ਾਨੀ config_ਸਤਰ,
                ਪੂਰਨ error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ਅਚਲ ਅੱਖਰ *file,
                ਪੂਰਨ ਸਤਰ,
                ਅਚਲ ਅੱਖਰ *func,
                ਅਚਲ ਅੱਖਰ *lvalue,
                ਅਚਲ ਅੱਖਰ *rvalue);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_syntax(unit, level, config_file, config_ਸਤਰ, error, ...)   \
        ({                                                              \
                ਪੂਰਨ _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_ਅੰਦਰੂਨੀ(unit, _level, config_file, config_ਸਤਰ, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ਮੁੱਲ(_e);                             \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_syntax_invalid_utf8(unit, level, config_file, config_ਸਤਰ, rvalue) \
        ({                                                              \
                ਪੂਰਨ _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_ਅੰਦਰੂਨੀ(unit, _level, config_file, config_ਸਤਰ, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_syntax_parse_error_ਭਰਿਆ(unit, config_file, config_ਸਤਰ, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_ਅੰਦਰੂਨੀ(unit, config_file, config_ਸਤਰ, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_syntax_parse_error(unit, config_file, config_ਸਤਰ, error, lvalue, rvalue) \
        log_syntax_parse_error_ਭਰਿਆ(unit, config_file, config_ਸਤਰ, error, /* critical= */ false, lvalue, rvalue)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

ਖਾਲੀ log_ਤਿਆਰੀ(ਖਾਲੀ);

ਅਚਲ ਅੱਖਰ* log_ਸੈੱਟ_ਅਗੇਤਰ(ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, bool force);

ਖਾਲੀ log_ਅਗੇਤਰ_ਅਦਲਾ_ਬਦਲੀ(ਅਚਲ ਅੱਖਰ **ਅਗੇਤਰ);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ ਅੱਖਰ* _log_unset_prefixp(ਅਚਲ ਅੱਖਰ **p) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        log_ਸੈੱਟ_ਅਗੇਤਰ(*p, true);
        ਵਾਪਸ ਨਿਰਾ;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਸੈੱਟ_ਅਗੇਤਰ(ਅਗੇਤਰ) \
        _cleanup_(_log_unset_prefixp) ਅਣਵਰਤਿਆ ਅਚਲ ਅੱਖਰ *CONCATENATE(cleanup_log_unset_ਅਗੇਤਰ, UNIQ) = log_ਸੈੱਟ_ਅਗੇਤਰ(ਅਗੇਤਰ, false);
