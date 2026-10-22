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

#define DEFAULT_RECURSION_MAX 100

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sort_func(ಸಂರಚನಾ dirent * ಧ್ರುವ *a, ಸಂರಚನಾ dirent * ಧ್ರುವ *b) {
        ಪ್ರತಿಜ್ಞಾ(a);
        ಪ್ರತಿಜ್ಞಾ(b);

        ಪ್ರತ್ಯಾಗಮ strcmp((*a)->d_name, (*b)->d_name);
}

ಸ್ಥಿರ bool ignore_dirent(ಧ್ರುವ ಸಂರಚನಾ dirent *de, RecurseDirFlags flags) {
        ಪ್ರತಿಜ್ಞಾ(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ಯದಿ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                ವಿಕಲ್ಪ (de->d_type) {

                ಪಕ್ಷ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ಭಙ್ಗ;

                ಸ್ವಭಾವ:
                        ಪ್ರತ್ಯಾಗಮ true;
                }

                ಯದಿ (!FLAGS_SET(flags, f))
                        ಪ್ರತ್ಯಾಗಮ true;
        }

        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

ಪೂರ್ಣಾಙ್ಕ readdir_all(ಪೂರ್ಣಾಙ್ಕ dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ರಿಕ್ತ;
        DirectoryEntries *nde;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);

        ಯದಿ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        ಯದಿ (!de)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        de->buffer_size = 0;
        ಪ್ರತಿ (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ಪ್ರತಿಜ್ಞಾ(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                ಪ್ರತಿಜ್ಞಾ(bs > de->buffer_size);

                n = getdents64(dir_fd, (ಸಂರಚನಾ dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                ಯದಿ (n < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (n == 0)
                        ಭಙ್ಗ;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                ಯದಿ (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ಅನುವರ್ತ;

                ಯದಿ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        ಪ್ರತ್ಯಾಗಮ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                ಯದಿ (!nde)
                        ಪ್ರತ್ಯಾಗಮ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ಸಂರಚನಾ dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        ಯದಿ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ಅನುವರ್ತ;
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (ignore_dirent(entry, flags))
                        ಅನುವರ್ತ;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        ಯದಿ (!INC_SAFE(&sz, ಪರಿಮಾಣ(ಸಂರಚನಾ dirent*) * de->n_entries))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        nde = realloc(de, sz);
        ಯದಿ (!nde)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;
        de = nde;

        de->entries = (ಸಂರಚನಾ dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        ಅನುವರ್ತ;

                ಯದಿ (ignore_dirent(entry, flags))
                        ಅನುವರ್ತ;

                de->entries[j++] = entry;
        }
        ಪ್ರತಿಜ್ಞಾ(j == de->n_entries);

        ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        ಯದಿ (ret)
                *ret = TAKE_PTR(de);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ readdir_all_at(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ವರ್ಣ *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ dir_fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        ಯದಿ (dir_fd < 0)
                ಪ್ರತ್ಯಾಗಮ dir_fd;

        ಪ್ರತ್ಯಾಗಮ readdir_all(dir_fd, flags, ret);
}

ಪೂರ್ಣಾಙ್ಕ recurse_dir(
                ಪೂರ್ಣಾಙ್ಕ dir_fd,
                ಧ್ರುವ ವರ್ಣ *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ಶೂನ್ಯ *userdata) {

        _cleanup_free_ DirectoryEntries *de = ರಿಕ್ತ;
        ಸಂರಚನಾ statx ಬೇರು_sx;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dir_fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ಪ್ರತಿಜ್ಞಾ((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ಯದಿ (n_depth_max == 0)
                ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;
        ಯದಿ (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                ಯದಿ (statx_mask != 0) {
                        ಯದಿ (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &ಬೇರು_sx) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ರಿಕ್ತ, /* we have no dirent */
                         statx_mask != 0 ? &ಬೇರು_sx : ರಿಕ್ತ,
                         userdata);
                ಯದಿ (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        ಪ್ರತ್ಯಾಗಮ 0;
                ಯದಿ (r != RECURSE_DIR_CONTINUE)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                ಸಂರಚನಾ dirent *i = *entry;
                _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ ವರ್ಣ *joined = ರಿಕ್ತ;
                ಸಂರಚನಾ statx sx;
                bool sx_valid = false;
                ಧ್ರುವ ವರ್ಣ *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ಯದಿ (path) {
                        joined = path_join(path, i->d_name);
                        ಯದಿ (!joined)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        p = joined;
                } ಅನ್ಯಥಾ
                        p = i->d_name;

                ಯದಿ (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ಯದಿ (subdir_fd < 0) {
                                ಯದಿ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ಅನುವರ್ತ;

                                /* If it is a subdir but we failed to open it, then fail */
                                ಯದಿ (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ರಿಕ್ತ,
                                                 userdata);
                                        ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ಭಙ್ಗ;
                                        ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        ಅನುವರ್ತ;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ಅನ್ಯಥಾ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ಯದಿ (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ಯದಿ (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                ಪ್ರತ್ಯಾಗಮ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ಯದಿ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ಯದಿ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                ಯದಿ (inode_fd < 0) {
                                        ಯದಿ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ಅನುವರ್ತ;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ರಿಕ್ತ,
                                                 userdata);
                                        ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ಭಙ್ಗ;
                                        ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        ಅನುವರ್ತ;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ಯದಿ (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        ಪ್ರತ್ಯಾಗಮ -errno;

                                ಪ್ರತಿಜ್ಞಾ(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ಯದಿ (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ಯದಿ (subdir_fd < 0)
                                                ಪ್ರತ್ಯಾಗಮ subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } ಅನ್ಯಥಾ ಯದಿ (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ಯದಿ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        ಯದಿ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ಅನುವರ್ತ;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ಪ್ರತಿಜ್ಞಾ(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ರಿಕ್ತ,
                                                 userdata);
                                        ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ಭಙ್ಗ;
                                        ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        ಅನುವರ್ತ;
                                }

                                ಪ್ರತಿಜ್ಞಾ(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ಯದಿ (S_ISDIR(sx.stx_mode)) {
                                        /* So it suddenly is a directory, but we couldn't open it as such
                                         * earlier?  That is weird, and probably means somebody is racing
                                         * against us. We could of course retry and open it as a directory
                                         * again, but the chance to win here is limited. Hence, let's
                                         * propagate this as EISDIR error instead. That way we make this
                                         * something that can be reasonably handled, even though we give the
                                         * guarantee that RECURSE_DIR_ENTRY is strictly issued for
                                         * non-directory dirents. */

                                        log_debug("Non-directory entry '%s' suddenly became a directory.", p);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + EISDIR,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ರಿಕ್ತ,
                                                 userdata);
                                        ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ಭಙ್ಗ;
                                        ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        ಅನುವರ್ತ;
                                }
                        }
                }

                ಯದಿ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ಯದಿ (sx.stx_mask & STATX_TYPE) {
                                ಪ್ರತಿಜ್ಞಾ((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        ಯದಿ (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ಯದಿ (subdir_fd >= 0) {
                        ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ಯದಿ (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ಬೇರು))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ಬೇರು);
                                ಅನ್ಯಥಾ {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ಯದಿ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ಯದಿ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : ರಿಕ್ತ, /* only pass sx if user asked for it */
                                                 userdata);
                                        ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ಭಙ್ಗ;
                                        ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ಪ್ರತ್ಯಾಗಮ r;

                                        ಅನುವರ್ತ;
                                }
                        }

                        ಯದಿ (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ರಿಕ್ತ, /* only pass sx if user asked for it */
                                         userdata);
                                ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ಭಙ್ಗ;
                                ಯದಿ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        ಪ್ರತ್ಯಾಗಮ r;

                                ಅನುವರ್ತ;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ರಿಕ್ತ, /* only pass sx if user asked for it */
                                 userdata);
                        ಯದಿ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ಯದಿ (r != 0)
                                        ಪ್ರತ್ಯಾಗಮ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ರಿಕ್ತ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ಅನ್ಯಥಾ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ರಿಕ್ತ, /* only pass sx if user asked for it */
                                 userdata);

                ಯದಿ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ಭಙ್ಗ;

                ಯದಿ (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        ಪೂರ್ಣಾಙ್ಕ f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        ಯದಿ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ಯದಿ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        ಪ್ರತ್ಯಾಗಮ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ಅನ್ಯಥಾ ಯದಿ (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ರಿಕ್ತ, /* we have no dirent */
                         statx_mask != 0 ? &ಬೇರು_sx : ರಿಕ್ತ,
                         userdata);
                ಯದಿ (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ recurse_dir_at(
                ಪೂರ್ಣಾಙ್ಕ atfd,
                ಧ್ರುವ ವರ್ಣ *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ಶೂನ್ಯ *userdata) {

        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಪ್ರತಿಜ್ಞಾ(atfd >= 0 || atfd == AT_FDCWD);
        ಪ್ರತಿಜ್ಞಾ(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
