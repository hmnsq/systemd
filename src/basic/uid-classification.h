/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* The container base should have the last 16 bit set to zero */
assert_cc((CONTAINER_UID_ਅਧਾਰ_MIN & 0xFFFFU) == 0);
assert_cc((CONTAINER_UID_ਅਧਾਰ_MAX & 0xFFFFU) == 0);

/* Given we assign 64K UIDs to containers, the last container UID is 0xFFFF larger than the base */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTAINER_UID_MIN ((uid_t) CONTAINER_UID_ਅਧਾਰ_MIN)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONTAINER_UID_MAX ((uid_t) CONTAINER_UID_ਅਧਾਰ_MAX + 0xFFFFU)

assert_cc((FOREIGN_UID_ਅਧਾਰ & 0xFFFFU) == 0);
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREIGN_UID_MIN (FOREIGN_UID_ਅਧਾਰ + 0U)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOREIGN_UID_MAX (FOREIGN_UID_ਅਧਾਰ + 0xFFFFU)

bool uid_is_system(uid_t uid);
bool gid_is_system(gid_t gid);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_is_greeter(uid_t uid) {
        ਵਾਪਸ GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_is_dynamic(uid_t uid) {
        ਵਾਪਸ DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool gid_is_dynamic(gid_t gid) {
        ਵਾਪਸ uid_is_dynamic((uid_t) gid);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_is_container(uid_t uid) {
        ਵਾਪਸ CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool gid_is_container(gid_t gid) {
        ਵਾਪਸ uid_is_container((uid_t) gid);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_is_foreign(uid_t uid) {
        ਵਾਪਸ FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool gid_is_foreign(gid_t gid) {
        ਵਾਪਸ uid_is_foreign((uid_t) gid);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool uid_is_transient(uid_t uid) {
        ਵਾਪਸ uid_is_container(uid) || uid_is_dynamic(uid);
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool gid_is_transient(gid_t gid) {
        ਵਾਪਸ uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

ਪੂਰਨ read_login_defs(UGIDAllocationRange *ret_defs, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ);
ਅਚਲ UGIDAllocationRange *acquire_ugid_ਵੰਡ_range(ਖਾਲੀ);

bool uid_for_system_journal(uid_t uid);
