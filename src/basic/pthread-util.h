/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ <pthread.h> /* IWYU pragma: export */

#அந்தர்பாவ "forward.h"

நிலையான உள்வரி pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        திரும்பு mutex;
}

நிலையான உள்வரி வெறுமை pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        எனில் (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
