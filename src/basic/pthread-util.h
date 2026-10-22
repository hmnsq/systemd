/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#شامل <pthread.h> /* IWYU pragma: export */

#شامل "forward.h"

ایستا درونخطی pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        بازگشت mutex;
}

ایستا درونخطی تهی pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        اگر (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
