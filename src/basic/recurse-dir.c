/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "dirent-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "mountpoint-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "recurse-dir.h"
#ଅନ୍ତର୍ଭାଵ "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sort_func(ସଂରଚନା dirent * ଅଚଳ *a, ସଂରଚନା dirent * ଅଚଳ *b) {
        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଫେରନ୍ତୁ strcmp((*a)->d_name, (*b)->d_name);
}

ସ୍ଥିର bool ignore_dirent(ଅଚଳ ସଂରଚନା dirent *de, RecurseDirFlags flags) {
        ପ୍ରତିଜ୍ଞା(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ଯଦି ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                ବାଛ (de->d_type) {

                ସ୍ଥିତି DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        ଭାଙ୍ଗ;

                ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
                        ଫେରନ୍ତୁ true;
                }

                ଯଦି (!FLAGS_SET(flags, f))
                        ଫେରନ୍ତୁ true;
        }

        ଫେରନ୍ତୁ FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readdir_all(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ଖାଲି;
        DirectoryEntries *nde;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0);

        ଯଦି ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        ଯଦି (!de)
                ଫେରନ୍ତୁ -ENOMEM;

        de->buffer_size = 0;
        ପାଇଁ (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ପ୍ରତିଜ୍ଞା(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                ପ୍ରତିଜ୍ଞା(bs > de->buffer_size);

                n = getdents64(dir_fd, (ସଂରଚନା dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                ଯଦି (n < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (n == 0)
                        ଭାଙ୍ଗ;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                ଯଦି (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        ଜାରି;

                ଯଦି (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        ଫେରନ୍ତୁ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                ଯଦି (!nde)
                        ଫେରନ୍ତୁ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ସଂରଚନା dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                ଯଦି (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        ଯଦି (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                ଜାରି;
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                }

                ଯଦି (ignore_dirent(entry, flags))
                        ଜାରି;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        ଯଦି (!INC_SAFE(&sz, ଆକାର(ସଂରଚନା dirent*) * de->n_entries))
                ଫେରନ୍ତୁ -ENOMEM;

        nde = realloc(de, sz);
        ଯଦି (!nde)
                ଫେରନ୍ତୁ -ENOMEM;
        de = nde;

        de->entries = (ସଂରଚନା dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ଯଦି (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        ଜାରି;

                ଯଦି (ignore_dirent(entry, flags))
                        ଜାରି;

                de->entries[j++] = entry;
        }
        ପ୍ରତିଜ୍ଞା(j == de->n_entries);

        ଯଦି (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        ଯଦି (ret)
                *ret = TAKE_PTR(de);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା readdir_all_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଅକ୍ଷର *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        ଯଦି (dir_fd < 0)
                ଫେରନ୍ତୁ dir_fd;

        ଫେରନ୍ତୁ readdir_all(dir_fd, flags, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା recurse_dir(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା dir_fd,
                ଅଚଳ ଅକ୍ଷର *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ଶୂନ୍ୟ *userdata) {

        _cleanup_free_ DirectoryEntries *de = ଖାଲି;
        ସଂରଚନା statx ମୂଳ_sx;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dir_fd >= 0);
        ପ୍ରତିଜ୍ଞା(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ପ୍ରତିଜ୍ଞା((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ଯଦି (n_depth_max == 0)
                ଫେରନ୍ତୁ -EOVERFLOW;
        ଯଦି (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        ଯଦି (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                ଯଦି (statx_mask != 0) {
                        ଯଦି (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &ମୂଳ_sx) < 0)
                                ଫେରନ୍ତୁ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ଖାଲି, /* we have no dirent */
                         statx_mask != 0 ? &ମୂଳ_sx : ଖାଲି,
                         userdata);
                ଯଦି (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        ଫେରନ୍ତୁ 0;
                ଯଦି (r != RECURSE_DIR_CONTINUE)
                        ଫେରନ୍ତୁ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                ସଂରଚନା dirent *i = *entry;
                _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ ଅକ୍ଷର *joined = ଖାଲି;
                ସଂରଚନା statx sx;
                bool sx_valid = false;
                ଅଚଳ ଅକ୍ଷର *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ଯଦି (path) {
                        joined = path_join(path, i->d_name);
                        ଯଦି (!joined)
                                ଫେରନ୍ତୁ -ENOMEM;

                        p = joined;
                } ଅନ୍ୟଥା
                        p = i->d_name;

                ଯଦି (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ଯଦି (subdir_fd < 0) {
                                ଯଦି (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        ଜାରି;

                                /* If it is a subdir but we failed to open it, then fail */
                                ଯଦି (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ପ୍ରତିଜ୍ଞା(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ଖାଲି,
                                                 userdata);
                                        ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ଭାଙ୍ଗ;
                                        ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ଫେରନ୍ତୁ r;

                                        ଜାରି;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ଅନ୍ୟଥା {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ଯଦି (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ଯଦି (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                ଫେରନ୍ତୁ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ଯଦି (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ଯଦି (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                ଯଦି (inode_fd < 0) {
                                        ଯଦି (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                ଜାରି;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ପ୍ରତିଜ୍ଞା(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ଖାଲି,
                                                 userdata);
                                        ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ଭାଙ୍ଗ;
                                        ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ଫେରନ୍ତୁ r;

                                        ଜାରି;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ଯଦି (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        ଫେରନ୍ତୁ -errno;

                                ପ୍ରତିଜ୍ଞା(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ଯଦି (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ଯଦି (subdir_fd < 0)
                                                ଫେରନ୍ତୁ subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } ଅନ୍ୟଥା ଯଦି (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ଯଦି (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        ଯଦି (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                ଜାରି;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ପ୍ରତିଜ୍ଞା(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ଖାଲି,
                                                 userdata);
                                        ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ଭାଙ୍ଗ;
                                        ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ଫେରନ୍ତୁ r;

                                        ଜାରି;
                                }

                                ପ୍ରତିଜ୍ଞା(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ଯଦି (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ ଖାଲି,
                                                 userdata);
                                        ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ଭାଙ୍ଗ;
                                        ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ଫେରନ୍ତୁ r;

                                        ଜାରି;
                                }
                        }
                }

                ଯଦି (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ଯଦି (sx.stx_mask & STATX_TYPE) {
                                ପ୍ରତିଜ୍ଞା((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        ଯଦି (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ଯଦି (subdir_fd >= 0) {
                        ଯଦି (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ଯଦି (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ମୂଳ))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ମୂଳ);
                                ଅନ୍ୟଥା {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ଯଦି (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ଯଦି (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : ଖାଲି, /* only pass sx if user asked for it */
                                                 userdata);
                                        ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                ଭାଙ୍ଗ;
                                        ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ଫେରନ୍ତୁ r;

                                        ଜାରି;
                                }
                        }

                        ଯଦି (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ଖାଲି, /* only pass sx if user asked for it */
                                         userdata);
                                ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        ଭାଙ୍ଗ;
                                ଯଦି (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        ଫେରନ୍ତୁ r;

                                ଜାରି;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ଖାଲି, /* only pass sx if user asked for it */
                                 userdata);
                        ଯଦି (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ଯଦି (r != 0)
                                        ଫେରନ୍ତୁ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ଖାଲି, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ଅନ୍ୟଥା
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ଖାଲି, /* only pass sx if user asked for it */
                                 userdata);

                ଯଦି (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        ଭାଙ୍ଗ;

                ଯଦି (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        ଯଦି (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ଯଦି (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        ଫେରନ୍ତୁ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ଅନ୍ୟଥା ଯଦି (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ଫେରନ୍ତୁ r;
        }

        ଯଦି (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ଖାଲି, /* we have no dirent */
                         statx_mask != 0 ? &ମୂଳ_sx : ଖାଲି,
                         userdata);
                ଯଦି (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ଫେରନ୍ତୁ r;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା recurse_dir_at(
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା atfd,
                ଅଚଳ ଅକ୍ଷର *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ଶୂନ୍ୟ *userdata) {

        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ପ୍ରତିଜ୍ଞା(atfd >= 0 || atfd == AT_FDCWD);
        ପ୍ରତିଜ୍ଞା(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
