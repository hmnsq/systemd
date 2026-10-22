/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <syslog.h>

#ಅನ್ತರ್ಭಾವ "forward.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ Log_ಗುರಿ{
        LOG_ಗುರಿ_CONSOLE,
        LOG_ಗುರಿ_KMSG,
        LOG_ಗುರಿ_JOURNAL,
        LOG_ಗುರಿ_SYSLOG,
        LOG_ಗುರಿ_CONSOLE_PREFIXED,
        LOG_ಗುರಿ_JOURNAL_OR_KMSG,
        LOG_ಗುರಿ_SYSLOG_OR_KMSG,
        LOG_ಗುರಿ_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_ಗುರಿ_NULL,
        LOG_ಗುರಿ_SINGLE_MAX = LOG_ಗುರಿ_SYSLOG + 1,
        LOG_ಗುರಿ_MAX = LOG_ಗುರಿ_NULL + 1,
        LOG_ಗುರಿ_INVALID = -EINVAL,
} Log_ಗುರಿ;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#ವ್ಯಾಖ್ಯಾನಿಸು SYNTHETIC_ERRNO(ಅಂಕೆ)                (ABS(ಅಂಕೆ) | (1 << 30))
#ವ್ಯಾಖ್ಯಾನಿಸು IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#ವ್ಯಾಖ್ಯಾನಿಸು ERRNO_ಮೌಲ್ಯ(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಶೂನ್ಯ (*log_syntax_callback_t)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit, ಪೂರ್ಣಾಂಕ level, ಶೂನ್ಯ *userdata);
ಶೂನ್ಯ ಗಣ_log_syntax_callback(log_syntax_callback_t cb, ಶೂನ್ಯ *userdata);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ clear_log_syntax_callback(dummy_t *dummy) {
          ಗಣ_log_syntax_callback(/* cb= */ ಖಾಲಿ, /* userdata= */ ಖಾಲಿ);
}

DECLARE_STRING_ಕೋಷ್ಟಕ_LOOKUP(log_ಗುರಿ, Log_ಗುರಿ);
ಶೂನ್ಯ log_ಗಣ_ಗುರಿ(Log_ಗುರಿ ಗುರಿ);
ಶೂನ್ಯ log_ಗಣ_ಗುರಿ_and_open(Log_ಗುರಿ ಗುರಿ);
ಪೂರ್ಣಾಂಕ log_ಗಣ_ಗುರಿ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
Log_ಗುರಿ log_get_ಗುರಿ(ಶೂನ್ಯ) ಶುದ್ಧ;
ಶೂನ್ಯ log_settle_ಗುರಿ(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ log_ಗಣ_max_level(ಪೂರ್ಣಾಂಕ level);
ಪೂರ್ಣಾಂಕ log_ಗಣ_max_level_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
ಪೂರ್ಣಾಂಕ log_get_max_level(ಶೂನ್ಯ) ಶುದ್ಧ;
ಪೂರ್ಣಾಂಕ log_get_ಗುರಿ_max_level(Log_ಗುರಿ ಗುರಿ);
ಪೂರ್ಣಾಂಕ log_max_ಮಟ್ಟಗಳು_ಗಮ್ಯ_string(ಪೂರ್ಣಾಂಕ level, ಅಕ್ಷರ **ret);

ಶೂನ್ಯ log_ಗಣ_facility(ಪೂರ್ಣಾಂಕ facility);

ಶೂನ್ಯ log_show_color(bool b);
bool log_get_show_color(ಶೂನ್ಯ) ಶುದ್ಧ;
ಶೂನ್ಯ log_show_location(bool b);
bool log_get_show_location(ಶೂನ್ಯ) ಶುದ್ಧ;
ಶೂನ್ಯ log_show_ಸಮಯ(bool b);
bool log_get_show_ಸಮಯ(ಶೂನ್ಯ) ಶುದ್ಧ;
ಶೂನ್ಯ log_show_tid(bool b);
bool log_get_show_tid(ಶೂನ್ಯ) ಶುದ್ಧ;
ಶೂನ್ಯ log_show_executable(bool b);
bool log_get_show_executable(ಶೂನ್ಯ) ಶುದ್ಧ;

ಪೂರ್ಣಾಂಕ log_show_color_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
ಪೂರ್ಣಾಂಕ log_show_location_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
ಪೂರ್ಣಾಂಕ log_show_ಸಮಯ_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
ಪೂರ್ಣಾಂಕ log_show_tid_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);
ಪೂರ್ಣಾಂಕ log_show_executable_from_string(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(ಶೂನ್ಯ);
ಪೂರ್ಣಾಂಕ log_open(ಶೂನ್ಯ);
ಶೂನ್ಯ log_close(ಶೂನ್ಯ);
ಶೂನ್ಯ log_forget_fds(ಶೂನ್ಯ);

ಶೂನ್ಯ log_parse_environment_variables(ಶೂನ್ಯ);
ಶೂನ್ಯ log_parse_environment(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ log_dispatch_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಅಕ್ಷರ *ಬಫರ್);

ಪೂರ್ಣಾಂಕ log_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(6,7);

ಪೂರ್ಣಾಂಕ log_internalv(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) _printf_(6,0);

ಪೂರ್ಣಾಂಕ log_object_internalv(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) _printf_(10,0);

ಪೂರ್ಣಾಂಕ log_object_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *object,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra_ಕ್ಷೇತ್ರ,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *extra,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(10,11);

ಪೂರ್ಣಾಂಕ log_struct_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _sentinel_;

ಪೂರ್ಣಾಂಕ log_oom_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func);

