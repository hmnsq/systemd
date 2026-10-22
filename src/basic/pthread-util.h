/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <pthread.h> /* IWYU pragma: export */

#شامل "forward.h"

ساکن اندرونی pthread_mutex_t* pthread_mutex_تالا_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        واپس mutex;
}

ساکن اندرونی خالی pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        اگر (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
