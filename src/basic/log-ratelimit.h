/* SPDX-License-Identifier: LGPL-2.1-or-later */
#رهنمود once

#ضمن "log.h"
#ضمن "ratelimit.h"

تعريفنوع بنية Log_نرخ_حد {
        صحيح error;
        صحيح level;
        نرخ_حد ratelimit;
} Log_نرخ_حد;

#تعریف_کن log_ratelimit_داخلی(_level, _error, _ratelimit, _file, سطر_2, _func, _format, ...)        \
({                                                                              \
        صحيح _log_ratelimit_error = (_error);                                    \
        صحيح _log_ratelimit_level = (_level);                                    \
        ساكن Log_نرخ_حد _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        بیعلامت رقم_dropped_errors = ratelimit_رقم_dropped(&_log_ratelimit.ratelimit); \
        إذا (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        إذا (log_get_max_level() == LOG_DEBUG || ratelimit_زیر(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = رقم_dropped_errors > 0                  \
                ? log_داخلی(_log_ratelimit_level, _log_ratelimit_error, _file, سطر_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, رقم_dropped_errors) \
                : log_داخلی(_log_ratelimit_level, _log_ratelimit_error, _file, سطر_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#تعریف_کن log_ratelimit_پر_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_داخلی(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_مقدار(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#تعریف_کن log_ratelimit_پر(level, _ratelimit, format, ...)                          \
        log_ratelimit_پر_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#تعریف_کن log_ratelimit_info(...)      log_ratelimit_پر(LOG_INFO,    __VA_ARGS__)
#تعریف_کن log_ratelimit_notice(...)    log_ratelimit_پر(LOG_NOTICE,  __VA_ARGS__)
#تعریف_کن log_ratelimit_warning(...)   log_ratelimit_پر(LOG_WARNING, __VA_ARGS__)
#تعریف_کن log_ratelimit_error(...)     log_ratelimit_پر(LOG_ERR,     __VA_ARGS__)
#تعریف_کن log_ratelimit_emergency(...) log_ratelimit_پر(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#تعریف_کن log_ratelimit_info_errno(error, ...)      log_ratelimit_پر_errno(LOG_INFO,    error, __VA_ARGS__)
#تعریف_کن log_ratelimit_notice_errno(error, ...)    log_ratelimit_پر_errno(LOG_NOTICE,  error, __VA_ARGS__)
#تعریف_کن log_ratelimit_warning_errno(error, ...)   log_ratelimit_پر_errno(LOG_WARNING, error, __VA_ARGS__)
#تعریف_کن log_ratelimit_error_errno(error, ...)     log_ratelimit_پر_errno(LOG_ERR,     error, __VA_ARGS__)
#تعریف_کن log_ratelimit_emergency_errno(error, ...) log_ratelimit_پر_errno(log_emergency_level(), error, __VA_ARGS__)
