/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"
#అన్తర్భావ "lock-util.h"

/* The following macros add 1 when converting things, since 0 is a valid mode, while the pointer
 * NULL is special */
స్థిర అంతఃవరుస mode_t PTR_గమ్యం_బహుళకం(శూన్య *p) {
        తిరిగి p ? (mode_t) ((uintptr_t) p - 1) : బహుళకం_INVALID;
}
స్థిర అంతఃవరుస శూన్య* బహుళకం_గమ్యం_PTR(mode_t m) {
        తిరిగి m == బహుళకం_INVALID ? ఖాళీ : (శూన్య *) ((uintptr_t) m + 1);
}

పూర్ణాంకం rmdir_parents(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *stop);

పూర్ణాంకం rename_noreplace(పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath);

పూర్ణాంకం readlinkat_malloc(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *p, అక్షరం **ret);
స్థిర అంతఃవరుస పూర్ణాంకం readlink_malloc(స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        తిరిగి readlinkat_malloc(AT_FDCWD, p, ret);
}
పూర్ణాంకం readlink_విలువ(స్థిరరాశి అక్షరం *p, అక్షరం **ret);
పూర్ణాంకం readlink_and_make_absolute(స్థిరరాశి అక్షరం *p, అక్షరం **ret);

పూర్ణాంకం chmod_and_chown_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid);
స్థిర అంతఃవరుస పూర్ణాంకం chmod_and_chown(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid) {
        తిరిగి chmod_and_chown_at(AT_FDCWD, మార్గం, బహుళకం, uid, gid);
}
పూర్ణాంకం fchmod_and_chown_with_fallback(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, uid_t uid, gid_t gid);
స్థిర అంతఃవరుస పూర్ణాంకం fchmod_and_chown(పూర్ణాంకం fd, mode_t బహుళకం, uid_t uid, gid_t gid) {
        తిరిగి fchmod_and_chown_with_fallback(fd, ఖాళీ, బహుళకం, uid, gid); /* no fallback */
}

పూర్ణాంకం fchmod_umask(పూర్ణాంకం fd, mode_t బహుళకం);
పూర్ణాంకం fchmod_opath(పూర్ణాంకం fd, mode_t m);

పూర్ణాంకం futimens_opath(పూర్ణాంకం fd, స్థిరరాశి నిర్మాణం timespec ts[2]);

పూర్ణాంకం fd_warn_permissions(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం fd);
పూర్ణాంకం stat_warn_permissions(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి నిర్మాణం stat *st);

పూర్ణాంకం access_nofollow(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం బహుళకం);
పూర్ణాంకం access_fd(పూర్ణాంకం fd, పూర్ణాంకం బహుళకం);

పూర్ణాంకం touch_fd(పూర్ణాంకం fd, usec_t stamp);
పూర్ణాంకం touch_file(స్థిరరాశి అక్షరం *మార్గం, bool parents, usec_t stamp, uid_t uid, gid_t gid, mode_t బహుళకం);
పూర్ణాంకం touch(స్థిరరాశి అక్షరం *మార్గం);

పూర్ణాంకం symlinkat_idempotent(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *linkpath, bool make_relative);
స్థిర అంతఃవరుస పూర్ణాంకం symlink_idempotent(స్థిరరాశి అక్షరం *లక్ష్యం, స్థిరరాశి అక్షరం *linkpath, bool make_relative) {
        తిరిగి symlinkat_idempotent(లక్ష్యం, AT_FDCWD, linkpath, make_relative);
}

రకనిర్వచనం గణన SymlinkFlags {
        SYMLINK_MAKE_RELATIVE = 1 << 0,
        SYMLINK_LABEL         = 1 << 1,
} SymlinkFlags;

