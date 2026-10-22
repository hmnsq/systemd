/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "mountpoint-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "recurse-dir.h"
#അന്തര്ഭാവ "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

സ്ഥിതം പൂർണ്ണസംഖ്യ sort_func(ഘടന dirent * സ്ഥിരം *a, ഘടന dirent * സ്ഥിരം *b) {
        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        തിരികെ strcmp((*a)->d_name, (*b)->d_name);
}

സ്ഥിതം bool ignore_dirent(സ്ഥിരം ഘടന dirent *de, RecurseDirFlags flags) {
        പ്രതിജ്ഞാ(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        എങ്കിൽ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                തിരഞ്ഞെടുപ്പ് (de->d_type) {

                അവസ്ഥ DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        നിർത്തുക;

                അവസ്ഥ DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        നിർത്തുക;

                അവസ്ഥ DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        നിർത്തുക;

                അവസ്ഥ DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        നിർത്തുക;

                സ്ഥിരസ്ഥിതി:
                        തിരികെ true;
                }

                എങ്കിൽ (!FLAGS_SET(flags, f))
                        തിരികെ true;
        }

        തിരികെ FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

പൂർണ്ണസംഖ്യ readdir_all(പൂർണ്ണസംഖ്യ dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = ശൂന്യ;
        DirectoryEntries *nde;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0);

        എങ്കിൽ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        എങ്കിൽ (!de)
                തിരികെ -ENOMEM;

        de->buffer_size = 0;
        വേണ്ടി (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                പ്രതിജ്ഞാ(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                പ്രതിജ്ഞാ(bs > de->buffer_size);

                n = getdents64(dir_fd, (ഘടന dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                എങ്കിൽ (n < 0)
                        തിരികെ -errno;
                എങ്കിൽ (n == 0)
                        നിർത്തുക;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                എങ്കിൽ (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        തുടരുക;

                എങ്കിൽ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        തിരികെ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                എങ്കിൽ (!nde)
                        തിരികെ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        ഘടന dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        എങ്കിൽ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                തുടരുക;
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                }

                എങ്കിൽ (ignore_dirent(entry, flags))
                        തുടരുക;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        എങ്കിൽ (!INC_SAFE(&sz, വലുപ്പം(ഘടന dirent*) * de->n_entries))
                തിരികെ -ENOMEM;

        nde = realloc(de, sz);
        എങ്കിൽ (!nde)
                തിരികെ -ENOMEM;
        de = nde;

        de->entries = (ഘടന dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        തുടരുക;

                എങ്കിൽ (ignore_dirent(entry, flags))
                        തുടരുക;

                de->entries[j++] = entry;
        }
        പ്രതിജ്ഞാ(j == de->n_entries);

        എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(de);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ readdir_all_at(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം അക്ഷരം *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ dir_fd = -EBADF;

        പ്രതിജ്ഞാ(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        എങ്കിൽ (dir_fd < 0)
                തിരികെ dir_fd;

        തിരികെ readdir_all(dir_fd, flags, ret);
}

പൂർണ്ണസംഖ്യ recurse_dir(
                പൂർണ്ണസംഖ്യ dir_fd,
                സ്ഥിരം അക്ഷരം *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ശൂന്യം *userdata) {

        _cleanup_free_ DirectoryEntries *de = ശൂന്യ;
        ഘടന statx വേര്_sx;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dir_fd >= 0);
        പ്രതിജ്ഞാ(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        പ്രതിജ്ഞാ((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        എങ്കിൽ (n_depth_max == 0)
                തിരികെ -EOVERFLOW;
        എങ്കിൽ (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                എങ്കിൽ (statx_mask != 0) {
                        എങ്കിൽ (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &വേര്_sx) < 0)
                                തിരികെ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ശൂന്യ, /* we have no dirent */
                         statx_mask != 0 ? &വേര്_sx : ശൂന്യ,
                         userdata);
                എങ്കിൽ (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        തിരികെ 0;
                എങ്കിൽ (r != RECURSE_DIR_CONTINUE)
                        തിരികെ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                ഘടന dirent *i = *entry;
                _cleanup_close_ പൂർണ്ണസംഖ്യ inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ അക്ഷരം *joined = ശൂന്യ;
                ഘടന statx sx;
                bool sx_valid = false;
                സ്ഥിരം അക്ഷരം *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                എങ്കിൽ (path) {
                        joined = path_join(path, i->d_name);
                        എങ്കിൽ (!joined)
                                തിരികെ -ENOMEM;

                        p = joined;
                } അല്ലെങ്കിൽ
                        p = i->d_name;

                എങ്കിൽ (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        എങ്കിൽ (subdir_fd < 0) {
                                എങ്കിൽ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        തുടരുക;

                                /* If it is a subdir but we failed to open it, then fail */
                                എങ്കിൽ (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        പ്രതിജ്ഞാ(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ശൂന്യ,
                                                 userdata);
                                        എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                നിർത്തുക;
                                        എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                തിരികെ r;

                                        തുടരുക;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } അല്ലെങ്കിൽ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                എങ്കിൽ (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        എങ്കിൽ (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                തിരികെ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                എങ്കിൽ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        എങ്കിൽ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                എങ്കിൽ (inode_fd < 0) {
                                        എങ്കിൽ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                തുടരുക;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        പ്രതിജ്ഞാ(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ശൂന്യ,
                                                 userdata);
                                        എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                നിർത്തുക;
                                        എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                തിരികെ r;

                                        തുടരുക;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                എങ്കിൽ (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        തിരികെ -errno;

                                പ്രതിജ്ഞാ(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                എങ്കിൽ (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        എങ്കിൽ (subdir_fd < 0)
                                                തിരികെ subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } അല്ലെങ്കിൽ എങ്കിൽ (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                എങ്കിൽ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        എങ്കിൽ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                തുടരുക;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        പ്രതിജ്ഞാ(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ ശൂന്യ,
                                                 userdata);
                                        എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                നിർത്തുക;
                                        എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                തിരികെ r;

                                        തുടരുക;
                                }

                                പ്രതിജ്ഞാ(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                എങ്കിൽ (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ ശൂന്യ,
                                                 userdata);
                                        എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                നിർത്തുക;
                                        എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                തിരികെ r;

                                        തുടരുക;
                                }
                        }
                }

                എങ്കിൽ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        എങ്കിൽ (sx.stx_mask & STATX_TYPE) {
                                പ്രതിജ്ഞാ((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        എങ്കിൽ (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                എങ്കിൽ (subdir_fd >= 0) {
                        എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                എങ്കിൽ (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_വേര്))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_വേര്);
                                അല്ലെങ്കിൽ {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        എങ്കിൽ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                എങ്കിൽ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : ശൂന്യ, /* only pass sx if user asked for it */
                                                 userdata);
                                        എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                നിർത്തുക;
                                        എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                തിരികെ r;

                                        തുടരുക;
                                }
                        }

                        എങ്കിൽ (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ശൂന്യ, /* only pass sx if user asked for it */
                                         userdata);
                                എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        നിർത്തുക;
                                എങ്കിൽ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        തിരികെ r;

                                തുടരുക;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ശൂന്യ, /* only pass sx if user asked for it */
                                 userdata);
                        എങ്കിൽ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                എങ്കിൽ (r != 0)
                                        തിരികെ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : ശൂന്യ, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } അല്ലെങ്കിൽ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : ശൂന്യ, /* only pass sx if user asked for it */
                                 userdata);

                എങ്കിൽ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        നിർത്തുക;

                എങ്കിൽ (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        പൂർണ്ണസംഖ്യ f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        എങ്കിൽ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                എങ്കിൽ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        തിരികെ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } അല്ലെങ്കിൽ എങ്കിൽ (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        തിരികെ r;
        }

        എങ്കിൽ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         ശൂന്യ, /* we have no dirent */
                         statx_mask != 0 ? &വേര്_sx : ശൂന്യ,
                         userdata);
                എങ്കിൽ (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        തിരികെ r;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ recurse_dir_at(
                പൂർണ്ണസംഖ്യ atfd,
                സ്ഥിരം അക്ഷരം *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                ശൂന്യം *userdata) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        പ്രതിജ്ഞാ(atfd >= 0 || atfd == AT_FDCWD);
        പ്രതിജ്ഞാ(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        തിരികെ recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
