/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <syslog.h>

#समावेश "forward.h"

प्रकार क्रमागत LogTarget{
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
#घोषणा LOG_NULL (LOG_EMERG - 1)
assert_cc(LOG_NULL == -1);

#घोषणा SYNTHETIC_ERRNO(अंक)                (ABS(अंक) | (1 << 30))
#घोषणा IS_SYNTHETIC_ERRNO(val)             (((val) >> 30) == 1)
#घोषणा ERRNO_मूल्य(val)                    (ABS(val) & ~(1 << 30))

/* The callback function to be invoked when syntax warnings are seen
 * in the unit files. */
प्रकार व्योम (*log_syntax_callback_t)(स्थिर अक्षर *unit, पूर्णांक level, व्योम *userdata);
व्योम set_log_syntax_callback(log_syntax_callback_t cb, व्योम *userdata);

अटल अंतरभूत व्योम clear_log_syntax_callback(dummy_t *dummy) {
          set_log_syntax_callback(/* cb= */ शून्य, /* userdata= */ शून्य);
}

DECLARE_STRING_TABLE_LOOKUP(log_target, LogTarget);
व्योम log_set_target(LogTarget target);
व्योम log_set_target_and_open(LogTarget target);
पूर्णांक log_set_target_from_string(स्थिर अक्षर *e);
LogTarget log_get_target(व्योम) _pure_;
व्योम log_settle_target(व्योम);

पूर्णांक log_set_max_level(पूर्णांक level);
पूर्णांक log_set_max_level_from_string(स्थिर अक्षर *e);
पूर्णांक log_get_max_level(व्योम) _pure_;
पूर्णांक log_get_target_max_level(LogTarget target);
पूर्णांक log_max_levels_to_string(पूर्णांक level, अक्षर **ret);

व्योम log_set_facility(पूर्णांक facility);

व्योम log_show_color(bool b);
bool log_get_show_color(व्योम) _pure_;
व्योम log_show_location(bool b);
bool log_get_show_location(व्योम) _pure_;
व्योम log_show_time(bool b);
bool log_get_show_time(व्योम) _pure_;
व्योम log_show_tid(bool b);
bool log_get_show_tid(व्योम) _pure_;
व्योम log_show_executable(bool b);
bool log_get_show_executable(व्योम) _pure_;

पूर्णांक log_show_color_from_string(स्थिर अक्षर *e);
पूर्णांक log_show_location_from_string(स्थिर अक्षर *e);
पूर्णांक log_show_time_from_string(स्थिर अक्षर *e);
पूर्णांक log_show_tid_from_string(स्थिर अक्षर *e);
पूर्णांक log_show_executable_from_string(स्थिर अक्षर *e);

/* Functions below that open and close logs or configure logging based on the
 * environment should not be called from library code — this is always a job
 * for the application itself. */

bool stderr_is_journal(व्योम);
पूर्णांक log_open(व्योम);
व्योम log_close(व्योम);
व्योम log_forget_fds(व्योम);

व्योम log_parse_environment_variables(व्योम);
व्योम log_parse_environment(व्योम);

पूर्णांक log_dispatch_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                अक्षर *बफर);

पूर्णांक log_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) _printf_(6,7);

पूर्णांक log_internalv(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format,
                बहु_सूची ap) _printf_(6,0);

पूर्णांक log_object_internalv(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                स्थिर अक्षर *format,
                बहु_सूची ap) _printf_(10,0);

पूर्णांक log_object_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *object_field,
                स्थिर अक्षर *object,
                स्थिर अक्षर *extra_field,
                स्थिर अक्षर *extra,
                स्थिर अक्षर *format, ...) _printf_(10,11);

पूर्णांक log_struct_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) _sentinel_;

पूर्णांक log_oom_internal(
                पूर्णांक level,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func);

