/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

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
#परिभाषय LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#परिभाषय SYNTHETIC_ERRNO(अङ्कः)                (ABS(अङ्कः) | (1 << 30))
#परिभाषय IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#परिभाषय ERRNO_मानम्(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
प्रकारनिर्देश शून्य (*log_syntax_callback_t)(ध्रुव वर्ण *unit, पूर्णाङ्क level, शून्य *userdata);
शून्य समुच्चयः_log_syntax_callback(log_syntax_callback_t cb, शून्य *userdata);

स्थिर अन्तःस्थ शून्य clear_log_syntax_callback(dummy_t *dummy) {
          समुच्चयः_log_syntax_callback(/* cb= */ रिक्त, /* userdata= */ रिक्त);
}

DECLARE_STRING_सारणी_LOOKUP(log_target, LogTarget);
शून्य log_समुच्चयः_target(LogTarget target);
शून्य log_समुच्चयः_target_and_open(LogTarget target);
पूर्णाङ्क log_समुच्चयः_target_from_string(ध्रुव वर्ण *e);
LogTarget log_get_target(शून्य) विशुद्धम्;
शून्य log_settle_target(शून्य);

पूर्णाङ्क log_समुच्चयः_max_level(पूर्णाङ्क level);
पूर्णाङ्क log_समुच्चयः_max_level_from_string(ध्रुव वर्ण *e);
पूर्णाङ्क log_get_max_level(शून्य) विशुद्धम्;
पूर्णाङ्क log_get_target_max_level(LogTarget target);
पूर्णाङ्क log_max_स्तराः_गन्तव्यम्_string(पूर्णाङ्क level, वर्ण **ret);

शून्य log_समुच्चयः_facility(पूर्णाङ्क facility);

शून्य log_show_color(bool b);
bool log_get_show_color(शून्य) विशुद्धम्;
शून्य log_show_location(bool b);
bool log_get_show_location(शून्य) विशुद्धम्;
शून्य log_show_time(bool b);
bool log_get_show_time(शून्य) विशुद्धम्;
शून्य log_show_tid(bool b);
bool log_get_show_tid(शून्य) विशुद्धम्;
शून्य log_show_executable(bool b);
bool log_get_show_executable(शून्य) विशुद्धम्;

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

पूर्णाङ्क log_dispatch_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_क्षेत्रम्,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_क्षेत्रम्,
                ध्रुव वर्ण *extra,
                वर्ण *बफर);

पूर्णाङ्क log_आन्तरिकम्(
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
                ध्रुव वर्ण *object_क्षेत्रम्,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_क्षेत्रम्,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format,
                va_list ap) _printf_(10,0);

पूर्णाङ्क log_object_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *object_क्षेत्रम्,
                ध्रुव वर्ण *object,
                ध्रुव वर्ण *extra_क्षेत्रम्,
                ध्रुव वर्ण *extra,
                ध्रुव वर्ण *format, ...) _printf_(10,11);

पूर्णाङ्क log_struct_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) _sentinel_;

पूर्णाङ्क log_oom_आन्तरिकम्(
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

पूर्णाङ्क log_struct_iovec_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव संरचना iovec *निवेशः_iovec,
                size_t n_निवेशः_iovec);

/* This modifies the buffer passed! */
पूर्णाङ्क log_dump_आन्तरिकम्(
                पूर्णाङ्क level,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                वर्ण *बफर);

#परिभाषय log_dispatch(level, error, बफर)                              \
        log_dispatch_आन्तरिकम्(level, error, PROJECT_FILE, __LINE__, __func__, रिक्त, रिक्त, रिक्त, रिक्त, बफर)

