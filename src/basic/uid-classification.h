/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* The container base should have the last 16 bit set to zero */
assert_cc((CONTAINER_UID_بنیاد_MIN & 0xFFFFU) == 0);
assert_cc((CONTAINER_UID_بنیاد_MAX & 0xFFFFU) == 0);

/* Given we assign 64K UIDs to containers, the last container UID is 0xFFFF larger than the base */
#تعریف_کرو CONTAINER_UID_MIN ((uid_t) CONTAINER_UID_بنیاد_MIN)
#تعریف_کرو CONTAINER_UID_MAX ((uid_t) CONTAINER_UID_بنیاد_MAX + 0xFFFFU)

assert_cc((FOREIGN_UID_بنیاد & 0xFFFFU) == 0);
#تعریف_کرو FOREIGN_UID_MIN (FOREIGN_UID_بنیاد + 0U)
#تعریف_کرو FOREIGN_UID_MAX (FOREIGN_UID_بنیاد + 0xFFFFU)

bool uid_is_system(uid_t uid);
bool gid_is_system(gid_t gid);

ساکن اندرونی bool uid_is_greeter(uid_t uid) {
        واپس GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

ساکن اندرونی bool uid_is_dynamic(uid_t uid) {
        واپس DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

ساکن اندرونی bool gid_is_dynamic(gid_t gid) {
        واپس uid_is_dynamic((uid_t) gid);
}

ساکن اندرونی bool uid_is_container(uid_t uid) {
        واپس CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

ساکن اندرونی bool gid_is_container(gid_t gid) {
        واپس uid_is_container((uid_t) gid);
}

ساکن اندرونی bool uid_is_foreign(uid_t uid) {
        واپس FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

ساکن اندرونی bool gid_is_foreign(gid_t gid) {
        واپس uid_is_foreign((uid_t) gid);
}

ساکن اندرونی bool uid_is_transient(uid_t uid) {
        واپس uid_is_container(uid) || uid_is_dynamic(uid);
}

ساکن اندرونی bool gid_is_transient(gid_t gid) {
        واپس uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

قسمتعریف ساخت UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

صحیح read_login_defs(UGIDAllocationRange *ret_defs, ثابت حرف *راستہ, ثابت حرف *جڑ);
ثابت UGIDAllocationRange *acquire_ugid_تخصیص_range(خالی);

bool uid_for_system_journal(uid_t uid);
