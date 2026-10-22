/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ratelimit.h"

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ Log_ਦਰ_ਹੱਦਬੰਦੀ {
        ਪੂਰਨ error;
        ਪੂਰਨ level;
        ਦਰ_ਹੱਦਬੰਦੀ ratelimit;
} Log_ਦਰ_ਹੱਦਬੰਦੀ;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_ਅੰਦਰੂਨੀ(_level, _error, _ratelimit, _file, ਸਤਰ_2, _func, _format, ...)        \
({                                                                              \
        ਪੂਰਨ _log_ratelimit_error = (_error);                                    \
        ਪੂਰਨ _log_ratelimit_level = (_level);                                    \
        ਸਥਿਰ Log_ਦਰ_ਹੱਦਬੰਦੀ _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੰਕ_dropped_errors = ratelimit_ਅੰਕ_dropped(&_log_ratelimit.ratelimit); \
        ਜੇ (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        ਜੇ (log_get_max_level() == LOG_DEBUG || ratelimit_ਹੇਠਾਂ(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = ਅੰਕ_dropped_errors > 0                  \
                ? log_ਅੰਦਰੂਨੀ(_log_ratelimit_level, _log_ratelimit_error, _file, ਸਤਰ_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, ਅੰਕ_dropped_errors) \
                : log_ਅੰਦਰੂਨੀ(_log_ratelimit_level, _log_ratelimit_error, _file, ਸਤਰ_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_ਭਰਿਆ_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                ਪੂਰਨ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_ਅੰਦਰੂਨੀ(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_ਮੁੱਲ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_ਭਰਿਆ(level, _ratelimit, format, ...)                          \
        log_ratelimit_ਭਰਿਆ_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_info(...)      log_ratelimit_ਭਰਿਆ(LOG_INFO,    __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_notice(...)    log_ratelimit_ਭਰਿਆ(LOG_NOTICE,  __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_warning(...)   log_ratelimit_ਭਰਿਆ(LOG_WARNING, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_error(...)     log_ratelimit_ਭਰਿਆ(LOG_ERR,     __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_emergency(...) log_ratelimit_ਭਰਿਆ(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_info_errno(error, ...)      log_ratelimit_ਭਰਿਆ_errno(LOG_INFO,    error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_notice_errno(error, ...)    log_ratelimit_ਭਰਿਆ_errno(LOG_NOTICE,  error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_warning_errno(error, ...)   log_ratelimit_ਭਰਿਆ_errno(LOG_WARNING, error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_error_errno(error, ...)     log_ratelimit_ਭਰਿਆ_errno(LOG_ERR,     error, __VA_ARGS__)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ log_ratelimit_emergency_errno(error, ...) log_ratelimit_ਭਰਿਆ_errno(log_emergency_level(), error, __VA_ARGS__)
