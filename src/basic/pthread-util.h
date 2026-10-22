/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <pthread.h> /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

స్థిర అంతఃవరుస pthread_mutex_t* pthread_mutex_తాళం_assert(pthread_mutex_t *mutex) {
        assert_se(pthread_mutex_lock(mutex) == 0);
        తిరిగి mutex;
}

స్థిర అంతఃవరుస శూన్య pthread_mutex_unlock_assertp(pthread_mutex_t **mutexp) {
        ఒకవేళ (*mutexp)
                assert_se(pthread_mutex_unlock(*mutexp) == 0);
}
