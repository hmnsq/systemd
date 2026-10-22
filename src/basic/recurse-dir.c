/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "dirent-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "mountpoint-util.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "recurse-dir.h"
#ಅನ್ತರ್ಭಾವ "sort-util.h"

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_RECURSION_MAX 100

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ವಿಂಗಡಿಸು_func(ರಚನೆ dirent * ಸ್ಥಿರಾಂಕ *a, ರಚನೆ dirent * ಸ್ಥಿರಾಂಕ *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಹಿಂತಿರುಗಿ strcmp((*a)->d_name, (*b)->d_name);
}

ಸ್ಥಿರ bool ignore_dirent(ಸ್ಥಿರಾಂಕ ರಚನೆ dirent *de, RecurseDirFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ಆದರೆ ((flags & RECURSE_DIR_MUST_BE_ಮುಖವಾಡ) != 0) {
                RecurseDirFlags f;

                ಆಯ್ಕೆ (de->d_type) {

                ಸ್ಥಿತಿ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ನಿಲ್ಲಿಸು;

                ಪೂರ್ವನಿಯೋಜಿತ:
                        ಹಿಂತಿರುಗಿ true;
                }

                ಆದರೆ (!FLAGS_ಗಣ(flags, f))
                        ಹಿಂತಿರುಗಿ true;
        }

        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

ಪೂರ್ಣಾಂಕ readdir_all(ಪೂರ್ಣಾಂಕ dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ಖಾಲಿ;
        DirectoryEntries *nde;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);

        ಆದರೆ ((flags & RECURSE_DIR_MUST_BE_ಮುಖವಾಡ) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, ಬಫರ್) <= SIZE_MAX - DIRENT_ಗಾತ್ರ_MAX * 8);
        de = ಸ್ಮರಣೆ_ಕೊಡು(offsetof(DirectoryEntries, ಬಫರ್) + DIRENT_ಗಾತ್ರ_MAX * 8);
        ಆದರೆ (!de)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        de->ಬಫರ್_ಗಾತ್ರ = 0;
        ಗಾಗಿ (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ಪ್ರತಿಜ್ಞಾ(MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(de) >= offsetof(DirectoryEntries, ಬಫರ್));
                bs = MIN(MALLOC_SIZEOF_ಸುರಕ್ಷಿತ(de) - offsetof(DirectoryEntries, ಬಫರ್), (size_t) SSIZE_MAX);
                ಪ್ರತಿಜ್ಞಾ(bs > de->ಬಫರ್_ಗಾತ್ರ);

                n = getdents64(dir_fd, (ರಚನೆ dirent*) ((uint8_t*) de->ಬಫರ್ + de->ಬಫರ್_ಗಾತ್ರ), bs - de->ಬಫರ್_ಗಾತ್ರ);
                ಆದರೆ (n < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (n == 0)
                        ನಿಲ್ಲಿಸು;

                msan_unpoison((uint8_t*) de->ಬಫರ್ + de->ಬಫರ್_ಗಾತ್ರ, n);

                de->ಬಫರ್_ಗಾತ್ರ += n;

                ಆದರೆ (de->ಬಫರ್_ಗಾತ್ರ < bs - DIRENT_ಗಾತ್ರ_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ಮುಂದುವರಿ;

                ಆದರೆ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, ಬಫರ್))
                        ಹಿಂತಿರುಗಿ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, ಬಫರ್))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, ಬಫರ್) : bs * 2;

                nde = realloc(de, bs);
                ಆದರೆ (!nde)
                        ಹಿಂತಿರುಗಿ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ರಚನೆ dirent *ನಮೂದು;
        FOREACH_DIRENT_ಒಳ_ಬಫರ್(ನಮೂದು, de->ಬಫರ್, de->ಬಫರ್_ಗಾತ್ರ) {
                ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, ನಮೂದು);
                        ಆದರೆ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ಮುಂದುವರಿ;
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (ignore_dirent(ನಮೂದು, flags))
                        ಮುಂದುವರಿ;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, ಬಫರ್) + de->ಬಫರ್_ಗಾತ್ರ);
        ಆದರೆ (!INC_ಸುರಕ್ಷಿತ(&sz, ಗಾತ್ರ(ರಚನೆ dirent*) * de->n_entries))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        nde = realloc(de, sz);
        ಆದರೆ (!nde)
                ಹಿಂತಿರುಗಿ -ENOMEM;
        de = nde;

        de->entries = (ರಚನೆ dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, ಬಫರ್) + de->ಬಫರ್_ಗಾತ್ರ));

        j = 0;
        FOREACH_DIRENT_ಒಳ_ಬಫರ್(ನಮೂದು, de->ಬಫರ್, de->ಬಫರ್_ಗಾತ್ರ) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_ENSURE_TYPE) && ನಮೂದು->d_type == DT_UNKNOWN)
                        ಮುಂದುವರಿ;

                ಆದರೆ (ignore_dirent(ನಮೂದು, flags))
                        ಮುಂದುವರಿ;

                de->entries[j++] = ನಮೂದು;
        }
        ಪ್ರತಿಜ್ಞಾ(j == de->n_entries);

        ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_ವಿಂಗಡಿಸು))
                typesafe_qsort(de->entries, de->n_entries, ವಿಂಗಡಿಸು_func);

        ಆದರೆ (ret)
                *ret = TAKE_PTR(de);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ readdir_all_at(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ dir_fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, ಮಾರ್ಗ, O_DIRECTORY|O_CLOEXEC);
        ಆದರೆ (dir_fd < 0)
                ಹಿಂತಿರುಗಿ dir_fd;

        ಹಿಂತಿರುಗಿ readdir_all(dir_fd, flags, ret);
}

