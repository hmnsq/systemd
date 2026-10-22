/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <syslog.h>

#അന്തര്ഭാവ "forward.h"

തരനിർവചനം എണ്ണൽ LogTarget{
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

#define SYNTHETIC_ERRNO(അക്കം)                (ABS(അക്കം) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_മൂല്യം(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
തരനിർവചനം ശൂന്യം (*log_syntax_callback_t)(സ്ഥിരം അക്ഷരം *unit, പൂർണ്ണസംഖ്യ level, ശൂന്യം *userdata);
ശൂന്യം set_log_syntax_callback(log_syntax_callback_t cb, ശൂന്യം *userdata);

സ്ഥിതം വരിയിൽ ശൂന്യം clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ ശൂന്യ, /* userdata= */ ശൂന്യ);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
ശൂന്യം log_set_target(LogTarget target);
ശൂന്യം log_set_target_and_open(LogTarget target);
പൂർണ്ണസംഖ്യ log_set_target_from_string(സ്ഥിരം അക്ഷരം *e);
LogTarget log_get_target(ശൂന്യം) _pure_;
ശൂന്യം log_settle_target(ശൂന്യം);

പൂർണ്ണസംഖ്യ log_set_max_level(പൂർണ്ണസംഖ്യ level);
പൂർണ്ണസംഖ്യ log_set_max_level_from_string(സ്ഥിരം അക്ഷരം *e);
പൂർണ്ണസംഖ്യ log_get_max_level(ശൂന്യം) _pure_;
പൂർണ്ണസംഖ്യ log_get_target_max_level(LogTarget target);
പൂർണ്ണസംഖ്യ log_max_levels_to_string(പൂർണ്ണസംഖ്യ level, അക്ഷരം **ret);

ശൂന്യം log_set_facility(പൂർണ്ണസംഖ്യ facility);

ശൂന്യം log_show_color(bool b);
bool log_get_show_color(ശൂന്യം) _pure_;
ശൂന്യം log_show_location(bool b);
bool log_get_show_location(ശൂന്യം) _pure_;
ശൂന്യം log_show_time(bool b);
bool log_get_show_time(ശൂന്യം) _pure_;
ശൂന്യം log_show_tid(bool b);
bool log_get_show_tid(ശൂന്യം) _pure_;
ശൂന്യം log_show_executable(bool b);
bool log_get_show_executable(ശൂന്യം) _pure_;

പൂർണ്ണസംഖ്യ log_show_color_from_string(സ്ഥിരം അക്ഷരം *e);
പൂർണ്ണസംഖ്യ log_show_location_from_string(സ്ഥിരം അക്ഷരം *e);
പൂർണ്ണസംഖ്യ log_show_time_from_string(സ്ഥിരം അക്ഷരം *e);
പൂർണ്ണസംഖ്യ log_show_tid_from_string(സ്ഥിരം അക്ഷരം *e);
പൂർണ്ണസംഖ്യ log_show_executable_from_string(സ്ഥിരം അക്ഷരം *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(ശൂന്യം);
പൂർണ്ണസംഖ്യ log_open(ശൂന്യം);
ശൂന്യം log_close(ശൂന്യം);
ശൂന്യം log_forget_fds(ശൂന്യം);

ശൂന്യം log_parse_environment_variables(ശൂന്യം);
ശൂന്യം log_parse_environment(ശൂന്യം);

പൂർണ്ണസംഖ്യ log_dispatch_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                അക്ഷരം *buffer);

പൂർണ്ണസംഖ്യ log_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) _printf_(6,7);

പൂർണ്ണസംഖ്യ log_internalv(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) _printf_(6,0);

പൂർണ്ണസംഖ്യ log_object_internalv(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) _printf_(10,0);

പൂർണ്ണസംഖ്യ log_object_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *object_field,
                സ്ഥിരം അക്ഷരം *object,
                സ്ഥിരം അക്ഷരം *extra_field,
                സ്ഥിരം അക്ഷരം *extra,
                സ്ഥിരം അക്ഷരം *format, ...) _printf_(10,11);

പൂർണ്ണസംഖ്യ log_struct_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) _sentinel_;

പൂർണ്ണസംഖ്യ log_oom_internal(
                പൂർണ്ണസംഖ്യ level,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func);

പൂർണ്ണസംഖ്യ log_format_iovec(
                ഘടന iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *format,
                va_list ap) _printf_(6, 0);

