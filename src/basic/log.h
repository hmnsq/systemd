/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <syslog.h>

#અન્તર્ભાવ "forward.h"

પ્રકારવ્યાખ્યા ગણના LogTarget{
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

#define SYNTHETIC_ERRNO(અંક)                (ABS(અંક) | (1 << 30))
#define IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#define ERRNO_કિંમત(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
પ્રકારવ્યાખ્યા શૂન્ય (*log_syntax_callback_t)(અચલ અક્ષર *unit, પૂર્ણાંક level, શૂન્ય *userdata);
શૂન્ય set_log_syntax_callback(log_syntax_callback_t cb, શૂન્ય *userdata);

સ્થિર અંતઃરેખ શૂન્ય clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ ખાલી, /* userdata= */ ખાલી);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
શૂન્ય log_set_target(LogTarget target);
શૂન્ય log_set_target_and_open(LogTarget target);
પૂર્ણાંક log_set_target_from_string(અચલ અક્ષર *e);
LogTarget log_get_target(શૂન્ય) _pure_;
શૂન્ય log_settle_target(શૂન્ય);

પૂર્ણાંક log_set_max_level(પૂર્ણાંક level);
પૂર્ણાંક log_set_max_level_from_string(અચલ અક્ષર *e);
પૂર્ણાંક log_get_max_level(શૂન્ય) _pure_;
પૂર્ણાંક log_get_target_max_level(LogTarget target);
પૂર્ણાંક log_max_levels_to_string(પૂર્ણાંક level, અક્ષર **ret);

શૂન્ય log_set_facility(પૂર્ણાંક facility);

શૂન્ય log_show_color(bool b);
bool log_get_show_color(શૂન્ય) _pure_;
શૂન્ય log_show_location(bool b);
bool log_get_show_location(શૂન્ય) _pure_;
શૂન્ય log_show_time(bool b);
bool log_get_show_time(શૂન્ય) _pure_;
શૂન્ય log_show_tid(bool b);
bool log_get_show_tid(શૂન્ય) _pure_;
શૂન્ય log_show_executable(bool b);
bool log_get_show_executable(શૂન્ય) _pure_;

પૂર્ણાંક log_show_color_from_string(અચલ અક્ષર *e);
પૂર્ણાંક log_show_location_from_string(અચલ અક્ષર *e);
પૂર્ણાંક log_show_time_from_string(અચલ અક્ષર *e);
પૂર્ણાંક log_show_tid_from_string(અચલ અક્ષર *e);
પૂર્ણાંક log_show_executable_from_string(અચલ અક્ષર *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(શૂન્ય);
પૂર્ણાંક log_open(શૂન્ય);
શૂન્ય log_close(શૂન્ય);
શૂન્ય log_forget_fds(શૂન્ય);

શૂન્ય log_parse_environment_variables(શૂન્ય);
શૂન્ય log_parse_environment(શૂન્ય);

પૂર્ણાંક log_dispatch_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અક્ષર *buffer);

પૂર્ણાંક log_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) _printf_(6,7);

પૂર્ણાંક log_internalv(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format,
                va_list ap) _printf_(6,0);

પૂર્ણાંક log_object_internalv(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અચલ અક્ષર *format,
                va_list ap) _printf_(10,0);

પૂર્ણાંક log_object_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *object_field,
                અચલ અક્ષર *object,
                અચલ અક્ષર *extra_field,
                અચલ અક્ષર *extra,
                અચલ અક્ષર *format, ...) _printf_(10,11);

પૂર્ણાંક log_struct_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) _sentinel_;

પૂર્ણાંક log_oom_internal(
                પૂર્ણાંક level,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func);

પૂર્ણાંક log_format_iovec(
                સંરચના iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                પૂર્ણાંક error,
                અચલ અક્ષર *format,
                va_list ap) _printf_(6, 0);

પૂર્ણાંક log_struct_iovec_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ સંરચના iovec *આગત_iovec,
                size_t n_આગત_iovec);

/* This modifies the buffer passed! */
પૂર્ણાંક log_dump_internal(
                પૂર્ણાંક level,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અક્ષર *buffer);

