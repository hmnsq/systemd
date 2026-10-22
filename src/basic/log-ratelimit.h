/* SPDX-License-Identifier: LGPL-2.1-or-later */
#هدايت once

#ضمن "log.h"
#ضمن "ratelimit.h"

تعريفنوع بنية Log_شرح_حد_بندي {
        صحيح error;
        صحيح level;
        شرح_حد_بندي ratelimit;
} Log_شرح_حد_بندي;

#وصف_ڪريو log_ratelimit_اندروني(_level, _error, _ratelimit, _file, سٽ_2, _func, _format, ...)        \
({                                                                              \
        صحيح _log_ratelimit_error = (_error);                                    \
        صحيح _log_ratelimit_level = (_level);                                    \
        ساكن Log_شرح_حد_بندي _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        بنانشان انگ_dropped_errors = ratelimit_انگ_dropped(&_log_ratelimit.ratelimit); \
        إذا (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        إذا (log_get_max_level() == LOG_DEBUG || ratelimit_هيٺ(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = انگ_dropped_errors > 0                  \
                ? log_اندروني(_log_ratelimit_level, _log_ratelimit_error, _file, سٽ_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, انگ_dropped_errors) \
                : log_اندروني(_log_ratelimit_level, _log_ratelimit_error, _file, سٽ_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#وصف_ڪريو log_ratelimit_ڀريل_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                صحيح _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_اندروني(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_قدر(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#وصف_ڪريو log_ratelimit_ڀريل(level, _ratelimit, format, ...)                          \
        log_ratelimit_ڀريل_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#وصف_ڪريو log_ratelimit_info(...)      log_ratelimit_ڀريل(LOG_INFO,    __VA_ARGS__)
#وصف_ڪريو log_ratelimit_notice(...)    log_ratelimit_ڀريل(LOG_NOTICE,  __VA_ARGS__)
#وصف_ڪريو log_ratelimit_warning(...)   log_ratelimit_ڀريل(LOG_WARNING, __VA_ARGS__)
#وصف_ڪريو log_ratelimit_error(...)     log_ratelimit_ڀريل(LOG_ERR,     __VA_ARGS__)
#وصف_ڪريو log_ratelimit_emergency(...) log_ratelimit_ڀريل(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#وصف_ڪريو log_ratelimit_info_errno(error, ...)      log_ratelimit_ڀريل_errno(LOG_INFO,    error, __VA_ARGS__)
#وصف_ڪريو log_ratelimit_notice_errno(error, ...)    log_ratelimit_ڀريل_errno(LOG_NOTICE,  error, __VA_ARGS__)
#وصف_ڪريو log_ratelimit_warning_errno(error, ...)   log_ratelimit_ڀريل_errno(LOG_WARNING, error, __VA_ARGS__)
#وصف_ڪريو log_ratelimit_error_errno(error, ...)     log_ratelimit_ڀريل_errno(LOG_ERR,     error, __VA_ARGS__)
#وصف_ڪريو log_ratelimit_emergency_errno(error, ...) log_ratelimit_ڀريل_errno(log_emergency_level(), error, __VA_ARGS__)
