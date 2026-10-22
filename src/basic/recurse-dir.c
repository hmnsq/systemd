/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "mountpoint-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "recurse-dir.h"
#অন্তর্ভাব "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

স্থির পূর্ণাঙ্ক sort_func(সংরচনা dirent * ধ্রুব *a, সংরচনা dirent * ধ্রুব *b) {
        প্রতিজ্ঞা(a);
        প্রতিজ্ঞা(b);

        প্রত্যাগম strcmp((*a)->d_name, (*b)->d_name);
}

স্থির bool ignore_dirent(ধ্রুব সংরচনা dirent *de, RecurseDirFlags flags) {
        প্রতিজ্ঞা(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        যদি ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                বিকল্প (de->d_type) {

                পক্ষ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ভঙ্গ;

                পক্ষ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ভঙ্গ;

                পক্ষ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ভঙ্গ;

                পক্ষ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ভঙ্গ;

                স্বভাব:
                        প্রত্যাগম true;
                }

                যদি (!FLAGS_SET(flags, f))
                        প্রত্যাগম true;
        }

        প্রত্যাগম FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

পূর্ণাঙ্ক readdir_all(পূর্ণাঙ্ক dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = রিক্ত;
        DirectoryEntries *nde;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0);

        যদি ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        যদি (!de)
                প্রত্যাগম -ENOMEM;

        de->buffer_size = 0;
        প্রতি (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                প্রতিজ্ঞা(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                প্রতিজ্ঞা(bs > de->buffer_size);

                n = getdents64(dir_fd, (সংরচনা dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                যদি (n < 0)
                        প্রত্যাগম -errno;
                যদি (n == 0)
                        ভঙ্গ;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                যদি (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        অনুবর্ত;

                যদি (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        প্রত্যাগম -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                যদি (!nde)
                        প্রত্যাগম -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        সংরচনা dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                যদি (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        যদি (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                অনুবর্ত;
                        যদি (r < 0)
                                প্রত্যাগম r;
                }

                যদি (ignore_dirent(entry, flags))
                        অনুবর্ত;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        যদি (!INC_SAFE(&sz, পরিমাণ(সংরচনা dirent*) * de->n_entries))
                প্রত্যাগম -ENOMEM;

        nde = realloc(de, sz);
        যদি (!nde)
                প্রত্যাগম -ENOMEM;
        de = nde;

        de->entries = (সংরচনা dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                যদি (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        অনুবর্ত;

                যদি (ignore_dirent(entry, flags))
                        অনুবর্ত;

                de->entries[j++] = entry;
        }
        প্রতিজ্ঞা(j == de->n_entries);

        যদি (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        যদি (ret)
                *ret = TAKE_PTR(de);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক readdir_all_at(পূর্ণাঙ্ক fd, ধ্রুব বর্ণ *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ পূর্ণাঙ্ক dir_fd = -EBADF;

        প্রতিজ্ঞা(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        যদি (dir_fd < 0)
                প্রত্যাগম dir_fd;

        প্রত্যাগম readdir_all(dir_fd, flags, ret);
}

পূর্ণাঙ্ক recurse_dir(
                পূর্ণাঙ্ক dir_fd,
                ধ্রুব বর্ণ *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                শূন্য *userdata) {

        _cleanup_free_ DirectoryEntries *de = রিক্ত;
        সংরচনা statx মূল_sx;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dir_fd >= 0);
        প্রতিজ্ঞা(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        প্রতিজ্ঞা((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        যদি (n_depth_max == 0)
                প্রত্যাগম -EOVERFLOW;
        যদি (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        যদি (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                যদি (statx_mask != 0) {
                        যদি (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &মূল_sx) < 0)
                                প্রত্যাগম -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         রিক্ত, /* we have no dirent */
                         statx_mask != 0 ? &মূল_sx : রিক্ত,
                         userdata);
                যদি (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        প্রত্যাগম 0;
                যদি (r != RECURSE_DIR_CONTINUE)
                        প্রত্যাগম r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        যদি (r < 0)
                প্রত্যাগম r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                সংরচনা dirent *i = *entry;
                _cleanup_close_ পূর্ণাঙ্ক inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ বর্ণ *joined = রিক্ত;
                সংরচনা statx sx;
                bool sx_valid = false;
                ধ্রুব বর্ণ *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                যদি (path) {
                        joined = path_join(path, i->d_name);
                        যদি (!joined)
                                প্রত্যাগম -ENOMEM;

                        p = joined;
                } অন্যথা
                        p = i->d_name;

                যদি (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        যদি (subdir_fd < 0) {
                                যদি (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        অনুবর্ত;

                                /* If it is a subdir but we failed to open it, then fail */
                                যদি (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        প্রতিজ্ঞা(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ রিক্ত,
                                                 userdata);
                                        যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ভঙ্গ;
                                        যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                প্রত্যাগম r;

                                        অনুবর্ত;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } অন্যথা {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                যদি (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        যদি (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                প্রত্যাগম -errno;

                                        sx_valid = true;
                                }
                        }
                }

                যদি (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        যদি (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                যদি (inode_fd < 0) {
                                        যদি (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                অনুবর্ত;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        প্রতিজ্ঞা(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ রিক্ত,
                                                 userdata);
                                        যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ভঙ্গ;
                                        যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                প্রত্যাগম r;

                                        অনুবর্ত;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                যদি (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        প্রত্যাগম -errno;

                                প্রতিজ্ঞা(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                যদি (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        যদি (subdir_fd < 0)
                                                প্রত্যাগম subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } অন্যথা যদি (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                যদি (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        যদি (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                অনুবর্ত;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        প্রতিজ্ঞা(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ রিক্ত,
                                                 userdata);
                                        যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ভঙ্গ;
                                        যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                প্রত্যাগম r;

                                        অনুবর্ত;
                                }

                                প্রতিজ্ঞা(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                যদি (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ রিক্ত,
                                                 userdata);
                                        যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ভঙ্গ;
                                        যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                প্রত্যাগম r;

                                        অনুবর্ত;
                                }
                        }
                }

                যদি (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        যদি (sx.stx_mask & STATX_TYPE) {
                                প্রতিজ্ঞা((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        যদি (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                যদি (subdir_fd >= 0) {
                        যদি (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                যদি (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_মূল))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_মূল);
                                অন্যথা {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        যদি (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                যদি (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : রিক্ত, /* only pass sx if user asked for it */
                                                 userdata);
                                        যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ভঙ্গ;
                                        যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                প্রত্যাগম r;

                                        অনুবর্ত;
                                }
                        }

                        যদি (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : রিক্ত, /* only pass sx if user asked for it */
                                         userdata);
                                যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ভঙ্গ;
                                যদি (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        প্রত্যাগম r;

                                অনুবর্ত;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : রিক্ত, /* only pass sx if user asked for it */
                                 userdata);
                        যদি (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                যদি (r != 0)
                                        প্রত্যাগম r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : রিক্ত, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } অন্যথা
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : রিক্ত, /* only pass sx if user asked for it */
                                 userdata);

                যদি (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ভঙ্গ;

                যদি (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        পূর্ণাঙ্ক f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        যদি (unlinkat(dir_fd, i->d_name, f) < 0) {
                                যদি (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        প্রত্যাগম -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } অন্যথা যদি (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        প্রত্যাগম r;
        }

        যদি (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         রিক্ত, /* we have no dirent */
                         statx_mask != 0 ? &মূল_sx : রিক্ত,
                         userdata);
                যদি (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        প্রত্যাগম r;
        }

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক recurse_dir_at(
                পূর্ণাঙ্ক atfd,
                ধ্রুব বর্ণ *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                শূন্য *userdata) {

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        প্রতিজ্ঞা(atfd >= 0 || atfd == AT_FDCWD);
        প্রতিজ্ঞা(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        যদি (fd < 0)
                প্রত্যাগম -errno;

        প্রত্যাগম recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
