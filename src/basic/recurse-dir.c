/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dirent-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mountpoint-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "recurse-dir.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sort-util.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_RECURSION_MAX 100

ਸਥਿਰ ਪੂਰਨ ਛਾਂਟੋ_func(ਬਣਤਰ dirent * ਅਚਲ *a, ਬਣਤਰ dirent * ਅਚਲ *b) {
        ਪ੍ਰਤਿਜ੍ਞਾ(a);
        ਪ੍ਰਤਿਜ੍ਞਾ(b);

        ਵਾਪਸ strcmp((*a)->d_name, (*b)->d_name);
}

ਸਥਿਰ bool ignore_dirent(ਅਚਲ ਬਣਤਰ dirent *de, RecurseDirFlags flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ਜੇ ((flags & RECURSE_DIR_MUST_BE_ਨਕਾਬ) != 0) {
                RecurseDirFlags f;

                ਚੋਣ (de->d_type) {

                ਹਾਲਤ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ਰੋਕੋ;

                ਹਾਲਤ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ਰੋਕੋ;

                ਹਾਲਤ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ਰੋਕੋ;

                ਹਾਲਤ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ਰੋਕੋ;

                ਮੂਲ:
                        ਵਾਪਸ true;
                }

                ਜੇ (!FLAGS_ਸੈੱਟ(flags, f))
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

ਪੂਰਨ readdir_all(ਪੂਰਨ dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ਨਿਰਾ;
        DirectoryEntries *nde;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0);

        ਜੇ ((flags & RECURSE_DIR_MUST_BE_ਨਕਾਬ) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, ਬਫਰ) <= SIZE_MAX - DIRENT_ਮਾਪ_MAX * 8);
        de = ਸਮ੍ਰਿਤੀ_ਦਿਓ(offsetof(DirectoryEntries, ਬਫਰ) + DIRENT_ਮਾਪ_MAX * 8);
        ਜੇ (!de)
                ਵਾਪਸ -ENOMEM;

        de->ਬਫਰ_ਮਾਪ = 0;
        ਲਈ (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ਪ੍ਰਤਿਜ੍ਞਾ(MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(de) >= offsetof(DirectoryEntries, ਬਫਰ));
                bs = MIN(MALLOC_SIZEOF_ਸੁਰੱਖਿਅਤ(de) - offsetof(DirectoryEntries, ਬਫਰ), (size_t) SSIZE_MAX);
                ਪ੍ਰਤਿਜ੍ਞਾ(bs > de->ਬਫਰ_ਮਾਪ);

                n = getdents64(dir_fd, (ਬਣਤਰ dirent*) ((uint8_t*) de->ਬਫਰ + de->ਬਫਰ_ਮਾਪ), bs - de->ਬਫਰ_ਮਾਪ);
                ਜੇ (n < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (n == 0)
                        ਰੋਕੋ;

                msan_unpoison((uint8_t*) de->ਬਫਰ + de->ਬਫਰ_ਮਾਪ, n);

                de->ਬਫਰ_ਮਾਪ += n;

                ਜੇ (de->ਬਫਰ_ਮਾਪ < bs - DIRENT_ਮਾਪ_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ਜਾਰੀ;

                ਜੇ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, ਬਫਰ))
                        ਵਾਪਸ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, ਬਫਰ))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, ਬਫਰ) : bs * 2;

                nde = realloc(de, bs);
                ਜੇ (!nde)
                        ਵਾਪਸ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ਬਣਤਰ dirent *ਇੰਦਰਾਜ;
        FOREACH_DIRENT_ਵਿੱਚ_ਬਫਰ(ਇੰਦਰਾਜ, de->ਬਫਰ, de->ਬਫਰ_ਮਾਪ) {
                ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, ਇੰਦਰਾਜ);
                        ਜੇ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ਜਾਰੀ;
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                }

                ਜੇ (ignore_dirent(ਇੰਦਰਾਜ, flags))
                        ਜਾਰੀ;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, ਬਫਰ) + de->ਬਫਰ_ਮਾਪ);
        ਜੇ (!INC_ਸੁਰੱਖਿਅਤ(&sz, ਆਕਾਰ(ਬਣਤਰ dirent*) * de->n_entries))
                ਵਾਪਸ -ENOMEM;

        nde = realloc(de, sz);
        ਜੇ (!nde)
                ਵਾਪਸ -ENOMEM;
        de = nde;

        de->entries = (ਬਣਤਰ dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, ਬਫਰ) + de->ਬਫਰ_ਮਾਪ));

        j = 0;
        FOREACH_DIRENT_ਵਿੱਚ_ਬਫਰ(ਇੰਦਰਾਜ, de->ਬਫਰ, de->ਬਫਰ_ਮਾਪ) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_ENSURE_TYPE) && ਇੰਦਰਾਜ->d_type == DT_UNKNOWN)
                        ਜਾਰੀ;

                ਜੇ (ignore_dirent(ਇੰਦਰਾਜ, flags))
                        ਜਾਰੀ;

                de->entries[j++] = ਇੰਦਰਾਜ;
        }
        ਪ੍ਰਤਿਜ੍ਞਾ(j == de->n_entries);

        ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_ਛਾਂਟੋ))
                typesafe_qsort(de->entries, de->n_entries, ਛਾਂਟੋ_func);

        ਜੇ (ret)
                *ret = TAKE_PTR(de);

        ਵਾਪਸ 0;
}

