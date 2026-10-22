/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

పూర్ణాంకం fopen_temporary_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం);
స్థిర అంతఃవరుస పూర్ణాంకం fopen_temporary(స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం) {
        తిరిగి fopen_temporary_at(AT_FDCWD, మార్గం, ret_file, ret_మార్గం);
}

పూర్ణాంకం fopen_temporary_బిడ్డ_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం);
స్థిర అంతఃవరుస పూర్ణాంకం fopen_temporary_బిడ్డ(స్థిరరాశి అక్షరం *మార్గం, FILE **ret_file, అక్షరం **ret_మార్గం) {
        తిరిగి fopen_temporary_బిడ్డ_at(AT_FDCWD, మార్గం, ret_file, ret_మార్గం);
}

పూర్ణాంకం mkostemp_సురక్షిత(అక్షరం *నమూనా);
పూర్ణాంకం fmkostemp_సురక్షిత(అక్షరం *నమూనా, స్థిరరాశి అక్షరం *బహుళకం, FILE**_f);

శూన్య unlink_tempfilep(అక్షరం (*p)[]);

పూర్ణాంకం tempfn_xxxxxx(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret);
పూర్ణాంకం tempfn_random(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret);
పూర్ణాంకం tempfn_random_బిడ్డ(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *extra, అక్షరం **ret);

పూర్ణాంకం open_tmpfile_unlinkable(స్థిరరాశి అక్షరం *directory, పూర్ణాంకం flags);
పూర్ణాంకం open_tmpfile_linkable_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం);
స్థిర అంతఃవరుస పూర్ణాంకం open_tmpfile_linkable(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం) {
        తిరిగి open_tmpfile_linkable_at(AT_FDCWD, లక్ష్యం, flags, ret_మార్గం);
}
పూర్ణాంకం fopen_tmpfile_linkable_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం, FILE **ret_file);
స్థిర అంతఃవరుస పూర్ణాంకం fopen_tmpfile_linkable(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం flags, అక్షరం **ret_మార్గం, FILE **ret_file) {
        తిరిగి fopen_tmpfile_linkable_at(AT_FDCWD, లక్ష్యం, flags, ret_మార్గం, ret_file);
}

రకనిర్వచనం గణన LinkTmpfileFlags {
        LINK_TMPFILE_REPLACE = 1 << 0,
        LINK_TMPFILE_SYNC    = 1 << 1,
} LinkTmpfileFlags;

పూర్ణాంకం link_tmpfile_at(పూర్ణాంకం fd, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags);
స్థిర అంతఃవరుస పూర్ణాంకం link_tmpfile(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags) {
        తిరిగి link_tmpfile_at(fd, AT_FDCWD, మార్గం, లక్ష్యం, flags);
}
పూర్ణాంకం flink_tmpfile_at(FILE *f, పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags);
స్థిర అంతఃవరుస పూర్ణాంకం flink_tmpfile(FILE *f, స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *లక్ష్యం, LinkTmpfileFlags flags) {
        తిరిగి flink_tmpfile_at(f, AT_FDCWD, మార్గం, లక్ష్యం, flags);
}

పూర్ణాంకం mkdtemp_malloc(స్థిరరాశి అక్షరం *మూస, అక్షరం **ret);
పూర్ణాంకం mkdtemp_open(స్థిరరాశి అక్షరం *మూస, పూర్ణాంకం flags, అక్షరం **ret);

/* A helper for removing link_tmpfile_at() via _cleanup_() */
నిర్మాణం cleanup_tmpfile_దత్తాంశం {
        పూర్ణాంకం *dir_fd;
        అక్షరం **filename;
};
శూన్య cleanup_tmpfile_దత్తాంశం_పూర్తి(నిర్మాణం cleanup_tmpfile_దత్తాంశం *d);
#నిర్వచించు _CLEANUP_TMPFILE_AT(u, dir_fd, filename)                        \
        వాడని _cleanup_(cleanup_tmpfile_దత్తాంశం_పూర్తి) నిర్మాణం cleanup_tmpfile_దత్తాంశం UNIQ_T(cta, u) = { &(dir_fd), &(filename) }
#నిర్వచించు CLEANUP_TMPFILE_AT(dir_fd, filename)                            \
        _CLEANUP_TMPFILE_AT(UNIQ, dir_fd, filename)
