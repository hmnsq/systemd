/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "mountpoint-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "recurse-dir.h"
#अन्तर्भाव "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

स्थिर पूर्णाङ्क sort_func(संरचना dirent * ध्रुव *a, संरचना dirent * ध्रुव *b) {
        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        पटिनिवत्त strcmp((*a)->d_name, (*b)->d_name);
}

स्थिर bool ignore_dirent(ध्रुव संरचना dirent *de, RecurseDirFlags flags) {
        प्रतिज्ञा(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        सचे ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                विकल्प (de->d_type) {

                पक्ष DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        भङ्ग;

                पक्ष DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        भङ्ग;

                पक्ष DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        भङ्ग;

                पक्ष DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        भङ्ग;

                स्वभाव:
                        पटिनिवत्त true;
                }

                सचे (!FLAGS_SET(flags, f))
                        पटिनिवत्त true;
        }

        पटिनिवत्त FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

पूर्णाङ्क readdir_all(पूर्णाङ्क dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = रिक्त;
        DirectoryEntries *nde;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0);

        सचे ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        सचे (!de)
                पटिनिवत्त -ENOMEM;

        de->buffer_size = 0;
        अत्थाय (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                प्रतिज्ञा(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                प्रतिज्ञा(bs > de->buffer_size);

                n = getdents64(dir_fd, (संरचना dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                सचे (n < 0)
                        पटिनिवत्त -errno;
                सचे (n == 0)
                        भङ्ग;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                सचे (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        अनुवर्त;

                सचे (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        पटिनिवत्त -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                सचे (!nde)
                        पटिनिवत्त -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        संरचना dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                सचे (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        सचे (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                अनुवर्त;
                        सचे (r < 0)
                                पटिनिवत्त r;
                }

                सचे (ignore_dirent(entry, flags))
                        अनुवर्त;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        सचे (!INC_SAFE(&sz, परिमाण(संरचना dirent*) * de->n_entries))
                पटिनिवत्त -ENOMEM;

        nde = realloc(de, sz);
        सचे (!nde)
                पटिनिवत्त -ENOMEM;
        de = nde;

        de->entries = (संरचना dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                सचे (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        अनुवर्त;

                सचे (ignore_dirent(entry, flags))
                        अनुवर्त;

                de->entries[j++] = entry;
        }
        प्रतिज्ञा(j == de->n_entries);

        सचे (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        सचे (ret)
                *ret = TAKE_PTR(de);

        पटिनिवत्त 0;
}

पूर्णाङ्क readdir_all_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ पूर्णाङ्क dir_fd = -EBADF;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        सचे (dir_fd < 0)
                पटिनिवत्त dir_fd;

        पटिनिवत्त readdir_all(dir_fd, flags, ret);
}

पूर्णाङ्क recurse_dir(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                शून्य *userdata) {

        _cleanup_free_ DirectoryEntries *de = रिक्त;
        संरचना statx मूलम्_sx;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0);
        प्रतिज्ञा(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        प्रतिज्ञा((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        सचे (n_depth_max == 0)
                पटिनिवत्त -EOVERFLOW;
        सचे (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        सचे (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                सचे (statx_mask != 0) {
                        सचे (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &मूलम्_sx) < 0)
                                पटिनिवत्त -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         रिक्त, /* we have no dirent */
                         statx_mask != 0 ? &मूलम्_sx : रिक्त,
                         userdata);
                सचे (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        पटिनिवत्त 0;
                सचे (r != RECURSE_DIR_CONTINUE)
                        पटिनिवत्त r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        सचे (r < 0)
                पटिनिवत्त r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                संरचना dirent *i = *entry;
                _cleanup_close_ पूर्णाङ्क inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ वर्ण *joined = रिक्त;
                संरचना statx sx;
                bool sx_valid = false;
                ध्रुव वर्ण *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                सचे (path) {
                        joined = path_join(path, i->d_name);
                        सचे (!joined)
                                पटिनिवत्त -ENOMEM;

                        p = joined;
                } अञ्ञथा
                        p = i->d_name;

                सचे (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        सचे (subdir_fd < 0) {
                                सचे (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        अनुवर्त;

                                /* If it is a subdir but we failed to open it, then fail */
                                सचे (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        प्रतिज्ञा(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                पटिनिवत्त r;

                                        अनुवर्त;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } अञ्ञथा {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                सचे (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        सचे (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                पटिनिवत्त -errno;

                                        sx_valid = true;
                                }
                        }
                }

                सचे (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        सचे (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                सचे (inode_fd < 0) {
                                        सचे (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                अनुवर्त;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        प्रतिज्ञा(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                पटिनिवत्त r;

                                        अनुवर्त;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                सचे (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        पटिनिवत्त -errno;

                                प्रतिज्ञा(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                सचे (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        सचे (subdir_fd < 0)
                                                पटिनिवत्त subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } अञ्ञथा सचे (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                सचे (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        सचे (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                अनुवर्त;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        प्रतिज्ञा(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                पटिनिवत्त r;

                                        अनुवर्त;
                                }

                                प्रतिज्ञा(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                सचे (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                पटिनिवत्त r;

                                        अनुवर्त;
                                }
                        }
                }

                सचे (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        सचे (sx.stx_mask & STATX_TYPE) {
                                प्रतिज्ञा((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        सचे (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                सचे (subdir_fd >= 0) {
                        सचे (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                सचे (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_मूलम्))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_मूलम्);
                                अञ्ञथा {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        सचे (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                सचे (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                                 userdata);
                                        सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                पटिनिवत्त r;

                                        अनुवर्त;
                                }
                        }

                        सचे (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                         userdata);
                                सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        भङ्ग;
                                सचे (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        पटिनिवत्त r;

                                अनुवर्त;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                 userdata);
                        सचे (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                सचे (r != 0)
                                        पटिनिवत्त r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } अञ्ञथा
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                 userdata);

                सचे (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        भङ्ग;

                सचे (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        पूर्णाङ्क f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        सचे (unlinkat(dir_fd, i->d_name, f) < 0) {
                                सचे (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        पटिनिवत्त -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } अञ्ञथा सचे (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        पटिनिवत्त r;
        }

        सचे (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         रिक्त, /* we have no dirent */
                         statx_mask != 0 ? &मूलम्_sx : रिक्त,
                         userdata);
                सचे (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        पटिनिवत्त r;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क recurse_dir_at(
                पूर्णाङ्क atfd,
                ध्रुव वर्ण *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                शून्य *userdata) {

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(atfd >= 0 || atfd == AT_FDCWD);
        प्रतिज्ञा(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        सचे (fd < 0)
                पटिनिवत्त -errno;

        पटिनिवत्त recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
