/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <syslog.h>

#అన్తర్భావ "forward.h"

రకనిర్వచనం గణన Log_లక్ష్యం{
        LOG_లక్ష్యం_CONSOLE,
        LOG_లక్ష్యం_KMSG,
        LOG_లక్ష్యం_JOURNAL,
        LOG_లక్ష్యం_SYSLOG,
        LOG_లక్ష్యం_CONSOLE_PREFIXED,
        LOG_లక్ష్యం_JOURNAL_OR_KMSG,
        LOG_లక్ష్యం_SYSLOG_OR_KMSG,
        LOG_లక్ష్యం_AUTO, /* console if stderr is not journal, JOURNAL_OR_KMSG otherwise */
        LOG_లక్ష్యం_NULL,
        LOG_లక్ష్యం_SINGLE_MAX = LOG_లక్ష్యం_SYSLOG + 1,
        LOG_లక్ష్యం_MAX = LOG_లక్ష్యం_NULL + 1,
        LOG_లక్ష్యం_INVALID = -EINVAL,
} Log_లక్ష్యం;

/* This log level disables logging completely. It can only be passed to log_set_max_level() and cannot be
 * used as a regular log level. */
#నిర్వచించు LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#నిర్వచించు SYNTHETIC_ERRNO(అంకె)                (ABS(అంకె) | (1 << 30))
#నిర్వచించు IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#నిర్వచించు ERRNO_విలువ(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
రకనిర్వచనం శూన్య (*log_syntax_callback_t)(స్థిరరాశి అక్షరం *unit, పూర్ణాంకం level, శూన్య *userdata);
శూన్య సమితి_log_syntax_callback(log_syntax_callback_t cb, శూన్య *userdata);

స్థిర అంతఃవరుస శూన్య clear_log_syntax_callback(dummy_t *dummy) {
          సమితి_log_syntax_callback(/* cb= */ ఖాళీ, /* userdata= */ ఖాళీ);
}

DECLARE_STRING_పట్టిక_LOOKUP(log_లక్ష్యం, Log_లక్ష్యం);
శూన్య log_సమితి_లక్ష్యం(Log_లక్ష్యం లక్ష్యం);
శూన్య log_సమితి_లక్ష్యం_and_open(Log_లక్ష్యం లక్ష్యం);
పూర్ణాంకం log_సమితి_లక్ష్యం_from_string(స్థిరరాశి అక్షరం *e);
Log_లక్ష్యం log_get_లక్ష్యం(శూన్య) శుద్ధ;
శూన్య log_settle_లక్ష్యం(శూన్య);

పూర్ణాంకం log_సమితి_max_level(పూర్ణాంకం level);
పూర్ణాంకం log_సమితి_max_level_from_string(స్థిరరాశి అక్షరం *e);
పూర్ణాంకం log_get_max_level(శూన్య) శుద్ధ;
పూర్ణాంకం log_get_లక్ష్యం_max_level(Log_లక్ష్యం లక్ష్యం);
పూర్ణాంకం log_max_స్థాయిలు_గమ్యం_string(పూర్ణాంకం level, అక్షరం **ret);

శూన్య log_సమితి_facility(పూర్ణాంకం facility);

శూన్య log_show_color(bool b);
bool log_get_show_color(శూన్య) శుద్ధ;
శూన్య log_show_location(bool b);
bool log_get_show_location(శూన్య) శుద్ధ;
శూన్య log_show_సమయం(bool b);
bool log_get_show_సమయం(శూన్య) శుద్ధ;
శూన్య log_show_tid(bool b);
bool log_get_show_tid(శూన్య) శుద్ధ;
శూన్య log_show_executable(bool b);
bool log_get_show_executable(శూన్య) శుద్ధ;

పూర్ణాంకం log_show_color_from_string(స్థిరరాశి అక్షరం *e);
పూర్ణాంకం log_show_location_from_string(స్థిరరాశి అక్షరం *e);
పూర్ణాంకం log_show_సమయం_from_string(స్థిరరాశి అక్షరం *e);
పూర్ణాంకం log_show_tid_from_string(స్థిరరాశి అక్షరం *e);
పూర్ణాంకం log_show_executable_from_string(స్థిరరాశి అక్షరం *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(శూన్య);
పూర్ణాంకం log_open(శూన్య);
శూన్య log_close(శూన్య);
శూన్య log_forget_fds(శూన్య);

శూన్య log_parse_environment_variables(శూన్య);
శూన్య log_parse_environment(శూన్య);

పూర్ణాంకం log_dispatch_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                అక్షరం *బఫరు);

పూర్ణాంకం log_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) _printf_(6,7);

పూర్ణాంకం log_internalv(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format,
                va_list ap) _printf_(6,0);

పూర్ణాంకం log_object_internalv(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                స్థిరరాశి అక్షరం *format,
                va_list ap) _printf_(10,0);

