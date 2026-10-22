/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "dirent-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "mountpoint-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "recurse-dir.h"
#அந்தர்பாவ "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

நிலையான முழு sort_func(கட்டமைப்பு dirent * மாறிலி *a, கட்டமைப்பு dirent * மாறிலி *b) {
        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        திரும்பு strcmp((*a)->d_name, (*b)->d_name);
}

நிலையான bool ignore_dirent(மாறிலி கட்டமைப்பு dirent *de, RecurseDirFlags flags) {
        ப்ரதிஜ்ஞா(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        எனில் ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                தேர்வு (de->d_type) {

                நிலை DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        நிறுத்து;

                நிலை DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        நிறுத்து;

                நிலை DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        நிறுத்து;

                நிலை DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        நிறுத்து;

                இயல்பு:
                        திரும்பு true;
                }

                எனில் (!FLAGS_SET(flags, f))
                        திரும்பு true;
        }

        திரும்பு FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

முழு readdir_all(முழு dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = வெற்று;
        DirectoryEntries *nde;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0);

        எனில் ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        எனில் (!de)
                திரும்பு -ENOMEM;

        de->buffer_size = 0;
        க்கு (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                ப்ரதிஜ்ஞா(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                ப்ரதிஜ்ஞா(bs > de->buffer_size);

                n = getdents64(dir_fd, (கட்டமைப்பு dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                எனில் (n < 0)
                        திரும்பு -errno;
                எனில் (n == 0)
                        நிறுத்து;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                எனில் (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        தொடர்;

                எனில் (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        திரும்பு -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                எனில் (!nde)
                        திரும்பு -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        கட்டமைப்பு dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                எனில் (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        எனில் (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                தொடர்;
                        எனில் (r < 0)
                                திரும்பு r;
                }

                எனில் (ignore_dirent(entry, flags))
                        தொடர்;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        எனில் (!INC_SAFE(&sz, அளவு(கட்டமைப்பு dirent*) * de->n_entries))
                திரும்பு -ENOMEM;

        nde = realloc(de, sz);
        எனில் (!nde)
                திரும்பு -ENOMEM;
        de = nde;

        de->entries = (கட்டமைப்பு dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                எனில் (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        தொடர்;

                எனில் (ignore_dirent(entry, flags))
                        தொடர்;

                de->entries[j++] = entry;
        }
        ப்ரதிஜ்ஞா(j == de->n_entries);

        எனில் (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        எனில் (ret)
                *ret = TAKE_PTR(de);

        திரும்பு 0;
}

முழு readdir_all_at(முழு fd, மாறிலி எழுத்து *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ முழு dir_fd = -EBADF;

        ப்ரதிஜ்ஞா(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        எனில் (dir_fd < 0)
                திரும்பு dir_fd;

        திரும்பு readdir_all(dir_fd, flags, ret);
}

முழு recurse_dir(
                முழு dir_fd,
                மாறிலி எழுத்து *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                வெறுமை *userdata) {

        _cleanup_free_ DirectoryEntries *de = வெற்று;
        கட்டமைப்பு statx வேர்_sx;
        முழு r;

        ப்ரதிஜ்ஞா(dir_fd >= 0);
        ப்ரதிஜ்ஞா(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        ப்ரதிஜ்ஞா((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        எனில் (n_depth_max == 0)
                திரும்பு -EOVERFLOW;
        எனில் (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        எனில் (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                எனில் (statx_mask != 0) {
                        எனில் (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &வேர்_sx) < 0)
                                திரும்பு -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         வெற்று, /* we have no dirent */
                         statx_mask != 0 ? &வேர்_sx : வெற்று,
                         userdata);
                எனில் (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        திரும்பு 0;
                எனில் (r != RECURSE_DIR_CONTINUE)
                        திரும்பு r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        எனில் (r < 0)
                திரும்பு r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                கட்டமைப்பு dirent *i = *entry;
                _cleanup_close_ முழு inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ எழுத்து *joined = வெற்று;
                கட்டமைப்பு statx sx;
                bool sx_valid = false;
                மாறிலி எழுத்து *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                எனில் (path) {
                        joined = path_join(path, i->d_name);
                        எனில் (!joined)
                                திரும்பு -ENOMEM;

                        p = joined;
                } இல்லையெனில்
                        p = i->d_name;

                எனில் (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        எனில் (subdir_fd < 0) {
                                எனில் (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        தொடர்;

                                /* If it is a subdir but we failed to open it, then fail */
                                எனில் (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        ப்ரதிஜ்ஞா(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ வெற்று,
                                                 userdata);
                                        எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                நிறுத்து;
                                        எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                திரும்பு r;

                                        தொடர்;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } இல்லையெனில் {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                எனில் (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        எனில் (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                திரும்பு -errno;

                                        sx_valid = true;
                                }
                        }
                }

                எனில் (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        எனில் (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                எனில் (inode_fd < 0) {
                                        எனில் (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                தொடர்;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        ப்ரதிஜ்ஞா(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ வெற்று,
                                                 userdata);
                                        எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                நிறுத்து;
                                        எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                திரும்பு r;

                                        தொடர்;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                எனில் (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        திரும்பு -errno;

                                ப்ரதிஜ்ஞா(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                எனில் (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        எனில் (subdir_fd < 0)
                                                திரும்பு subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } இல்லையெனில் எனில் (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                எனில் (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        எனில் (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                தொடர்;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        ப்ரதிஜ்ஞா(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ வெற்று,
                                                 userdata);
                                        எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                நிறுத்து;
                                        எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                திரும்பு r;

                                        தொடர்;
                                }

                                ப்ரதிஜ்ஞா(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                எனில் (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ வெற்று,
                                                 userdata);
                                        எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                நிறுத்து;
                                        எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                திரும்பு r;

                                        தொடர்;
                                }
                        }
                }

                எனில் (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        எனில் (sx.stx_mask & STATX_TYPE) {
                                ப்ரதிஜ்ஞா((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        எனில் (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                எனில் (subdir_fd >= 0) {
                        எனில் (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                எனில் (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_வேர்))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_வேர்);
                                இல்லையெனில் {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        எனில் (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                எனில் (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : வெற்று, /* only pass sx if user asked for it */
                                                 userdata);
                                        எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                நிறுத்து;
                                        எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                திரும்பு r;

                                        தொடர்;
                                }
                        }

                        எனில் (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : வெற்று, /* only pass sx if user asked for it */
                                         userdata);
                                எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        நிறுத்து;
                                எனில் (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        திரும்பு r;

                                தொடர்;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : வெற்று, /* only pass sx if user asked for it */
                                 userdata);
                        எனில் (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                எனில் (r != 0)
                                        திரும்பு r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : வெற்று, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } இல்லையெனில்
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : வெற்று, /* only pass sx if user asked for it */
                                 userdata);

                எனில் (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        நிறுத்து;

                எனில் (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        முழு f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        எனில் (unlinkat(dir_fd, i->d_name, f) < 0) {
                                எனில் (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        திரும்பு -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } இல்லையெனில் எனில் (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        திரும்பு r;
        }

        எனில் (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         வெற்று, /* we have no dirent */
                         statx_mask != 0 ? &வேர்_sx : வெற்று,
                         userdata);
                எனில் (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        திரும்பு r;
        }

        திரும்பு 0;
}

முழு recurse_dir_at(
                முழு atfd,
                மாறிலி எழுத்து *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                வெறுமை *userdata) {

        _cleanup_close_ முழு fd = -EBADF;

        ப்ரதிஜ்ஞா(atfd >= 0 || atfd == AT_FDCWD);
        ப்ரதிஜ்ஞா(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        எனில் (fd < 0)
                திரும்பு -errno;

        திரும்பு recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
