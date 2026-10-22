/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ LogTarget{
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

#define SYNTHETIC_ERRNO(ಅಂಕೆ)                (ABS(ಅಂಕೆ) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_ಮೌಲ್ಯ(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
ಪ್ರಕಾರನಿರ್ದೇಶ ಶೂನ್ಯ (*log_syntax_callback_t)(ಧ್ರುವ ವರ್ಣ *unit, ಪೂರ್ಣಾಙ್ಕ level, ಶೂನ್ಯ *userdata);
ಶೂನ್ಯ set_log_syntax_callback(log_syntax_callback_t cb, ಶೂನ್ಯ *userdata);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ ರಿಕ್ತ, /* userdata= */ ರಿಕ್ತ);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
ಶೂನ್ಯ log_set_target(LogTarget target);
ಶೂನ್ಯ log_set_target_and_open(LogTarget target);
ಪೂರ್ಣಾಙ್ಕ log_set_target_from_string(ಧ್ರುವ ವರ್ಣ *e);
LogTarget log_get_target(ಶೂನ್ಯ) _pure_;
ಶೂನ್ಯ log_settle_target(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ log_set_max_level(ಪೂರ್ಣಾಙ್ಕ level);
ಪೂರ್ಣಾಙ್ಕ log_set_max_level_from_string(ಧ್ರುವ ವರ್ಣ *e);
ಪೂರ್ಣಾಙ್ಕ log_get_max_level(ಶೂನ್ಯ) _pure_;
ಪೂರ್ಣಾಙ್ಕ log_get_target_max_level(LogTarget target);
ಪೂರ್ಣಾಙ್ಕ log_max_levels_to_string(ಪೂರ್ಣಾಙ್ಕ level, ವರ್ಣ **ret);

ಶೂನ್ಯ log_set_facility(ಪೂರ್ಣಾಙ್ಕ facility);

ಶೂನ್ಯ log_show_color(bool b);
bool log_get_show_color(ಶೂನ್ಯ) _pure_;
ಶೂನ್ಯ log_show_location(bool b);
bool log_get_show_location(ಶೂನ್ಯ) _pure_;
ಶೂನ್ಯ log_show_time(bool b);
bool log_get_show_time(ಶೂನ್ಯ) _pure_;
ಶೂನ್ಯ log_show_tid(bool b);
bool log_get_show_tid(ಶೂನ್ಯ) _pure_;
ಶೂನ್ಯ log_show_executable(bool b);
bool log_get_show_executable(ಶೂನ್ಯ) _pure_;

ಪೂರ್ಣಾಙ್ಕ log_show_color_from_string(ಧ್ರುವ ವರ್ಣ *e);
ಪೂರ್ಣಾಙ್ಕ log_show_location_from_string(ಧ್ರುವ ವರ್ಣ *e);
ಪೂರ್ಣಾಙ್ಕ log_show_time_from_string(ಧ್ರುವ ವರ್ಣ *e);
ಪೂರ್ಣಾಙ್ಕ log_show_tid_from_string(ಧ್ರುವ ವರ್ಣ *e);
ಪೂರ್ಣಾಙ್ಕ log_show_executable_from_string(ಧ್ರುವ ವರ್ಣ *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(ಶೂನ್ಯ);
ಪೂರ್ಣಾಙ್ಕ log_open(ಶೂನ್ಯ);
ಶೂನ್ಯ log_close(ಶೂನ್ಯ);
ಶೂನ್ಯ log_forget_fds(ಶೂನ್ಯ);

ಶೂನ್ಯ log_parse_environment_variables(ಶೂನ್ಯ);
ಶೂನ್ಯ log_parse_environment(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ log_dispatch_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ವರ್ಣ *buffer);

ಪೂರ್ಣಾಙ್ಕ log_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(6,7);

ಪೂರ್ಣಾಙ್ಕ log_internalv(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) _printf_(6,0);

ಪೂರ್ಣಾಙ್ಕ log_object_internalv(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) _printf_(10,0);

ಪೂರ್ಣಾಙ್ಕ log_object_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *object_field,
                ಧ್ರುವ ವರ್ಣ *object,
                ಧ್ರುವ ವರ್ಣ *extra_field,
                ಧ್ರುವ ವರ್ಣ *extra,
                ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(10,11);

ಪೂರ್ಣಾಙ್ಕ log_struct_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) _sentinel_;

ಪೂರ್ಣಾಙ್ಕ log_oom_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func);

ಪೂರ್ಣಾಙ್ಕ log_format_iovec(
                ಸಂರಚನಾ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *format,
                va_list ap) _printf_(6, 0);