ಪೂರ್ಣಾಂಕ log_format_iovec(
                ರಚನೆ iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format,
                va_list ap) _printf_(6, 0);

ಪೂರ್ಣಾಂಕ log_struct_iovec_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ರಚನೆ iovec *ಒಳಹರಿವು_iovec,
                size_t n_ಒಳಹರಿವು_iovec);

/* This modifies the buffer passed! */
ಪೂರ್ಣಾಂಕ log_dump_ಆಂತರಿಕ(
                ಪೂರ್ಣಾಂಕ level,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಅಕ್ಷರ *ಬಫರ್);

#ವ್ಯಾಖ್ಯಾನಿಸು log_dispatch(level, error, ಬಫರ್)                              \
        log_dispatch_ಆಂತರಿಕ(level, error, PROJECT_FILE, __LINE__, __func__, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಬಫರ್)

/* Logging with level */
#ವ್ಯಾಖ್ಯಾನಿಸು log_ತುಂಬಿದ_errno_zerook(level, error, ...)                        \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ಆಂತರಿಕ(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ಮೌಲ್ಯ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ಆದರೆ BUILD_ಬಹುಲಕ_DEVELOPER && !defined(TEST_ಸಂಕೇತ)
#  define ASSERT_NON_ZERO(x) ಪ್ರತಿಜ್ಞಾ((x) != 0)
#  define ASSERT_UNDERFLOW(x) ಪ್ರತಿಜ್ಞಾ((x) >= INT_MIN)
#ಇಲ್ಲದಿದ್ದರೆ
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#ಆದರೆ_ಅಂತ್ಯ

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#ವ್ಯಾಖ್ಯಾನಿಸು log_ತುಂಬಿದ_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_ತುಂಬಿದ_errno_zerook(level, (ಪೂರ್ಣಾಂಕ)_error, __VA_ARGS__); \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು log_ತುಂಬಿದ(level, fmt, ...)                                      \
        ({                                                             \
                ಆದರೆ (BUILD_ಬಹುಲಕ_DEVELOPER)                              \
                        ಪ್ರತಿಜ್ಞಾ(!ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(fmt, "%m"));                    \
                (ಶೂನ್ಯ) log_ತುಂಬಿದ_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

ಪೂರ್ಣಾಂಕ log_emergency_level(ಶೂನ್ಯ);

/* Normal logging */
#ವ್ಯಾಖ್ಯಾನಿಸು log_debug(...)     log_ತುಂಬಿದ(LOG_DEBUG,   __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_info(...)      log_ತುಂಬಿದ(LOG_INFO,    __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_notice(...)    log_ತುಂಬಿದ(LOG_NOTICE,  __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_warning(...)   log_ತುಂಬಿದ(LOG_WARNING, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_error(...)     log_ತುಂಬಿದ(LOG_ERR,     __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_emergency(...) log_ತುಂಬಿದ(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#ವ್ಯಾಖ್ಯಾನಿಸು log_debug_errno(error, ...)     log_ತುಂಬಿದ_errno(LOG_DEBUG,   error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_info_errno(error, ...)      log_ತುಂಬಿದ_errno(LOG_INFO,    error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_notice_errno(error, ...)    log_ತುಂಬಿದ_errno(LOG_NOTICE,  error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_warning_errno(error, ...)   log_ತುಂಬಿದ_errno(LOG_WARNING, error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_error_errno(error, ...)     log_ತುಂಬಿದ_errno(LOG_ERR,     error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_emergency_errno(error, ...) log_ತುಂಬಿದ_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#ವ್ಯಾಖ್ಯಾನಿಸು log_once(level, ...)                                             \
        ({                                                               \
                ಆದರೆ (ONCE)                                                \
                        log_ತುಂಬಿದ(level, __VA_ARGS__);                    \
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು log_once_errno(level, error, ...)                                \
        ({                                                               \
                ಪೂರ್ಣಾಂಕ _err = (error);                                      \
                ಆದರೆ (ONCE)                                                \
                        _err = log_ತುಂಬಿದ_errno(level, _err, __VA_ARGS__); \
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                ಇಲ್ಲದಿದ್ದರೆ                                                     \
                        _err = -ERRNO_ಮೌಲ್ಯ(_err);                       \
                _err;                                                    \
        })

#ಆದರೆ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#ಇಲ್ಲದಿದ್ದರೆ
#  define log_trace(...)          ಮಾಡು {} ಆಗ (0)
#  define log_trace_errno(e, ...) (-ERRNO_ಮೌಲ್ಯ(e))
#ಆದರೆ_ಅಂತ್ಯ

/* Structured logging */
#ವ್ಯಾಖ್ಯಾನಿಸು log_struct_errno(level, error, ...)                             \
        log_struct_ಆಂತರಿಕ(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ಖಾಲಿ)
#ವ್ಯಾಖ್ಯಾನಿಸು log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#ವ್ಯಾಖ್ಯಾನಿಸು log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_ಆಂತರಿಕ(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#ವ್ಯಾಖ್ಯಾನಿಸು log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#ವ್ಯಾಖ್ಯಾನಿಸು log_struct_once(level, ...)                                     \
        ({                                                              \
                ಆದರೆ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#ವ್ಯಾಖ್ಯಾನಿಸು log_dump(level, ಬಫರ್)                                         \
        log_dump_ಆಂತರಿಕ(level, 0, PROJECT_FILE, __LINE__, __func__, ಬಫರ್)

#ವ್ಯಾಖ್ಯಾನಿಸು log_oom_ತುಂಬಿದ(level) log_oom_ಆಂತರಿಕ(level, PROJECT_FILE, __LINE__, __func__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_oom()           log_oom_ತುಂಬಿದ(LOG_ERR)
#ವ್ಯಾಖ್ಯಾನಿಸು log_oom_debug()     log_oom_ತುಂಬಿದ(LOG_DEBUG)
#ವ್ಯಾಖ್ಯಾನಿಸು log_oom_warning()   log_oom_ತುಂಬಿದ(LOG_WARNING)

bool log_on_console(ಶೂನ್ಯ) ಶುದ್ಧ;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ಅಂಶ(fmt, ...)    "dummy", ಖಾಲಿ, ##__VA_ARGS__
#ಅಥವಾಆದರೆ LOG_ಸಂದೇಶ_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
ಬಾಹ್ಯ bool log_ಸಂದೇಶ_dummy;
#  define LOG_ಅಂಶ(fmt, ...) "%.0d" fmt, (log_ಸಂದೇಶ_dummy && ಬರೆ(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#ಇಲ್ಲದಿದ್ದರೆ
#  define LOG_ಅಂಶ(fmt, ...) fmt, ##__VA_ARGS__
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಸಂದೇಶ(fmt, ...) LOG_ಅಂಶ("MESSAGE=" fmt, ##__VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಸಂದೇಶ_ID(id)    LOG_ಅಂಶ("MESSAGE_ID=" id)

ಶೂನ್ಯ log_received_signal(ಪೂರ್ಣಾಂಕ level, ಸ್ಥಿರಾಂಕ ರಚನೆ signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
ಶೂನ್ಯ log_ಗಣ_upgrade_syslog_ಗಮ್ಯ_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
ಶೂನ್ಯ log_ಗಣ_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
ಶೂನ್ಯ log_ಗಣ_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
ಶೂನ್ಯ log_ಗಣ_prohibit_ipc(bool b);

ಪೂರ್ಣಾಂಕ log_dup_console(ಶೂನ್ಯ);

ಪೂರ್ಣಾಂಕ log_syntax_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಪೂರ್ಣಾಂಕ level,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಪೂರ್ಣಾಂಕ error,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, ...) _printf_(9, 10);

ಪೂರ್ಣಾಂಕ log_syntax_invalid_utf8_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಪೂರ್ಣಾಂಕ level,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue);

ಪೂರ್ಣಾಂಕ log_syntax_parse_error_ಆಂತರಿಕ(
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *unit,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *config_file,
                ಚಿಹ್ನೆರಹಿತ config_ಸಾಲು,
                ಪೂರ್ಣಾಂಕ error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file,
                ಪೂರ್ಣಾಂಕ ಸಾಲು,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *func,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lvalue,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *rvalue);

#ವ್ಯಾಖ್ಯಾನಿಸು log_syntax(unit, level, config_file, config_ಸಾಲು, error, ...)   \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_ಆಂತರಿಕ(unit, _level, config_file, config_ಸಾಲು, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_ಮೌಲ್ಯ(_e);                             \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು log_syntax_invalid_utf8(unit, level, config_file, config_ಸಾಲು, rvalue) \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_ಆಂತರಿಕ(unit, _level, config_file, config_ಸಾಲು, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು log_syntax_parse_error_ತುಂಬಿದ(unit, config_file, config_ಸಾಲು, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_ಆಂತರಿಕ(unit, config_file, config_ಸಾಲು, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#ವ್ಯಾಖ್ಯಾನಿಸು log_syntax_parse_error(unit, config_file, config_ಸಾಲು, error, lvalue, rvalue) \
        log_syntax_parse_error_ತುಂಬಿದ(unit, config_file, config_ಸಾಲು, error, /* critical= */ false, lvalue, rvalue)

#ವ್ಯಾಖ್ಯಾನಿಸು DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

ಶೂನ್ಯ log_ಸಿದ್ಧತೆ(ಶೂನ್ಯ);

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* log_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪೂರ್ವಪ್ರತ್ಯಯ, bool force);

ಶೂನ್ಯ log_ಪೂರ್ವಪ್ರತ್ಯಯ_ಅದಲುಬದಲು(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ಪೂರ್ವಪ್ರತ್ಯಯ);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* _log_unset_prefixp(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **p) {
        ಪ್ರತಿಜ್ಞಾ(p);
        log_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(*p, true);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಪೂರ್ವಪ್ರತ್ಯಯ) \
        _cleanup_(_log_unset_prefixp) ಬಳಸದ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *CONCATENATE(cleanup_log_unset_ಪೂರ್ವಪ್ರತ್ಯಯ, UNIQ) = log_ಗಣ_ಪೂರ್ವಪ್ರತ್ಯಯ(ಪೂರ್ವಪ್ರತ್ಯಯ, false);
