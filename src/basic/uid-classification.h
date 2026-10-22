/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* The container base should have the last 16 bit set to zero */
assert_cc((CONTAINER_UID_ತಳ_MIN & 0xFFFFU) == 0);
assert_cc((CONTAINER_UID_ತಳ_MAX & 0xFFFFU) == 0);

/* Given we assign 64K UIDs to containers, the last container UID is 0xFFFF larger than the base */
#ವ್ಯಾಖ್ಯಾನಿಸು CONTAINER_UID_MIN ((uid_t) CONTAINER_UID_ತಳ_MIN)
#ವ್ಯಾಖ್ಯಾನಿಸು CONTAINER_UID_MAX ((uid_t) CONTAINER_UID_ತಳ_MAX + 0xFFFFU)

assert_cc((FOREIGN_UID_ತಳ & 0xFFFFU) == 0);
#ವ್ಯಾಖ್ಯಾನಿಸು FOREIGN_UID_MIN (FOREIGN_UID_ತಳ + 0U)
#ವ್ಯಾಖ್ಯಾನಿಸು FOREIGN_UID_MAX (FOREIGN_UID_ತಳ + 0xFFFFU)

bool uid_is_system(uid_t uid);
bool gid_is_system(gid_t gid);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_is_greeter(uid_t uid) {
        ಹಿಂತಿರುಗಿ GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_is_dynamic(uid_t uid) {
        ಹಿಂತಿರುಗಿ DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool gid_is_dynamic(gid_t gid) {
        ಹಿಂತಿರುಗಿ uid_is_dynamic((uid_t) gid);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_is_container(uid_t uid) {
        ಹಿಂತಿರುಗಿ CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool gid_is_container(gid_t gid) {
        ಹಿಂತಿರುಗಿ uid_is_container((uid_t) gid);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_is_foreign(uid_t uid) {
        ಹಿಂತಿರುಗಿ FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool gid_is_foreign(gid_t gid) {
        ಹಿಂತಿರುಗಿ uid_is_foreign((uid_t) gid);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool uid_is_transient(uid_t uid) {
        ಹಿಂತಿರುಗಿ uid_is_container(uid) || uid_is_dynamic(uid);
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool gid_is_transient(gid_t gid) {
        ಹಿಂತಿರುಗಿ uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

ಪೂರ್ಣಾಂಕ read_login_defs(UGIDAllocationRange *ret_defs, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬೇರು);
ಸ್ಥಿರಾಂಕ UGIDAllocationRange *acquire_ugid_ಹಂಚಿಕೆ_range(ಶೂನ್ಯ);

bool uid_for_system_journal(uid_t uid);