ಪೂರ್ಣಾಙ್ಕ log_struct_iovec_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ಸಂರಚನಾ iovec *ಒಳಹರಿವು_iovec,
                size_t n_ಒಳಹರಿವು_iovec);

/* This modifies the buffer passed! */
ಪೂರ್ಣಾಙ್ಕ log_dump_internal(
                ಪೂರ್ಣಾಙ್ಕ level,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ವರ್ಣ *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ಮೌಲ್ಯ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ಯದಿ BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) ಪ್ರತಿಜ್ಞಾ((x) != 0)
#  define ASSERT_UNDERFLOW(x) ಪ್ರತಿಜ್ಞಾ((x) >= INT_MIN)
#ಅನ್ಯಥಾ
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
                log_full_errno_zerook(level, (ಪೂರ್ಣಾಙ್ಕ)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                ಯದಿ (BUILD_MODE_DEVELOPER)                              \
                        ಪ್ರತಿಜ್ಞಾ(!strstr(fmt, "%m"));                    \
                (ಶೂನ್ಯ) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

ಪೂರ್ಣಾಙ್ಕ log_emergency_level(ಶೂನ್ಯ);

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
                ಯದಿ (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                ಅನ್ಯಥಾ ಯದಿ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                ಪೂರ್ಣಾಙ್ಕ _err = (error);                                      \
                ಯದಿ (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                ಅನ್ಯಥಾ ಯದಿ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ಅನ್ಯಥಾ                                                     \
                        _err = -ERRNO_ಮೌಲ್ಯ(_err);                       \
                _err;                                                    \
        })

#ಯದಿ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ಅನ್ಯಥಾ
#  define log_trace(...)          ಕುರು {} ಯಾವತ್ (0)
#  define log_trace_errno(e, ...) (-ERRNO_ಮೌಲ್ಯ(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ರಿಕ್ತ)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                ಯದಿ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ಅನ್ಯಥಾ ಯದಿ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(ಶೂನ್ಯ) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", ರಿಕ್ತ, ##__VA_ARGS__
#ನೋಚೇದ್ಯದಿ LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
ಬಾಹ್ಯ bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ಅನ್ಯಥಾ
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

ಶೂನ್ಯ log_received_signal(ಪೂರ್ಣಾಙ್ಕ level, ಧ್ರುವ ಸಂರಚನಾ signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
ಶೂನ್ಯ log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
ಶೂನ್ಯ log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
ಶೂನ್ಯ log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
ಶೂನ್ಯ log_set_prohibit_ipc(bool b);

ಪೂರ್ಣಾಙ್ಕ log_dup_console(ಶೂನ್ಯ);

ಪೂರ್ಣಾಙ್ಕ log_syntax_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಪೂರ್ಣಾಙ್ಕ level,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಪೂರ್ಣಾಙ್ಕ error,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *format, ...) _printf_(9, 10);

ಪೂರ್ಣಾಙ್ಕ log_syntax_invalid_utf8_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಪೂರ್ಣಾಙ್ಕ level,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *rvalue);

ಪೂರ್ಣಾಙ್ಕ log_syntax_parse_error_internal(
                ಧ್ರುವ ವರ್ಣ *unit,
                ಧ್ರುವ ವರ್ಣ *config_file,
                unsigned config_ಸಾಲು,
                ಪೂರ್ಣಾಙ್ಕ error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ಧ್ರುವ ವರ್ಣ *file,
                ಪೂರ್ಣಾಙ್ಕ ಸಾಲು,
                ಧ್ರುವ ವರ್ಣ *func,
                ಧ್ರುವ ವರ್ಣ *lvalue,
                ಧ್ರುವ ವರ್ಣ *rvalue);

#define log_syntax(unit, level, config_file, config_ಸಾಲು, error, ...)   \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_ಸಾಲು, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ಮೌಲ್ಯ(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_ಸಾಲು, rvalue) \
        ({                                                              \
                ಪೂರ್ಣಾಙ್ಕ _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_ಸಾಲು, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_ಸಾಲು, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_ಸಾಲು, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_ಸಾಲು, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_ಸಾಲು, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

ಶೂನ್ಯ log_setup(ಶೂನ್ಯ);

ಧ್ರುವ ವರ್ಣ* _log_set_prefix(ಧ್ರುವ ವರ್ಣ *prefix, bool force);

ಶೂನ್ಯ log_prefix_swap(ಧ್ರುವ ವರ್ಣ **prefix);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಧ್ರುವ ವರ್ಣ* _log_unset_prefixp(ಧ್ರುವ ವರ್ಣ **p) {
        ಪ್ರತಿಜ್ಞಾ(p);
        _log_set_prefix(*p, true);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ ಧ್ರುವ ವರ್ಣ *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
