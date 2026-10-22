/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <syslog.h>

#अन्तर्भाव "forward.h"

प्रकारनिर्देश गणना LogTarget{
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

#define SYNTHETIC_ERRNO(अङ्कः)                (ABS(अङ्कः) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_मानम्(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
प्रकारनिर्देश शून्य (*log_syntax_callback_t)(ध्रुव वर्ण *unit, पूर्णाङ्क level, शून्य *userdata);
शून्य set_log_syntax_callback(log_syntax_callback_t cb, शून्य *userdata);

स्थिर अन्तःस्थ शून्य clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ रिक्त, /* userdata= */ रिक्त);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
शून्य log_set_target(LogTarget target);
शून्य log_set_target_and_open(LogTarget target);
पूर्णाङ्क log_set_target_from_string(ध्रुव वर्ण *e);
LogTarget log_get_target(शून्य) _pure_;
शून्य log_settle_target(शून्य);

पूर्णाङ्क log_set_max_level(पूर्णाङ्क level);
पूर्णाङ्क log_set_max_level_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_get_max_level(शून्य) _pure_;
पूर्णाङ्क log_get_target_max_level(LogTarget target);
पूर्णाङ्क log_max_levels_to_string(पूर्णाङ्क level, वर्ण **ret);

शून्य log_set_facility(पूर्णाङ्क facility);

शून्य log_show_color(bool b);
bool log_get_show_color(शून्य) _pure_;
शून्य log_show_location(bool b);
bool log_get_show_location(शून्य) _pure_;
शून्य log_show_time(bool b);
bool log_get_show_time(शून्य) _pure_;
शून्य log_show_tid(bool b);
bool log_get_show_tid(शून्य) _pure_;
शून्य log_show_executable(bool b);
bool log_get_show_executable(शून्य) _pure_;

पूर्णाङ्क log_show_color_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_show_location_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_show_time_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_show_tid_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_show_executable_from_string(ध्रुव वर्ण *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(शून्य);
पूर्णाङ्क log_open(शून्य);
शून्य log_close(शून्य);
शून्य log_forget_fds(शून्य);

शून्य log_parse_environment_variables(शून्य);
शून्य log_parse_environment(शून्य);

पूर्णाङ्क log_dispatch_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                वर्ण *buffer);

पूर्णाङ्क log_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) _printf_(6,7);

पूर्णाङ्क log_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format,
                va_list ap) _printf_(6,0);

पूर्णाङ्क log_object_internalv(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format,
                va_list ap) _printf_(10,0);

पूर्णाङ्क log_object_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_field,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_field,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format, ...) _printf_(10,11);

पूर्णाङ्क log_struct_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) _sentinel_;

पूर्णाङ्क log_oom_internal(
                पूर्णाङ्क level,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func);

पूर्णाङ्क log_format_iovec(
                संरचना iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                पूर्णाङ्क error,
                ध्रुव वर्ण *format,
                va_list ap) _printf_(6, 0);

पूर्णाङ्क log_struct_iovec_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव संरचना iovec *निवेशः_iovec,
                size_t n_निवेशः_iovec);

/* This modifies the buffer passed! */
पूर्णाङ्क log_dump_internal(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                वर्ण *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, रिक्त, रिक्त, रिक्त, रिक्त, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                पूर्णाङ्क _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मानम्(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#सचे BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) प्रतिज्ञा((x) != 0)
#  define ASSERT_UNDERFLOW(x) प्रतिज्ञा((x) >= INT_MIN)
#अञ्ञथा
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
                log_full_errno_zerook(level, (पूर्णाङ्क)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                सचे (BUILD_MODE_DEVELOPER)                              \
                        प्रतिज्ञा(!strstr(fmt, "%m"));                    \
                (शून्य) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

पूर्णाङ्क log_emergency_level(शून्य);

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
                सचे (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                अञ्ञथा सचे (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                पूर्णाङ्क _err = (error);                                      \
                सचे (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                अञ्ञथा सचे (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                अञ्ञथा                                                     \
                        _err = -ERRNO_मानम्(_err);                       \
                _err;                                                    \
        })

#सचे LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#अञ्ञथा
#  define log_trace(...)          करोहि {} याव (0)
#  define log_trace_errno(e, ...) (-ERRNO_मानम्(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, रिक्त)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                सचे (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                अञ्ञथा सचे (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(शून्य) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", रिक्त, ##__VA_ARGS__
#नोचेद्यदि LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
बाह्य bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#अञ्ञथा
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

शून्य log_received_signal(पूर्णाङ्क level, ध्रुव संरचना signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
शून्य log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
शून्य log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
शून्य log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
शून्य log_set_prohibit_ipc(bool b);

पूर्णाङ्क log_dup_console(शून्य);

पूर्णाङ्क log_syntax_internal(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) _printf_(9, 10);

पूर्णाङ्क log_syntax_invalid_utf8_internal(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *rvalue);

पूर्णाङ्क log_syntax_parse_error_internal(
                ध्रुव वर्ण *unit,
                ध्रुव वर्ण *config_file,
                unsigned config_पङ्क्तिः,
                पूर्णाङ्क error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *lvalue,
                ध्रुव वर्ण *rvalue);

#define log_syntax(unit, level, config_file, config_पङ्क्तिः, error, ...)   \
        ({                                                              \
                पूर्णाङ्क _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_पङ्क्तिः, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मानम्(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_पङ्क्तिः, rvalue) \
        ({                                                              \
                पूर्णाङ्क _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_पङ्क्तिः, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_पङ्क्तिः, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_पङ्क्तिः, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_पङ्क्तिः, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_पङ्क्तिः, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

शून्य log_setup(शून्य);

ध्रुव वर्ण* _log_set_prefix(ध्रुव वर्ण *prefix, bool force);

शून्य log_prefix_swap(ध्रुव वर्ण **prefix);
स्थिर अन्तःस्थ ध्रुव वर्ण* _log_unset_prefixp(ध्रुव वर्ण **p) {
        प्रतिज्ञा(p);
        _log_set_prefix(*p, true);
        पटिनिवत्त रिक्त;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ ध्रुव वर्ण *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