/* Logging with level */
#परिभाषय log_पूर्णम्_errno_zerook(level, error, ...)                        \
        ({                                                              \
                पूर्णाङ्क _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_आन्तरिकम्(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मानम्(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#यदि BUILD_बहुलकम्_DEVELOPER && !defined(TEST_संहिता)
#  define ASSERT_NON_ZERO(x) प्रतिज्ञा((x) != 0)
#  define ASSERT_UNDERFLOW(x) प्रतिज्ञा((x) >= INT_MIN)
#अन्यथा
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#यदि_अन्तः

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#परिभाषय log_पूर्णम्_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_पूर्णम्_errno_zerook(level, (पूर्णाङ्क)_error, __VA_ARGS__); \
        })

#परिभाषय log_पूर्णम्(level, fmt, ...)                                      \
        ({                                                             \
                यदि (BUILD_बहुलकम्_DEVELOPER)                              \
                        प्रतिज्ञा(!वाक्ये_अन्विष(fmt, "%m"));                    \
                (शून्य) log_पूर्णम्_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

पूर्णाङ्क log_emergency_level(शून्य);

/* Normal logging */
#परिभाषय log_debug(...)     log_पूर्णम्(LOG_DEBUG,   __VA_ARGS__)
#परिभाषय log_info(...)      log_पूर्णम्(LOG_INFO,    __VA_ARGS__)
#परिभाषय log_notice(...)    log_पूर्णम्(LOG_NOTICE,  __VA_ARGS__)
#परिभाषय log_warning(...)   log_पूर्णम्(LOG_WARNING, __VA_ARGS__)
#परिभाषय log_error(...)     log_पूर्णम्(LOG_ERR,     __VA_ARGS__)
#परिभाषय log_emergency(...) log_पूर्णम्(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#परिभाषय log_debug_errno(error, ...)     log_पूर्णम्_errno(LOG_DEBUG,   error, __VA_ARGS__)
#परिभाषय log_info_errno(error, ...)      log_पूर्णम्_errno(LOG_INFO,    error, __VA_ARGS__)
#परिभाषय log_notice_errno(error, ...)    log_पूर्णम्_errno(LOG_NOTICE,  error, __VA_ARGS__)
#परिभाषय log_warning_errno(error, ...)   log_पूर्णम्_errno(LOG_WARNING, error, __VA_ARGS__)
#परिभाषय log_error_errno(error, ...)     log_पूर्णम्_errno(LOG_ERR,     error, __VA_ARGS__)
#परिभाषय log_emergency_errno(error, ...) log_पूर्णम्_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#परिभाषय log_once(level, ...)                                             \
        ({                                                               \
                यदि (ONCE)                                                \
                        log_पूर्णम्(level, __VA_ARGS__);                    \
                अन्यथा यदि (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#परिभाषय log_once_errno(level, error, ...)                                \
        ({                                                               \
                पूर्णाङ्क _err = (error);                                      \
                यदि (ONCE)                                                \
                        _err = log_पूर्णम्_errno(level, _err, __VA_ARGS__); \
                अन्यथा यदि (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                अन्यथा                                                     \
                        _err = -ERRNO_मानम्(_err);                       \
                _err;                                                    \
        })

#यदि LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#अन्यथा
#  define log_trace(...)          कुरु {} यावत् (0)
#  define log_trace_errno(e, ...) (-ERRNO_मानम्(e))
#यदि_अन्तः

/* Structured logging */
#परिभाषय log_struct_errno(level, error, ...)                             \
        log_struct_आन्तरिकम्(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, रिक्त)
#परिभाषय log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#परिभाषय log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_आन्तरिकम्(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#परिभाषय log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#परिभाषय log_struct_once(level, ...)                                     \
        ({                                                              \
                यदि (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                अन्यथा यदि (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#परिभाषय log_dump(level, बफर)                                         \
        log_dump_आन्तरिकम्(level, 0, PROJECT_FILE, __LINE__, __func__, बफर)

#परिभाषय log_oom_पूर्णम्(level) log_oom_आन्तरिकम्(level, PROJECT_FILE, __LINE__, __func__)
#परिभाषय log_oom()           log_oom_पूर्णम्(LOG_ERR)
#परिभाषय log_oom_debug()     log_oom_पूर्णम्(LOG_DEBUG)
#परिभाषय log_oom_warning()   log_oom_पूर्णम्(LOG_WARNING)

bool log_on_console(शून्य) विशुद्धम्;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#यदि_परिभाषितम् __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", रिक्त, ##__VA_ARGS__
#नोचेद्यदि LOG_सन्देशः_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
बाह्य bool log_सन्देशः_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (log_सन्देशः_dummy && प्रारूप_लिख(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#अन्यथा
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#यदि_अन्तः

#परिभाषय LOG_सन्देशः(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#परिभाषय LOG_सन्देशः_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

शून्य log_received_signal(पूर्णाङ्क level, ध्रुव संरचना signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
शून्य log_समुच्चयः_upgrade_syslog_गन्तव्यम्_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
शून्य log_समुच्चयः_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
शून्य log_समुच्चयः_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
शून्य log_समुच्चयः_prohibit_ipc(bool b);

पूर्णाङ्क log_dup_console(शून्य);

पूर्णाङ्क log_syntax_आन्तरिकम्(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                अचिह्नित config_पङ्क्तिः,
                पूर्णाङ्क error,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *format, ...) _printf_(9, 10);

पूर्णाङ्क log_syntax_invalid_utf8_आन्तरिकम्(
                ध्रुव वर्ण *unit,
                पूर्णाङ्क level,
                ध्रुव वर्ण *config_file,
                अचिह्नित config_पङ्क्तिः,
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *rvalue);

पूर्णाङ्क log_syntax_parse_error_आन्तरिकम्(
                ध्रुव वर्ण *unit,
                ध्रुव वर्ण *config_file,
                अचिह्नित config_पङ्क्तिः,
                पूर्णाङ्क error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ध्रुव वर्ण *file,
                पूर्णाङ्क पङ्क्तिः,
                ध्रुव वर्ण *func,
                ध्रुव वर्ण *lvalue,
                ध्रुव वर्ण *rvalue);

#परिभाषय log_syntax(unit, level, config_file, config_पङ्क्तिः, error, ...)   \
        ({                                                              \
                पूर्णाङ्क _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_आन्तरिकम्(unit, _level, config_file, config_पङ्क्तिः, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मानम्(_e);                             \
        })

#परिभाषय log_syntax_invalid_utf8(unit, level, config_file, config_पङ्क्तिः, rvalue) \
        ({                                                              \
                पूर्णाङ्क _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_आन्तरिकम्(unit, _level, config_file, config_पङ्क्तिः, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#परिभाषय log_syntax_parse_error_पूर्णम्(unit, config_file, config_पङ्क्तिः, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_आन्तरिकम्(unit, config_file, config_पङ्क्तिः, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#परिभाषय log_syntax_parse_error(unit, config_file, config_पङ्क्तिः, error, lvalue, rvalue) \
        log_syntax_parse_error_पूर्णम्(unit, config_file, config_पङ्क्तिः, error, /* critical= */ false, lvalue, rvalue)

#परिभाषय DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

शून्य log_सज्जता(शून्य);

ध्रुव वर्ण* log_समुच्चयः_उपसर्गः(ध्रुव वर्ण *उपसर्गः, bool force);

शून्य log_उपसर्गः_swap(ध्रुव वर्ण **उपसर्गः);
स्थिर अन्तःस्थ ध्रुव वर्ण* _log_unset_prefixp(ध्रुव वर्ण **p) {
        प्रतिज्ञा(p);
        log_समुच्चयः_उपसर्गः(*p, true);
        प्रत्यागम रिक्त;
}

#परिभाषय LOG_समुच्चयः_उपसर्गः(उपसर्गः) \
        _cleanup_(_log_unset_prefixp) अप्रयुक्तम् ध्रुव वर्ण *CONCATENATE(cleanup_log_unset_उपसर्गः, UNIQ) = log_समुच्चयः_उपसर्गः(उपसर्गः, false);
