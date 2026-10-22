/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <pthread.h> /* IWYU pragma: export */

#ଅନ୍ତର୍ଭାଵ "forward.h"

ସ୍ଥିର ଧାଡ଼ିଭିତରେ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        ଫେରନ୍ତୁ mutex;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ ଶୂନ୍ୟ pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        ଯଦି (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
