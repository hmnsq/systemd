/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <pthread.h> /* IWYU pragma: export */

#ಅನ್ತರ್ಭಾವ "forward.h"

ಸ್ಥಿರ ಸಾಲೊಳಗೆ pthread_mutex_t* pthread_mutex_ಬೀಗ_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        ಹಿಂತಿರುಗಿ mutex;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಶೂನ್ಯ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        ಆದರೆ (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