ಪೂರ್ಣಾಂಕ recurse_dir(
                ಪೂರ್ಣಾಂಕ dir_fd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಚಿಹ್ನೆರಹಿತ statx_ಮುಖವಾಡ,
                ಚಿಹ್ನೆರಹಿತ n_ಆಳ_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ಶೂನ್ಯ *userdata) {

        _cleanup_free_ DirectoryEntries *de = ಖಾಲಿ;
        ರಚನೆ statx ಬೇರು_sx;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ಪ್ರತಿಜ್ಞಾ((flags & RECURSE_DIR_MUST_BE_ಮುಖವಾಡ) == 0 || FLAGS_ಗಣ(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ಆದರೆ (n_ಆಳ_max == 0)
                ಹಿಂತಿರುಗಿ -EOVERFLOW;
        ಆದರೆ (n_ಆಳ_max == UINT_MAX) /* special marker for "default" */
                n_ಆಳ_max = DEFAULT_RECURSION_MAX;

        ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_TOPLEVEL)) {
                ಆದರೆ (statx_ಮುಖವಾಡ != 0) {
                        ಆದರೆ (statx(dir_fd, "", AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, statx_ಮುಖವಾಡ, &ಬೇರು_sx) < 0)
                                ಹಿಂತಿರುಗಿ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         ಮಾರ್ಗ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ಖಾಲಿ, /* we have no dirent */
                         statx_ಮುಖವಾಡ != 0 ? &ಬೇರು_sx : ಖಾಲಿ,
                         userdata);
                ಆದರೆ (ಒಳ_ಗಣ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ನಮೂದು))
                        ಹಿಂತಿರುಗಿ 0;
                ಆದರೆ (r != RECURSE_DIR_CONTINUE)
                        ಹಿಂತಿರುಗಿ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        FOREACH_ARRAY(ನಮೂದು, de->entries, de->n_entries) {
                ರಚನೆ dirent *i = *ನಮೂದು;
                _cleanup_close_ ಪೂರ್ಣಾಂಕ inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ ಅಕ್ಷರ *joined = ಖಾಲಿ;
                ರಚನೆ statx sx;
                bool sx_valid = false;
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ಆದರೆ (ಮಾರ್ಗ) {
                        joined = ಮಾರ್ಗ_join(ಮಾರ್ಗ, i->d_name);
                        ಆದರೆ (!joined)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        p = joined;
                } ಇಲ್ಲದಿದ್ದರೆ
                        p = i->d_name;

                ಆದರೆ (ಒಳ_ಗಣ(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ಆದರೆ (subdir_fd < 0) {
                                ಆದರೆ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ಮುಂದುವರಿ;

                                /* If it is a subdir but we failed to open it, then fail */
                                ಆದರೆ (!ಒಳ_ಗಣ(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_ತಳ);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_ತಳ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ಖಾಲಿ,
                                                 userdata);
                                        ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ನಿಲ್ಲಿಸು;
                                        ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                                ಹಿಂತಿರುಗಿ r;

                                        ಮುಂದುವರಿ;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ಇಲ್ಲದಿದ್ದರೆ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ಆದರೆ (statx_ಮುಖವಾಡ != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ಆದರೆ (statx(subdir_fd, "", AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, statx_ಮುಖವಾಡ, &sx) < 0)
                                                ಹಿಂತಿರುಗಿ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ಆದರೆ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ಆದರೆ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_ಮಾರ್ಗ|O_NOFOLLOW|O_CLOEXEC);
                                ಆದರೆ (inode_fd < 0) {
                                        ಆದರೆ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ಮುಂದುವರಿ;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_ತಳ);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_ತಳ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ಖಾಲಿ,
                                                 userdata);
                                        ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ನಿಲ್ಲಿಸು;
                                        ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                                ಹಿಂತಿರುಗಿ r;

                                        ಮುಂದುವರಿ;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ಆದರೆ (statx(inode_fd, "", AT_ಖಾಲಿಗಳು_ಮಾರ್ಗ, statx_ಮುಖವಾಡ | STATX_TYPE, &sx) < 0)
                                        ಹಿಂತಿರುಗಿ -errno;

                                ಪ್ರತಿಜ್ಞಾ(sx.stx_ಮುಖವಾಡ & STATX_TYPE);
                                sx_valid = true;

                                ಆದರೆ (S_ISDIR(sx.stx_ಬಹುಲಕ)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ಆದರೆ (subdir_fd < 0)
                                                ಹಿಂತಿರುಗಿ subdir_fd;

                                        inode_fd = ಸುರಕ್ಷಿತ_close(inode_fd);
                                }

                        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (statx_ಮುಖವಾಡ != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ಆದರೆ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_ಮುಖವಾಡ | STATX_TYPE, &sx) < 0) {
                                        ಆದರೆ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ಮುಂದುವರಿ;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_ತಳ);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_ತಳ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ಖಾಲಿ,
                                                 userdata);
                                        ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ನಿಲ್ಲಿಸು;
                                        ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                                ಹಿಂತಿರುಗಿ r;

                                        ಮುಂದುವರಿ;
                                }

                                ಪ್ರತಿಜ್ಞಾ(sx.stx_ಮುಖವಾಡ & STATX_TYPE);
                                sx_valid = true;

                                ಆದರೆ (S_ISDIR(sx.stx_ಬಹುಲಕ)) {
                                        /* So it suddenly is a directory, but we couldn't open it as such
                                         * earlier?  That is weird, and probably means somebody is racing
                                         * against us. We could of course retry and open it as a directory
                                         * again, but the chance to win here is limited. Hence, let's
                                         * propagate this as EISDIR error instead. That way we make this
                                         * something that can be reasonably handled, even though we give the
                                         * guarantee that RECURSE_DIR_ENTRY is strictly issued for
                                         * non-directory dirents. */

                                        log_debug("Non-directory entry '%s' suddenly became a directory.", p);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_ತಳ + EISDIR,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ಖಾಲಿ,
                                                 userdata);
                                        ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ನಿಲ್ಲಿಸು;
                                        ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                                ಹಿಂತಿರುಗಿ r;

                                        ಮುಂದುವರಿ;
                                }
                        }
                }

                ಆದರೆ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ಆದರೆ (sx.stx_ಮುಖವಾಡ & STATX_TYPE) {
                                ಪ್ರತಿಜ್ಞಾ((subdir_fd < 0) == !S_ISDIR(sx.stx_ಬಹುಲಕ));
                                i->d_type = IFTODT(sx.stx_ಬಹುಲಕ);
                        }

                        ಆದರೆ (sx.stx_ಮುಖವಾಡ & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ಆದರೆ (subdir_fd >= 0) {
                        ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ಆದರೆ (sx_valid && FLAGS_ಗಣ(sx.stx_ಗುಣಲಕ್ಷಣಗಳು_ಮುಖವಾಡ, STATX_ATTR_MOUNT_ಬೇರು))
                                        is_mount = FLAGS_ಗಣ(sx.stx_ಗುಣಲಕ್ಷಣಗಳು, STATX_ATTR_MOUNT_ಬೇರು);
                                ಇಲ್ಲದಿದ್ದರೆ {
                                        r = is_mount_ಬಿಂದು_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ಆದರೆ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ಆದರೆ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_ಮುಖವಾಡ != 0 ? &sx : ಖಾಲಿ, /* only pass sx if user asked for it */
                                                 userdata);
                                        ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ನಿಲ್ಲಿಸು;
                                        ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                                ಹಿಂತಿರುಗಿ r;

                                        ಮುಂದುವರಿ;
                                }
                        }

                        ಆದರೆ (n_ಆಳ_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_ಆಳ,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ಮುಖವಾಡ != 0 ? &sx : ಖಾಲಿ, /* only pass sx if user asked for it */
                                         userdata);
                                ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ನಿಲ್ಲಿಸು;
                                ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ನಮೂದು))
                                        ಹಿಂತಿರುಗಿ r;

                                ಮುಂದುವರಿ;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_ಮುಖವಾಡ != 0 ? &sx : ಖಾಲಿ, /* only pass sx if user asked for it */
                                 userdata);
                        ಆದರೆ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_ಮುಖವಾಡ,
                                                n_ಆಳ_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ಆದರೆ (r != 0)
                                        ಹಿಂತಿರುಗಿ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ಮುಖವಾಡ != 0 ? &sx : ಖಾಲಿ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ಇಲ್ಲದಿದ್ದರೆ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ನಮೂದು,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_ಮುಖವಾಡ != 0 ? &sx : ಖಾಲಿ, /* only pass sx if user asked for it */
                                 userdata);

                ಆದರೆ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ನಿಲ್ಲಿಸು;

                ಆದರೆ (ಒಳ_ಗಣ(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        ಪೂರ್ಣಾಂಕ f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = ಸುರಕ್ಷಿತ_close(subdir_fd);
                        inode_fd = ಸುರಕ್ಷಿತ_close(inode_fd);

                        ಆದರೆ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ಆದರೆ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        ಹಿಂತಿರುಗಿ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_SKIP_ನಮೂದು, RECURSE_DIR_CONTINUE))
                        ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (FLAGS_ಗಣ(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         ಮಾರ್ಗ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ಖಾಲಿ, /* we have no dirent */
                         statx_ಮುಖವಾಡ != 0 ? &ಬೇರು_sx : ಖಾಲಿ,
                         userdata);
                ಆದರೆ (!ಒಳ_ಗಣ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ನಮೂದು, RECURSE_DIR_CONTINUE))
                        ಹಿಂತಿರುಗಿ r;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ recurse_dir_at(
                ಪೂರ್ಣಾಂಕ atfd,
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                ಚಿಹ್ನೆರಹಿತ statx_ಮುಖವಾಡ,
                ಚಿಹ್ನೆರಹಿತ n_ಆಳ_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ಶೂನ್ಯ *userdata) {

        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(atfd >= 0 || atfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(func);

        fd = openat(atfd, ಮಾರ್ಗ ?: ".", O_DIRECTORY|O_CLOEXEC);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ recurse_dir(fd, ಮಾರ್ಗ, statx_ಮುಖವಾಡ, n_ಆಳ_max, flags, func, userdata);
}
