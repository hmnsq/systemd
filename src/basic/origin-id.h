/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश <pthread.h>

#समावेश "process-util.h"
#समावेश "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#घोषणा _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
अटल uint64_t origin_id;                                            \
                                                                      \
अटल व्योम origin_id_initialize(व्योम) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
अटल uint64_t origin_id_query(व्योम) {                               \
        अटल pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        वापस origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        निश्चित(p);                                                    \
        वापस p->origin_id != origin_id_query();                     \
}

#घोषणा DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#घोषणा DEFINE_PRIVATE_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, अटल);