ਪੂਰਨ readdir_all_at(ਪੂਰਨ fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ ਪੂਰਨ dir_fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, ਰਸਤਾ, O_DIRECTORY|O_CLOEXEC);
        ਜੇ (dir_fd < 0)
                ਵਾਪਸ dir_fd;

        ਵਾਪਸ readdir_all(dir_fd, flags, ret);
}

ਪੂਰਨ recurse_dir(
                ਪੂਰਨ dir_fd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ statx_ਨਕਾਬ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਡੂੰਘਾਈ_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ਖਾਲੀ *userdata) {

        _cleanup_free_ DirectoryEntries *de = ਨਿਰਾ;
        ਬਣਤਰ statx ਜੜ੍ਹ_sx;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dir_fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ਪ੍ਰਤਿਜ੍ਞਾ((flags & RECURSE_DIR_MUST_BE_ਨਕਾਬ) == 0 || FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ਜੇ (n_ਡੂੰਘਾਈ_max == 0)
                ਵਾਪਸ -EOVERFLOW;
        ਜੇ (n_ਡੂੰਘਾਈ_max == UINT_MAX) /* special marker for "default" */
                n_ਡੂੰਘਾਈ_max = DEFAULT_RECURSION_MAX;

        ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_TOPLEVEL)) {
                ਜੇ (statx_ਨਕਾਬ != 0) {
                        ਜੇ (statx(dir_fd, "", AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, statx_ਨਕਾਬ, &ਜੜ੍ਹ_sx) < 0)
                                ਵਾਪਸ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         ਰਸਤਾ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ਨਿਰਾ, /* we have no dirent */
                         statx_ਨਕਾਬ != 0 ? &ਜੜ੍ਹ_sx : ਨਿਰਾ,
                         userdata);
                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                        ਵਾਪਸ 0;
                ਜੇ (r != RECURSE_DIR_CONTINUE)
                        ਵਾਪਸ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        FOREACH_ARRAY(ਇੰਦਰਾਜ, de->entries, de->n_entries) {
                ਬਣਤਰ dirent *i = *ਇੰਦਰਾਜ;
                _cleanup_close_ ਪੂਰਨ inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ ਅੱਖਰ *joined = ਨਿਰਾ;
                ਬਣਤਰ statx sx;
                bool sx_valid = false;
                ਅਚਲ ਅੱਖਰ *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ਜੇ (ਰਸਤਾ) {
                        joined = ਰਸਤਾ_join(ਰਸਤਾ, i->d_name);
                        ਜੇ (!joined)
                                ਵਾਪਸ -ENOMEM;

                        p = joined;
                } ਨਹੀਂਤਾਂ
                        p = i->d_name;

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ਜੇ (subdir_fd < 0) {
                                ਜੇ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ਜਾਰੀ;

                                /* If it is a subdir but we failed to open it, then fail */
                                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ਪ੍ਰਤਿਜ੍ਞਾ(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_ਅਧਾਰ);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_ਅਧਾਰ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ਨਿਰਾ,
                                                 userdata);
                                        ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ਰੋਕੋ;
                                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                                ਵਾਪਸ r;

                                        ਜਾਰੀ;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ਨਹੀਂਤਾਂ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ਜੇ (statx_ਨਕਾਬ != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ਜੇ (statx(subdir_fd, "", AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, statx_ਨਕਾਬ, &sx) < 0)
                                                ਵਾਪਸ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ਜੇ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ਜੇ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_ਰਸਤਾ|O_NOFOLLOW|O_CLOEXEC);
                                ਜੇ (inode_fd < 0) {
                                        ਜੇ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ਜਾਰੀ;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ਪ੍ਰਤਿਜ੍ਞਾ(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_ਅਧਾਰ);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_ਅਧਾਰ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ਨਿਰਾ,
                                                 userdata);
                                        ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ਰੋਕੋ;
                                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                                ਵਾਪਸ r;

                                        ਜਾਰੀ;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ਜੇ (statx(inode_fd, "", AT_ਖਾਲੀ_ਥਾਵਾਂ_ਰਸਤਾ, statx_ਨਕਾਬ | STATX_TYPE, &sx) < 0)
                                        ਵਾਪਸ -errno;

                                ਪ੍ਰਤਿਜ੍ਞਾ(sx.stx_ਨਕਾਬ & STATX_TYPE);
                                sx_valid = true;

                                ਜੇ (S_ISDIR(sx.stx_ਬਹੁਲਕ)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ਜੇ (subdir_fd < 0)
                                                ਵਾਪਸ subdir_fd;

                                        inode_fd = ਸੁਰੱਖਿਅਤ_close(inode_fd);
                                }

                        } ਨਹੀਂਤਾਂ ਜੇ (statx_ਨਕਾਬ != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ਜੇ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_ਨਕਾਬ | STATX_TYPE, &sx) < 0) {
                                        ਜੇ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ਜਾਰੀ;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ਪ੍ਰਤਿਜ੍ਞਾ(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_ਅਧਾਰ);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_ਅਧਾਰ + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ਨਿਰਾ,
                                                 userdata);
                                        ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ਰੋਕੋ;
                                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                                ਵਾਪਸ r;

                                        ਜਾਰੀ;
                                }

                                ਪ੍ਰਤਿਜ੍ਞਾ(sx.stx_ਨਕਾਬ & STATX_TYPE);
                                sx_valid = true;

                                ਜੇ (S_ISDIR(sx.stx_ਬਹੁਲਕ)) {
                                        /* So it suddenly is a directory, but we couldn't open it as such
                                         * earlier?  That is weird, and probably means somebody is racing
                                         * against us. We could of course retry and open it as a directory
                                         * again, but the chance to win here is limited. Hence, let's
                                         * propagate this as EISDIR error instead. That way we make this
                                         * something that can be reasonably handled, even though we give the
                                         * guarantee that RECURSE_DIR_ENTRY is strictly issued for
                                         * non-directory dirents. */

                                        log_debug("Non-directory entry '%s' suddenly became a directory.", p);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_ਅਧਾਰ + EISDIR,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ਨਿਰਾ,
                                                 userdata);
                                        ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ਰੋਕੋ;
                                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                                ਵਾਪਸ r;

                                        ਜਾਰੀ;
                                }
                        }
                }

                ਜੇ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ਜੇ (sx.stx_ਨਕਾਬ & STATX_TYPE) {
                                ਪ੍ਰਤਿਜ੍ਞਾ((subdir_fd < 0) == !S_ISDIR(sx.stx_ਬਹੁਲਕ));
                                i->d_type = IFTODT(sx.stx_ਬਹੁਲਕ);
                        }

                        ਜੇ (sx.stx_ਨਕਾਬ & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ਜੇ (subdir_fd >= 0) {
                        ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ਜੇ (sx_valid && FLAGS_ਸੈੱਟ(sx.stx_ਗੁਣ_ਨਕਾਬ, STATX_ATTR_MOUNT_ਜੜ੍ਹ))
                                        is_mount = FLAGS_ਸੈੱਟ(sx.stx_ਗੁਣ, STATX_ATTR_MOUNT_ਜੜ੍ਹ);
                                ਨਹੀਂਤਾਂ {
                                        r = is_mount_ਬਿੰਦੂ_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ਜੇ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ਜੇ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_ਨਕਾਬ != 0 ? &sx : ਨਿਰਾ, /* only pass sx if user asked for it */
                                                 userdata);
                                        ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ਰੋਕੋ;
                                        ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                                ਵਾਪਸ r;

                                        ਜਾਰੀ;
                                }
                        }

                        ਜੇ (n_ਡੂੰਘਾਈ_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_ਡੂੰਘਾਈ,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ਨਕਾਬ != 0 ? &sx : ਨਿਰਾ, /* only pass sx if user asked for it */
                                         userdata);
                                ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ਰੋਕੋ;
                                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ))
                                        ਵਾਪਸ r;

                                ਜਾਰੀ;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_ਨਕਾਬ != 0 ? &sx : ਨਿਰਾ, /* only pass sx if user asked for it */
                                 userdata);
                        ਜੇ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_ਨਕਾਬ,
                                                n_ਡੂੰਘਾਈ_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ਜੇ (r != 0)
                                        ਵਾਪਸ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_ਨਕਾਬ != 0 ? &sx : ਨਿਰਾ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ਨਹੀਂਤਾਂ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ਇੰਦਰਾਜ,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_ਨਕਾਬ != 0 ? &sx : ਨਿਰਾ, /* only pass sx if user asked for it */
                                 userdata);

                ਜੇ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ਰੋਕੋ;

                ਜੇ (ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        ਪੂਰਨ f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = ਸੁਰੱਖਿਅਤ_close(subdir_fd);
                        inode_fd = ਸੁਰੱਖਿਅਤ_close(inode_fd);

                        ਜੇ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ਜੇ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        ਵਾਪਸ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ਨਹੀਂਤਾਂ ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ, RECURSE_DIR_CONTINUE))
                        ਵਾਪਸ r;
        }

        ਜੇ (FLAGS_ਸੈੱਟ(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         ਰਸਤਾ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ਨਿਰਾ, /* we have no dirent */
                         statx_ਨਕਾਬ != 0 ? &ਜੜ੍ਹ_sx : ਨਿਰਾ,
                         userdata);
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ਇੰਦਰਾਜ, RECURSE_DIR_CONTINUE))
                        ਵਾਪਸ r;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ recurse_dir_at(
                ਪੂਰਨ atfd,
                ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ statx_ਨਕਾਬ,
                ਬਿਨਾਨਿਸ਼ਾਨੀ n_ਡੂੰਘਾਈ_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ਖਾਲੀ *userdata) {

        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਪ੍ਰਤਿਜ੍ਞਾ(atfd >= 0 || atfd == AT_FDCWD);
        ਪ੍ਰਤਿਜ੍ਞਾ(func);

        fd = openat(atfd, ਰਸਤਾ ?: ".", O_DIRECTORY|O_CLOEXEC);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ recurse_dir(fd, ਰਸਤਾ, statx_ਨਕਾਬ, n_ਡੂੰਘਾਈ_max, flags, func, userdata);
}
