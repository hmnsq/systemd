/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن <pthread.h> /* IWYU pragma: export */

#ضمن "forward.h"

ساكن مضمن pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        إرجاع mutex;
}

ساكن مضمن فراغ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        إذا (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
