/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <pthread.h> /* IWYU pragma: export */

#অন্তর্ভাব "forward.h"

স্থির অন্তঃরেখ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        ফেরত mutex;
}

স্থির অন্তঃরেখ শূন্য pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        যদি (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
