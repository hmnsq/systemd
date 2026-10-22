/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <syslog.h>

#ضمن "forward.h"

تعريفنوع تعداد Log_هدف{
        LOG_هدف_CONSOLE,
        LOG_هدف_KMSG,
        LOG_هدف_JOURNAL,
        LOG_هدف_SYSLOG,
        LOG_هدف_CONSOLE_PREFIXED,
        LOG_هدف_JOURNAL_OR_KMSG,
        LOG_هدف_SYSLOG_OR_KMSG,
        LOG_هدف_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_هدف_NULL,
        LOG_هدف_SINGLE_MAX = LOG_هدف_SYSLOG + 1,
        LOG_هدف_MAX = LOG_هدف_NULL + 1,
        LOG_هدف_INVALID = -EINVAL,
} Log_هدف;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#تعریف_کن LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#تعریف_کن SYNTHETIC_ERRNO(رقم)                (ABS(رقم) | (1 << 30))
#تعریف_کن IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#تعریف_کن ERRNO_مقدار(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
تعريفنوع فراغ (*log_syntax_callback_t)(ثابت حرف *unit, صحيح level, فراغ *userdata);
فراغ مجموعه_log_syntax_callback(log_syntax_callback_t cb, فراغ *userdata);

ساكن مضمن فراغ clear_log_syntax_callback(dummy_t *dummy) {
          مجموعه_log_syntax_callback(/* cb= */ عدم, /* userdata= */ عدم);
}

DECLARE_STRING_جدول_LOOKUP(log_هدف, Log_هدف);
فراغ log_مجموعه_هدف(Log_هدف هدف);
فراغ log_مجموعه_هدف_and_open(Log_هدف هدف);
صحيح log_مجموعه_هدف_from_string(ثابت حرف *e);
Log_هدف log_get_هدف(فراغ) خالص;
فراغ log_settle_هدف(فراغ);

صحيح log_مجموعه_max_level(صحيح level);
صحيح log_مجموعه_max_level_from_string(ثابت حرف *e);
صحيح log_get_max_level(فراغ) خالص;
صحيح log_get_هدف_max_level(Log_هدف هدف);
صحيح log_max_سطوح_مقصد_string(صحيح level, حرف **ret);

فراغ log_مجموعه_facility(صحيح facility);

فراغ log_show_color(bool b);
bool log_get_show_color(فراغ) خالص;
فراغ log_show_location(bool b);
bool log_get_show_location(فراغ) خالص;
فراغ log_show_زمان(bool b);
bool log_get_show_زمان(فراغ) خالص;
فراغ log_show_tid(bool b);
bool log_get_show_tid(فراغ) خالص;
فراغ log_show_executable(bool b);
bool log_get_show_executable(فراغ) خالص;

صحيح log_show_color_from_string(ثابت حرف *e);
صحيح log_show_location_from_string(ثابت حرف *e);
صحيح log_show_زمان_from_string(ثابت حرف *e);
صحيح log_show_tid_from_string(ثابت حرف *e);
صحيح log_show_executable_from_string(ثابت حرف *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(فراغ);
صحيح log_open(فراغ);
فراغ log_close(فراغ);
فراغ log_forget_fds(فراغ);

فراغ log_parse_environment_variables(فراغ);
فراغ log_parse_environment(فراغ);

صحيح log_dispatch_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *object_فیلد,
                ثابت حرف *object,
                ثابت حرف *extra_فیلد,
                ثابت حرف *extra,
                حرف *میانگیر);

صحيح log_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(6,7);

صحيح log_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format,
                va_list ap) _printf_(6,0);

صحيح log_object_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *object_فیلد,
                ثابت حرف *object,
                ثابت حرف *extra_فیلد,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) _printf_(10,0);

صحيح log_object_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *object_فیلد,
                ثابت حرف *object,
                ثابت حرف *extra_فیلد,
                ثابت حرف *extra,
                ثابت حرف *format, ...) _printf_(10,11);

صحيح log_struct_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _sentinel_;

صحيح log_oom_داخلی(
                صحيح level,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func);

صحيح log_format_iovec(
                بنية iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحيح error,
                ثابت حرف *format,
                va_list ap) _printf_(6, 0);

صحيح log_struct_iovec_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت بنية iovec *درون_داد_iovec,
                size_t n_درون_داد_iovec);

/* This modifies the buffer passed! */
صحيح log_dump_داخلی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                حرف *میانگیر);

#تعریف_کن log_dispatch(level, error, میانگیر)                              \
        log_dispatch_داخلی(level, error, PROJECT_FILE, __LINE__, __func__, عدم, عدم, عدم, عدم, میانگیر)

/* Logging with level */
#تعریف_کن log_پر_errno_zerook(level, error, ...)                        \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_داخلی(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_مقدار(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#إذا BUILD_نما_DEVELOPER && !defined(TEST_کد)
#  define ASSERT_NON_ZERO(x) أكد((x) != 0)
#  define ASSERT_UNDERFLOW(x) أكد((x) >= INT_MIN)
#وإلا
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#پایان_اگر

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#تعریف_کن log_پر_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_پر_errno_zerook(level, (صحيح)_error, __VA_ARGS__); \
        })

