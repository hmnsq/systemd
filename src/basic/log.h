/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <stdio.h>
#شامل <string.h>
#شامل <syslog.h>

#شامل "forward.h"

قسمتعریف شمار Log_ہدف{
        LOG_ہدف_CONSOLE,
        LOG_ہدف_KMSG,
        LOG_ہدف_JOURNAL,
        LOG_ہدف_SYSLOG,
        LOG_ہدف_CONSOLE_PREFIXED,
        LOG_ہدف_JOURNAL_OR_KMSG,
        LOG_ہدف_SYSLOG_OR_KMSG,
        LOG_ہدف_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_ہدف_NULL,
        LOG_ہدف_SINGLE_MAX = LOG_ہدف_SYSLOG + 1,
        LOG_ہدف_MAX = LOG_ہدف_NULL + 1,
        LOG_ہدف_INVALID = -EINVAL,
} Log_ہدف;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#تعریف_کرو LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#تعریف_کرو SYNTHETIC_ERRNO(عدد)                (ABS(عدد) | (1 << 30))
#تعریف_کرو IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#تعریف_کرو ERRNO_قدر(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
قسمتعریف خالی (*log_syntax_callback_t)(ثابت حرف *unit, صحیح level, خالی *userdata);
خالی سیٹ_log_syntax_callback(log_syntax_callback_t cb, خالی *userdata);

ساکن اندرونی خالی clear_log_syntax_callback(dummy_t *dummy) {
          سیٹ_log_syntax_callback(/* cb= */ تہی, /* userdata= */ تہی);
}

DECLARE_STRING_جدول_LOOKUP(log_ہدف, Log_ہدف);
خالی log_سیٹ_ہدف(Log_ہدف ہدف);
خالی log_سیٹ_ہدف_and_open(Log_ہدف ہدف);
صحیح log_سیٹ_ہدف_from_string(ثابت حرف *e);
Log_ہدف log_get_ہدف(خالی) خالص;
خالی log_settle_ہدف(خالی);

صحیح log_سیٹ_max_level(صحیح level);
صحیح log_سیٹ_max_level_from_string(ثابت حرف *e);
صحیح log_get_max_level(خالی) خالص;
صحیح log_get_ہدف_max_level(Log_ہدف ہدف);
صحیح log_max_سطحیں_مقصد_string(صحیح level, حرف **ret);

خالی log_سیٹ_facility(صحیح facility);

خالی log_show_color(bool b);
bool log_get_show_color(خالی) خالص;
خالی log_show_location(bool b);
bool log_get_show_location(خالی) خالص;
خالی log_show_وقت(bool b);
bool log_get_show_وقت(خالی) خالص;
خالی log_show_tid(bool b);
bool log_get_show_tid(خالی) خالص;
خالی log_show_executable(bool b);
bool log_get_show_executable(خالی) خالص;

صحیح log_show_color_from_string(ثابت حرف *e);
صحیح log_show_location_from_string(ثابت حرف *e);
صحیح log_show_وقت_from_string(ثابت حرف *e);
صحیح log_show_tid_from_string(ثابت حرف *e);
صحیح log_show_executable_from_string(ثابت حرف *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(خالی);
صحیح log_open(خالی);
خالی log_close(خالی);
خالی log_forget_fds(خالی);

خالی log_parse_environment_variables(خالی);
خالی log_parse_environment(خالی);

صحیح log_dispatch_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                حرف *بفر);

صحیح log_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(6,7);

صحیح log_internalv(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format,
                va_list ap) _printf_(6,0);

صحیح log_object_internalv(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) _printf_(10,0);

صحیح log_object_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format, ...) _printf_(10,11);

صحیح log_struct_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _sentinel_;

صحیح log_oom_اندرونی(
                صحیح level,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func);

صحیح log_format_iovec(
                ساخت iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحیح error,
                ثابت حرف *format,
                va_list ap) _printf_(6, 0);

صحیح log_struct_iovec_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت ساخت iovec *داخلہ_iovec,
                size_t n_داخلہ_iovec);

/* This modifies the buffer passed! */
صحیح log_dump_اندرونی(
                صحیح level,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                حرف *بفر);

#تعریف_کرو log_dispatch(level, error, بفر)                              \
        log_dispatch_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, تہی, تہی, تہی, تہی, بفر)

