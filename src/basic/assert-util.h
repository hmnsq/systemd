/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(ਖਾਲੀ) _weak_ ਸ਼ੁੱਧ;

ਖਾਲੀ log_assert_failed_return(ਅਚਲ ਅੱਖਰ *ਪਾਠ, ਅਚਲ ਅੱਖਰ *file, ਪੂਰਨ ਸਤਰ, ਅਚਲ ਅੱਖਰ *func);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_log(ਵਿਅੰਜਕ)                                                \
        (_likely_(ਵਿਅੰਜਕ) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_return(ਵਿਅੰਜਕ, r)                                          \
        ਕਰੋ {                                                            \
                ਜੇ (!assert_log(ਵਿਅੰਜਕ))                                  \
                        ਵਾਪਸ (r);                                     \
        } ਜਦੋਂ (false)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ assert_return_errno(ਵਿਅੰਜਕ, r, err)                               \
        ਕਰੋ {                                                            \
                ਜੇ (!assert_log(ਵਿਅੰਜਕ)) {                                \
                        errno = err;                                    \
                        ਵਾਪਸ (r);                                     \
                }                                                       \
        } ਜਦੋਂ (false)
