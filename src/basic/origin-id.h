/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব <pthread.h>

#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#define _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
স্থির uint64_t origin_id;                                            \
                                                                      \
স্থির শূন্য origin_id_initialize(শূন্য) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
স্থির uint64_t origin_id_query(শূন্য) {                               \
        স্থির pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        প্রত্যাগম origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        প্রতিজ্ঞা(p);                                                    \
        প্রত্যাগম p->origin_id != origin_id_query();                     \
}

#define DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#define DEFINE_PRIVATE_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, স্থির);
