/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "log.h"
#அந்தர்பாவ "ratelimit.h"

வகைவரை கட்டமைப்பு LogRateLimit {
        முழு error;
        முழு level;
        RateLimit ratelimit;
} LogRateLimit;

#define log_ratelimit_internal(_level, _error, _ratelimit, _file, வரி_2, _func, _format, ...)        \
({                                                                              \
        முழு _log_ratelimit_error = (_error);                                    \
        முழு _log_ratelimit_level = (_level);                                    \
        நிலையான LogRateLimit _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        unsigned இலக்கம்_dropped_errors = ratelimit_இலக்கம்_dropped(&_log_ratelimit.ratelimit); \
        எனில் (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        எனில் (log_get_max_level() == LOG_DEBUG || ratelimit_below(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = இலக்கம்_dropped_errors > 0                  \
                ? log_internal(_log_ratelimit_level, _log_ratelimit_error, _file, வரி_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, இலக்கம்_dropped_errors) \
                : log_internal(_log_ratelimit_level, _log_ratelimit_error, _file, வரி_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#define log_ratelimit_full_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                முழு _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_internal(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_மதிப்பு(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#define log_ratelimit_full(level, _ratelimit, format, ...)                          \
        log_ratelimit_full_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#define log_ratelimit_info(...)      log_ratelimit_full(LOG_INFO,    __VA_ARGS__)
#define log_ratelimit_notice(...)    log_ratelimit_full(LOG_NOTICE,  __VA_ARGS__)
#define log_ratelimit_warning(...)   log_ratelimit_full(LOG_WARNING, __VA_ARGS__)
#define log_ratelimit_error(...)     log_ratelimit_full(LOG_ERR,     __VA_ARGS__)
#define log_ratelimit_emergency(...) log_ratelimit_full(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#define log_ratelimit_info_errno(error, ...)      log_ratelimit_full_errno(LOG_INFO,    error, __VA_ARGS__)
#define log_ratelimit_notice_errno(error, ...)    log_ratelimit_full_errno(LOG_NOTICE,  error, __VA_ARGS__)
#define log_ratelimit_warning_errno(error, ...)   log_ratelimit_full_errno(LOG_WARNING, error, __VA_ARGS__)
#define log_ratelimit_error_errno(error, ...)     log_ratelimit_full_errno(LOG_ERR,     error, __VA_ARGS__)
#define log_ratelimit_emergency_errno(error, ...) log_ratelimit_full_errno(log_emergency_level(), error, __VA_ARGS__)
