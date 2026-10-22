/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "log.h"
#ضمن "ratelimit.h"

تعريفنوع بنية Log_شرح_حد_بندی {
        صحيح error;
        صحيح level;
        شرح_حد_بندی ratelimit;
} Log_شرح_حد_بندی;

#تعریف_کرو log_ratelimit_اندرونی(_level, _error, _ratelimit, _file, سطر_2, _func, _format, ...)        \
({                                                                              \
        صحيح _log_ratelimit_error = (_error);                                    \
        صحيح _log_ratelimit_level = (_level);                                    \
        ساكن Log_شرح_حد_بندی _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        بنانشانی عدد_dropped_errors = ratelimit_عدد_dropped(&_log_ratelimit.ratelimit); \
        إذا (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        إذا (log_get_max_level() == LOG_DEBUG || ratelimit_ہیٹھاں(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = عدد_dropped_errors > 0                  \
                ? log_اندرونی(_log_ratelimit_level, _log_ratelimit_error, _file, سطر_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, عدد_dropped_errors) \
                : log_اندرونی(_log_ratelimit_level, _log_ratelimit_error, _file, سطر_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#تعریف_کرو log_ratelimit_بھریا_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_اندرونی(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#تعریف_کرو log_ratelimit_بھریا(level, _ratelimit, format, ...)                          \
        log_ratelimit_بھریا_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#تعریف_کرو log_ratelimit_info(...)      log_ratelimit_بھریا(LOG_INFO,    __VA_ARGS__)
#تعریف_کرو log_ratelimit_notice(...)    log_ratelimit_بھریا(LOG_NOTICE,  __VA_ARGS__)
#تعریف_کرو log_ratelimit_warning(...)   log_ratelimit_بھریا(LOG_WARNING, __VA_ARGS__)
#تعریف_کرو log_ratelimit_error(...)     log_ratelimit_بھریا(LOG_ERR,     __VA_ARGS__)
#تعریف_کرو log_ratelimit_emergency(...) log_ratelimit_بھریا(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#تعریف_کرو log_ratelimit_info_errno(error, ...)      log_ratelimit_بھریا_errno(LOG_INFO,    error, __VA_ARGS__)
#تعریف_کرو log_ratelimit_notice_errno(error, ...)    log_ratelimit_بھریا_errno(LOG_NOTICE,  error, __VA_ARGS__)
#تعریف_کرو log_ratelimit_warning_errno(error, ...)   log_ratelimit_بھریا_errno(LOG_WARNING, error, __VA_ARGS__)
#تعریف_کرو log_ratelimit_error_errno(error, ...)     log_ratelimit_بھریا_errno(LOG_ERR,     error, __VA_ARGS__)
#تعریف_کرو log_ratelimit_emergency_errno(error, ...) log_ratelimit_بھریا_errno(log_emergency_level(), error, __VA_ARGS__)