పూర్ణాంకం log_object_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *object_క్షేత్రం,
                స్థిరరాశి అక్షరం *object,
                స్థిరరాశి అక్షరం *extra_క్షేత్రం,
                స్థిరరాశి అక్షరం *extra,
                స్థిరరాశి అక్షరం *format, ...) _printf_(10,11);

పూర్ణాంకం log_struct_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) _sentinel_;

పూర్ణాంకం log_oom_అంతర్గత(
                పూర్ణాంకం level,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func);

పూర్ణాంకం log_format_iovec(
                నిర్మాణం iovec *iovec,
                size_t iovec_len,
                size_t *n,
                bool newline_separator,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *format,
                va_list ap) _printf_(6, 0);

పూర్ణాంకం log_struct_iovec_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి నిర్మాణం iovec *ఆదానం_iovec,
                size_t n_ఆదానం_iovec);

/* This modifies the buffer passed! */
పూర్ణాంకం log_dump_అంతర్గత(
                పూర్ణాంకం level,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                అక్షరం *బఫరు);

#నిర్వచించు log_dispatch(level, error, బఫరు)                              \
        log_dispatch_అంతర్గత(level, error, PROJECT_FILE, __LINE__, __func__, ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ, బఫరు)

/* Logging with level */
#నిర్వచించు log_నిండిన_errno_zerook(level, error, ...)                        \
        ({                                                              \
                పూర్ణాంకం _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_అంతర్గత(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_విలువ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ఒకవేళ BUILD_బహుళకం_DEVELOPER && !defined(TEST_సంకేతం)
#  define ASSERT_NON_ZERO(x) ప్రతిజ్ఞా((x) != 0)
#  define ASSERT_UNDERFLOW(x) ప్రతిజ్ఞా((x) >= INT_MIN)
#లేకపోతే
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#అయితే_ముగింపు

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#నిర్వచించు log_నిండిన_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_నిండిన_errno_zerook(level, (పూర్ణాంకం)_error, __VA_ARGS__); \
        })

#నిర్వచించు log_నిండిన(level, fmt, ...)                                      \
        ({                                                             \
                ఒకవేళ (BUILD_బహుళకం_DEVELOPER)                              \
                        ప్రతిజ్ఞా(!అక్షరశ్రేణి_శోధన(fmt, "%m"));                    \
                (శూన్య) log_నిండిన_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

పూర్ణాంకం log_emergency_level(శూన్య);

/* Normal logging */
#నిర్వచించు log_debug(...)     log_నిండిన(LOG_DEBUG,   __VA_ARGS__)
#నిర్వచించు log_info(...)      log_నిండిన(LOG_INFO,    __VA_ARGS__)
#నిర్వచించు log_notice(...)    log_నిండిన(LOG_NOTICE,  __VA_ARGS__)
#నిర్వచించు log_warning(...)   log_నిండిన(LOG_WARNING, __VA_ARGS__)
#నిర్వచించు log_error(...)     log_నిండిన(LOG_ERR,     __VA_ARGS__)
#నిర్వచించు log_emergency(...) log_నిండిన(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#నిర్వచించు log_debug_errno(error, ...)     log_నిండిన_errno(LOG_DEBUG,   error, __VA_ARGS__)
#నిర్వచించు log_info_errno(error, ...)      log_నిండిన_errno(LOG_INFO,    error, __VA_ARGS__)
#నిర్వచించు log_notice_errno(error, ...)    log_నిండిన_errno(LOG_NOTICE,  error, __VA_ARGS__)
#నిర్వచించు log_warning_errno(error, ...)   log_నిండిన_errno(LOG_WARNING, error, __VA_ARGS__)
#నిర్వచించు log_error_errno(error, ...)     log_నిండిన_errno(LOG_ERR,     error, __VA_ARGS__)
#నిర్వచించు log_emergency_errno(error, ...) log_నిండిన_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#నిర్వచించు log_once(level, ...)                                             \
        ({                                                               \
                ఒకవేళ (ONCE)                                                \
                        log_నిండిన(level, __VA_ARGS__);                    \
                లేకపోతే ఒకవేళ (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#నిర్వచించు log_once_errno(level, error, ...)                                \
        ({                                                               \
                పూర్ణాంకం _err = (error);                                      \
                ఒకవేళ (ONCE)                                                \
                        _err = log_నిండిన_errno(level, _err, __VA_ARGS__); \
                లేకపోతే ఒకవేళ (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                లేకపోతే                                                     \
                        _err = -ERRNO_విలువ(_err);                       \
                _err;                                                    \
        })

#ఒకవేళ LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#లేకపోతే
#  define log_trace(...)          చేయి {} అయితే (0)
#  define log_trace_errno(e, ...) (-ERRNO_విలువ(e))
#అయితే_ముగింపు

/* Structured logging */
#నిర్వచించు log_struct_errno(level, error, ...)                             \
        log_struct_అంతర్గత(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, ఖాళీ)
#నిర్వచించు log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#నిర్వచించు log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_అంతర్గత(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#నిర్వచించు log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#నిర్వచించు log_struct_once(level, ...)                                     \
        ({                                                              \
                ఒకవేళ (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                లేకపోతే ఒకవేళ (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#నిర్వచించు log_dump(level, బఫరు)                                         \
        log_dump_అంతర్గత(level, 0, PROJECT_FILE, __LINE__, __func__, బఫరు)

#నిర్వచించు log_oom_నిండిన(level) log_oom_అంతర్గత(level, PROJECT_FILE, __LINE__, __func__)
#నిర్వచించు log_oom()           log_oom_నిండిన(LOG_ERR)
#నిర్వచించు log_oom_debug()     log_oom_నిండిన(LOG_DEBUG)
#నిర్వచించు log_oom_warning()   log_oom_నిండిన(LOG_WARNING)

bool log_on_console(శూన్య) శుద్ధ;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#నిర్వచితమైతే __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_అంశం(fmt, ...)    "dummy", ఖాళీ, ##__VA_ARGS__
#లేదావేళ LOG_సందేశం_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
బాహ్య bool log_సందేశం_dummy;
#  define LOG_అంశం(fmt, ...) "%.0d" fmt, (log_సందేశం_dummy && రాయి(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#లేకపోతే
#  define LOG_అంశం(fmt, ...) fmt, ##__VA_ARGS__
#అయితే_ముగింపు

#నిర్వచించు LOG_సందేశం(fmt, ...) LOG_అంశం("MESSAGE=" fmt, ##__VA_ARGS__)
#నిర్వచించు LOG_సందేశం_ID(id)    LOG_అంశం("MESSAGE_ID=" id)

శూన్య log_received_signal(పూర్ణాంకం level, స్థిరరాశి నిర్మాణం signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
శూన్య log_సమితి_upgrade_syslog_గమ్యం_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
శూన్య log_సమితి_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
శూన్య log_సమితి_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
శూన్య log_సమితి_prohibit_ipc(bool b);

పూర్ణాంకం log_dup_console(శూన్య);

పూర్ణాంకం log_syntax_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                పూర్ణాంకం level,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                పూర్ణాంకం error,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *format, ...) _printf_(9, 10);

పూర్ణాంకం log_syntax_invalid_utf8_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                పూర్ణాంకం level,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *rvalue);

పూర్ణాంకం log_syntax_parse_error_అంతర్గత(
                స్థిరరాశి అక్షరం *unit,
                స్థిరరాశి అక్షరం *config_file,
                గుర్తులేని config_పంక్తి,
                పూర్ణాంకం error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                స్థిరరాశి అక్షరం *file,
                పూర్ణాంకం పంక్తి,
                స్థిరరాశి అక్షరం *func,
                స్థిరరాశి అక్షరం *lvalue,
                స్థిరరాశి అక్షరం *rvalue);

#నిర్వచించు log_syntax(unit, level, config_file, config_పంక్తి, error, ...)   \
        ({                                                              \
                పూర్ణాంకం _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_అంతర్గత(unit, _level, config_file, config_పంక్తి, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_విలువ(_e);                             \
        })

#నిర్వచించు log_syntax_invalid_utf8(unit, level, config_file, config_పంక్తి, rvalue) \
        ({                                                              \
                పూర్ణాంకం _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_అంతర్గత(unit, _level, config_file, config_పంక్తి, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#నిర్వచించు log_syntax_parse_error_నిండిన(unit, config_file, config_పంక్తి, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_అంతర్గత(unit, config_file, config_పంక్తి, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#నిర్వచించు log_syntax_parse_error(unit, config_file, config_పంక్తి, error, lvalue, rvalue) \
        log_syntax_parse_error_నిండిన(unit, config_file, config_పంక్తి, error, /* critical= */ false, lvalue, rvalue)

#నిర్వచించు DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

శూన్య log_అమరిక(శూన్య);

స్థిరరాశి అక్షరం* log_సమితి_ఉపసర్గ(స్థిరరాశి అక్షరం *ఉపసర్గ, bool force);

శూన్య log_ఉపసర్గ_మార్పిడి(స్థిరరాశి అక్షరం **ఉపసర్గ);
స్థిర అంతఃవరుస స్థిరరాశి అక్షరం* _log_unset_prefixp(స్థిరరాశి అక్షరం **p) {
        ప్రతిజ్ఞా(p);
        log_సమితి_ఉపసర్గ(*p, true);
        తిరిగి ఖాళీ;
}

#నిర్వచించు LOG_సమితి_ఉపసర్గ(ఉపసర్గ) \
        _cleanup_(_log_unset_prefixp) వాడని స్థిరరాశి అక్షరం *CONCATENATE(cleanup_log_unset_ఉపసర్గ, UNIQ) = log_సమితి_ఉపసర్గ(ఉపసర్గ, false);
