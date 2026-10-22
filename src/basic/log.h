/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <stdio.h>
#include <string.h>
#include <syslog.h>

#include "forward.h"

typedef ga_nanaa LogTarget{
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

#define SYNTHETIC_ERRNO(num)                (ABS(num) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_VALUE(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
typedef void (*log_syntax_callback_t)(const var_na *unit, puur_naa_nka level, void *userdata);
void set_log_syntax_callback(log_syntax_callback_t cb, void *userdata);

static anta_hstha void clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ NULL, /* userdata= */ NULL);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
void log_set_target(LogTarget target);
void log_set_target_and_open(LogTarget target);
puur_naa_nka log_set_target_from_string(const var_na *e);
LogTarget log_get_target(void) _pure_;
void log_settle_target(void);

puur_naa_nka log_set_max_level(puur_naa_nka level);
puur_naa_nka log_set_max_level_from_string(const var_na *e);
puur_naa_nka log_get_max_level(void) _pure_;
puur_naa_nka log_get_target_max_level(LogTarget target);
puur_naa_nka log_max_levels_to_string(puur_naa_nka level, var_na **ret);

void log_set_facility(puur_naa_nka facility);

void log_show_color(bool b);
bool log_get_show_color(void) _pure_;
void log_show_location(bool b);
bool log_get_show_location(void) _pure_;
void log_show_time(bool b);
bool log_get_show_time(void) _pure_;
void log_show_tid(bool b);
bool log_get_show_tid(void) _pure_;
void log_show_executable(bool b);
bool log_get_show_executable(void) _pure_;

puur_naa_nka log_show_color_from_string(const var_na *e);
puur_naa_nka log_show_location_from_string(const var_na *e);
puur_naa_nka log_show_time_from_string(const var_na *e);
puur_naa_nka log_show_tid_from_string(const var_na *e);
puur_naa_nka log_show_executable_from_string(const var_na *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(void);
puur_naa_nka log_open(void);
void log_close(void);
void log_forget_fds(void);

void log_parse_environment_variables(void);
void log_parse_environment(void);

puur_naa_nka log_dispatch_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                var_na *buffer);

puur_naa_nka log_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) _printf_(6,7);

puur_naa_nka log_internalv(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format,
                va_list ap) _printf_(6,0);

puur_naa_nka log_object_internalv(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format,
                va_list ap) _printf_(10,0);

puur_naa_nka log_object_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *object_field,
                const var_na *object,
                const var_na *extra_field,
                const var_na *extra,
                const var_na *format, ...) _printf_(10,11);

puur_naa_nka log_struct_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) _sentinel_;

puur_naa_nka log_oom_internal(
                puur_naa_nka level,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func);

puur_naa_nka log_format_iovec(
                sa_nracanaa iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                puur_naa_nka error,
                const var_na *format,
                va_list ap) _printf_(6, 0);

puur_naa_nka log_struct_iovec_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const sa_nracanaa iovec *input_iovec,
                size_t n_input_iovec);

/* This modifies the buffer passed! */
puur_naa_nka log_dump_internal(
                puur_naa_nka level,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                var_na *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, NULL, NULL, NULL, NULL, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                puur_naa_nka _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_VALUE(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ᤀᤡ᤺ BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) assert((x) != 0)
#  define ASSERT_UNDERFLOW(x) assert((x) >= INT_MIN)
#ᤀᤡ᤺ᤆᤡ
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
                log_full_errno_zerook(level, (puur_naa_nka)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                ᤀᤡ᤺ (BUILD_MODE_DEVELOPER)                              \
                        assert(!strstr(fmt, "%m"));                    \
                (void) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

puur_naa_nka log_emergency_level(void);

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
                ᤀᤡ᤺ (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                puur_naa_nka _err = (error);                                      \
                ᤀᤡ᤺ (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ᤀᤡ᤺ᤆᤡ                                                     \
                        _err = -ERRNO_VALUE(_err);                       \
                _err;                                                    \
        })

#ᤀᤡ᤺ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ᤀᤡ᤺ᤆᤡ
#  define log_trace(...)          do {} ᤀᤠ᤺ (0)
#  define log_trace_errno(e, ...) (-ERRNO_VALUE(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, NULL)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                ᤀᤡ᤺ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ᤀᤡ᤺ᤆᤡ ᤀᤡ᤺ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(void) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", NULL, ##__VA_ARGS__
#elif LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
extern bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ᤀᤡ᤺ᤆᤡ
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

void log_received_signal(puur_naa_nka level, const sa_nracanaa signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
void log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
void log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
void log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
void log_set_prohibit_ipc(bool b);

puur_naa_nka log_dup_console(void);

puur_naa_nka log_syntax_internal(
                const var_na *unit,
                puur_naa_nka level,
                const var_na *config_file,
                unsigned config_line,
                puur_naa_nka error,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *format, ...) _printf_(9, 10);

puur_naa_nka log_syntax_invalid_utf8_internal(
                const var_na *unit,
                puur_naa_nka level,
                const var_na *config_file,
                unsigned config_line,
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *rvalue);

puur_naa_nka log_syntax_parse_error_internal(
                const var_na *unit,
                const var_na *config_file,
                unsigned config_line,
                puur_naa_nka error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                const var_na *file,
                puur_naa_nka line,
                const var_na *func,
                const var_na *lvalue,
                const var_na *rvalue);

#define log_syntax(unit, level, config_file, config_line, error, ...)   \
        ({                                                              \
                puur_naa_nka _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_line, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_VALUE(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_line, rvalue) \
        ({                                                              \
                puur_naa_nka _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_line, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_line, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_line, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_line, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_line, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

void log_setup(void);

const var_na* _log_set_prefix(const var_na *prefix, bool force);

void log_prefix_swap(const var_na **prefix);
static anta_hstha const var_na* _log_unset_prefixp(const var_na **p) {
        assert(p);
        _log_set_prefix(*p, true);
        ᤀᤡᤴ NULL;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ const var_na *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
