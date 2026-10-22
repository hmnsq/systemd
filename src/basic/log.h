/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <syslog.h>

#அந்தர்பாவ "forward.h"

வகைவரை எண்ணிக்கை LogTarget{
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

#define SYNTHETIC_ERRNO(இலக்கம்)                (ABS(இலக்கம்) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_மதிப்பு(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
வகைவரை வெறுமை (*log_syntax_callback_t)(மாறிலி எழுத்து *unit, முழு level, வெறுமை *userdata);
வெறுமை set_log_syntax_callback(log_syntax_callback_t cb, வெறுமை *userdata);

நிலையான உள்வரி வெறுமை clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ வெற்று, /* userdata= */ வெற்று);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
வெறுமை log_set_target(LogTarget target);
வெறுமை log_set_target_and_open(LogTarget target);
முழு log_set_target_from_string(மாறிலி எழுத்து *e);
LogTarget log_get_target(வெறுமை) _pure_;
வெறுமை log_settle_target(வெறுமை);

முழு log_set_max_level(முழு level);
முழு log_set_max_level_from_string(மாறிலி எழுத்து *e);
முழு log_get_max_level(வெறுமை) _pure_;
முழு log_get_target_max_level(LogTarget target);
முழு log_max_levels_to_string(முழு level, எழுத்து **ret);

வெறுமை log_set_facility(முழு facility);

வெறுமை log_show_color(bool b);
bool log_get_show_color(வெறுமை) _pure_;
வெறுமை log_show_location(bool b);
bool log_get_show_location(வெறுமை) _pure_;
வெறுமை log_show_time(bool b);
bool log_get_show_time(வெறுமை) _pure_;
வெறுமை log_show_tid(bool b);
bool log_get_show_tid(வெறுமை) _pure_;
வெறுமை log_show_executable(bool b);
bool log_get_show_executable(வெறுமை) _pure_;

முழு log_show_color_from_string(மாறிலி எழுத்து *e);
முழு log_show_location_from_string(மாறிலி எழுத்து *e);
முழு log_show_time_from_string(மாறிலி எழுத்து *e);
முழு log_show_tid_from_string(மாறிலி எழுத்து *e);
முழு log_show_executable_from_string(மாறிலி எழுத்து *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(வெறுமை);
முழு log_open(வெறுமை);
வெறுமை log_close(வெறுமை);
வெறுமை log_forget_fds(வெறுமை);

வெறுமை log_parse_environment_variables(வெறுமை);
வெறுமை log_parse_environment(வெறுமை);

முழு log_dispatch_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                எழுத்து *buffer);

முழு log_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) _printf_(6,7);

முழு log_internalv(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format,
                va_list ap) _printf_(6,0);

முழு log_object_internalv(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                மாறிலி எழுத்து *format,
                va_list ap) _printf_(10,0);

முழு log_object_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *object_field,
                மாறிலி எழுத்து *object,
                மாறிலி எழுத்து *extra_field,
                மாறிலி எழுத்து *extra,
                மாறிலி எழுத்து *format, ...) _printf_(10,11);

முழு log_struct_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) _sentinel_;

முழு log_oom_internal(
                முழு level,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func);

முழு log_format_iovec(
                கட்டமைப்பு iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                முழு error,
                மாறிலி எழுத்து *format,
                va_list ap) _printf_(6, 0);

முழு log_struct_iovec_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி கட்டமைப்பு iovec *உள்ளீடு_iovec,
                size_t n_உள்ளீடு_iovec);

/* This modifies the buffer passed! */
முழு log_dump_internal(
                முழு level,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                எழுத்து *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, வெற்று, வெற்று, வெற்று, வெற்று, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                முழு _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_மதிப்பு(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#எனில் BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) ப்ரதிஜ்ஞா((x) != 0)
#  define ASSERT_UNDERFLOW(x) ப்ரதிஜ்ஞா((x) >= INT_MIN)
#இல்லையெனில்
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
                log_full_errno_zerook(level, (முழு)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                எனில் (BUILD_MODE_DEVELOPER)                              \
                        ப்ரதிஜ்ஞா(!strstr(fmt, "%m"));                    \
                (வெறுமை) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

முழு log_emergency_level(வெறுமை);

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
                எனில் (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                இல்லையெனில் எனில் (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                முழு _err = (error);                                      \
                எனில் (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                இல்லையெனில் எனில் (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                இல்லையெனில்                                                     \
                        _err = -ERRNO_மதிப்பு(_err);                       \
                _err;                                                    \
        })

#எனில் LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#இல்லையெனில்
#  define log_trace(...)          செய் {} போது (0)
#  define log_trace_errno(e, ...) (-ERRNO_மதிப்பு(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, வெற்று)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                எனில் (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                இல்லையெனில் எனில் (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(வெறுமை) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", வெற்று, ##__VA_ARGS__
#அல்லதெனில் LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
வெளி bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#இல்லையெனில்
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

வெறுமை log_received_signal(முழு level, மாறிலி கட்டமைப்பு signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
வெறுமை log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
வெறுமை log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
வெறுமை log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
வெறுமை log_set_prohibit_ipc(bool b);

முழு log_dup_console(வெறுமை);

முழு log_syntax_internal(
                மாறிலி எழுத்து *unit,
                முழு level,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                முழு error,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *format, ...) _printf_(9, 10);

முழு log_syntax_invalid_utf8_internal(
                மாறிலி எழுத்து *unit,
                முழு level,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *rvalue);

முழு log_syntax_parse_error_internal(
                மாறிலி எழுத்து *unit,
                மாறிலி எழுத்து *config_file,
                unsigned config_வரி,
                முழு error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                மாறிலி எழுத்து *file,
                முழு வரி,
                மாறிலி எழுத்து *func,
                மாறிலி எழுத்து *lvalue,
                மாறிலி எழுத்து *rvalue);

#define log_syntax(unit, level, config_file, config_வரி, error, ...)   \
        ({                                                              \
                முழு _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_வரி, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_மதிப்பு(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_வரி, rvalue) \
        ({                                                              \
                முழு _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_வரி, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_வரி, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_வரி, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_வரி, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_வரி, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

வெறுமை log_setup(வெறுமை);

மாறிலி எழுத்து* _log_set_prefix(மாறிலி எழுத்து *prefix, bool force);

வெறுமை log_prefix_swap(மாறிலி எழுத்து **prefix);
நிலையான உள்வரி மாறிலி எழுத்து* _log_unset_prefixp(மாறிலி எழுத்து **p) {
        ப்ரதிஜ்ஞா(p);
        _log_set_prefix(*p, true);
        திரும்பு வெற்று;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ மாறிலி எழுத்து *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
