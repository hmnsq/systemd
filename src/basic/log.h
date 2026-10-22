/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <stdio.h>
#شامل <string.h>
#شامل <syslog.h>

#شامل "forward.h"

تعریفنوع شمارش LogTarget{
        LOG_TARGET_CONSOLE,
        LOG_TARGET_KMSG,
        LOG_TARGET_JOURNAL,
        LOG_TARGET_SYSLOG,
        LOG_TARGET_CONSOLE_PREFIXED,
        LOG_TARGET_JOURNAL_OR_KMSG,
        LOG_TARGET_SYSLOG_OR_KMSG,
        LOG_TARGET_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_TARGET_NULL,
        _LOG_TARGET_SINGLE_MAX = LOG_TARGET_SYSLOG + 1,
        _LOG_TARGET_MAX = LOG_TARGET_NULL + 1,
        _LOG_TARGET_INVALID = -EINVAL,
} LogTarget;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#define LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#define SYNTHETIC_ERRNO(رقم)                (ABS(رقم) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_مقدار(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
تعریفنوع تهی (*log_syntax_callback_t)(ثابت نویسه *unit, صحیح level, تهی *userdata);
تهی set_log_syntax_callback(log_syntax_callback_t cb, تهی *userdata);

ایستا درونخطی تهی clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ پوچ, /* userdata= */ پوچ);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
تهی log_set_target(LogTarget target);
تهی log_set_target_and_open(LogTarget target);
صحیح log_set_target_from_string(ثابت نویسه *e);
LogTarget log_get_target(تهی) _pure_;
تهی log_settle_target(تهی);

صحیح log_set_max_level(صحیح level);
صحیح log_set_max_level_from_string(ثابت نویسه *e);
صحیح log_get_max_level(تهی) _pure_;
صحیح log_get_target_max_level(LogTarget target);
صحیح log_max_levels_to_string(صحیح level, نویسه **ret);

تهی log_set_facility(صحیح facility);

تهی log_show_color(bool b);
bool log_get_show_color(تهی) _pure_;
تهی log_show_location(bool b);
bool log_get_show_location(تهی) _pure_;
تهی log_show_time(bool b);
bool log_get_show_time(تهی) _pure_;
تهی log_show_tid(bool b);
bool log_get_show_tid(تهی) _pure_;
تهی log_show_executable(bool b);
bool log_get_show_executable(تهی) _pure_;

صحیح log_show_color_from_string(ثابت نویسه *e);
صحیح log_show_location_from_string(ثابت نویسه *e);
صحیح log_show_time_from_string(ثابت نویسه *e);
صحیح log_show_tid_from_string(ثابت نویسه *e);
صحیح log_show_executable_from_string(ثابت نویسه *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(تهی);
صحیح log_open(تهی);
تهی log_close(تهی);
تهی log_forget_fds(تهی);

تهی log_parse_environment_variables(تهی);
تهی log_parse_environment(تهی);

صحیح log_dispatch_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                نویسه *buffer);

صحیح log_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) _printf_(6,7);

صحیح log_internalv(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format,
                va_list ap) _printf_(6,0);

صحیح log_object_internalv(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                ثابت نویسه *format,
                va_list ap) _printf_(10,0);

صحیح log_object_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *object_field,
                ثابت نویسه *object,
                ثابت نویسه *extra_field,
                ثابت نویسه *extra,
                ثابت نویسه *format, ...) _printf_(10,11);

صحیح log_struct_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) _sentinel_;

صحیح log_oom_internal(
                صحیح level,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func);

صحیح log_format_iovec(
                ساختار iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                صحیح error,
                ثابت نویسه *format,
                va_list ap) _printf_(6, 0);

صحیح log_struct_iovec_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت ساختار iovec *درون_داد_iovec,
                size_t n_درون_داد_iovec);

