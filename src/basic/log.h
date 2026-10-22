/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <syslog.h>

#অন্তর্ভাব "forward.h"

প্রকারনির্দেশ গণনা LogTarget{
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

#define SYNTHETIC_ERRNO(অঙ্ক)                (ABS(অঙ্ক) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_মান(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
প্রকারনির্দেশ শূন্য (*log_syntax_callback_t)(ধ্রুব বর্ণ *unit, পূর্ণাঙ্ক level, শূন্য *userdata);
শূন্য set_log_syntax_callback(log_syntax_callback_t cb, শূন্য *userdata);

স্থির অন্তঃস্থ শূন্য clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ রিক্ত, /* userdata= */ রিক্ত);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
শূন্য log_set_target(LogTarget target);
শূন্য log_set_target_and_open(LogTarget target);
পূর্ণাঙ্ক log_set_target_from_string(ধ্রুব বর্ণ *e);
LogTarget log_get_target(শূন্য) _pure_;
শূন্য log_settle_target(শূন্য);

পূর্ণাঙ্ক log_set_max_level(পূর্ণাঙ্ক level);
পূর্ণাঙ্ক log_set_max_level_from_string(ধ্রুব বর্ণ *e);
পূর্ণাঙ্ক log_get_max_level(শূন্য) _pure_;
পূর্ণাঙ্ক log_get_target_max_level(LogTarget target);
পূর্ণাঙ্ক log_max_levels_to_string(পূর্ণাঙ্ক level, বর্ণ **ret);

শূন্য log_set_facility(পূর্ণাঙ্ক facility);

শূন্য log_show_color(bool b);
bool log_get_show_color(শূন্য) _pure_;
শূন্য log_show_location(bool b);
bool log_get_show_location(শূন্য) _pure_;
শূন্য log_show_time(bool b);
bool log_get_show_time(শূন্য) _pure_;
শূন্য log_show_tid(bool b);
bool log_get_show_tid(শূন্য) _pure_;
শূন্য log_show_executable(bool b);
bool log_get_show_executable(শূন্য) _pure_;

পূর্ণাঙ্ক log_show_color_from_string(ধ্রুব বর্ণ *e);
পূর্ণাঙ্ক log_show_location_from_string(ধ্রুব বর্ণ *e);
পূর্ণাঙ্ক log_show_time_from_string(ধ্রুব বর্ণ *e);
পূর্ণাঙ্ক log_show_tid_from_string(ধ্রুব বর্ণ *e);
পূর্ণাঙ্ক log_show_executable_from_string(ধ্রুব বর্ণ *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(শূন্য);
পূর্ণাঙ্ক log_open(শূন্য);
শূন্য log_close(শূন্য);
শূন্য log_forget_fds(শূন্য);

শূন্য log_parse_environment_variables(শূন্য);
শূন্য log_parse_environment(শূন্য);

পূর্ণাঙ্ক log_dispatch_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *object_field,
                ধ্রুব বর্ণ *object,
                ধ্রুব বর্ণ *extra_field,
                ধ্রুব বর্ণ *extra,
                বর্ণ *buffer);

পূর্ণাঙ্ক log_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *format, ...) _printf_(6,7);

পূর্ণাঙ্ক log_internalv(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *format,
                va_list ap) _printf_(6,0);

পূর্ণাঙ্ক log_object_internalv(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *object_field,
                ধ্রুব বর্ণ *object,
                ধ্রুব বর্ণ *extra_field,
                ধ্রুব বর্ণ *extra,
                ধ্রুব বর্ণ *format,
                va_list ap) _printf_(10,0);

পূর্ণাঙ্ক log_object_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *object_field,
                ধ্রুব বর্ণ *object,
                ধ্রুব বর্ণ *extra_field,
                ধ্রুব বর্ণ *extra,
                ধ্রুব বর্ণ *format, ...) _printf_(10,11);

পূর্ণাঙ্ক log_struct_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *format, ...) _sentinel_;

পূর্ণাঙ্ক log_oom_internal(
                পূর্ণাঙ্ক level,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func);

