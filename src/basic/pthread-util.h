/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن <pthread.h> /* IWYU pragma: export */

#ضمن "forward.h"

ساكن مضمن pthread_mutex_t* pthread_mutex_جندرا_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        أرجع mutex;
}

ساكن مضمن فراغ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        إذا (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
