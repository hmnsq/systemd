/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <pthread.h> /* IWYU pragma: export */

#समावेश "forward.h"

अटल अंतरभूत pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        वापस mutex;
}

अटल अंतरभूत व्योम pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        अगर (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
