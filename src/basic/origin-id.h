/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <pthread.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "random-util.h"

/* This pattern needs to be repeated exactly in multiple modules, so macro it.
 * To ensure an object is not passed into a different module (e.g.: when two shared objects statically
 * linked to libsystemd get loaded in the same process, and the object created by one is passed to the
 * other, see https://github.com/systemd/systemd/issues/27216), create a random static global random
 * (mixed with PID, so that we can also check for reuse after fork) that is stored in the object and
 * checked by public API on use. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _DEFINE_ORIGIN_ID_HELPERS(type, name, scope)                  \
ਸਥਿਰ uint64_t origin_id;                                            \
                                                                      \
ਸਥਿਰ ਖਾਲੀ origin_id_initialize(ਖਾਲੀ) {                              \
        origin_id = random_u64();                                     \
}                                                                     \
                                                                      \
ਸਥਿਰ uint64_t origin_id_query(ਖਾਲੀ) {                               \
        ਸਥਿਰ pthread_once_t once = PTHREAD_ONCE_INIT;               \
        assert_se(pthread_once(&once, origin_id_initialize) == 0);    \
        ਵਾਪਸ origin_id ^ getpid_cached();                           \
}                                                                     \
                                                                      \
scope bool name##_origin_changed(type *p) {                           \
        ਪ੍ਰਤਿਜ੍ਞਾ(p);                                                    \
        ਵਾਪਸ p->origin_id != origin_id_query();                     \
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ORIGIN_ID_HELPERS(type, name)                          \
        _DEFINE_ORIGIN_ID_HELPERS(type, name,);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFINE_ਨਿੱਜੀ_ORIGIN_ID_HELPERS(type, name)                  \
        _DEFINE_ORIGIN_ID_HELPERS(type, name, ਸਥਿਰ);
