/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

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
#وصف_ڪريو LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#وصف_ڪريو SYNTHETIC_ERRNO(انگ)                (ABS(انگ) | (1 << 30))
#وصف_ڪريو IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#وصف_ڪريو ERRNO_قدر(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
تعريفنوع فراغ (*log_syntax_callback_t)(ثابت حرف *unit, صحيح level, فراغ *userdata);
فراغ سيٽ_log_syntax_callback(log_syntax_callback_t cb, فراغ *userdata);

ساكن مضمن فراغ clear_log_syntax_callback(dummy_t *dummy) {
          سيٽ_log_syntax_callback(/* cb= */ عدم, /* userdata= */ عدم);
}

DECLARE_STRING_جدول_LOOKUP(log_هدف, Log_هدف);
فراغ log_سيٽ_هدف(Log_هدف هدف);
فراغ log_سيٽ_هدف_and_open(Log_هدف هدف);
صحيح log_سيٽ_هدف_from_string(ثابت حرف *e);
Log_هدف log_get_هدف(فراغ) خالص;
فراغ log_settle_هدف(فراغ);

صحيح log_سيٽ_max_level(صحيح level);
صحيح log_سيٽ_max_level_from_string(ثابت حرف *e);
صحيح log_get_max_level(فراغ) خالص;
صحيح log_get_هدف_max_level(Log_هدف هدف);
صحيح log_max_سطحون_منزل_string(صحيح level, حرف **ret);

فراغ log_سيٽ_facility(صحيح facility);

فراغ log_show_color(bool b);
bool log_get_show_color(فراغ) خالص;
فراغ log_show_location(bool b);
bool log_get_show_location(فراغ) خالص;
فراغ log_show_وقت(bool b);
bool log_get_show_وقت(فراغ) خالص;
فراغ log_show_tid(bool b);
bool log_get_show_tid(فراغ) خالص;
فراغ log_show_executable(bool b);
bool log_get_show_executable(فراغ) خالص;

صحيح log_show_color_from_string(ثابت حرف *e);
صحيح log_show_location_from_string(ثابت حرف *e);
صحيح log_show_وقت_from_string(ثابت حرف *e);
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

صحيح log_dispatch_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *object_ميدان,
                ثابت حرف *object,
                ثابت حرف *extra_ميدان,
                ثابت حرف *extra,
                حرف *بفر);

صحيح log_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(6,7);

صحيح log_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *format,
                va_list ap) _printf_(6,0);

صحيح log_object_internalv(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *object_ميدان,
                ثابت حرف *object,
                ثابت حرف *extra_ميدان,
                ثابت حرف *extra,
                ثابت حرف *format,
                va_list ap) _printf_(10,0);

صحيح log_object_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *object_ميدان,
                ثابت حرف *object,
                ثابت حرف *extra_ميدان,
                ثابت حرف *extra,
                ثابت حرف *format, ...) _printf_(10,11);

صحيح log_struct_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *format, ...) _sentinel_;

صحيح log_oom_اندروني(
                صحيح level,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func);

صحيح log_format_iovec(
                بنية iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحيح error,
                ثابت حرف *format,
                va_list ap) _printf_(6, 0);

صحيح log_struct_iovec_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت بنية iovec *داخلا_iovec,
                size_t n_داخلا_iovec);

/* This modifies the buffer passed! */
صحيح log_dump_اندروني(
                صحيح level,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                حرف *بفر);

#وصف_ڪريو log_dispatch(level, error, بفر)                              \
        log_dispatch_اندروني(level, error, PROJECT_FILE, __LINE__, __func__, عدم, عدم, عدم, عدم, بفر)

/* Logging with level */
#وصف_ڪريو log_ڀريل_errno_zerook(level, error, ...)                        \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_اندروني(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#إذا BUILD_گهڻائي_DEVELOPER && !defined(TEST_ڪوڊ)
#  define ASSERT_NON_ZERO(x) أكد((x) != 0)
#  define ASSERT_UNDERFLOW(x) أكد((x) >= INT_MIN)
#وإلا
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#جيڪڏهن_پورو

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#وصف_ڪريو log_ڀريل_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_ڀريل_errno_zerook(level, (صحيح)_error, __VA_ARGS__); \
        })