പൂർണ്ണസംഖ്യ log_struct_iovec_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം ഘടന iovec *നിവേശം_iovec,
                size_t n_നിവേശം_iovec);

/* This modifies the buffer passed! */
പൂർണ്ണസംഖ്യ log_dump_internal(
                പൂർണ്ണസംഖ്യ level,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                അക്ഷരം *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                പൂർണ്ണസംഖ്യ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_മൂല്യം(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#എങ്കിൽ BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) പ്രതിജ്ഞാ((x) != 0)
#  define ASSERT_UNDERFLOW(x) പ്രതിജ്ഞാ((x) >= INT_MIN)
#അല്ലെങ്കിൽ
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
                log_full_errno_zerook(level, (പൂർണ്ണസംഖ്യ)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                എങ്കിൽ (BUILD_MODE_DEVELOPER)                              \
                        പ്രതിജ്ഞാ(!strstr(fmt, "%m"));                    \
                (ശൂന്യം) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

പൂർണ്ണസംഖ്യ log_emergency_level(ശൂന്യം);

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
                എങ്കിൽ (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                അല്ലെങ്കിൽ എങ്കിൽ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                പൂർണ്ണസംഖ്യ _err = (error);                                      \
                എങ്കിൽ (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                അല്ലെങ്കിൽ എങ്കിൽ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                അല്ലെങ്കിൽ                                                     \
                        _err = -ERRNO_മൂല്യം(_err);                       \
                _err;                                                    \
        })

#എങ്കിൽ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#അല്ലെങ്കിൽ
#  define log_trace(...)          ചെയ്യുക {} ആകുമ്പോൾ (0)
#  define log_trace_errno(e, ...) (-ERRNO_മൂല്യം(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ശൂന്യ)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                എങ്കിൽ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                അല്ലെങ്കിൽ എങ്കിൽ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(ശൂന്യം) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", ശൂന്യ, ##__VA_ARGS__
#അല്ലെങ്കിലെങ്കിൽ LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
ബാഹ്യ bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#അല്ലെങ്കിൽ
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

ശൂന്യം log_received_signal(പൂർണ്ണസംഖ്യ level, സ്ഥിരം ഘടന signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
ശൂന്യം log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
ശൂന്യം log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
ശൂന്യം log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
ശൂന്യം log_set_prohibit_ipc(bool b);

പൂർണ്ണസംഖ്യ log_dup_console(ശൂന്യം);

പൂർണ്ണസംഖ്യ log_syntax_internal(
                സ്ഥിരം അക്ഷരം *unit,
                പൂർണ്ണസംഖ്യ level,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                പൂർണ്ണസംഖ്യ error,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *format, ...) _printf_(9, 10);

പൂർണ്ണസംഖ്യ log_syntax_invalid_utf8_internal(
                സ്ഥിരം അക്ഷരം *unit,
                പൂർണ്ണസംഖ്യ level,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *rvalue);

പൂർണ്ണസംഖ്യ log_syntax_parse_error_internal(
                സ്ഥിരം അക്ഷരം *unit,
                സ്ഥിരം അക്ഷരം *config_file,
                unsigned config_വരി,
                പൂർണ്ണസംഖ്യ error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                സ്ഥിരം അക്ഷരം *file,
                പൂർണ്ണസംഖ്യ വരി,
                സ്ഥിരം അക്ഷരം *func,
                സ്ഥിരം അക്ഷരം *lvalue,
                സ്ഥിരം അക്ഷരം *rvalue);

#define log_syntax(unit, level, config_file, config_വരി, error, ...)   \
        ({                                                              \
                പൂർണ്ണസംഖ്യ _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_വരി, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_മൂല്യം(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_വരി, rvalue) \
        ({                                                              \
                പൂർണ്ണസംഖ്യ _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_വരി, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_വരി, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_വരി, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_വരി, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_വരി, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

ശൂന്യം log_setup(ശൂന്യം);

സ്ഥിരം അക്ഷരം* _log_set_prefix(സ്ഥിരം അക്ഷരം *prefix, bool force);

ശൂന്യം log_prefix_swap(സ്ഥിരം അക്ഷരം **prefix);
സ്ഥിതം വരിയിൽ സ്ഥിരം അക്ഷരം* _log_unset_prefixp(സ്ഥിരം അക്ഷരം **p) {
        പ്രതിജ്ഞാ(p);
        _log_set_prefix(*p, true);
        തിരികെ ശൂന്യ;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ സ്ഥിരം അക്ഷരം *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
