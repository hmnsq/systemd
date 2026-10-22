/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <pthread.h> /* IWYU pragma: export */

#include "forward.h"

static inline pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        戻る mutex;
}

static inline void pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        もし (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
