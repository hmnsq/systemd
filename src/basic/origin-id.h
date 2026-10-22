/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ <pthread.h>

#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#define _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
സ്ഥിതം uint64_t origin_id;                                            \
                                                                      \
സ്ഥിതം ശൂന്യം origin_id_initialize(ശൂന്യം) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
സ്ഥിതം uint64_t origin_id_query(ശൂന്യം) {                               \
        സ്ഥിതം pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        തിരികെ origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        പ്രതിജ്ഞാ(p);                                                    \
        തിരികെ p->origin_id != origin_id_query();                     \
}

#define DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#define DEFINE_PRIVATE_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, സ്ഥിതം);
