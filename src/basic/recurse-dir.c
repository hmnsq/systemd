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

        प्रत्यागम म_भेद((*a)->d_name, (*b)->d_name);
}

स्थिर bool ignore_dirent(ध्रुव संरचना dirent *de, RecurseDirFlags flags) {
        प्रतिज्ञा(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        यदि ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
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
                        प्रत्यागम true;
                }

                यदि (!FLAGS_SET(flags, f))
                        प्रत्यागम true;
        }

        प्रत्यागम FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

पूर्णाङ्क readdir_all(पूर्णाङ्क dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = रिक्त;
        DirectoryEntries *nde;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0);

        यदि ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(दुरत्व(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = दो_स्मृति(दुरत्व(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        यदि (!de)
                प्रत्यागम -ENOMEM;

        de->buffer_size = 0;
        प्रति (;;) {
                माप_प्रकार bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                प्रतिज्ञा(MALLOC_SIZEOF_SAFE(de) >= दुरत्व(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - दुरत्व(DirectoryEntries, buffer), (माप_प्रकार) SSIZE_MAX);
                प्रतिज्ञा(bs > de->buffer_size);

                n = getdents64(dir_fd, (संरचना dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                यदि (n < 0)
                        प्रत्यागम -त्रुटि_सं;
                यदि (n == 0)
                        भङ्ग;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                यदि (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        अनुवर्त;

                यदि (bs >= SSIZE_MAX - दुरत्व(DirectoryEntries, buffer))
                        प्रत्यागम -EFBIG;
                bs = bs >= (SSIZE_MAX - दुरत्व(DirectoryEntries, buffer))/2 ? SSIZE_MAX - दुरत्व(DirectoryEntries, buffer) : bs * 2;

                nde = पुनः_स्मृति(de, bs);
                यदि (!nde)
                        प्रत्यागम -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        संरचना dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                यदि (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        यदि (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                अनुवर्त;
                        यदि (r < 0)
                                प्रत्यागम r;
                }

                यदि (ignore_dirent(entry, flags))
                        अनुवर्त;

                de->n_entries++;
        }

        माप_प्रकार sz, j;

        sz = ALIGN(दुरत्व(DirectoryEntries, buffer) + de->buffer_size);
        यदि (!INC_SAFE(&sz, परिमाण(संरचना dirent*) * de->n_entries))
                प्रत्यागम -ENOMEM;

        nde = पुनः_स्मृति(de, sz);
        यदि (!nde)
                प्रत्यागम -ENOMEM;
        de = nde;

        de->entries = (संरचना dirent**) ((uint8_t*) de + ALIGN(दुरत्व(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                यदि (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        अनुवर्त;

                यदि (ignore_dirent(entry, flags))
                        अनुवर्त;

                de->entries[j++] = entry;
        }
        प्रतिज्ञा(j == de->n_entries);

        यदि (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        यदि (ret)
                *ret = TAKE_PTR(de);

        प्रत्यागम 0;
}

पूर्णाङ्क readdir_all_at(पूर्णाङ्क fd, ध्रुव वर्ण *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ पूर्णाङ्क dir_fd = -EBADF;

        प्रतिज्ञा(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        यदि (dir_fd < 0)
                प्रत्यागम dir_fd;

        प्रत्यागम readdir_all(dir_fd, flags, ret);
}

पूर्णाङ्क recurse_dir(
                पूर्णाङ्क dir_fd,
                ध्रुव वर्ण *path,
                अचिन्हित statx_mask,
                अचिन्हित n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                शून्य *userdata) {

        _cleanup_free_ DirectoryEntries *de = रिक्त;
        संरचना statx मूल_sx;
        पूर्णाङ्क r;

        प्रतिज्ञा(dir_fd >= 0);
        प्रतिज्ञा(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        प्रतिज्ञा((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        यदि (n_depth_max == 0)
                प्रत्यागम -EOVERFLOW;
        यदि (n_depth_max == अच_पूर्णांक_उच्च) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        यदि (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                यदि (statx_mask != 0) {
                        यदि (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &मूल_sx) < 0)
                                प्रत्यागम -त्रुटि_सं;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         रिक्त, /* we have no dirent */
                         statx_mask != 0 ? &मूल_sx : रिक्त,
                         userdata);
                यदि (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        प्रत्यागम 0;
                यदि (r != RECURSE_DIR_CONTINUE)
                        प्रत्यागम r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        यदि (r < 0)
                प्रत्यागम r;

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

                यदि (path) {
                        joined = path_join(path, i->d_name);
                        यदि (!joined)
                                प्रत्यागम -ENOMEM;

                        p = joined;
                } अन्यथा
                        p = i->d_name;

                यदि (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        यदि (subdir_fd < 0) {
                                यदि (त्रुटि_सं == ENOENT) /* Vanished by now, go for next file immediately */
                                        अनुवर्त;

                                /* If it is a subdir but we failed to open it, then fail */
                                यदि (!IN_SET(त्रुटि_सं, ENOTDIR, ELOOP)) {
                                        log_debug_errno(त्रुटि_सं, "Failed to open directory '%s': %m", p);

                                        प्रतिज्ञा(त्रुटि_सं <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                प्रत्यागम r;

                                        अनुवर्त;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } अन्यथा {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                यदि (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        यदि (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                प्रत्यागम -त्रुटि_सं;

                                        sx_valid = true;
                                }
                        }
                }

                यदि (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        यदि (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                यदि (inode_fd < 0) {
                                        यदि (त्रुटि_सं == ENOENT) /* Vanished by now, go for next file immediately */
                                                अनुवर्त;

                                        log_debug_errno(त्रुटि_सं, "Failed to open directory entry '%s': %m", p);

                                        प्रतिज्ञा(त्रुटि_सं <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                प्रत्यागम r;

                                        अनुवर्त;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                यदि (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        प्रत्यागम -त्रुटि_सं;

                                प्रतिज्ञा(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                यदि (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        यदि (subdir_fd < 0)
                                                प्रत्यागम subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } अन्यथा यदि (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                यदि (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        यदि (त्रुटि_सं == ENOENT) /* Vanished by now? Go for next file immediately */
                                                अनुवर्त;

                                        log_debug_errno(त्रुटि_सं, "Failed to stat directory entry '%s': %m", p);

                                        प्रतिज्ञा(त्रुटि_सं <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ रिक्त,
                                                 userdata);
                                        यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                प्रत्यागम r;

                                        अनुवर्त;
                                }

                                प्रतिज्ञा(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                यदि (S_ISDIR(sx.stx_mode)) {
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
                                        यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                प्रत्यागम r;

                                        अनुवर्त;
                                }
                        }
                }

                यदि (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        यदि (sx.stx_mask & STATX_TYPE) {
                                प्रतिज्ञा((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        यदि (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                यदि (subdir_fd >= 0) {
                        यदि (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                यदि (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_मूल))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_मूल);
                                अन्यथा {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        यदि (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                यदि (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                                 userdata);
                                        यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                भङ्ग;
                                        यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                प्रत्यागम r;

                                        अनुवर्त;
                                }
                        }

                        यदि (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                         userdata);
                                यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        भङ्ग;
                                यदि (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        प्रत्यागम r;

                                अनुवर्त;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                 userdata);
                        यदि (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                यदि (r != 0)
                                        प्रत्यागम r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } अन्यथा
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : रिक्त, /* only pass sx if user asked for it */
                                 userdata);

                यदि (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        भङ्ग;

                यदि (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        पूर्णाङ्क f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        यदि (unlinkat(dir_fd, i->d_name, f) < 0) {
                                यदि (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        प्रत्यागम -त्रुटि_सं;

                                log_debug_errno(त्रुटि_सं, "Unable to remove '%s', ignoring: %m", p);
                        }
                } अन्यथा यदि (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        प्रत्यागम r;
        }

        यदि (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         रिक्त, /* we have no dirent */
                         statx_mask != 0 ? &मूल_sx : रिक्त,
                         userdata);
                यदि (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        प्रत्यागम r;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क recurse_dir_at(
                पूर्णाङ्क atfd,
                ध्रुव वर्ण *path,
                अचिन्हित statx_mask,
                अचिन्हित n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                शून्य *userdata) {

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        प्रतिज्ञा(atfd >= 0 || atfd == AT_FDCWD);
        प्रतिज्ञा(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        यदि (fd < 0)
                प्रत्यागम -त्रुटि_सं;

        प्रत्यागम recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