পূর্ণাঙ্ক log_format_iovec(
                সংরচনা iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *format,
                va_list ap) _printf_(6, 0);

পূর্ণাঙ্ক log_struct_iovec_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব সংরচনা iovec *প্রবেশ_iovec,
                size_t n_প্রবেশ_iovec);

/* This modifies the buffer passed! */
পূর্ণাঙ্ক log_dump_internal(
                পূর্ণাঙ্ক level,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                বর্ণ *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, রিক্ত, রিক্ত, রিক্ত, রিক্ত, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                পূর্ণাঙ্ক _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_মান(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#যদি BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) প্রতিজ্ঞা((x) != 0)
#  define ASSERT_UNDERFLOW(x) প্রতিজ্ঞা((x) >= INT_MIN)
#অন্যথা
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
                log_full_errno_zerook(level, (পূর্ণাঙ্ক)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                যদি (BUILD_MODE_DEVELOPER)                              \
                        প্রতিজ্ঞা(!strstr(fmt, "%m"));                    \
                (শূন্য) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

পূর্ণাঙ্ক log_emergency_level(শূন্য);

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
                যদি (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                অন্যথা যদি (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                পূর্ণাঙ্ক _err = (error);                                      \
                যদি (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                অন্যথা যদি (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                অন্যথা                                                     \
                        _err = -ERRNO_মান(_err);                       \
                _err;                                                    \
        })

#যদি LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#অন্যথা
#  define log_trace(...)          কুরু {} যাবত্ (0)
#  define log_trace_errno(e, ...) (-ERRNO_মান(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, রিক্ত)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                যদি (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                অন্যথা যদি (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(শূন্য) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", রিক্ত, ##__VA_ARGS__
#নোচেদ্যদি LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
বাহ্য bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#অন্যথা
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

শূন্য log_received_signal(পূর্ণাঙ্ক level, ধ্রুব সংরচনা signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
শূন্য log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
শূন্য log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
শূন্য log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
শূন্য log_set_prohibit_ipc(bool b);

পূর্ণাঙ্ক log_dup_console(শূন্য);

পূর্ণাঙ্ক log_syntax_internal(
                ধ্রুব বর্ণ *unit,
                পূর্ণাঙ্ক level,
                ধ্রুব বর্ণ *config_file,
                unsigned config_সারি,
                পূর্ণাঙ্ক error,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *format, ...) _printf_(9, 10);

পূর্ণাঙ্ক log_syntax_invalid_utf8_internal(
                ধ্রুব বর্ণ *unit,
                পূর্ণাঙ্ক level,
                ধ্রুব বর্ণ *config_file,
                unsigned config_সারি,
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *rvalue);

পূর্ণাঙ্ক log_syntax_parse_error_internal(
                ধ্রুব বর্ণ *unit,
                ধ্রুব বর্ণ *config_file,
                unsigned config_সারি,
                পূর্ণাঙ্ক error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ধ্রুব বর্ণ *file,
                পূর্ণাঙ্ক সারি,
                ধ্রুব বর্ণ *func,
                ধ্রুব বর্ণ *lvalue,
                ধ্রুব বর্ণ *rvalue);

#define log_syntax(unit, level, config_file, config_সারি, error, ...)   \
        ({                                                              \
                পূর্ণাঙ্ক _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_সারি, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_মান(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_সারি, rvalue) \
        ({                                                              \
                পূর্ণাঙ্ক _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_সারি, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_সারি, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_সারি, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_সারি, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_সারি, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

শূন্য log_setup(শূন্য);

ধ্রুব বর্ণ* _log_set_prefix(ধ্রুব বর্ণ *prefix, bool force);

শূন্য log_prefix_swap(ধ্রুব বর্ণ **prefix);
স্থির অন্তঃস্থ ধ্রুব বর্ণ* _log_unset_prefixp(ধ্রুব বর্ণ **p) {
        প্রতিজ্ঞা(p);
        _log_set_prefix(*p, true);
        প্রত্যাগম রিক্ত;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ ধ্রুব বর্ণ *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
