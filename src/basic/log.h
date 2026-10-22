/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <syslog.h>

#ضمن "forward.h"

تعريفنوع تعداد Log_نشانہ{
        LOG_نشانہ_CONSOLE,
        LOG_نشانہ_KMSG,
        LOG_نشانہ_JOURNAL,
        LOG_نشانہ_SYSLOG,
        LOG_نشانہ_CONSOLE_PREFIXED,
        LOG_نشانہ_JOURNAL_OR_KMSG,
        LOG_نشانہ_SYSLOG_OR_KMSG,
        LOG_نشانہ_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_نشانہ_NULL,
        LOG_نشانہ_SINGLE_MAX = LOG_نشانہ_SYSLOG + 1,
        LOG_نشانہ_MAX = LOG_نشانہ_NULL + 1,
        LOG_نشانہ_INVALID = -EINVAL,
} Log_نشانہ;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#تعریف_کرو LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#تعریف_کرو SYNTHETIC_ERRNO(عدد)                (ABS(عدد) | (1 << 30))
#تعریف_کرو IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#تعریف_کرو ERRNO_قدر(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
تعريفنوع فراغ (*log_syntax_callback_t)(ثابت حرف *unit, صحيح level, فراغ *userdata);
فراغ سیٹ_log_syntax_callback(log_syntax_callback_t cb, فراغ *userdata);

ساكن مضمن فراغ clear_log_syntax_callback(dummy_t *dummy) {
          سیٹ_log_syntax_callback(/* cb= */ عدم, /* userdata= */ عدم);
}

DECLARE_STRING_جدول_LOOKUP(log_نشانہ, Log_نشانہ);
فراغ log_سیٹ_نشانہ(Log_نشانہ نشانہ);
فراغ log_سیٹ_نشانہ_and_open(Log_نشانہ نشانہ);
صحيح log_سیٹ_نشانہ_from_string(ثابت حرف *e);
Log_نشانہ log_get_نشانہ(فراغ) خالص;
فراغ log_settle_نشانہ(فراغ);

صحيح log_سیٹ_max_level(صحيح level);
صحيح log_سیٹ_max_level_from_string(ثابت حرف *e);
صحيح log_get_max_level(فراغ) خالص;
صحيح log_get_نشانہ_max_level(Log_نشانہ نشانہ);
صحيح log_max_پدھراں_منزل_string(صحيح level, حرف **ret);

فراغ log_سیٹ_facility(صحيح facility);

فراغ log_show_color(bool b);
bool log_get_show_color(فراغ) خالص;
فراغ log_show_location(bool b);
bool log_get_show_location(فراغ) خالص;
فراغ log_show_ویلا(bool b);
bool log_get_show_ویلا(فراغ) خالص;
فراغ log_show_tid(bool b);
bool log_get_show_tid(فراغ) خالص;
فراغ log_show_executable(bool b);
bool log_get_show_executable(فراغ) خالص;

صحيح log_show_color_from_string(ثابت حرف *e);
صحيح log_show_location_from_string(ثابت حرف *e);
صحيح log_show_ویلا_from_string(ثابت حرف *e);
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

صحيح log_dispatch_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                حرف *بفر);

صحيح log_اندرونی(
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
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) _printf_(10,0);

صحيح log_object_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *object_میدان,
                ثابت حرف *object,
                ثابت حرف *extra_میدان,
                ثابت حرف *extra,
                ثابت حرف *format, ...) _printf_(10,11);

صحيح log_struct_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _sentinel_;

صحيح log_oom_اندرونی(
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

صحيح log_struct_iovec_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت بنية iovec *داخلہ_iovec,
                size_t n_داخلہ_iovec);

/* This modifies the buffer passed! */
صحيح log_dump_اندرونی(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                حرف *بفر);

#تعریف_کرو log_dispatch(level, error, بفر)                              \
        log_dispatch_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, عدم, عدم, عدم, عدم, بفر)

/* Logging with level */
#تعریف_کرو log_بھریا_errno_zerook(level, error, ...)                        \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_اندرونی(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#إذا BUILD_منوال_DEVELOPER && !defined(TEST_کوڈ)
#  define ASSERT_NON_ZERO(x) أكد((x) != 0)
#  define ASSERT_UNDERFLOW(x) أكد((x) >= INT_MIN)
#وإلا
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#جے_انت

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#تعریف_کرو log_بھریا_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_بھریا_errno_zerook(level, (صحيح)_error, __VA_ARGS__); \
        })

