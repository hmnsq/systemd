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

#تعریف_کرو DEFAULT_RECURSION_MAX 100

ساکن صحیح ترتیب_دو_func(ساخت dirent * ثابت *a, ساخت dirent * ثابت *b) {
        أكد(a);
        أكد(b);

        واپس strcmp((*a)->d_name, (*b)->d_name);
}

ساکن bool ignore_dirent(ثابت ساخت dirent *de, RecurseDirFlags flags) {
        أكد(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        اگر ((flags & RECURSE_DIR_MUST_BE_نقاب) != 0) {
                RecurseDirFlags f;

                انتخاب (de->d_type) {

                حالت DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        توڑو;

                حالت DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        توڑو;

                حالت DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        توڑو;

                حالت DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        توڑو;

                طےشدہ:
                        واپس true;
                }

                اگر (!FLAGS_سیٹ(flags, f))
                        واپس true;
        }

        واپس FLAGS_سیٹ(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

صحیح readdir_all(صحیح dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = تہی;
        DirectoryEntries *nde;
        صحیح r;

        أكد(dir_fd >= 0);

        اگر ((flags & RECURSE_DIR_MUST_BE_نقاب) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, بفر) <= SIZE_MAX - DIRENT_سائز_MAX * 8);
        de = حافظہ_دو(offsetof(DirectoryEntries, بفر) + DIRENT_سائز_MAX * 8);
        اگر (!de)
                واپس -ENOMEM;

        de->بفر_سائز = 0;
        برائے (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                أكد(MALLOC_SIZEOF_محفوظ(de) >= offsetof(DirectoryEntries, بفر));
                bs = MIN(MALLOC_SIZEOF_محفوظ(de) - offsetof(DirectoryEntries, بفر), (size_t) SSIZE_MAX);
                أكد(bs > de->بفر_سائز);

                n = getdents64(dir_fd, (ساخت dirent*) ((uint8_t*) de->بفر + de->بفر_سائز), bs - de->بفر_سائز);
                اگر (n < 0)
                        واپس -errno;
                اگر (n == 0)
                        توڑو;

                msan_unpoison((uint8_t*) de->بفر + de->بفر_سائز, n);

                de->بفر_سائز += n;

                اگر (de->بفر_سائز < bs - DIRENT_سائز_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        جاری;

                اگر (bs >= SSIZE_MAX - offsetof(DirectoryEntries, بفر))
                        واپس -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, بفر))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, بفر) : bs * 2;

                nde = realloc(de, bs);
                اگر (!nde)
                        واپس -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ساخت dirent *اندراج_2;
        FOREACH_DIRENT_اندر_بفر(اندراج_2, de->بفر, de->بفر_سائز) {
                اگر (FLAGS_سیٹ(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, اندراج_2);
                        اگر (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                جاری;
                        اگر (r < 0)
                                واپس r;
                }

                اگر (ignore_dirent(اندراج_2, flags))
                        جاری;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, بفر) + de->بفر_سائز);
        اگر (!INC_محفوظ(&sz, حجم(ساخت dirent*) * de->n_entries))
                واپس -ENOMEM;

        nde = realloc(de, sz);
        اگر (!nde)
                واپس -ENOMEM;
        de = nde;

        de->entries = (ساخت dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, بفر) + de->بفر_سائز));

        j = 0;
        FOREACH_DIRENT_اندر_بفر(اندراج_2, de->بفر, de->بفر_سائز) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                اگر (FLAGS_سیٹ(flags, RECURSE_DIR_ENSURE_TYPE) && اندراج_2->d_type == DT_UNKNOWN)
                        جاری;

                اگر (ignore_dirent(اندراج_2, flags))
                        جاری;

                de->entries[j++] = اندراج_2;
        }
        أكد(j == de->n_entries);

        اگر (FLAGS_سیٹ(flags, RECURSE_DIR_ترتیب_دو))
                typesafe_qsort(de->entries, de->n_entries, ترتیب_دو_func);

        اگر (ret)
                *ret = TAKE_PTR(de);

        واپس 0;
}

