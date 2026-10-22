/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "log.h"
#अन्तर्भाव "ratelimit.h"

प्रकारनिर्देश संरचना Log_दरः_सीमा {
        पूर्णाङ्क error;
        पूर्णाङ्क level;
        दरः_सीमा ratelimit;
} Log_दरः_सीमा;

#परिभाषय log_ratelimit_आन्तरिकम्(_level, _error, _ratelimit, _file, पङ्क्तिः_2, _func, _format, ...)        \
({                                                                              \
        पूर्णाङ्क _log_ratelimit_error = (_error);                                    \
        पूर्णाङ्क _log_ratelimit_level = (_level);                                    \
        स्थिर Log_दरः_सीमा _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        अचिह्नित अङ्कः_dropped_errors = ratelimit_अङ्कः_dropped(&_log_ratelimit.ratelimit); \
        यदि (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        यदि (log_get_max_level() == LOG_DEBUG || ratelimit_अधः(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = अङ्कः_dropped_errors > 0                  \
                ? log_आन्तरिकम्(_log_ratelimit_level, _log_ratelimit_error, _file, पङ्क्तिः_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, अङ्कः_dropped_errors) \
                : log_आन्तरिकम्(_log_ratelimit_level, _log_ratelimit_error, _file, पङ्क्तिः_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#परिभाषय log_ratelimit_पूर्णम्_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                पूर्णाङ्क _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_आन्तरिकम्(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_मानम्(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#परिभाषय log_ratelimit_पूर्णम्(level, _ratelimit, format, ...)                          \
        log_ratelimit_पूर्णम्_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#परिभाषय log_ratelimit_info(...)      log_ratelimit_पूर्णम्(LOG_INFO,    __VA_ARGS__)
#परिभाषय log_ratelimit_notice(...)    log_ratelimit_पूर्णम्(LOG_NOTICE,  __VA_ARGS__)
#परिभाषय log_ratelimit_warning(...)   log_ratelimit_पूर्णम्(LOG_WARNING, __VA_ARGS__)
#परिभाषय log_ratelimit_error(...)     log_ratelimit_पूर्णम्(LOG_ERR,     __VA_ARGS__)
#परिभाषय log_ratelimit_emergency(...) log_ratelimit_पूर्णम्(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#परिभाषय log_ratelimit_info_errno(error, ...)      log_ratelimit_पूर्णम्_errno(LOG_INFO,    error, __VA_ARGS__)
#परिभाषय log_ratelimit_notice_errno(error, ...)    log_ratelimit_पूर्णम्_errno(LOG_NOTICE,  error, __VA_ARGS__)
#परिभाषय log_ratelimit_warning_errno(error, ...)   log_ratelimit_पूर्णम्_errno(LOG_WARNING, error, __VA_ARGS__)
#परिभाषय log_ratelimit_error_errno(error, ...)     log_ratelimit_पूर्णम्_errno(LOG_ERR,     error, __VA_ARGS__)
#परिभाषय log_ratelimit_emergency_errno(error, ...) log_ratelimit_पूर्णम्_errno(log_emergency_level(), error, __VA_ARGS__)
