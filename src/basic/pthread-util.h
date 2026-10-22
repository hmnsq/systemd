/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <pthread.h> /* IWYU pragma: export */

#अन्तर्भाव "forward.h"

स्थिर अन्तःस्थ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        प्रत्यागम mutex;
}

स्थिर अन्तःस्थ शून्य pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        यदि (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