صحیح readdir_all_at(صحیح fd, ثابت حرف *راستہ, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ صحیح dir_fd = -EBADF;

        أكد(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, راستہ, O_DIRECTORY|O_CLOEXEC);
        اگر (dir_fd < 0)
                واپس dir_fd;

        واپس readdir_all(dir_fd, flags, ret);
}

صحیح recurse_dir(
                صحیح dir_fd,
                ثابت حرف *راستہ,
                غیرعلامتی statx_نقاب,
                غیرعلامتی n_گہرائی_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                خالی *userdata) {

        _cleanup_free_ DirectoryEntries *de = تہی;
        ساخت statx جڑ_sx;
        صحیح r;

        أكد(dir_fd >= 0);
        أكد(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        أكد((flags & RECURSE_DIR_MUST_BE_نقاب) == 0 || FLAGS_سیٹ(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        اگر (n_گہرائی_max == 0)
                واپس -EOVERFLOW;
        اگر (n_گہرائی_max == UINT_MAX) /* special marker for "default" */
                n_گہرائی_max = DEFAULT_RECURSION_MAX;

        اگر (FLAGS_سیٹ(flags, RECURSE_DIR_TOPLEVEL)) {
                اگر (statx_نقاب != 0) {
                        اگر (statx(dir_fd, "", AT_خالی_خانے_راستہ, statx_نقاب, &جڑ_sx) < 0)
                                واپس -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         راستہ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         تہی, /* we have no dirent */
                         statx_نقاب != 0 ? &جڑ_sx : تہی,
                         userdata);
                اگر (اندر_سیٹ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_اندراج))
                        واپس 0;
                اگر (r != RECURSE_DIR_CONTINUE)
                        واپس r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        اگر (r < 0)
                واپس r;

        FOREACH_ARRAY(اندراج_2, de->entries, de->n_entries) {
                ساخت dirent *i = *اندراج_2;
                _cleanup_close_ صحیح inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ حرف *joined = تہی;
                ساخت statx sx;
                bool sx_valid = false;
                ثابت حرف *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                اگر (راستہ) {
                        joined = راستہ_join(راستہ, i->d_name);
                        اگر (!joined)
                                واپس -ENOMEM;

                        p = joined;
                } ورنہ
                        p = i->d_name;

                اگر (اندر_سیٹ(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        اگر (subdir_fd < 0) {
                                اگر (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        جاری;

                                /* If it is a subdir but we failed to open it, then fail */
                                اگر (!اندر_سیٹ(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_بنیاد);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_بنیاد + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ تہی,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                توڑو;
                                        اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                                واپس r;

                                        جاری;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ورنہ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                اگر (statx_نقاب != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        اگر (statx(subdir_fd, "", AT_خالی_خانے_راستہ, statx_نقاب, &sx) < 0)
                                                واپس -errno;

                                        sx_valid = true;
                                }
                        }
                }

                اگر (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        اگر (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_راستہ|O_NOFOLLOW|O_CLOEXEC);
                                اگر (inode_fd < 0) {
                                        اگر (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                جاری;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_بنیاد);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_بنیاد + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ تہی,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                توڑو;
                                        اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                                واپس r;

                                        جاری;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                اگر (statx(inode_fd, "", AT_خالی_خانے_راستہ, statx_نقاب | STATX_TYPE, &sx) < 0)
                                        واپس -errno;

                                أكد(sx.stx_نقاب & STATX_TYPE);
                                sx_valid = true;

                                اگر (S_ISDIR(sx.stx_منوال)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        اگر (subdir_fd < 0)
                                                واپس subdir_fd;

                                        inode_fd = محفوظ_close(inode_fd);
                                }

                        } ورنہ اگر (statx_نقاب != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                اگر (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_نقاب | STATX_TYPE, &sx) < 0) {
                                        اگر (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                جاری;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        أكد(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_بنیاد);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_بنیاد + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ تہی,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                توڑو;
                                        اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                                واپس r;

                                        جاری;
                                }

                                أكد(sx.stx_نقاب & STATX_TYPE);
                                sx_valid = true;

                                اگر (S_ISDIR(sx.stx_منوال)) {
                                        /* So it suddenly is a directory, but we couldn't open it as such
                                         * earlier?  That is weird, and probably means somebody is racing
                                         * against us. We could of course retry and open it as a directory
                                         * again, but the chance to win here is limited. Hence, let's
                                         * propagate this as EISDIR error instead. That way we make this
                                         * something that can be reasonably handled, even though we give the
                                         * guarantee that RECURSE_DIR_ENTRY is strictly issued for
                                         * non-directory dirents. */

                                        log_debug("Non-directory entry '%s' suddenly became a directory.", p);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_بنیاد + EISDIR,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ تہی,
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                توڑو;
                                        اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                                واپس r;

                                        جاری;
                                }
                        }
                }

                اگر (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        اگر (sx.stx_نقاب & STATX_TYPE) {
                                أكد((subdir_fd < 0) == !S_ISDIR(sx.stx_منوال));
                                i->d_type = IFTODT(sx.stx_منوال);
                        }

                        اگر (sx.stx_نقاب & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                اگر (subdir_fd >= 0) {
                        اگر (FLAGS_سیٹ(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                اگر (sx_valid && FLAGS_سیٹ(sx.stx_اوصاف_نقاب, STATX_ATTR_MOUNT_جڑ))
                                        is_mount = FLAGS_سیٹ(sx.stx_اوصاف, STATX_ATTR_MOUNT_جڑ);
                                ورنہ {
                                        r = is_mount_نقطہ_at(dir_fd, i->d_name, /* flags= */ 0);
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
                                                 statx_نقاب != 0 ? &sx : تہی, /* only pass sx if user asked for it */
                                                 userdata);
                                        اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                توڑو;
                                        اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                                واپس r;

                                        جاری;
                                }
                        }

                        اگر (n_گہرائی_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_گہرائی,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_نقاب != 0 ? &sx : تہی, /* only pass sx if user asked for it */
                                         userdata);
                                اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        توڑو;
                                اگر (!اندر_سیٹ(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_اندراج))
                                        واپس r;

                                جاری;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_نقاب != 0 ? &sx : تہی, /* only pass sx if user asked for it */
                                 userdata);
                        اگر (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_نقاب,
                                                n_گہرائی_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                اگر (r != 0)
                                        واپس r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_نقاب != 0 ? &sx : تہی, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ورنہ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_اندراج,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_نقاب != 0 ? &sx : تہی, /* only pass sx if user asked for it */
                                 userdata);

                اگر (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        توڑو;

                اگر (اندر_سیٹ(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        صحیح f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = محفوظ_close(subdir_fd);
                        inode_fd = محفوظ_close(inode_fd);

                        اگر (unlinkat(dir_fd, i->d_name, f) < 0) {
                                اگر (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        واپس -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ورنہ اگر (!اندر_سیٹ(r, RECURSE_DIR_SKIP_اندراج, RECURSE_DIR_CONTINUE))
                        واپس r;
        }

        اگر (FLAGS_سیٹ(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         راستہ,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         تہی, /* we have no dirent */
                         statx_نقاب != 0 ? &جڑ_sx : تہی,
                         userdata);
                اگر (!اندر_سیٹ(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_اندراج, RECURSE_DIR_CONTINUE))
                        واپس r;
        }

        واپس 0;
}

صحیح recurse_dir_at(
                صحیح atfd,
                ثابت حرف *راستہ,
                غیرعلامتی statx_نقاب,
                غیرعلامتی n_گہرائی_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                خالی *userdata) {

        _cleanup_close_ صحیح fd = -EBADF;

        أكد(atfd >= 0 || atfd == AT_FDCWD);
        أكد(func);

        fd = openat(atfd, راستہ ?: ".", O_DIRECTORY|O_CLOEXEC);
        اگر (fd < 0)
                واپس -errno;

        واپس recurse_dir(fd, راستہ, statx_نقاب, n_گہرائی_max, flags, func, userdata);
}