#تعریف_کن log_پر(level, fmt, ...)                                      \
        ({                                                             \
                إذا (BUILD_نما_DEVELOPER)                              \
                        أكد(!جستجوی_رشته(fmt, "%m"));                    \
                (فراغ) log_پر_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

صحيح log_emergency_level(فراغ);

/* Normal logging */
#تعریف_کن log_debug(...)     log_پر(LOG_DEBUG,   __VA_ARGS__)
#تعریف_کن log_info(...)      log_پر(LOG_INFO,    __VA_ARGS__)
#تعریف_کن log_notice(...)    log_پر(LOG_NOTICE,  __VA_ARGS__)
#تعریف_کن log_warning(...)   log_پر(LOG_WARNING, __VA_ARGS__)
#تعریف_کن log_error(...)     log_پر(LOG_ERR,     __VA_ARGS__)
#تعریف_کن log_emergency(...) log_پر(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#تعریف_کن log_debug_errno(error, ...)     log_پر_errno(LOG_DEBUG,   error, __VA_ARGS__)
#تعریف_کن log_info_errno(error, ...)      log_پر_errno(LOG_INFO,    error, __VA_ARGS__)
#تعریف_کن log_notice_errno(error, ...)    log_پر_errno(LOG_NOTICE,  error, __VA_ARGS__)
#تعریف_کن log_warning_errno(error, ...)   log_پر_errno(LOG_WARNING, error, __VA_ARGS__)
#تعریف_کن log_error_errno(error, ...)     log_پر_errno(LOG_ERR,     error, __VA_ARGS__)
#تعریف_کن log_emergency_errno(error, ...) log_پر_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#تعریف_کن log_once(level, ...)                                             \
        ({                                                               \
                إذا (ONCE)                                                \
                        log_پر(level, __VA_ARGS__);                    \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#تعریف_کن log_once_errno(level, error, ...)                                \
        ({                                                               \
                صحيح _err = (error);                                      \
                إذا (ONCE)                                                \
                        _err = log_پر_errno(level, _err, __VA_ARGS__); \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                وإلا                                                     \
                        _err = -ERRNO_مقدار(_err);                       \
                _err;                                                    \
        })

#إذا LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#وإلا
#  define log_trace(...)          افعل {} طالما (0)
#  define log_trace_errno(e, ...) (-ERRNO_مقدار(e))
#پایان_اگر

/* Structured logging */
#تعریف_کن log_struct_errno(level, error, ...)                             \
        log_struct_داخلی(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, عدم)
#تعریف_کن log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#تعریف_کن log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_داخلی(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#تعریف_کن log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#تعریف_کن log_struct_once(level, ...)                                     \
        ({                                                              \
                إذا (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#تعریف_کن log_dump(level, میانگیر)                                         \
        log_dump_داخلی(level, 0, PROJECT_FILE, __LINE__, __func__, میانگیر)

#تعریف_کن log_oom_پر(level) log_oom_داخلی(level, PROJECT_FILE, __LINE__, __func__)
#تعریف_کن log_oom()           log_oom_پر(LOG_ERR)
#تعریف_کن log_oom_debug()     log_oom_پر(LOG_DEBUG)
#تعریف_کن log_oom_warning()   log_oom_پر(LOG_WARNING)

bool log_on_console(فراغ) خالص;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#اگر_تعریف_شده __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_قلم(fmt, ...)    "dummy", عدم, ##__VA_ARGS__
#وإلاإذا LOG_پیام_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
خارجي bool log_پیام_dummy;
#  define LOG_قلم(fmt, ...) "%.0d" fmt, (log_پیام_dummy && چاپ_کن(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#وإلا
#  define LOG_قلم(fmt, ...) fmt, ##__VA_ARGS__
#پایان_اگر

#تعریف_کن LOG_پیام(fmt, ...) LOG_قلم("MESSAGE=" fmt, ##__VA_ARGS__)
#تعریف_کن LOG_پیام_ID(id)    LOG_قلم("MESSAGE_ID=" id)

فراغ log_received_signal(صحيح level, ثابت بنية signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
فراغ log_مجموعه_upgrade_syslog_مقصد_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
فراغ log_مجموعه_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
فراغ log_مجموعه_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
فراغ log_مجموعه_prohibit_ipc(bool b);

صحيح log_dup_console(فراغ);

صحيح log_syntax_داخلی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بیعلامت config_سطر,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(9, 10);

صحيح log_syntax_invalid_utf8_داخلی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بیعلامت config_سطر,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *rvalue);

صحيح log_syntax_parse_error_داخلی(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                بیعلامت config_سطر,
                صحيح error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue);

#تعریف_کن log_syntax(unit, level, config_file, config_سطر, error, ...)   \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_داخلی(unit, _level, config_file, config_سطر, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_مقدار(_e);                             \
        })

#تعریف_کن log_syntax_invalid_utf8(unit, level, config_file, config_سطر, rvalue) \
        ({                                                              \
                صحيح _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_داخلی(unit, _level, config_file, config_سطر, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#تعریف_کن log_syntax_parse_error_پر(unit, config_file, config_سطر, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_داخلی(unit, config_file, config_سطر, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#تعریف_کن log_syntax_parse_error(unit, config_file, config_سطر, error, lvalue, rvalue) \
        log_syntax_parse_error_پر(unit, config_file, config_سطر, error, /* critical= */ false, lvalue, rvalue)

#تعریف_کن DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

فراغ log_راهاندازی(فراغ);

ثابت حرف* log_مجموعه_پیشوند(ثابت حرف *پیشوند, bool force);

فراغ log_پیشوند_جابجا(ثابت حرف **پیشوند);
ساكن مضمن ثابت حرف* _log_unset_prefixp(ثابت حرف **p) {
        أكد(p);
        log_مجموعه_پیشوند(*p, true);
        أرجع عدم;
}

#تعریف_کن LOG_مجموعه_پیشوند(پیشوند) \
        _cleanup_(_log_unset_prefixp) استفادهنشده ثابت حرف *CONCATENATE(cleanup_log_unset_پیشوند, UNIQ) = log_مجموعه_پیشوند(پیشوند, false);
