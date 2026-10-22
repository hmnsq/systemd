/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "dirent-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "mountpoint-util.h"
#ضمن "path-util.h"
#ضمن "recurse-dir.h"
#ضمن "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

ساكن صحيح sort_func(بنية dirent * ثابت *a, بنية dirent * ثابت *b) {
        أكد(a);
        أكد(b);

        החזר strcmp((*a)->d_name, (*b)->d_name);
}

ساكن bool ignore_dirent(ثابت بنية dirent *de, RecurseDirFlags flags) {
        أكد(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        אם ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                بدل (de->d_type) {

                حالة DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        اكسر;

                حالة DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        اكسر;

                حالة DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        اكسر;

                حالة DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        اكسر;

                افتراضي:
                        החזר true;
                }

                אם (!FLAGS_SET(flags, f))
                        החזר true;
        }

        החזר FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

صحيح readdir_all(صحيح dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = عدم;
        DirectoryEntries *nde;
        صحيح r;

        أكد(dir_fd >= 0);

        אם ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        אם (!de)
                החזר -ENOMEM;

        de->buffer_size = 0;
        עבור (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                أكد(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                أكد(bs > de->buffer_size);

                n = getdents64(dir_fd, (بنية dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                אם (n < 0)
                        החזר -errno;
                אם (n == 0)
                        اكسر;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                אם (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        استمر;

                אם (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        החזר -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                אם (!nde)
                        החזר -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        بنية dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                אם (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        אם (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                استمر;
                        אם (r < 0)
                                החזר r;
                }

                אם (ignore_dirent(entry, flags))
                        استمر;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        אם (!INC_SAFE(&sz, حجم(بنية dirent*) * de->n_entries))
                החזר -ENOMEM;

        nde = realloc(de, sz);
        אם (!nde)
                החזר -ENOMEM;
        de = nde;

        de->entries = (بنية dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                אם (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        استمر;

                אם (ignore_dirent(entry, flags))
                        استمر;

                de->entries[j++] = entry;
        }
        أكد(j == de->n_entries);

        אם (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        אם (ret)
                *ret = TAKE_PTR(de);

        החזר 0;
}

صحيح readdir_all_at(صحيح fd, ثابت حرف *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ صحيح dir_fd = -EBADF;

        أكد(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        אם (dir_fd < 0)
                החזר dir_fd;

        החזר readdir_all(dir_fd, flags, ret);
}

صحيح recurse_dir(
                صحيح dir_fd,
                ثابت حرف *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                فراغ *userdata) {

        _cleanup_free_ DirectoryEntries *de = عدم;
        بنية statx root_sx;
        صحيح r;

        أكد(dir_fd >= 0);
        أكد(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        أكد((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        אם (n_depth_max == 0)
                החזר -EOVERFLOW;
        אם (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        אם (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                אם (statx_mask != 0) {
                        אם (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &root_sx) < 0)
                                החזר -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         عدم, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : عدم,
                         userdata);
                אם (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        החזר 0;
                אם (r != RECURSE_DIR_CONTINUE)
                        החזר r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        אם (r < 0)
                החזר r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                بنية dirent *i = *entry;
                _cleanup_close_ صحيح inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ حرف *joined = عدم;
                بنية statx sx;
                bool sx_valid = false;
                ثابت حرف *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                אם (path) {
                        joined = path_join(path, i->d_name);
                        אם (!joined)
                                החזר -ENOMEM;

                        p = joined;
                } אחרת
                        p = i->d_name;

                אם (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        אם (subdir_fd < 0) {
                                אם (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        استمر;

                                /* If it is a subdir but we failed to open it, then fail */
                                אם (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ عدم,
                                                 userdata);
                                        אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                اكسر;
                                        אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                החזר r;

                                        استمر;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } אחרת {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                אם (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        אם (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                החזר -errno;

                                        sx_valid = true;
                                }
                        }
                }

                אם (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        אם (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                אם (inode_fd < 0) {
                                        אם (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                استمر;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ عدم,
                                                 userdata);
                                        אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                اكسر;
                                        אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                החזר r;

                                        استمر;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                אם (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        החזר -errno;

                                أكد(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                אם (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        אם (subdir_fd < 0)
                                                החזר subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } אחרת אם (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                אם (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        אם (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                استمر;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ عدم,
                                                 userdata);
                                        אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                اكسر;
                                        אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                החזר r;

                                        استمر;
                                }

                                أكد(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                אם (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ عدم,
                                                 userdata);
                                        אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                اكسر;
                                        אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                החזר r;

                                        استمر;
                                }
                        }
                }

                אם (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        אם (sx.stx_mask & STATX_TYPE) {
                                أكد((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        אם (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                אם (subdir_fd >= 0) {
                        אם (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                אם (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ROOT))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT);
                                אחרת {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        אם (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                אם (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : عدم, /* only pass sx if user asked for it */
                                                 userdata);
                                        אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                اكسر;
                                        אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                החזר r;

                                        استمر;
                                }
                        }

                        אם (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : عدم, /* only pass sx if user asked for it */
                                         userdata);
                                אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        اكسر;
                                אם (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        החזר r;

                                استمر;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : عدم, /* only pass sx if user asked for it */
                                 userdata);
                        אם (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                אם (r != 0)
                                        החזר r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : عدم, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } אחרת
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : عدم, /* only pass sx if user asked for it */
                                 userdata);

                אם (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        اكسر;

                אם (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        صحيح f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        אם (unlinkat(dir_fd, i->d_name, f) < 0) {
                                אם (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        החזר -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } אחרת אם (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        החזר r;
        }

        אם (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         عدم, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : عدم,
                         userdata);
                אם (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        החזר r;
        }

        החזר 0;
}

صحيح recurse_dir_at(
                صحيح atfd,
                ثابت حرف *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                فراغ *userdata) {

        _cleanup_close_ صحيح fd = -EBADF;

        أكد(atfd >= 0 || atfd == AT_FDCWD);
        أكد(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        אם (fd < 0)
                החזר -errno;

        החזר recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
