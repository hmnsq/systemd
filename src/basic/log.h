/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <syslog.h>

#ଅନ୍ତର୍ଭାଵ "forward.h"

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା LogTarget{
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

#define SYNTHETIC_ERRNO(ଅଙ୍କ)                (ABS(ଅଙ୍କ) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_ମୂଲ୍ୟ(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଶୂନ୍ୟ (*log_syntax_callback_t)(ଅଚଳ ଅକ୍ଷର *unit, ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଶୂନ୍ୟ *userdata);
ଶୂନ୍ୟ set_log_syntax_callback(log_syntax_callback_t cb, ଶୂନ୍ୟ *userdata);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ ଖାଲି, /* userdata= */ ଖାଲି);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
ଶୂନ୍ୟ log_set_target(LogTarget target);
ଶୂନ୍ୟ log_set_target_and_open(LogTarget target);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_target_from_string(ଅଚଳ ଅକ୍ଷର *e);
LogTarget log_get_target(ଶୂନ୍ୟ) _pure_;
ଶୂନ୍ୟ log_settle_target(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_max_level(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_set_max_level_from_string(ଅଚଳ ଅକ୍ଷର *e);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_get_max_level(ଶୂନ୍ୟ) _pure_;
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_get_target_max_level(LogTarget target);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_max_levels_to_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅକ୍ଷର **ret);

ଶୂନ୍ୟ log_set_facility(ପୂର୍ଣ୍ଣସଂଖ୍ୟା facility);

ଶୂନ୍ୟ log_show_color(bool b);
bool log_get_show_color(ଶୂନ୍ୟ) _pure_;
ଶୂନ୍ୟ log_show_location(bool b);
bool log_get_show_location(ଶୂନ୍ୟ) _pure_;
ଶୂନ୍ୟ log_show_time(bool b);
bool log_get_show_time(ଶୂନ୍ୟ) _pure_;
ଶୂନ୍ୟ log_show_tid(bool b);
bool log_get_show_tid(ଶୂନ୍ୟ) _pure_;
ଶୂନ୍ୟ log_show_executable(bool b);
bool log_get_show_executable(ଶୂନ୍ୟ) _pure_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_color_from_string(ଅଚଳ ଅକ୍ଷର *e);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_location_from_string(ଅଚଳ ଅକ୍ଷର *e);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_time_from_string(ଅଚଳ ଅକ୍ଷର *e);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_tid_from_string(ଅଚଳ ଅକ୍ଷର *e);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_show_executable_from_string(ଅଚଳ ଅକ୍ଷର *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(ଶୂନ୍ୟ);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_open(ଶୂନ୍ୟ);
ଶୂନ୍ୟ log_close(ଶୂନ୍ୟ);
ଶୂନ୍ୟ log_forget_fds(ଶୂନ୍ୟ);

ଶୂନ୍ୟ log_parse_environment_variables(ଶୂନ୍ୟ);
ଶୂନ୍ୟ log_parse_environment(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dispatch_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅକ୍ଷର *buffer);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(6,7);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_internalv(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) _printf_(6,0);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_object_internalv(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) _printf_(10,0);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_object_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *object_field,
                ଅଚଳ ଅକ୍ଷର *object,
                ଅଚଳ ଅକ୍ଷର *extra_field,
                ଅଚଳ ଅକ୍ଷର *extra,
                ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(10,11);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_struct_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) _sentinel_;

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_oom_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_format_iovec(
                ସଂରଚନା iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *format,
                va_list ap) _printf_(6, 0);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_struct_iovec_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ସଂରଚନା iovec *ନିବେଶ_iovec,
                size_t n_ନିବେଶ_iovec);

/* This modifies the buffer passed! */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dump_internal(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅକ୍ଷର *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, ଖାଲି, ଖାଲି, ଖାଲି, ଖାଲି, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ମୂଲ୍ୟ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ଯଦି BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) ପ୍ରତିଜ୍ଞା((x) != 0)
#  define ASSERT_UNDERFLOW(x) ପ୍ରତିଜ୍ଞା((x) >= INT_MIN)
#ଅନ୍ୟଥା
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
                log_full_errno_zerook(level, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                ଯଦି (BUILD_MODE_DEVELOPER)                              \
                        ପ୍ରତିଜ୍ଞା(!strstr(fmt, "%m"));                    \
                (ଶୂନ୍ୟ) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_emergency_level(ଶୂନ୍ୟ);

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
                ଯଦି (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                ଅନ୍ୟଥା ଯଦି (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _err = (error);                                      \
                ଯଦି (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                ଅନ୍ୟଥା ଯଦି (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ଅନ୍ୟଥା                                                     \
                        _err = -ERRNO_ମୂଲ୍ୟ(_err);                       \
                _err;                                                    \
        })

#ଯଦି LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ଅନ୍ୟଥା
#  define log_trace(...)          କର {} ଯେତେବେଳେ (0)
#  define log_trace_errno(e, ...) (-ERRNO_ମୂଲ୍ୟ(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ଖାଲି)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                ଯଦି (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ଅନ୍ୟଥା ଯଦି (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(ଶୂନ୍ୟ) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", ଖାଲି, ##__VA_ARGS__
#ନଚେତଯଦି LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
ବାହ୍ୟ bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ଅନ୍ୟଥା
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

ଶୂନ୍ୟ log_received_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା level, ଅଚଳ ସଂରଚନା signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
ଶୂନ୍ୟ log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
ଶୂନ୍ୟ log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
ଶୂନ୍ୟ log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
ଶୂନ୍ୟ log_set_prohibit_ipc(bool b);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_dup_console(ଶୂନ୍ୟ);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *format, ...) _printf_(9, 10);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_invalid_utf8_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା level,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *rvalue);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_syntax_parse_error_internal(
                ଅଚଳ ଅକ୍ଷର *unit,
                ଅଚଳ ଅକ୍ଷର *config_file,
                unsigned config_ଧାଡ଼ି,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ଅଚଳ ଅକ୍ଷର *file,
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଧାଡ଼ି,
                ଅଚଳ ଅକ୍ଷର *func,
                ଅଚଳ ଅକ୍ଷର *lvalue,
                ଅଚଳ ଅକ୍ଷର *rvalue);

#define log_syntax(unit, level, config_file, config_ଧାଡ଼ି, error, ...)   \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_ଧାଡ଼ି, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ମୂଲ୍ୟ(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_ଧାଡ଼ି, rvalue) \
        ({                                                              \
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_ଧାଡ଼ି, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_ଧାଡ଼ି, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_ଧାଡ଼ି, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_ଧାଡ଼ି, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_ଧାଡ଼ି, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

ଶୂନ୍ୟ log_setup(ଶୂନ୍ୟ);

ଅଚଳ ଅକ୍ଷର* _log_set_prefix(ଅଚଳ ଅକ୍ଷର *prefix, bool force);

ଶୂନ୍ୟ log_prefix_swap(ଅଚଳ ଅକ୍ଷର **prefix);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଅଚଳ ଅକ୍ଷର* _log_unset_prefixp(ଅଚଳ ଅକ୍ଷର **p) {
        ପ୍ରତିଜ୍ଞା(p);
        _log_set_prefix(*p, true);
        ଫେରନ୍ତୁ ଖାଲି;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ ଅଚଳ ଅକ୍ଷର *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
