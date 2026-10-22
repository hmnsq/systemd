/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव <pthread.h>

#अन्तर्भाव "process-util.h"
#अन्तर्भाव "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#define _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
स्थिर uint64_t origin_id;                                            \
                                                                      \
स्थिर शून्य origin_id_initialize(शून्य) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
स्थिर uint64_t origin_id_query(शून्य) {                               \
        स्थिर pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        प्रत्यागम origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        प्रतिज्ञा(p);                                                    \
        प्रत्यागम p->origin_id != origin_id_query();                     \
}

#define DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#define DEFINE_PRIVATE_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, स्थिर);