పూర్ణాంకం symlinkat_atomic_నిండిన_label(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *linkpath, SymlinkFlags flags, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం symlinkat_atomic_నిండిన(స్థిరరాశి అక్షరం *లక్ష్యం, పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *linkpath, SymlinkFlags flags) {
        తిరిగి symlinkat_atomic_నిండిన_label(లక్ష్యం, atfd, linkpath, flags, /* label_context= */ ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం symlink_atomic(స్థిరరాశి అక్షరం *లక్ష్యం, స్థిరరాశి అక్షరం *linkpath) {
        తిరిగి symlinkat_atomic_నిండిన(లక్ష్యం, AT_FDCWD, linkpath, 0);
}

పూర్ణాంకం mknodat_atomic(పూర్ణాంకం atfd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, dev_t dev);
స్థిర అంతఃవరుస పూర్ణాంకం mknod_atomic(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం, dev_t dev) {
        తిరిగి mknodat_atomic(AT_FDCWD, మార్గం, బహుళకం, dev);
}

పూర్ణాంకం mkfifoat_atomic(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం);
స్థిర అంతఃవరుస పూర్ణాంకం mkfifo_atomic(స్థిరరాశి అక్షరం *మార్గం, mode_t బహుళకం) {
        తిరిగి mkfifoat_atomic(AT_FDCWD, మార్గం, బహుళకం);
}

పూర్ణాంకం get_files_లోపల_directory(స్థిరరాశి అక్షరం *మార్గం, అక్షరం ***list);

పూర్ణాంకం tmp_dir(స్థిరరాశి అక్షరం **ret);
పూర్ణాంకం var_tmp_dir(స్థిరరాశి అక్షరం **ret);

పూర్ణాంకం unlink_or_warn(స్థిరరాశి అక్షరం *filename);

/* Useful for usage with _cleanup_(), removes a directory and frees the pointer */
అక్షరం *rmdir_and_free(అక్షరం *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(అక్షరం*, rmdir_and_free);

అక్షరం* unlink_and_free(అక్షరం *p);
DEFINE_TRIVIAL_CLEANUP_FUNC(అక్షరం*, unlink_and_free);

రకనిర్వచనం గణన UnlinkDeallocateFlags {
        UNLINK_REMOVEDIR = 1 << 0,
        UNLINK_ERASE     = 1 << 1,
} UnlinkDeallocateFlags;

పూర్ణాంకం unlinkat_deallocate(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *name, UnlinkDeallocateFlags flags);

పూర్ణాంకం open_మాతృ_at(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, mode_t బహుళకం);
స్థిర అంతఃవరుస పూర్ణాంకం open_మాతృ(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, mode_t బహుళకం) {
        తిరిగి open_మాతృ_at(AT_FDCWD, మార్గం, flags, బహుళకం);
}

పూర్ణాంకం conservative_renameat(పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath);
స్థిర అంతఃవరుస పూర్ణాంకం conservative_rename(స్థిరరాశి అక్షరం *oldpath, స్థిరరాశి అక్షరం *newpath) {
        తిరిగి conservative_renameat(AT_FDCWD, oldpath, AT_FDCWD, newpath);
}

పూర్ణాంకం posix_fallocate_loop(పూర్ణాంకం fd, uint64_t స్థానభ్రంశం, uint64_t కొలత);

పూర్ణాంకం parse_cifs_service(స్థిరరాశి అక్షరం *s, అక్షరం **ret_host, అక్షరం **ret_service, అక్షరం **ret_మార్గం);

రకనిర్వచనం గణన XOpenFlags {
        XO_LABEL             = 1 << 0, /* When creating: relabel */
        XO_SUBVOLUME         = 1 << 1, /* When creating as directory: make it a subvolume */
        XO_NOCOW             = 1 << 2, /* Enable NOCOW mode after opening */
        XO_COW               = 1 << 3, /* Enable COW mode after opening */
        XO_REGULAR           = 1 << 4, /* Fail if the inode is not a regular file */
        XO_SOCKET            = 1 << 5, /* Fail if the inode is not a socket */
        XO_TRIGGER_AUTOMOUNT = 1 << 6, /* Trigger automounts via open_tree(). Requires O_PATH. */
        XO_AUTO_RW_RO        = 1 << 7, /* Open in O_RDWR mode if possible, O_RDONLY if not */
} XOpenFlags;

పూర్ణాంకం open_mkdir_at_నిండిన_label(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, XOpenFlags xopen_flags, mode_t బహుళకం, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం open_mkdir_at_నిండిన(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, XOpenFlags xopen_flags, mode_t బహుళకం) {
        తిరిగి open_mkdir_at_నిండిన_label(dirfd, మార్గం, flags, xopen_flags, బహుళకం, /* label_context= */ ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం open_mkdir_at(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, mode_t బహుళకం) {
        తిరిగి open_mkdir_at_నిండిన(dirfd, మార్గం, flags, 0, బహుళకం);
}
స్థిర అంతఃవరుస పూర్ణాంకం open_mkdir(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, mode_t బహుళకం) {
        తిరిగి open_mkdir_at_నిండిన(AT_FDCWD, మార్గం, flags, 0, బహుళకం);
}

పూర్ణాంకం openat_report_new(పూర్ణాంకం dirfd, స్థిరరాశి అక్షరం *pathname, పూర్ణాంకం flags, mode_t బహుళకం, bool *ret_newly_created);

పూర్ణాంకం xopenat_నిండిన_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, XOpenFlags xopen_flags, mode_t బహుళకం, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం xopenat_నిండిన(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, XOpenFlags xopen_flags, mode_t బహుళకం) {
        తిరిగి xopenat_నిండిన_label(dir_fd, మార్గం, open_flags, xopen_flags, బహుళకం, /* label_context= */ ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం xopenat(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags) {
        తిరిగి xopenat_నిండిన(dir_fd, మార్గం, open_flags, 0, బహుళకం_INVALID);
}

పూర్ణాంకం xopenat_తాళం_నిండిన_label(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, XOpenFlags xopen_flags, mode_t బహుళకం, తాళం_Type locktype, పూర్ణాంకం operation, LabelContext *label_context);
స్థిర అంతఃవరుస పూర్ణాంకం xopenat_తాళం_నిండిన(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, XOpenFlags xopen_flags, mode_t బహుళకం, తాళం_Type locktype, పూర్ణాంకం operation) {
        తిరిగి xopenat_తాళం_నిండిన_label(dir_fd, మార్గం, open_flags, xopen_flags, బహుళకం, locktype, operation, /* label_context= */ ఖాళీ);
}
స్థిర అంతఃవరుస పూర్ణాంకం xopenat_తాళం(పూర్ణాంకం dir_fd, స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం open_flags, తాళం_Type locktype, పూర్ణాంకం operation) {
        తిరిగి xopenat_తాళం_నిండిన(dir_fd, మార్గం, open_flags, 0, 0, locktype, operation);
}

పూర్ణాంకం link_fd(పూర్ణాంకం fd, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath);

పూర్ణాంకం linkat_replace(పూర్ణాంకం olddirfd, స్థిరరాశి అక్షరం *oldpath, పూర్ణాంకం newdirfd, స్థిరరాశి అక్షరం *newpath);

స్థిర అంతఃవరుస పూర్ణాంకం at_flags_normalize_nofollow(పూర్ణాంకం flags) {
        ఒకవేళ (FLAGS_సమితి(flags, AT_SYMLINK_FOLLOW)) {
                ప్రతిజ్ఞా(!FLAGS_సమితి(flags, AT_SYMLINK_NOFOLLOW));
                flags &= ~AT_SYMLINK_FOLLOW;
        } లేకపోతే
                flags |= AT_SYMLINK_NOFOLLOW;
        తిరిగి flags;
}

స్థిర అంతఃవరుస పూర్ణాంకం at_flags_normalize_follow(పూర్ణాంకం flags) {
        ఒకవేళ (FLAGS_సమితి(flags, AT_SYMLINK_NOFOLLOW)) {
                ప్రతిజ్ఞా(!FLAGS_సమితి(flags, AT_SYMLINK_FOLLOW));
                flags &= ~AT_SYMLINK_NOFOLLOW;
        } లేకపోతే
                flags |= AT_SYMLINK_FOLLOW;
        తిరిగి flags;
}