/* Logging with level */
#تعریف_کرو log_بھرا_errno_zerook(level, error, ...)                        \
        ({                                                              \
                صحیح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_اندرونی(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#اگر BUILD_منوال_DEVELOPER && !defined(TEST_رمز)
#  define ASSERT_NON_ZERO(x) أكد((x) != 0)
#  define ASSERT_UNDERFLOW(x) أكد((x) >= INT_MIN)
#ورنہ
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#اگر_ختم

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#تعریف_کرو log_بھرا_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_بھرا_errno_zerook(level, (صحیح)_error, __VA_ARGS__); \
        })

#تعریف_کرو log_بھرا(level, fmt, ...)                                      \
        ({                                                             \
                اگر (BUILD_منوال_DEVELOPER)                              \
                        أكد(!لڑی_تلاش(fmt, "%m"));                    \
                (خالی) log_بھرا_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

صحیح log_emergency_level(خالی);

/* Normal logging */
#تعریف_کرو log_debug(...)     log_بھرا(LOG_DEBUG,   __VA_ARGS__)
#تعریف_کرو log_info(...)      log_بھرا(LOG_INFO,    __VA_ARGS__)
#تعریف_کرو log_notice(...)    log_بھرا(LOG_NOTICE,  __VA_ARGS__)
#تعریف_کرو log_warning(...)   log_بھرا(LOG_WARNING, __VA_ARGS__)
#تعریف_کرو log_error(...)     log_بھرا(LOG_ERR,     __VA_ARGS__)
#تعریف_کرو log_emergency(...) log_بھرا(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#تعریف_کرو log_debug_errno(error, ...)     log_بھرا_errno(LOG_DEBUG,   error, __VA_ARGS__)
#تعریف_کرو log_info_errno(error, ...)      log_بھرا_errno(LOG_INFO,    error, __VA_ARGS__)
#تعریف_کرو log_notice_errno(error, ...)    log_بھرا_errno(LOG_NOTICE,  error, __VA_ARGS__)
#تعریف_کرو log_warning_errno(error, ...)   log_بھرا_errno(LOG_WARNING, error, __VA_ARGS__)
#تعریف_کرو log_error_errno(error, ...)     log_بھرا_errno(LOG_ERR,     error, __VA_ARGS__)
#تعریف_کرو log_emergency_errno(error, ...) log_بھرا_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#تعریف_کرو log_once(level, ...)                                             \
        ({                                                               \
                اگر (ONCE)                                                \
                        log_بھرا(level, __VA_ARGS__);                    \
                ورنہ اگر (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#تعریف_کرو log_once_errno(level, error, ...)                                \
        ({                                                               \
                صحیح _err = (error);                                      \
                اگر (ONCE)                                                \
                        _err = log_بھرا_errno(level, _err, __VA_ARGS__); \
                ورنہ اگر (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ورنہ                                                     \
                        _err = -ERRNO_قدر(_err);                       \
                _err;                                                    \
        })

#اگر LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ورنہ
#  define log_trace(...)          کرو {} جب (0)
#  define log_trace_errno(e, ...) (-ERRNO_قدر(e))
#اگر_ختم

/* Structured logging */
#تعریف_کرو log_struct_errno(level, error, ...)                             \
        log_struct_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, تہی)
#تعریف_کرو log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#تعریف_کرو log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#تعریف_کرو log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#تعریف_کرو log_struct_once(level, ...)                                     \
        ({                                                              \
                اگر (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ورنہ اگر (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#تعریف_کرو log_dump(level, بفر)                                         \
        log_dump_اندرونی(level, 0, PROJECT_FILE, __LINE__, __func__, بفر)

#تعریف_کرو log_oom_بھرا(level) log_oom_اندرونی(level, PROJECT_FILE, __LINE__, __func__)
#تعریف_کرو log_oom()           log_oom_بھرا(LOG_ERR)
#تعریف_کرو log_oom_debug()     log_oom_بھرا(LOG_DEBUG)
#تعریف_کرو log_oom_warning()   log_oom_بھرا(LOG_WARNING)

bool log_on_console(خالی) خالص;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#اگر_تعریف_شدہ __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_شے(fmt, ...)    "dummy", تہی, ##__VA_ARGS__
#وگرنہ_اگر LOG_پیغام_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
خارجی bool log_پیغام_dummy;
#  define LOG_شے(fmt, ...) "%.0d" fmt, (log_پیغام_dummy && لکھو(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ورنہ
#  define LOG_شے(fmt, ...) fmt, ##__VA_ARGS__
#اگر_ختم

#تعریف_کرو LOG_پیغام(fmt, ...) LOG_شے("MESSAGE=" fmt, ##__VA_ARGS__)
#تعریف_کرو LOG_پیغام_ID(id)    LOG_شے("MESSAGE_ID=" id)

خالی log_received_signal(صحیح level, ثابت ساخت signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
خالی log_سیٹ_upgrade_syslog_مقصد_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
خالی log_سیٹ_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
خالی log_سیٹ_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
خالی log_سیٹ_prohibit_ipc(bool b);

صحیح log_dup_console(خالی);

صحیح log_syntax_اندرونی(
                ثابت حرف *unit,
                صحیح level,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                صحیح error,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(9, 10);

صحیح log_syntax_invalid_utf8_اندرونی(
                ثابت حرف *unit,
                صحیح level,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *rvalue);

صحیح log_syntax_parse_error_اندرونی(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                غیرعلامتی config_سطر,
                صحیح error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ثابت حرف *file,
                صحیح سطر,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue);

#تعریف_کرو log_syntax(unit, level, config_file, config_سطر, error, ...)   \
        ({                                                              \
                صحیح _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_اندرونی(unit, _level, config_file, config_سطر, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
        })

#تعریف_کرو log_syntax_invalid_utf8(unit, level, config_file, config_سطر, rvalue) \
        ({                                                              \
                صحیح _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_اندرونی(unit, _level, config_file, config_سطر, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#تعریف_کرو log_syntax_parse_error_بھرا(unit, config_file, config_سطر, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_اندرونی(unit, config_file, config_سطر, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#تعریف_کرو log_syntax_parse_error(unit, config_file, config_سطر, error, lvalue, rvalue) \
        log_syntax_parse_error_بھرا(unit, config_file, config_سطر, error, /* critical= */ false, lvalue, rvalue)

#تعریف_کرو DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

خالی log_تیاری(خالی);

ثابت حرف* log_سیٹ_سابقہ(ثابت حرف *سابقہ, bool force);

خالی log_سابقہ_تبادلہ(ثابت حرف **سابقہ);
ساکن اندرونی ثابت حرف* _log_unset_prefixp(ثابت حرف **p) {
        أكد(p);
        log_سیٹ_سابقہ(*p, true);
        واپس تہی;
}

#تعریف_کرو LOG_سیٹ_سابقہ(سابقہ) \
        _cleanup_(_log_unset_prefixp) غیر_مستعمل ثابت حرف *CONCATENATE(cleanup_log_unset_سابقہ, UNIQ) = log_سیٹ_سابقہ(سابقہ, false);
