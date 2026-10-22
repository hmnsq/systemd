/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل <pthread.h>

#شامل "process-util.h"
#شامل "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#تعریف_کرو _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
ساکن uint64_t origin_id;                                            \
                                                                      \
ساکن خالی origin_id_initialize(خالی) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
ساکن uint64_t origin_id_query(خالی) {                               \
        ساکن pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        واپس origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        أكد(p);                                                    \
        واپس p->origin_id != origin_id_query();                     \
}

#تعریف_کرو DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#تعریف_کرو DEFINE_نجی_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, ساکن);
