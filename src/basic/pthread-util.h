/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ <pthread.h> /* IWYU pragma: export */

#અન્તર્ભાવ "forward.h"

સ્થિર અંતઃરેખ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        પાછા mutex;
}

સ્થિર અંતઃરેખ શૂન્ય pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        જો (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