#وصف_ڪريو log_ڀريل(level, fmt, ...)                                      \
        ({                                                             \
                إذا (BUILD_گهڻائي_DEVELOPER)                              \
                        أكد(!لڙي_ڳولا(fmt, "%m"));                    \
                (فراغ) log_ڀريل_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

صحيح log_emergency_level(فراغ);

/* Normal logging */
#وصف_ڪريو log_debug(...)     log_ڀريل(LOG_DEBUG,   __VA_ARGS__)
#وصف_ڪريو log_info(...)      log_ڀريل(LOG_INFO,    __VA_ARGS__)
#وصف_ڪريو log_notice(...)    log_ڀريل(LOG_NOTICE,  __VA_ARGS__)
#وصف_ڪريو log_warning(...)   log_ڀريل(LOG_WARNING, __VA_ARGS__)
#وصف_ڪريو log_error(...)     log_ڀريل(LOG_ERR,     __VA_ARGS__)
#وصف_ڪريو log_emergency(...) log_ڀريل(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#وصف_ڪريو log_debug_errno(error, ...)     log_ڀريل_errno(LOG_DEBUG,   error, __VA_ARGS__)
#وصف_ڪريو log_info_errno(error, ...)      log_ڀريل_errno(LOG_INFO,    error, __VA_ARGS__)
#وصف_ڪريو log_notice_errno(error, ...)    log_ڀريل_errno(LOG_NOTICE,  error, __VA_ARGS__)
#وصف_ڪريو log_warning_errno(error, ...)   log_ڀريل_errno(LOG_WARNING, error, __VA_ARGS__)
#وصف_ڪريو log_error_errno(error, ...)     log_ڀريل_errno(LOG_ERR,     error, __VA_ARGS__)
#وصف_ڪريو log_emergency_errno(error, ...) log_ڀريل_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#وصف_ڪريو log_once(level, ...)                                             \
        ({                                                               \
                إذا (ONCE)                                                \
                        log_ڀريل(level, __VA_ARGS__);                    \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#وصف_ڪريو log_once_errno(level, error, ...)                                \
        ({                                                               \
                صحيح _err = (error);                                      \
                إذا (ONCE)                                                \
                        _err = log_ڀريل_errno(level, _err, __VA_ARGS__); \
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
#جيڪڏهن_پورو

/* Structured logging */
#وصف_ڪريو log_struct_errno(level, error, ...)                             \
        log_struct_اندروني(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, عدم)
#وصف_ڪريو log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#وصف_ڪريو log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_اندروني(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#وصف_ڪريو log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#وصف_ڪريو log_struct_once(level, ...)                                     \
        ({                                                              \
                إذا (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                وإلا إذا (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#وصف_ڪريو log_dump(level, بفر)                                         \
        log_dump_اندروني(level, 0, PROJECT_FILE, __LINE__, __func__, بفر)

#وصف_ڪريو log_oom_ڀريل(level) log_oom_اندروني(level, PROJECT_FILE, __LINE__, __func__)
#وصف_ڪريو log_oom()           log_oom_ڀريل(LOG_ERR)
#وصف_ڪريو log_oom_debug()     log_oom_ڀريل(LOG_DEBUG)
#وصف_ڪريو log_oom_warning()   log_oom_ڀريل(LOG_WARNING)

bool log_on_console(فراغ) خالص;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#جيڪڏهن_وصف_ٿيل __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_شي(fmt, ...)    "dummy", عدم, ##__VA_ARGS__
#وإلاإذا LOG_پيغام_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
خارجي bool log_پيغام_dummy;
#  define LOG_شي(fmt, ...) "%.0d" fmt, (log_پيغام_dummy && لکو(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#وإلا
#  define LOG_شي(fmt, ...) fmt, ##__VA_ARGS__
#جيڪڏهن_پورو

#وصف_ڪريو LOG_پيغام(fmt, ...) LOG_شي("MESSAGE=" fmt, ##__VA_ARGS__)
#وصف_ڪريو LOG_پيغام_ID(id)    LOG_شي("MESSAGE_ID=" id)

فراغ log_received_signal(صحيح level, ثابت بنية signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
فراغ log_سيٽ_upgrade_syslog_منزل_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
فراغ log_سيٽ_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
فراغ log_سيٽ_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
فراغ log_سيٽ_prohibit_ipc(bool b);

صحيح log_dup_console(فراغ);

صحيح log_syntax_اندروني(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بنانشان config_سٽ,
                صحيح error,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *format, ...) _printf_(9, 10);

صحيح log_syntax_invalid_utf8_اندروني(
                ثابت حرف *unit,
                صحيح level,
                ثابت حرف *config_file,
                بنانشان config_سٽ,
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *rvalue);

صحيح log_syntax_parse_error_اندروني(
                ثابت حرف *unit,
                ثابت حرف *config_file,
                بنانشان config_سٽ,
                صحيح error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ثابت حرف *file,
                صحيح سٽ,
                ثابت حرف *func,
                ثابت حرف *lvalue,
                ثابت حرف *rvalue);

#وصف_ڪريو log_syntax(unit, level, config_file, config_سٽ, error, ...)   \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_اندروني(unit, _level, config_file, config_سٽ, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
        })

#وصف_ڪريو log_syntax_invalid_utf8(unit, level, config_file, config_سٽ, rvalue) \
        ({                                                              \
                صحيح _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_اندروني(unit, _level, config_file, config_سٽ, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#وصف_ڪريو log_syntax_parse_error_ڀريل(unit, config_file, config_سٽ, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_اندروني(unit, config_file, config_سٽ, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#وصف_ڪريو log_syntax_parse_error(unit, config_file, config_سٽ, error, lvalue, rvalue) \
        log_syntax_parse_error_ڀريل(unit, config_file, config_سٽ, error, /* critical= */ false, lvalue, rvalue)

#وصف_ڪريو DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

فراغ log_تياري(فراغ);

ثابت حرف* log_سيٽ_اڳياڙي(ثابت حرف *اڳياڙي, bool force);

فراغ log_اڳياڙي_مٽا_سٽا(ثابت حرف **اڳياڙي);
ساكن مضمن ثابت حرف* _log_unset_prefixp(ثابت حرف **p) {
        أكد(p);
        log_سيٽ_اڳياڙي(*p, true);
        أرجع عدم;
}

#وصف_ڪريو LOG_سيٽ_اڳياڙي(اڳياڙي) \
        _cleanup_(_log_unset_prefixp) اڻ_استعمال_ٿيل ثابت حرف *CONCATENATE(cleanup_log_unset_اڳياڙي, UNIQ) = log_سيٽ_اڳياڙي(اڳياڙي, false);