पूर्णांक log_format_iovec(
                काष्ठा iovec *iovec,
                माप_प्रकार iovec_len,
                माप_प्रकार *n,
                bool newline_separator,
                पूर्णांक error,
                स्थिर अक्षर *format,
                बहु_सूची ap) _printf_(6, 0);

पूर्णांक log_struct_iovec_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर काष्ठा iovec *निवेश_iovec,
                माप_प्रकार n_निवेश_iovec);

/* This modifies the buffer passed! */
पूर्णांक log_dump_internal(
                पूर्णांक level,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                अक्षर *बफर);

#घोषणा log_dispatch(level, error, बफर)                              \
        log_dispatch_internal(level, error, PROJECT_FILE, __LINE__, __func__, शून्य, शून्य, शून्य, शून्य, बफर)

/* Logging with level */
#घोषणा log_full_errno_zerook(level, error, ...)                        \
        ({                                                              \
                पूर्णांक _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_internal(_level, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मूल्य(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#अगर BUILD_MODE_DEVELOPER && !defined(TEST_CODE)
#  define ASSERT_NON_ZERO(x) निश्चित((x) != 0)
#  define ASSERT_UNDERFLOW(x) निश्चित((x) >= पूर्णांक_न्यून)
#अन्यथा
#  define ASSERT_NON_ZERO(x)
#  define ASSERT_UNDERFLOW(x)
#पूर्ण_अगर

/* We often call log macros with ssize_t instead of int, so check for underflows,
 * as ssize_t is not guaranteed to be the same as int, and we usually do
 * 'return log_errno...' from functions that return 'int' */
#घोषणा log_full_errno(level, error, ...)                               \
        ({                                                              \
                int64_t _error = (error);                               \
                ASSERT_UNDERFLOW(_error);                               \
                ASSERT_NON_ZERO(_error);                                \
                log_full_errno_zerook(level, (पूर्णांक)_error, __VA_ARGS__); \
        })

#घोषणा log_full(level, fmt, ...)                                      \
        ({                                                             \
                अगर (BUILD_MODE_DEVELOPER)                              \
                        निश्चित(!म_माला(fmt, "%m"));                    \
                (व्योम) log_full_errno_zerook(level, 0, fmt, ##__VA_ARGS__); \
        })

पूर्णांक log_emergency_level(व्योम);

/* Normal logging */
#घोषणा log_debug(...)     log_full(LOG_DEBUG,   __VA_ARGS__)
#घोषणा log_info(...)      log_full(LOG_INFO,    __VA_ARGS__)
#घोषणा log_notice(...)    log_full(LOG_NOTICE,  __VA_ARGS__)
#घोषणा log_warning(...)   log_full(LOG_WARNING, __VA_ARGS__)
#घोषणा log_error(...)     log_full(LOG_ERR,     __VA_ARGS__)
#घोषणा log_emergency(...) log_full(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#घोषणा log_debug_errno(error, ...)     log_full_errno(LOG_DEBUG,   error, __VA_ARGS__)
#घोषणा log_info_errno(error, ...)      log_full_errno(LOG_INFO,    error, __VA_ARGS__)
#घोषणा log_notice_errno(error, ...)    log_full_errno(LOG_NOTICE,  error, __VA_ARGS__)
#घोषणा log_warning_errno(error, ...)   log_full_errno(LOG_WARNING, error, __VA_ARGS__)
#घोषणा log_error_errno(error, ...)     log_full_errno(LOG_ERR,     error, __VA_ARGS__)
#घोषणा log_emergency_errno(error, ...) log_full_errno(log_emergency_level(), error, __VA_ARGS__)

/* This logs at the specified level the first time it is called, and then
 * logs at debug. If the specified level is debug, this logs only the first
 * time it is called. */
#घोषणा log_once(level, ...)                                             \
        ({                                                               \
                अगर (ONCE)                                                \
                        log_full(level, __VA_ARGS__);                    \
                अन्यथा अगर (LOG_PRI(level) != LOG_DEBUG)                    \
                        log_debug(__VA_ARGS__);                          \
        })

#घोषणा log_once_errno(level, error, ...)                                \
        ({                                                               \
                पूर्णांक _err = (error);                                      \
                अगर (ONCE)                                                \
                        _err = log_full_errno(level, _err, __VA_ARGS__); \
                अन्यथा अगर (LOG_PRI(level) != LOG_DEBUG)                    \
                        _err = log_debug_errno(_err, __VA_ARGS__);       \
                अन्यथा                                                     \
                        _err = -ERRNO_मूल्य(_err);                       \
                _err;                                                    \
        })

#अगर LOG_TRACE
#  define log_trace(...)          log_debug(__VA_ARGS__)
#  define log_trace_errno(...)    log_debug_errno(__VA_ARGS__)
#अन्यथा
#  define log_trace(...)          करो {} जबतक (0)
#  define log_trace_errno(e, ...) (-ERRNO_मूल्य(e))
#पूर्ण_अगर

/* Structured logging */
#घोषणा log_struct_errno(level, error, ...)                             \
        log_struct_internal(level, error, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__, शून्य)
#घोषणा log_struct(level, ...) log_struct_errno(level, 0, __VA_ARGS__)

#घोषणा log_struct_iovec_errno(level, error, iovec, n_iovec)            \
        log_struct_iovec_internal(level, error, PROJECT_FILE, __LINE__, __func__, iovec, n_iovec)
#घोषणा log_struct_iovec(level, iovec, n_iovec) log_struct_iovec_errno(level, 0, iovec, n_iovec)

/* Like log_struct(), but with log_once() semantics */
#घोषणा log_struct_once(level, ...)                                     \
        ({                                                              \
                अगर (ONCE)                                               \
                        log_struct(level, __VA_ARGS__);                 \
                अन्यथा अगर (LOG_PRI(level) != LOG_DEBUG)                   \
                        log_struct(LOG_DEBUG, __VA_ARGS__);             \
        })

/* This modifies the buffer passed! */
#घोषणा log_dump(level, बफर)                                         \
        log_dump_internal(level, 0, PROJECT_FILE, __LINE__, __func__, बफर)

#घोषणा log_oom_full(level) log_oom_internal(level, PROJECT_FILE, __LINE__, __func__)
#घोषणा log_oom()           log_oom_full(LOG_ERR)
#घोषणा log_oom_debug()     log_oom_full(LOG_DEBUG)
#घोषणा log_oom_warning()   log_oom_full(LOG_WARNING)

bool log_on_console(व्योम) _pure_;

/* Helper to wrap the main message in structured logging. The macro doesn't do much,
 * except to provide visual grouping of the format string and its arguments. */
#अगर_घोषित __COVERITY__
/* Coverity does not like the concatenation of multiple formats and arguments. Let's replace each format
 * string with a dummy string. The validity of the formats is hopefully checked by other CIs. */
#  define LOG_ITEM(fmt, ...)    "dummy", शून्य, ##__VA_ARGS__
#या_अगर LOG_MESSAGE_VERIFICATION
/* Do a fake formatting of the message string to let the scanner verify the arguments against the format
 * message. The variable will never be set to true, but we don't tell the compiler that :) */
बाह्य bool _log_message_dummy;
#  define LOG_ITEM(fmt, ...) "%.0d" fmt, (_log_message_dummy && म_लिखो(fmt, ##__VA_ARGS__)), ##__VA_ARGS__
#अन्यथा
#  define LOG_ITEM(fmt, ...) fmt, ##__VA_ARGS__
#पूर्ण_अगर

#घोषणा LOG_MESSAGE(fmt, ...) LOG_ITEM("MESSAGE=" fmt, ##__VA_ARGS__)
#घोषणा LOG_MESSAGE_ID(id)    LOG_ITEM("MESSAGE_ID=" id)

व्योम log_received_signal(पूर्णांक level, स्थिर काष्ठा signalfd_siginfo *si);

/* If turned on, any requests for a log target involving "syslog" will be implicitly upgraded to the equivalent journal target */
व्योम log_set_upgrade_syslog_to_journal(bool b);

/* If turned on, and log_open() is called, we'll not use STDERR_FILENO for logging ever, but rather open /dev/console */
व्योम log_set_always_reopen_console(bool b);

/* If turned on, we'll open the log stream implicitly if needed on each individual log call. This is normally not
 * desired as we want to reuse our logging streams. It is useful however  */
व्योम log_set_open_when_needed(bool b);

/* If turned on, then we'll never use IPC-based logging, i.e. never log to syslog or the journal. We'll only log to
 * stderr, the console or kmsg */
व्योम log_set_prohibit_ipc(bool b);

पूर्णांक log_dup_console(व्योम);

पूर्णांक log_syntax_internal(
                स्थिर अक्षर *unit,
                पूर्णांक level,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                पूर्णांक error,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *format, ...) _printf_(9, 10);

पूर्णांक log_syntax_invalid_utf8_internal(
                स्थिर अक्षर *unit,
                पूर्णांक level,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *rvalue);

पूर्णांक log_syntax_parse_error_internal(
                स्थिर अक्षर *unit,
                स्थिर अक्षर *config_file,
                अचिन्हित config_पंक्ति,
                पूर्णांक error,
                bool critical, /* When true, propagate the passed error, otherwise this always returns 0. */
                स्थिर अक्षर *file,
                पूर्णांक पंक्ति,
                स्थिर अक्षर *func,
                स्थिर अक्षर *lvalue,
                स्थिर अक्षर *rvalue);

#घोषणा log_syntax(unit, level, config_file, config_पंक्ति, error, ...)   \
        ({                                                              \
                पूर्णांक _level = (level), _e = (error);                     \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_internal(unit, _level, config_file, config_पंक्ति, _e, PROJECT_FILE, __LINE__, __func__, __VA_ARGS__) \
                        : -ERRNO_मूल्य(_e);                             \
        })

#घोषणा log_syntax_invalid_utf8(unit, level, config_file, config_पंक्ति, rvalue) \
        ({                                                              \
                पूर्णांक _level = (level);                                   \
                (log_get_max_level() >= LOG_PRI(_level))                \
                        ? log_syntax_invalid_utf8_internal(unit, _level, config_file, config_पंक्ति, PROJECT_FILE, __LINE__, __func__, rvalue) \
                        : -EINVAL;                                      \
        })

#घोषणा log_syntax_parse_error_full(unit, config_file, config_पंक्ति, error, critical, lvalue, rvalue) \
        log_syntax_parse_error_internal(unit, config_file, config_पंक्ति, error, critical, PROJECT_FILE, __LINE__, __func__, lvalue, rvalue)

#घोषणा log_syntax_parse_error(unit, config_file, config_पंक्ति, error, lvalue, rvalue) \
        log_syntax_parse_error_full(unit, config_file, config_पंक्ति, error, /* critical= */ false, lvalue, rvalue)

#घोषणा DEBUG_LOGGING _unlikely_(log_get_max_level() >= LOG_DEBUG)

व्योम log_setup(व्योम);

स्थिर अक्षर* _log_set_prefix(स्थिर अक्षर *prefix, bool force);

व्योम log_prefix_swap(स्थिर अक्षर **prefix);
अटल अंतरभूत स्थिर अक्षर* _log_unset_prefixp(स्थिर अक्षर **p) {
        निश्चित(p);
        _log_set_prefix(*p, true);
        वापस शून्य;
}

#घोषणा LOG_SET_PREFIX(prefix) \
        _cleanup_(_log_unset_prefixp) _unused_ स्थिर अक्षर *CONCATENATE(_cleanup_log_unset_prefix_, UNIQ) = _log_set_prefix(prefix, false);
