/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <pthread.h>

#అన్తర్భావ "process-util.h"
#అన్తర్భావ "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#నిర్వచించు _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
స్థిర uint64_t origin_id;                                            \
                                                                      \
స్థిర శూన్య origin_id_initialize(శూన్య) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
స్థిర uint64_t origin_id_query(శూన్య) {                               \
        స్థిర pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        తిరిగి origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        ప్రతిజ్ఞా(p);                                                    \
        తిరిగి p->origin_id != origin_id_query();                     \
}

#నిర్వచించు DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#నిర్వచించు DEFINE_వ్యక్తిగత_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, స్థిర);
