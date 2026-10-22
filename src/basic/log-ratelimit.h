/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "ratelimit.h"

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ Log_ದರ_ಎಲ್ಲೆ {
        ಪೂರ್ಣಾಂಕ error;
        ಪೂರ್ಣಾಂಕ level;
        ದರ_ಎಲ್ಲೆ ratelimit;
} Log_ದರ_ಎಲ್ಲೆ;

#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_ಆಂತರಿಕ(_level, _error, _ratelimit, _file, ಸಾಲು_2, _func, _format, ...)        \
({                                                                              \
        ಪೂರ್ಣಾಂಕ _log_ratelimit_error = (_error);                                    \
        ಪೂರ್ಣಾಂಕ _log_ratelimit_level = (_level);                                    \
        ಸ್ಥಿರ Log_ದರ_ಎಲ್ಲೆ _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        ಚಿಹ್ನೆರಹಿತ ಅಂಕೆ_dropped_errors = ratelimit_ಅಂಕೆ_dropped(&_log_ratelimit.ratelimit); \
        ಆದರೆ (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        ಆದರೆ (log_get_max_level() == LOG_DEBUG || ratelimit_ಕೆಳಗೆ(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = ಅಂಕೆ_dropped_errors > 0                  \
                ? log_ಆಂತರಿಕ(_log_ratelimit_level, _log_ratelimit_error, _file, ಸಾಲು_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, ಅಂಕೆ_dropped_errors) \
                : log_ಆಂತರಿಕ(_log_ratelimit_level, _log_ratelimit_error, _file, ಸಾಲು_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_ತುಂಬಿದ_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                ಪೂರ್ಣಾಂಕ _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_ಆಂತರಿಕ(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_ಮೌಲ್ಯ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_ತುಂಬಿದ(level, _ratelimit, format, ...)                          \
        log_ratelimit_ತುಂಬಿದ_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_info(...)      log_ratelimit_ತುಂಬಿದ(LOG_INFO,    __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_notice(...)    log_ratelimit_ತುಂಬಿದ(LOG_NOTICE,  __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_warning(...)   log_ratelimit_ತುಂಬಿದ(LOG_WARNING, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_error(...)     log_ratelimit_ತುಂಬಿದ(LOG_ERR,     __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_emergency(...) log_ratelimit_ತುಂಬಿದ(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_info_errno(error, ...)      log_ratelimit_ತುಂಬಿದ_errno(LOG_INFO,    error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_notice_errno(error, ...)    log_ratelimit_ತುಂಬಿದ_errno(LOG_NOTICE,  error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_warning_errno(error, ...)   log_ratelimit_ತುಂಬಿದ_errno(LOG_WARNING, error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_error_errno(error, ...)     log_ratelimit_ತುಂಬಿದ_errno(LOG_ERR,     error, __VA_ARGS__)
#ವ್ಯಾಖ್ಯಾನಿಸು log_ratelimit_emergency_errno(error, ...) log_ratelimit_ತುಂಬಿದ_errno(log_emergency_level(), error, __VA_ARGS__)
