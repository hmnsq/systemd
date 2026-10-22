/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#अन्तर्भाव "forward.h"

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

स्थिर अन्तःस्थ bool uid_is_greeter(uid_t uid) {
        फर्काऊ GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

स्थिर अन्तःस्थ bool uid_is_dynamic(uid_t uid) {
        फर्काऊ DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

स्थिर अन्तःस्थ bool gid_is_dynamic(gid_t gid) {
        फर्काऊ uid_is_dynamic((uid_t) gid);
}

स्थिर अन्तःस्थ bool uid_is_container(uid_t uid) {
        फर्काऊ CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

स्थिर अन्तःस्थ bool gid_is_container(gid_t gid) {
        फर्काऊ uid_is_container((uid_t) gid);
}

स्थिर अन्तःस्थ bool uid_is_foreign(uid_t uid) {
        फर्काऊ FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

स्थिर अन्तःस्थ bool gid_is_foreign(gid_t gid) {
        फर्काऊ uid_is_foreign((uid_t) gid);
}

स्थिर अन्तःस्थ bool uid_is_transient(uid_t uid) {
        फर्काऊ uid_is_container(uid) || uid_is_dynamic(uid);
}

स्थिर अन्तःस्थ bool gid_is_transient(gid_t gid) {
        फर्काऊ uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

प्रकारनिर्देश संरचना UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

पूर्णाङ्क read_login_defs(UGIDAllocationRange *ret_defs, अचर वर्ण *path, अचर वर्ण *जरा);
अचर UGIDAllocationRange *acquire_ugid_allocation_range(शून्य);

bool uid_for_system_journal(uid_t uid);