#define log_dispatch(level, error, buffer)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, ખાલી, ખાલી, ખાલી, ખાલી, buffer)

/* Logging with level */
#define log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                પૂર્ણાંક _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_કિંમત(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#જો BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) પ્રતિજ્ઞા((x) != 0)
#  define ASSERT_UNDERFLOW(x) પ્રતિજ્ઞા((x) >= INT_MIN)
#નહીંતર
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
                log_full_errno_zerook(level, (પૂર્ણાંક)_error, __VA_ARGS__); \
        })

#define log_full(level, fmt, ...)                                      \
        ({                                                             \
                જો (BUILD_MODE_DEVELOPER)                              \
                        પ્રતિજ્ઞા(!strstr(fmt, "%m"));                    \
                (શૂન્ય) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

પૂર્ણાંક log_emergency_level(શૂન્ય);

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
                જો (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                નહીંતર જો (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#define log_once_errno(level, error, ...)                                \
        ({                                                               \
                પૂર્ણાંક _err = (error);                                      \
                જો (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                નહીંતર જો (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                નહીંતર                                                     \
                        _err = -ERRNO_કિંમત(_err);                       \
                _err;                                                    \
        })

#જો LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#નહીંતર
#  define log_trace(...)          કરો {} જ્યારે (0)
#  define log_trace_errno(e, ...) (-ERRNO_કિંમત(e))
#endif

/* Structured logging */
#define log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ખાલી)
#define log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#define log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#define log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#define log_struct_once(level, ...)                                     \
        ({                                                              \
                જો (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                નહીંતર જો (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#define log_dump(level, buffer)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, buffer)

#define log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#define log_oom()           log_oom_full(LOG_ERR)
#define log_oom_debug()     log_oom_full(LOG_DEBUG)
#define log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(શૂન્ય) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ifdef __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", ખાલી, ##__VA_ARGS__
#અથવાજો LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
બાહ્ય bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && printf(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#નહીંતર
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#endif

#define LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#define LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

શૂન્ય log_received_signal(પૂર્ણાંક level, અચલ સંરચના signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
શૂન્ય log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
શૂન્ય log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
શૂન્ય log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
શૂન્ય log_set_prohibit_ipc(bool b);

પૂર્ણાંક log_dup_console(શૂન્ય);

પૂર્ણાંક log_syntax_internal(
                અચલ અક્ષર *unit,
                પૂર્ણાંક level,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                પૂર્ણાંક error,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *format, ...) _printf_(9, 10);

પૂર્ણાંક log_syntax_invalid_utf8_internal(
                અચલ અક્ષર *unit,
                પૂર્ણાંક level,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *rvalue);

પૂર્ણાંક log_syntax_parse_error_internal(
                અચલ અક્ષર *unit,
                અચલ અક્ષર *config_file,
                unsigned config_લીટી,
                પૂર્ણાંક error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                અચલ અક્ષર *file,
                પૂર્ણાંક લીટી,
                અચલ અક્ષર *func,
                અચલ અક્ષર *lvalue,
                અચલ અક્ષર *rvalue);

#define log_syntax(unit, level, config_file, config_લીટી, error, ...)   \
        ({                                                              \
                પૂર્ણાંક _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_લીટી, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_કિંમત(_e);                             \
        })

#define log_syntax_invalid_utf8(unit, level, config_file, config_લીટી, rvalue) \
        ({                                                              \
                પૂર્ણાંક _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_લીટી, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#define log_syntax_parse_error_full(unit, config_file, config_લીટી, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_લીટી, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#define log_syntax_parse_error(unit, config_file, config_લીટી, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_લીટી, error, /* critical= */ false, lvalue, rvalue)

#define DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

શૂન્ય log_setup(શૂન્ય);

અચલ અક્ષર* _log_set_prefix(અચલ અક્ષર *prefix, bool force);

શૂન્ય log_prefix_swap(અચલ અક્ષર **prefix);
સ્થિર અંતઃરેખ અચલ અક્ષર* _log_unset_prefixp(અચલ અક્ષર **p) {
        પ્રતિજ્ઞા(p);
        _log_set_prefix(*p, true);
        પાછા ખાલી;
}

#define LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ અચલ અક્ષર *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
