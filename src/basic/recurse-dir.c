/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "dirent-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "mountpoint-util.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "recurse-dir.h"
#અન્તર્ભાવ "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

સ્થિર પૂર્ણાંક sort_func(સંરચના dirent * અચલ *a, સંરચના dirent * અચલ *b) {
        પ્રતિજ્ઞા(a);
        પ્રતિજ્ઞા(b);

        પાછા strcmp((*a)->d_name, (*b)->d_name);
}

સ્થિર bool ignore_dirent(અચલ સંરચના dirent *de, RecurseDirFlags flags) {
        પ્રતિજ્ઞા(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        જો ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                પસંદગી (de->d_type) {

                સ્થિતિ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        તોડો;

                સ્થિતિ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        તોડો;

                સ્થિતિ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        તોડો;

                સ્થિતિ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        તોડો;

                મૂળભૂત:
                        પાછા true;
                }

                જો (!FLAGS_SET(flags, f))
                        પાછા true;
        }

        પાછા FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

પૂર્ણાંક readdir_all(પૂર્ણાંક dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ખાલી;
        DirectoryEntries *nde;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0);

        જો ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        જો (!de)
                પાછા -ENOMEM;

        de->buffer_size = 0;
        માટે (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                પ્રતિજ્ઞા(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                પ્રતિજ્ઞા(bs > de->buffer_size);

                n = getdents64(dir_fd, (સંરચના dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                જો (n < 0)
                        પાછા -errno;
                જો (n == 0)
                        તોડો;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                જો (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ચાલુ;

                જો (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        પાછા -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                જો (!nde)
                        પાછા -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        સંરચના dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                જો (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        જો (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ચાલુ;
                        જો (r < 0)
                                પાછા r;
                }

                જો (ignore_dirent(entry, flags))
                        ચાલુ;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        જો (!INC_SAFE(&sz, કદ(સંરચના dirent*) * de->n_entries))
                પાછા -ENOMEM;

        nde = realloc(de, sz);
        જો (!nde)
                પાછા -ENOMEM;
        de = nde;

        de->entries = (સંરચના dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                જો (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        ચાલુ;

                જો (ignore_dirent(entry, flags))
                        ચાલુ;

                de->entries[j++] = entry;
        }
        પ્રતિજ્ઞા(j == de->n_entries);

        જો (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        જો (ret)
                *ret = TAKE_PTR(de);

        પાછા 0;
}

પૂર્ણાંક readdir_all_at(પૂર્ણાંક fd, અચલ અક્ષર *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ પૂર્ણાંક dir_fd = -EBADF;

        પ્રતિજ્ઞા(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        જો (dir_fd < 0)
                પાછા dir_fd;

        પાછા readdir_all(dir_fd, flags, ret);
}

પૂર્ણાંક recurse_dir(
                પૂર્ણાંક dir_fd,
                અચલ અક્ષર *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                શૂન્ય *userdata) {

        _cleanup_free_ DirectoryEntries *de = ખાલી;
        સંરચના statx મૂળ_sx;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(dir_fd >= 0);
        પ્રતિજ્ઞા(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        પ્રતિજ્ઞા((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        જો (n_depth_max == 0)
                પાછા -EOVERFLOW;
        જો (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        જો (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                જો (statx_mask != 0) {
                        જો (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &મૂળ_sx) < 0)
                                પાછા -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ખાલી, /* we have no dirent */
                         statx_mask != 0 ? &મૂળ_sx : ખાલી,
                         userdata);
                જો (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        પાછા 0;
                જો (r != RECURSE_DIR_CONTINUE)
                        પાછા r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        જો (r < 0)
                પાછા r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                સંરચના dirent *i = *entry;
                _cleanup_close_ પૂર્ણાંક inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ અક્ષર *joined = ખાલી;
                સંરચના statx sx;
                bool sx_valid = false;
                અચલ અક્ષર *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                જો (path) {
                        joined = path_join(path, i->d_name);
                        જો (!joined)
                                પાછા -ENOMEM;

                        p = joined;
                } નહીંતર
                        p = i->d_name;

                જો (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        જો (subdir_fd < 0) {
                                જો (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ચાલુ;

                                /* If it is a subdir but we failed to open it, then fail */
                                જો (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        પ્રતિજ્ઞા(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ખાલી,
                                                 userdata);
                                        જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                તોડો;
                                        જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                પાછા r;

                                        ચાલુ;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } નહીંતર {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                જો (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        જો (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                પાછા -errno;

                                        sx_valid = true;
                                }
                        }
                }

                જો (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        જો (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                જો (inode_fd < 0) {
                                        જો (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ચાલુ;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        પ્રતિજ્ઞા(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ખાલી,
                                                 userdata);
                                        જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                તોડો;
                                        જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                પાછા r;

                                        ચાલુ;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                જો (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        પાછા -errno;

                                પ્રતિજ્ઞા(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                જો (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        જો (subdir_fd < 0)
                                                પાછા subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } નહીંતર જો (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                જો (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        જો (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ચાલુ;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        પ્રતિજ્ઞા(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ખાલી,
                                                 userdata);
                                        જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                તોડો;
                                        જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                પાછા r;

                                        ચાલુ;
                                }

                                પ્રતિજ્ઞા(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                જો (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ ખાલી,
                                                 userdata);
                                        જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                તોડો;
                                        જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                પાછા r;

                                        ચાલુ;
                                }
                        }
                }

                જો (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        જો (sx.stx_mask & STATX_TYPE) {
                                પ્રતિજ્ઞા((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        જો (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                જો (subdir_fd >= 0) {
                        જો (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                જો (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_મૂળ))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_મૂળ);
                                નહીંતર {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        જો (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                જો (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : ખાલી, /* only pass sx if user asked for it */
                                                 userdata);
                                        જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                તોડો;
                                        જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                પાછા r;

                                        ચાલુ;
                                }
                        }

                        જો (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ખાલી, /* only pass sx if user asked for it */
                                         userdata);
                                જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        તોડો;
                                જો (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        પાછા r;

                                ચાલુ;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ખાલી, /* only pass sx if user asked for it */
                                 userdata);
                        જો (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                જો (r != 0)
                                        પાછા r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ખાલી, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } નહીંતર
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ખાલી, /* only pass sx if user asked for it */
                                 userdata);

                જો (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        તોડો;

                જો (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        પૂર્ણાંક f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        જો (unlinkat(dir_fd, i->d_name, f) < 0) {
                                જો (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        પાછા -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } નહીંતર જો (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        પાછા r;
        }

        જો (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ખાલી, /* we have no dirent */
                         statx_mask != 0 ? &મૂળ_sx : ખાલી,
                         userdata);
                જો (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        પાછા r;
        }

        પાછા 0;
}

પૂર્ણાંક recurse_dir_at(
                પૂર્ણાંક atfd,
                અચલ અક્ષર *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                શૂન્ય *userdata) {

        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        પ્રતિજ્ઞા(atfd >= 0 || atfd == AT_FDCWD);
        પ્રતિજ્ઞા(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        જો (fd < 0)
                પાછા -errno;

        પાછા recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
