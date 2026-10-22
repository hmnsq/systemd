/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

/* Include here so consumers have LOCK_{EX,SH,NB} available. */
#అన్తర్భావ <sys/file.h> /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

రకనిర్వచనం నిర్మాణం తాళం_File {
        పూర్ణాంకం dir_fd;
        అక్షరం *మార్గం;
        పూర్ణాంకం fd;
        పూర్ణాంకం operation;
} తాళం_File;

పూర్ణాంకం make_తాళం_file_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *p, పూర్ణాంకం operation, తాళం_File *ret);
స్థిర అంతఃవరుస పూర్ణాంకం make_తాళం_file(స్థిరరాశి అక్షరం *p, పూర్ణాంకం operation, తాళం_File *ret) {
        తిరిగి make_తాళం_file_at(AT_FDCWD, p, operation, ret);
}
పూర్ణాంకం make_తాళం_file_for(స్థిరరాశి అక్షరం *p, పూర్ణాంకం operation, తాళం_File *ret);
శూన్య release_తాళం_file(తాళం_File *f);

#నిర్వచించు తాళం_FILE_INIT (తాళం_File) { .dir_fd = -EBADF, .fd = -EBADF }

/* POSIX locks with the same interface as flock(). */
పూర్ణాంకం posix_తాళం(పూర్ణాంకం fd, పూర్ణాంకం operation);
శూన్య posix_unlockpp(పూర్ణాంకం **fd);

#నిర్వచించు CLEANUP_POSIX_UNLOCK(fd)                                   \
        _cleanup_(posix_unlockpp) వాడని పూర్ణాంకం *CONCATENATE(_cleanup_posix_unlock_, UNIQ) = &(fd)

/* Open File Description locks with the same interface as flock(). */
పూర్ణాంకం unposix_తాళం(పూర్ణాంకం fd, పూర్ణాంకం operation);
శూన్య unposix_unlockpp(పూర్ణాంకం **fd);

#నిర్వచించు CLEANUP_UNPOSIX_UNLOCK(fd)                                   \
        _cleanup_(unposix_unlockpp) వాడని పూర్ణాంకం *CONCATENATE(_cleanup_unposix_unlock_, UNIQ) = &(fd)

రకనిర్వచనం గణన తాళం_Type {
        తాళం_NONE, /* Don't lock the file descriptor. Useful if you need to conditionally lock a file. */
        తాళం_BSD,
        తాళం_POSIX,
        తాళం_UNPOSIX,
} తాళం_Type;

పూర్ణాంకం తాళం_generic(పూర్ణాంకం fd, తాళం_Type type, పూర్ణాంకం operation);

పూర్ణాంకం తాళం_generic_with_timeout(పూర్ణాంకం fd, తాళం_Type type, పూర్ణాంకం operation, usec_t timeout);
