/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ <pthread.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        ಪ್ರತ್ಯಾಗಮ mutex;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಶೂನ್ಯ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        ಯದಿ (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
