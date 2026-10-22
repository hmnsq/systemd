/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "../fundamental/assert-util.h" /* IWYU pragma: export */

/* Logging for various assertions */

bool log_get_assert_return_is_critical(வெறுமை) _weak_ _pure_;

வெறுமை log_assert_failed_return(மாறிலி எழுத்து *text, மாறிலி எழுத்து *file, முழு வரி, மாறிலி எழுத்து *func);

#define assert_log(கோவை)                                                \
        (_likely_(கோவை) ?                                               \
         true :                                                         \
         (log_assert_failed_return(#expr, PROJECT_FILE, __LINE__, __func__), false))

#define assert_return(கோவை, r)                                          \
        செய் {                                                            \
                எனில் (!assert_log(கோவை))                                  \
                        திரும்பு (r);                                     \
        } போது (false)

#define assert_return_errno(கோவை, r, err)                               \
        செய் {                                                            \
                எனில் (!assert_log(கோவை)) {                                \
                        errno = err;                                    \
                        திரும்பு (r);                                     \
                }                                                       \
        } போது (false)