/* This modifies the buffer passed! */
صحیح log_dump_internal(
                صحیح level,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                نویسه *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, پوچ, پوچ, پوچ, پوچ, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                صحیح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_مقدار(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#اگر BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) أكد((x) != 0)
#  define ASSERT_UNDERFLOW(x) أكد((x) >= INT_MIN)
#وگرنه
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#endif

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#define log_full_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_full_errno_zerook(level, (صحیح)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                اگر (BUILD_MODE_DEVELOPER)                              \
                        أكد(!strstr(fmt, "%m"));                    \
                (تهی) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

صحیح log_emergency_level(تهی);

/* Normal logging */
#define log_debug(...)     log_full(LOG_DEBUG,   __VA_ARGS__)
#define log_info(...)      log_full(LOG_INFO,    __VA_ARGS__)
#define log_notice(...)    log_full(LOG_NOTICE,  __VA_ARGS__)
#define log_warning(...)   log_full(LOG_WARNING, __VA_ARGS__)
#define log_error(...)     log_full(LOG_ERR,     __VA_ARGS__)
#define log_emergency(...) log_full(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#define log_debug_errno(error, ...)     log_full_errno(LOG_DEBUG,   error, __VA_ARGS__)
#define log_info_errno(error, ...)      log_full_errno(LOG_INFO,    error, __VA_ARGS__)
#define log_notice_errno(error, ...)    log_full_errno(LOG_NOTICE,  error, __VA_ARGS__)
#define log_warning_errno(error, ...)   log_full_errno(LOG_WARNING, error, __VA_ARGS__)
#define log_error_errno(error, ...)     log_full_errno(LOG_ERR,     error, __VA_ARGS__)
#define log_emergency_errno(error, ...) log_full_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#define log_once(level, ...)                                             \
        ({                                                               \
                اگر (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                وگرنه اگر (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                صحیح _err = (error);                                      \
                اگر (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                وگرنه اگر (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                وگرنه                                                     \
                        _err = -ERRNO_مقدار(_err);                       \
                _err;                                                    \
        })

#اگر LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#وگرنه
#  define log_trace(...)          انجامبده {} وقتی (0)
#  define log_trace_errno(e, ...) (-ERRNO_مقدار(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, پوچ)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                اگر (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                وگرنه اگر (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(تهی) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", پوچ, ##__VA_ARGS__
#وگرنهاگر LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
بیرونی bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#وگرنه
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

تهی log_received_signal(صحیح level, ثابت ساختار signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
تهی log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
تهی log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
تهی log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
تهی log_set_prohibit_ipc(bool b);

صحیح log_dup_console(تهی);

صحیح log_syntax_internal(
                ثابت نویسه *unit,
                صحیح level,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                صحیح error,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *format, ...) _printf_(9, 10);

صحیح log_syntax_invalid_utf8_internal(
                ثابت نویسه *unit,
                صحیح level,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *rvalue);

صحیح log_syntax_parse_error_internal(
                ثابت نویسه *unit,
                ثابت نویسه *config_file,
                unsigned config_سطر,
                صحیح error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ثابت نویسه *file,
                صحیح سطر,
                ثابت نویسه *func,
                ثابت نویسه *lvalue,
                ثابت نویسه *rvalue);

#define log_syntax(unit, level, config_file, config_سطر, error, ...)   \
        ({                                                              \
                صحیح _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_سطر, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_مقدار(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_سطر, rvalue) \
        ({                                                              \
                صحیح _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_سطر, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_سطر, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_سطر, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_سطر, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_سطر, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

تهی log_setup(تهی);

ثابت نویسه* _log_set_prefix(ثابت نویسه *prefix, bool force);

تهی log_prefix_swap(ثابت نویسه **prefix);
ایستا درونخطی ثابت نویسه* _log_unset_prefixp(ثابت نویسه **p) {
        أكد(p);
        _log_set_prefix(*p, true);
        بازگشت پوچ;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ ثابت نویسه *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
