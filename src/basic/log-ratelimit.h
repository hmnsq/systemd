/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "log.h"
#అన్తర్భావ "ratelimit.h"

రకనిర్వచనం నిర్మాణం Log_రేటు_హద్దు {
        పూర్ణాంకం error;
        పూర్ణాంకం level;
        రేటు_హద్దు ratelimit;
} Log_రేటు_హద్దు;

#నిర్వచించు log_ratelimit_అంతర్గత(_level, _error, _ratelimit, _file, పంక్తి_2, _func, _format, ...)        \
({                                                                              \
        పూర్ణాంకం _log_ratelimit_error = (_error);                                    \
        పూర్ణాంకం _log_ratelimit_level = (_level);                                    \
        స్థిర Log_రేటు_హద్దు _log_ratelimit = {                                  \
                .ratelimit = (_ratelimit),                                      \
        };                                                                      \
        గుర్తులేని అంకె_dropped_errors = ratelimit_అంకె_dropped(&_log_ratelimit.ratelimit); \
        ఒకవేళ (_log_ratelimit_error != _log_ratelimit.error || _log_ratelimit_level != _log_ratelimit.level) { \
                ratelimit_reset(&_log_ratelimit.ratelimit);                     \
                _log_ratelimit.error = _log_ratelimit_error;                    \
                _log_ratelimit.level = _log_ratelimit_level;                    \
        }                                                                       \
        ఒకవేళ (log_get_max_level() == LOG_DEBUG || ratelimit_క్రింద(&_log_ratelimit.ratelimit)) \
                _log_ratelimit_error = అంకె_dropped_errors > 0                  \
                ? log_అంతర్గత(_log_ratelimit_level, _log_ratelimit_error, _file, పంక్తి_2, _func, _format " (Dropped %u similar message(s))", ##__VA_ARGS__, అంకె_dropped_errors) \
                : log_అంతర్గత(_log_ratelimit_level, _log_ratelimit_error, _file, పంక్తి_2, _func, _format, ##__VA_ARGS__); \
        _log_ratelimit_error;                                                   \
})

#నిర్వచించు log_ratelimit_నిండిన_errno(level, error, _ratelimit, format, ...)             \
        ({                                                              \
                పూర్ణాంకం _level = (level), _e = (error);                     \
                _e = (log_get_max_level() >= LOG_PRI(_level))           \
                        ? log_ratelimit_అంతర్గత(_level, _e, _ratelimit, PROJECT_FILE, __LINE__, __func__, format, ##__VA_ARGS__) \
                        : -ERRNO_విలువ(_e);                             \
                _e < 0 ? _e : -ESTRPIPE;                                \
        })

#నిర్వచించు log_ratelimit_నిండిన(level, _ratelimit, format, ...)                          \
        log_ratelimit_నిండిన_errno(level, 0, _ratelimit, format, ##__VA_ARGS__)

/* Normal logging */
#నిర్వచించు log_ratelimit_info(...)      log_ratelimit_నిండిన(LOG_INFO,    __VA_ARGS__)
#నిర్వచించు log_ratelimit_notice(...)    log_ratelimit_నిండిన(LOG_NOTICE,  __VA_ARGS__)
#నిర్వచించు log_ratelimit_warning(...)   log_ratelimit_నిండిన(LOG_WARNING, __VA_ARGS__)
#నిర్వచించు log_ratelimit_error(...)     log_ratelimit_నిండిన(LOG_ERR,     __VA_ARGS__)
#నిర్వచించు log_ratelimit_emergency(...) log_ratelimit_నిండిన(log_emergency_level(), __VA_ARGS__)

/* Logging triggered by an errno-like error */
#నిర్వచించు log_ratelimit_info_errno(error, ...)      log_ratelimit_నిండిన_errno(LOG_INFO,    error, __VA_ARGS__)
#నిర్వచించు log_ratelimit_notice_errno(error, ...)    log_ratelimit_నిండిన_errno(LOG_NOTICE,  error, __VA_ARGS__)
#నిర్వచించు log_ratelimit_warning_errno(error, ...)   log_ratelimit_నిండిన_errno(LOG_WARNING, error, __VA_ARGS__)
#నిర్వచించు log_ratelimit_error_errno(error, ...)     log_ratelimit_నిండిన_errno(LOG_ERR,     error, __VA_ARGS__)
#నిర్వచించు log_ratelimit_emergency_errno(error, ...) log_ratelimit_నిండిన_errno(log_emergency_level(), error, __VA_ARGS__)
