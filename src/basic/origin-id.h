/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ <pthread.h>

#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#define _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
ସ୍ଥିର uint64_t origin_id;                                            \
                                                                      \
ସ୍ଥିର ଶୂନ୍ୟ origin_id_initialize(ଶୂନ୍ୟ) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
ସ୍ଥିର uint64_t origin_id_query(ଶୂନ୍ୟ) {                               \
        ସ୍ଥିର pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        ଫେରନ୍ତୁ origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        ପ୍ରତିଜ୍ଞା(p);                                                    \
        ଫେରନ୍ତୁ p->origin_id != origin_id_query();                     \
}

#define DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#define DEFINE_PRIVATE_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, ସ୍ଥିର);
