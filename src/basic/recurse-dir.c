/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "dirent-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "mountpoint-util.h"
#شامل "path-util.h"
#شامل "recurse-dir.h"
#شامل "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

ایستا صحیح sort_func(ساختار dirent * ثابت *a, ساختار dirent * ثابت *b) {
        أكد(a);
        أكد(b);

        بازگشت strcmp((*a)->d_name, (*b)->d_name);
}

ایستا bool ignore_dirent(ثابت ساختار dirent *de, RecurseDirFlags flags) {
        أكد(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        اگر ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                انتخاب (de->d_type) {

                حالت DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        بشکن;

                حالت DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        بشکن;

                حالت DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        بشکن;

                حالت DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        بشکن;

                پیشفرض:
                        بازگشت true;
                }

                اگر (!FLAGS_SET(flags, f))
                        بازگشت true;
        }

        بازگشت FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

صحیح readdir_all(صحیح dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = پوچ;
        DirectoryEntries *nde;
        صحیح r;

        أكد(dir_fd >= 0);

        اگر ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        اگر (!de)
                بازگشت -ENOMEM;

        de->buffer_size = 0;
        برای (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                أكد(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                أكد(bs > de->buffer_size);

                n = getdents64(dir_fd, (ساختار dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                اگر (n < 0)
                        بازگشت -errno;
                اگر (n == 0)
                        بشکن;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                اگر (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ادامه;

                اگر (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        بازگشت -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                اگر (!nde)
                        بازگشت -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ساختار dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                اگر (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        اگر (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ادامه;
                        اگر (r < 0)
                                بازگشت r;
                }

                اگر (ignore_dirent(entry, flags))
                        ادامه;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        اگر (!INC_SAFE(&sz, اندازه(ساختار dirent*) * de->n_entries))
                بازگشت -ENOMEM;

        nde = realloc(de, sz);
        اگر (!nde)
                بازگشت -ENOMEM;
        de = nde;

        de->entries = (ساختار dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                اگر (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        ادامه;

                اگر (ignore_dirent(entry, flags))
                        ادامه;

                de->entries[j++] = entry;
        }
        أكد(j == de->n_entries);

        اگر (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        اگر (ret)
                *ret = TAKE_PTR(de);

        بازگشت 0;
}

صحیح readdir_all_at(صحیح fd, ثابت نویسه *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ صحیح dir_fd = -EBADF;

        أكد(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        اگر (dir_fd < 0)
                بازگشت dir_fd;

        بازگشت readdir_all(dir_fd, flags, ret);
}

صحیح recurse_dir(
                صحیح dir_fd,
                ثابت نویسه *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                تهی *userdata) {

        _cleanup_free_ DirectoryEntries *de = پوچ;
        ساختار statx ریشه_sx;
        صحیح r;

        أكد(dir_fd >= 0);
        أكد(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        أكد((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        اگر (n_depth_max == 0)
                بازگشت -EOVERFLOW;
        اگر (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        اگر (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                اگر (statx_mask != 0) {
                        اگر (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &ریشه_sx) < 0)
                                بازگشت -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         پوچ, /* we have no dirent */
                         statx_mask != 0 ? &ریشه_sx : پوچ,
                         userdata);
                اگر (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        بازگشت 0;
                اگر (r != RECURSE_DIR_CONTINUE)
                        بازگشت r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        اگر (r < 0)
                بازگشت r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                ساختار dirent *i = *entry;
                _cleanup_close_ صحیح inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ نویسه *joined = پوچ;
                ساختار statx sx;
                bool sx_valid = false;
                ثابت نویسه *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                اگر (path) {
                        joined = path_join(path, i->d_name);
                        اگر (!joined)
                                بازگشت -ENOMEM;

                        p = joined;
                } وگرنه
                        p = i->d_name;

                اگر (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        اگر (subdir_fd < 0) {
                                اگر (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ادامه;

                                /* If it is a subdir but we failed to open it, then fail */
                                اگر (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ پوچ,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                بشکن;
                                        اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                بازگشت r;

                                        ادامه;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } وگرنه {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                اگر (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        اگر (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                بازگشت -errno;

                                        sx_valid = true;
                                }
                        }
                }

                اگر (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        اگر (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                اگر (inode_fd < 0) {
                                        اگر (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ادامه;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ پوچ,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                بشکن;
                                        اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                بازگشت r;

                                        ادامه;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                اگر (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        بازگشت -errno;

                                أكد(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                اگر (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        اگر (subdir_fd < 0)
                                                بازگشت subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } وگرنه اگر (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                اگر (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        اگر (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ادامه;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ پوچ,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                بشکن;
                                        اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                بازگشت r;

                                        ادامه;
                                }

                                أكد(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                اگر (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ پوچ,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                بشکن;
                                        اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                بازگشت r;

                                        ادامه;
                                }
                        }
                }

                اگر (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        اگر (sx.stx_mask & STATX_TYPE) {
                                أكد((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        اگر (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                اگر (subdir_fd >= 0) {
                        اگر (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                اگر (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ریشه))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ریشه);
                                وگرنه {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        اگر (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                اگر (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : پوچ, /* only pass sx if user asked for it */
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                بشکن;
                                        اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                بازگشت r;

                                        ادامه;
                                }
                        }

                        اگر (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : پوچ, /* only pass sx if user asked for it */
                                         userdata);
                                اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        بشکن;
                                اگر (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        بازگشت r;

                                ادامه;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : پوچ, /* only pass sx if user asked for it */
                                 userdata);
                        اگر (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                اگر (r != 0)
                                        بازگشت r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : پوچ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } وگرنه
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : پوچ, /* only pass sx if user asked for it */
                                 userdata);

                اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        بشکن;

                اگر (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        صحیح f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        اگر (unlinkat(dir_fd, i->d_name, f) < 0) {
                                اگر (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        بازگشت -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } وگرنه اگر (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        بازگشت r;
        }

        اگر (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         پوچ, /* we have no dirent */
                         statx_mask != 0 ? &ریشه_sx : پوچ,
                         userdata);
                اگر (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        بازگشت r;
        }

        بازگشت 0;
}

صحیح recurse_dir_at(
                صحیح atfd,
                ثابت نویسه *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                تهی *userdata) {

        _cleanup_close_ صحیح fd = -EBADF;

        أكد(atfd >= 0 || atfd == AT_FDCWD);
        أكد(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        اگر (fd < 0)
                بازگشت -errno;

        بازگشت recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
