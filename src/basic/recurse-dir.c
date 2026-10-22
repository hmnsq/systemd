/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "dirent-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "mountpoint-util.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "recurse-dir.h"
#అన్తర్భావ "sort-util.h"

#నిర్వచించు DEFAULT_RECURSION_MAX 100

స్థిర పూర్ణాంకం క్రమీకరించు_func(నిర్మాణం dirent * స్థిరరాశి *a, నిర్మాణం dirent * స్థిరరాశి *b) {
        ప్రతిజ్ఞా(a);
        ప్రతిజ్ఞా(b);

        తిరిగి strcmp((*a)->d_name, (*b)->d_name);
}

స్థిర bool ignore_dirent(స్థిరరాశి నిర్మాణం dirent *de, RecurseDirFlags flags) {
        ప్రతిజ్ఞా(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ఒకవేళ ((flags & RECURSE_DIR_MUST_BE_ముసుగు) != 0) {
                RecurseDirFlags f;

                ఎంపిక (de->d_type) {

                స్థితి DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ఆపు;

                స్థితి DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ఆపు;

                స్థితి DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ఆపు;

                స్థితి DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ఆపు;

                అప్రమేయ:
                        తిరిగి true;
                }

                ఒకవేళ (!FLAGS_సమితి(flags, f))
                        తిరిగి true;
        }

        తిరిగి FLAGS_సమితి(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

పూర్ణాంకం readdir_all(పూర్ణాంకం dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ఖాళీ;
        DirectoryEntries *nde;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0);

        ఒకవేళ ((flags & RECURSE_DIR_MUST_BE_ముసుగు) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, బఫరు) <= SIZE_MAX - DIRENT_కొలత_MAX * 8);
        de = జ్ఞాపకం_ఇవ్వు(offsetof(DirectoryEntries, బఫరు) + DIRENT_కొలత_MAX * 8);
        ఒకవేళ (!de)
                తిరిగి -ENOMEM;

        de->బఫరు_కొలత = 0;
        కొరకు (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ప్రతిజ్ఞా(MALLOC_SIZEOF_సురక్షిత(de) >= offsetof(DirectoryEntries, బఫరు));
                bs = MIN(MALLOC_SIZEOF_సురక్షిత(de) - offsetof(DirectoryEntries, బఫరు), (size_t) SSIZE_MAX);
                ప్రతిజ్ఞా(bs > de->బఫరు_కొలత);

                n = getdents64(dir_fd, (నిర్మాణం dirent*) ((uint8_t*) de->బఫరు + de->బఫరు_కొలత), bs - de->బఫరు_కొలత);
                ఒకవేళ (n < 0)
                        తిరిగి -errno;
                ఒకవేళ (n == 0)
                        ఆపు;

                msan_unpoison((uint8_t*) de->బఫరు + de->బఫరు_కొలత, n);

                de->బఫరు_కొలత += n;

                ఒకవేళ (de->బఫరు_కొలత < bs - DIRENT_కొలత_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        కొనసాగు;

                ఒకవేళ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, బఫరు))
                        తిరిగి -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, బఫరు))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, బఫరు) : bs * 2;

                nde = realloc(de, bs);
                ఒకవేళ (!nde)
                        తిరిగి -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        నిర్మాణం dirent *నమోదు_2;
        FOREACH_DIRENT_లోపల_బఫరు(నమోదు_2, de->బఫరు, de->బఫరు_కొలత) {
                ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, నమోదు_2);
                        ఒకవేళ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                కొనసాగు;
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                }

                ఒకవేళ (ignore_dirent(నమోదు_2, flags))
                        కొనసాగు;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, బఫరు) + de->బఫరు_కొలత);
        ఒకవేళ (!INC_సురక్షిత(&sz, పరిమాణం(నిర్మాణం dirent*) * de->n_entries))
                తిరిగి -ENOMEM;

        nde = realloc(de, sz);
        ఒకవేళ (!nde)
                తిరిగి -ENOMEM;
        de = nde;

        de->entries = (నిర్మాణం dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, బఫరు) + de->బఫరు_కొలత));

        j = 0;
        FOREACH_DIRENT_లోపల_బఫరు(నమోదు_2, de->బఫరు, de->బఫరు_కొలత) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_ENSURE_TYPE) && నమోదు_2->d_type == DT_UNKNOWN)
                        కొనసాగు;

                ఒకవేళ (ignore_dirent(నమోదు_2, flags))
                        కొనసాగు;

                de->entries[j++] = నమోదు_2;
        }
        ప్రతిజ్ఞా(j == de->n_entries);

        ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_క్రమీకరించు))
                typesafe_qsort(de->entries, de->n_entries, క్రమీకరించు_func);

        ఒకవేళ (ret)
                *ret = TAKE_PTR(de);

        తిరిగి 0;
}

