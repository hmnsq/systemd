/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ضمن "forward.h"

/* The container base should have the last 16 bit set to zero */
assert_cc((CONTAINER_UID_BASE_MIN & 0xFFFFU) == 0);
assert_cc((CONTAINER_UID_BASE_MAX & 0xFFFFU) == 0);

/* Given we assign 64K UIDs to containers, the last container UID is 0xFFFF larger than the base */
#define CONTAINER_UID_MIN ((uid_t) CONTAINER_UID_BASE_MIN)
#define CONTAINER_UID_MAX ((uid_t) CONTAINER_UID_BASE_MAX + 0xFFFFU)

assert_cc((FOREIGN_UID_BASE & 0xFFFFU) == 0);
#define FOREIGN_UID_MIN (FOREIGN_UID_BASE + 0U)
#define FOREIGN_UID_MAX (FOREIGN_UID_BASE + 0xFFFFU)

bool uid_is_system(uid_t uid);
bool gid_is_system(gid_t gid);

ساكن مضمن bool uid_is_greeter(uid_t uid) {
        أرجع GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

ساكن مضمن bool uid_is_dynamic(uid_t uid) {
        أرجع DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

ساكن مضمن bool gid_is_dynamic(gid_t gid) {
        أرجع uid_is_dynamic((uid_t) gid);
}

ساكن مضمن bool uid_is_container(uid_t uid) {
        أرجع CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

ساكن مضمن bool gid_is_container(gid_t gid) {
        أرجع uid_is_container((uid_t) gid);
}

ساكن مضمن bool uid_is_foreign(uid_t uid) {
        أرجع FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

ساكن مضمن bool gid_is_foreign(gid_t gid) {
        أرجع uid_is_foreign((uid_t) gid);
}

ساكن مضمن bool uid_is_transient(uid_t uid) {
        أرجع uid_is_container(uid) || uid_is_dynamic(uid);
}

ساكن مضمن bool gid_is_transient(gid_t gid) {
        أرجع uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

تعريفنوع بنية UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

صحيح read_login_defs(UGIDAllocationRange *ret_defs, ثابت حرف *path, ثابت حرف *يىلتىز);
ثابت UGIDAllocationRange *acquire_ugid_allocation_range(فراغ);

bool uid_for_system_journal(uid_t uid);
