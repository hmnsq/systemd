/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* The container base should have the last 16 bit set to zero */
assert_cc((CONTAINER_UID_పునాది_MIN & 0xFFFFU) == 0);
assert_cc((CONTAINER_UID_పునాది_MAX & 0xFFFFU) == 0);

/* Given we assign 64K UIDs to containers, the last container UID is 0xFFFF larger than the base */
#నిర్వచించు CONTAINER_UID_MIN ((uid_t) CONTAINER_UID_పునాది_MIN)
#నిర్వచించు CONTAINER_UID_MAX ((uid_t) CONTAINER_UID_పునాది_MAX + 0xFFFFU)

assert_cc((FOREIGN_UID_పునాది & 0xFFFFU) == 0);
#నిర్వచించు FOREIGN_UID_MIN (FOREIGN_UID_పునాది + 0U)
#నిర్వచించు FOREIGN_UID_MAX (FOREIGN_UID_పునాది + 0xFFFFU)

bool uid_is_system(uid_t uid);
bool gid_is_system(gid_t gid);

స్థిర అంతఃవరుస bool uid_is_greeter(uid_t uid) {
        తిరిగి GREETER_UID_MIN <= uid && uid <= GREETER_UID_MAX;
}

స్థిర అంతఃవరుస bool uid_is_dynamic(uid_t uid) {
        తిరిగి DYNAMIC_UID_MIN <= uid && uid <= DYNAMIC_UID_MAX;
}

స్థిర అంతఃవరుస bool gid_is_dynamic(gid_t gid) {
        తిరిగి uid_is_dynamic((uid_t) gid);
}

స్థిర అంతఃవరుస bool uid_is_container(uid_t uid) {
        తిరిగి CONTAINER_UID_MIN <= uid && uid <= CONTAINER_UID_MAX;
}

స్థిర అంతఃవరుస bool gid_is_container(gid_t gid) {
        తిరిగి uid_is_container((uid_t) gid);
}

స్థిర అంతఃవరుస bool uid_is_foreign(uid_t uid) {
        తిరిగి FOREIGN_UID_MIN <= uid && uid <= FOREIGN_UID_MAX;
}

స్థిర అంతఃవరుస bool gid_is_foreign(gid_t gid) {
        తిరిగి uid_is_foreign((uid_t) gid);
}

స్థిర అంతఃవరుస bool uid_is_transient(uid_t uid) {
        తిరిగి uid_is_container(uid) || uid_is_dynamic(uid);
}

స్థిర అంతఃవరుస bool gid_is_transient(gid_t gid) {
        తిరిగి uid_is_container((uid_t) gid) || uid_is_dynamic((uid_t) gid);
}

రకనిర్వచనం నిర్మాణం UGIDAllocationRange {
        uid_t system_alloc_uid_min;
        uid_t system_uid_max;
        gid_t system_alloc_gid_min;
        gid_t system_gid_max;
} UGIDAllocationRange;

పూర్ణాంకం read_login_defs(UGIDAllocationRange *ret_defs, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *మూలం);
స్థిరరాశి UGIDAllocationRange *acquire_ugid_కేటాయింపు_range(శూన్య);

bool uid_for_system_journal(uid_t uid);