పూర్ణాంకం readdir_all_at(పూర్ణాంకం fd, స్థిరరాశి అక్షరం *మార్గం, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ పూర్ణాంకం dir_fd = -EBADF;

        ప్రతిజ్ఞా(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, మార్గం, O_DIRECTORY|O_CLOEXEC);
        ఒకవేళ (dir_fd < 0)
                తిరిగి dir_fd;

        తిరిగి readdir_all(dir_fd, flags, ret);
}

పూర్ణాంకం recurse_dir(
                పూర్ణాంకం dir_fd,
                స్థిరరాశి అక్షరం *మార్గం,
                గుర్తులేని statx_ముసుగు,
                గుర్తులేని n_లోతు_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                శూన్య *userdata) {

        _cleanup_free_ DirectoryEntries *de = ఖాళీ;
        నిర్మాణం statx మూలం_sx;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(dir_fd >= 0);
        ప్రతిజ్ఞా(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ప్రతిజ్ఞా((flags & RECURSE_DIR_MUST_BE_ముసుగు) == 0 || FLAGS_సమితి(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ఒకవేళ (n_లోతు_max == 0)
                తిరిగి -EOVERFLOW;
        ఒకవేళ (n_లోతు_max == UINT_MAX) /* special marker for "default" */
                n_లోతు_max = DEFAULT_RECURSION_MAX;

        ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_TOPLEVEL)) {
                ఒకవేళ (statx_ముసుగు != 0) {
                        ఒకవేళ (statx(dir_fd, "", AT_ఖాళీలు_మార్గం, statx_ముసుగు, &మూలం_sx) < 0)
                                తిరిగి -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         మార్గం,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ఖాళీ, /* we have no dirent */
                         statx_ముసుగు != 0 ? &మూలం_sx : ఖాళీ,
                         userdata);
                ఒకవేళ (లోపల_సమితి(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_నమోదు))
                        తిరిగి 0;
                ఒకవేళ (r != RECURSE_DIR_CONTINUE)
                        తిరిగి r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ఒకవేళ (r < 0)
                తిరిగి r;

        FOREACH_ARRAY(నమోదు_2, de->entries, de->n_entries) {
                నిర్మాణం dirent *i = *నమోదు_2;
                _cleanup_close_ పూర్ణాంకం inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ అక్షరం *joined = ఖాళీ;
                నిర్మాణం statx sx;
                bool sx_valid = false;
                స్థిరరాశి అక్షరం *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ఒకవేళ (మార్గం) {
                        joined = మార్గం_join(మార్గం, i->d_name);
                        ఒకవేళ (!joined)
                                తిరిగి -ENOMEM;

                        p = joined;
                } లేకపోతే
                        p = i->d_name;

                ఒకవేళ (లోపల_సమితి(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ఒకవేళ (subdir_fd < 0) {
                                ఒకవేళ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        కొనసాగు;

                                /* If it is a subdir but we failed to open it, then fail */
                                ఒకవేళ (!లోపల_సమితి(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ప్రతిజ్ఞా(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_పునాది);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_పునాది + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ఖాళీ,
                                                 userdata);
                                        ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ఆపు;
                                        ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                                తిరిగి r;

                                        కొనసాగు;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } లేకపోతే {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ఒకవేళ (statx_ముసుగు != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ఒకవేళ (statx(subdir_fd, "", AT_ఖాళీలు_మార్గం, statx_ముసుగు, &sx) < 0)
                                                తిరిగి -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ఒకవేళ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ఒకవేళ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_మార్గం|O_NOFOLLOW|O_CLOEXEC);
                                ఒకవేళ (inode_fd < 0) {
                                        ఒకవేళ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                కొనసాగు;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ప్రతిజ్ఞా(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_పునాది);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_పునాది + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ఖాళీ,
                                                 userdata);
                                        ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ఆపు;
                                        ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                                తిరిగి r;

                                        కొనసాగు;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ఒకవేళ (statx(inode_fd, "", AT_ఖాళీలు_మార్గం, statx_ముసుగు | STATX_TYPE, &sx) < 0)
                                        తిరిగి -errno;

                                ప్రతిజ్ఞా(sx.stx_ముసుగు & STATX_TYPE);
                                sx_valid = true;

                                ఒకవేళ (S_ISDIR(sx.stx_బహుళకం)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ఒకవేళ (subdir_fd < 0)
                                                తిరిగి subdir_fd;

                                        inode_fd = సురక్షిత_close(inode_fd);
                                }

                        } లేకపోతే ఒకవేళ (statx_ముసుగు != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ఒకవేళ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_ముసుగు | STATX_TYPE, &sx) < 0) {
                                        ఒకవేళ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                కొనసాగు;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ప్రతిజ్ఞా(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_పునాది);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_పునాది + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ఖాళీ,
                                                 userdata);
                                        ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ఆపు;
                                        ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                                తిరిగి r;

                                        కొనసాగు;
                                }

                                ప్రతిజ్ఞా(sx.stx_ముసుగు & STATX_TYPE);
                                sx_valid = true;

                                ఒకవేళ (S_ISDIR(sx.stx_బహుళకం)) {
                                        /* So it suddenly is a directory, but we couldn't open it as such
                                         * earlier?  That is weird, and probably means somebody is racing
                                         * against us. We could of course retry and open it as a directory
                                         * again, but the chance to win here is limited. Hence, let's
                                         * propagate this as EISDIR error instead. That way we make this
                                         * something that can be reasonably handled, even though we give the
                                         * guarantee that RECURSE_DIR_ENTRY is strictly issued for
                                         * non-directory dirents. */

                                        log_debug("Non-directory entry '%s' suddenly became a directory.", p);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_పునాది + EISDIR,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ఖాళీ,
                                                 userdata);
                                        ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ఆపు;
                                        ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                                తిరిగి r;

                                        కొనసాగు;
                                }
                        }
                }

                ఒకవేళ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ఒకవేళ (sx.stx_ముసుగు & STATX_TYPE) {
                                ప్రతిజ్ఞా((subdir_fd < 0) == !S_ISDIR(sx.stx_బహుళకం));
                                i->d_type = IFTODT(sx.stx_బహుళకం);
                        }

                        ఒకవేళ (sx.stx_ముసుగు & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ఒకవేళ (subdir_fd >= 0) {
                        ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ఒకవేళ (sx_valid && FLAGS_సమితి(sx.stx_లక్షణాలు_ముసుగు, STATX_ATTR_MOUNT_మూలం))
                                        is_mount = FLAGS_సమితి(sx.stx_లక్షణాలు, STATX_ATTR_MOUNT_మూలం);
                                లేకపోతే {
                                        r = is_mount_బిందువు_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ఒకవేళ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ఒకవేళ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_ముసుగు != 0 ? &sx : ఖాళీ, /* only pass sx if user asked for it */
                                                 userdata);
                                        ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ఆపు;
                                        ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                                తిరిగి r;

                                        కొనసాగు;
                                }
                        }

                        ఒకవేళ (n_లోతు_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_లోతు,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ముసుగు != 0 ? &sx : ఖాళీ, /* only pass sx if user asked for it */
                                         userdata);
                                ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ఆపు;
                                ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_నమోదు))
                                        తిరిగి r;

                                కొనసాగు;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_ముసుగు != 0 ? &sx : ఖాళీ, /* only pass sx if user asked for it */
                                 userdata);
                        ఒకవేళ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_ముసుగు,
                                                n_లోతు_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ఒకవేళ (r != 0)
                                        తిరిగి r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ముసుగు != 0 ? &sx : ఖాళీ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } లేకపోతే
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_నమోదు,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_ముసుగు != 0 ? &sx : ఖాళీ, /* only pass sx if user asked for it */
                                 userdata);

                ఒకవేళ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ఆపు;

                ఒకవేళ (లోపల_సమితి(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        పూర్ణాంకం f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = సురక్షిత_close(subdir_fd);
                        inode_fd = సురక్షిత_close(inode_fd);

                        ఒకవేళ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ఒకవేళ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        తిరిగి -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } లేకపోతే ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_SKIP_నమోదు, RECURSE_DIR_CONTINUE))
                        తిరిగి r;
        }

        ఒకవేళ (FLAGS_సమితి(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         మార్గం,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ఖాళీ, /* we have no dirent */
                         statx_ముసుగు != 0 ? &మూలం_sx : ఖాళీ,
                         userdata);
                ఒకవేళ (!లోపల_సమితి(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_నమోదు, RECURSE_DIR_CONTINUE))
                        తిరిగి r;
        }

        తిరిగి 0;
}

పూర్ణాంకం recurse_dir_at(
                పూర్ణాంకం atfd,
                స్థిరరాశి అక్షరం *మార్గం,
                గుర్తులేని statx_ముసుగు,
                గుర్తులేని n_లోతు_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                శూన్య *userdata) {

        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ప్రతిజ్ఞా(atfd >= 0 || atfd == AT_FDCWD);
        ప్రతిజ్ఞా(func);

        fd = openat(atfd, మార్గం ?: ".", O_DIRECTORY|O_CLOEXEC);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        తిరిగి recurse_dir(fd, మార్గం, statx_ముసుగు, n_లోతు_max, flags, func, userdata);
}
