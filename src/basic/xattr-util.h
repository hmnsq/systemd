/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

పూర్ణాంకం getxattr_at_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, పూర్ణాంకం at_flags, అక్షరం **ret, size_t *ret_కొలత);
స్థిర అంతఃవరుస పూర్ణాంకం getxattr_malloc(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, అక్షరం **ret, size_t *ret_కొలత) {
        తిరిగి getxattr_at_malloc(AT_FDCWD, మార్గం, name, AT_SYMLINK_FOLLOW, ret, ret_కొలత);
}
స్థిర అంతఃవరుస పూర్ణాంకం lgetxattr_malloc(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, అక్షరం **ret, size_t *ret_కొలత) {
        తిరిగి getxattr_at_malloc(AT_FDCWD, మార్గం, name, 0, ret, ret_కొలత);
}
స్థిర అంతఃవరుస పూర్ణాంకం fgetxattr_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *name, అక్షరం **ret, size_t *ret_కొలత) {
        తిరిగి getxattr_at_malloc(fd, ఖాళీ, name, AT_ఖాళీలు_మార్గం, ret, ret_కొలత);
}

పూర్ణాంకం getxattr_at_bool(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, పూర్ణాంకం at_flags);
పూర్ణాంకం getxattr_at_strv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *name, పూర్ణాంకం at_flags, అక్షరం ***ret_strv);

పూర్ణాంకం listxattr_at_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం listxattr_malloc(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        తిరిగి listxattr_at_malloc(AT_FDCWD, మార్గం, AT_SYMLINK_FOLLOW, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం llistxattr_malloc(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        తిరిగి listxattr_at_malloc(AT_FDCWD, మార్గం, 0, ret);
}
స్థిర అంతఃవరుస పూర్ణాంకం flistxattr_malloc(పూర్ణాంకం fd, అక్షరం **ret) {
        తిరిగి listxattr_at_malloc(fd, ఖాళీ, AT_ఖాళీలు_మార్గం, ret);
}

పూర్ణాంకం xsetxattr_నిండిన(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *విలువ,
                size_t కొలత,
                పూర్ణాంకం xattr_flags);
స్థిర అంతఃవరుస పూర్ణాంకం xsetxattr(
                పూర్ణాంకం fd,
                స్థిరరాశి అక్షరం *మార్గం,
                పూర్ణాంకం at_flags,
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *విలువ) {
        తిరిగి xsetxattr_నిండిన(fd, మార్గం, at_flags, name, విలువ, SIZE_MAX, 0);
}

పూర్ణాంకం xsetxattr_strv(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, స్థిరరాశి అక్షరం *name, అక్షరం * స్థిరరాశి *l);

పూర్ణాంకం xremovexattr(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, స్థిరరాశి అక్షరం *name);

పూర్ణాంకం fd_setcrtime(పూర్ణాంకం fd, usec_t usec);
పూర్ణాంకం getcrtime_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం at_flags, usec_t *ret);
స్థిర అంతఃవరుస పూర్ణాంకం fd_getcrtime(పూర్ణాంకం fd, usec_t *ret) {
        తిరిగి getcrtime_at(fd, ఖాళీ, 0, ret);
}

bool xattr_is_acl(స్థిరరాశి అక్షరం *name);
bool xattr_is_selinux(స్థిరరాశి అక్షరం *name);