#تعریف_کرو log_بھریا(level, fmt, ...)                                      \
        ({                                                             \
                إذا (BUILD_منوال_DEVELOPER)                              \
                        أكد(!لڑی_کھوج(fmt, "%m"));                    \
                (فراغ) log_بھریا_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

صحيح log_emergency_level(فراغ);

/* Normal logging */
#تعریف_کرو log_debug(...)     log_بھریا(LOG_DEBUG,   __VA_ARGS__)
#تعریف_کرو log_info(...)      log_بھریا(LOG_INFO,    __VA_ARGS__)
#تعریف_کرو log_notice(...)    log_بھریا(LOG_NOTICE,  __VA_ARGS__)
#تعریف_کرو log_warning(...)   log_بھریا(LOG_WARNING, __VA_ARGS__)
#تعریف_کرو log_error(...)     log_بھریا(LOG_ERR,     __VA_ARGS__)
#تعریف_کرو log_emergency(...) log_بھریا(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#تعریف_کرو log_debug_errno(error, ...)     log_بھریا_errno(LOG_DEBUG,   error, __VA_ARGS__)
#تعریف_کرو log_info_errno(error, ...)      log_بھریا_errno(LOG_INFO,    error, __VA_ARGS__)
#تعریف_کرو log_notice_errno(error, ...)    log_بھریا_errno(LOG_NOTICE,  error, __VA_ARGS__)
#تعریف_کرو log_warning_errno(error, ...)   log_بھریا_errno(LOG_WARNING, error, __VA_ARGS__)
#تعریف_کرو log_error_errno(error, ...)     log_بھریا_errno(LOG_ERR,     error, __VA_ARGS__)
#تعریف_کرو log_emergency_errno(error, ...) log_بھریا_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#تعریف_کرو log_once(level, ...)                                             \
        ({                                                               \
                إذا (ONCE)                                                \
                        log_بھریا(level, __VA_ARGS__);                    \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#تعریف_کرو log_once_errno(level, error, ...)                                \
        ({                                                               \
                صحيح _err = (error);                                      \
                إذا (ONCE)                                                \
                        _err = log_بھریا_errno(level, _err, __VA_ARGS__); \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                وإلا                                                     \
                        _err = -ERRNO_قدر(_err);                       \
                _err;                                                    \
        })

#إذا LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#وإلا
#  define log_trace(...)          افعل {} طالما (0)
#  define log_trace_errno(e, ...) (-ERRNO_قدر(e))
#جے_انت

/* Structured logging */
#تعریف_کرو log_struct_errno(level, error, ...)                             \
        log_struct_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, عدم)
#تعریف_کرو log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#تعریف_کرو log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_اندرونی(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#تعریف_کرو log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#تعریف_کرو log_struct_once(level, ...)                                     \
        ({                                                              \
                إذا (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#تعریف_کرو log_dump(level, بفر)                                         \
        log_dump_اندرونی(level, 0, PROJECT_FILE, __LINE__, __func__, بفر)

#تعریف_کرو log_oom_بھریا(level) log_oom_اندرونی(level, PROJECT_FILE, __LINE__, __func__)
#تعریف_کرو log_oom()           log_oom_بھریا(LOG_ERR)
#تعریف_کرو log_oom_debug()     log_oom_بھریا(LOG_DEBUG)
#تعریف_کرو log_oom_warning()   log_oom_بھریا(LOG_WARNING)

bool log_on_console(فراغ) خالص;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#جے_تعریف_شدہ __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_شے(fmt, ...)    "dummy", عدم, ##__VA_ARGS__
#وإلاإذا LOG_سنیہا_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
خارجي bool log_سنیہا_dummy;
#  define LOG_شے(fmt, ...) "%.0d" fmt, (log_سنیہا_dummy && لکھو(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#وإلا
#  define LOG_شے(fmt, ...) fmt, ##__VA_ARGS__
#جے_انت

#تعریف_کرو LOG_سنیہا(fmt, ...) LOG_شے("MESSAGE=" fmt, ##__VA_ARGS__)
#تعریف_کرو LOG_سنیہا_ID(id)    LOG_شے("MESSAGE_ID=" id)

فراغ log_received_signal(صحيح level, ثابت بنية signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
فراغ log_سیٹ_upgrade_syslog_منزل_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
فراغ log_سیٹ_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
فراغ log_سیٹ_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
فراغ log_سیٹ_prohibit_ipc(bool b);

صحيح log_dup_console(فراغ);

صحيح log_syntax_اندرونی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بنانشانی config_سطر,
                صحيح error,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(9, 10);

صحيح log_syntax_invalid_utf8_اندرونی(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بنانشانی config_سطر,
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *rvalue);

صحيح log_syntax_parse_error_اندرونی(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                بنانشانی config_سطر,
                صحيح error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ثابت حرف *file,
                صحيح سطر,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue);

#تعریف_کرو log_syntax(unit, level, config_file, config_سطر, error, ...)   \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_اندرونی(unit, _level, config_file, config_سطر, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
        })

#تعریف_کرو log_syntax_invalid_utf8(unit, level, config_file, config_سطر, rvalue) \
        ({                                                              \
                صحيح _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_اندرونی(unit, _level, config_file, config_سطر, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#تعریف_کرو log_syntax_parse_error_بھریا(unit, config_file, config_سطر, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_اندرونی(unit, config_file, config_سطر, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#تعریف_کرو log_syntax_parse_error(unit, config_file, config_سطر, error, lvalue, rvalue) \
        log_syntax_parse_error_بھریا(unit, config_file, config_سطر, error, /* critical= */ false, lvalue, rvalue)

#تعریف_کرو DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

فراغ log_تیاری(فراغ);

ثابت حرف* log_سیٹ_اگیتر(ثابت حرف *اگیتر, bool force);

فراغ log_اگیتر_وٹاندرا(ثابت حرف **اگیتر);
ساكن مضمن ثابت حرف* _log_unset_prefixp(ثابت حرف **p) {
        أكد(p);
        log_سیٹ_اگیتر(*p, true);
        أرجع عدم;
}

#تعریف_کرو LOG_سیٹ_اگیتر(اگیتر) \
        _cleanup_(_log_unset_prefixp) بن_ورتیا ثابت حرف *CONCATENATE(cleanup_log_unset_اگیتر, UNIQ) = log_سیٹ_اگیتر(اگیتر, false);
