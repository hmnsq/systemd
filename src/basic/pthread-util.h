/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <pthread.h> /* IWYU pragma: export */

#അന്തര്ഭാവ "forward.h"

സ്ഥിതം വരിയിൽ pthread_mutex_t* pthread_mutex_lock_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        തിരികെ mutex;
}

സ്ഥിതം വരിയിൽ ശൂന്യം pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        എങ്കിൽ (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
