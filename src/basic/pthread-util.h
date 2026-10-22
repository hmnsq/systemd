/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <pthread.h> /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਸਥਿਰ ਸਤਰਅੰਦਰ pthread_mutex_t* pthread_mutex_ਤਾਲਾ_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        ਵਾਪਸ mutex;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        ਜੇ (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
