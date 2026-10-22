/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "dirent-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "mountpoint-util.h"
#समावेश "path-util.h"
#समावेश "recurse-dir.h"
#समावेश "sort-util.h"

#घोषणा DEFAULT_RECURSION_MAX 100

अटल पूर्णांक sort_func(काष्ठा dirent * स्थिर *a, काष्ठा dirent * स्थिर *b) {
        निश्चित(a);
        निश्चित(b);

        वापस म_भेद((*a)->d_name, (*b)->d_name);
}

अटल bool ignore_dirent(स्थिर काष्ठा dirent *de, RecurseDirFlags flags) {
        निश्चित(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        अगर ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                चयन (de->d_type) {

                हाल DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        अवरोध;

                हाल DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        अवरोध;

                हाल DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        अवरोध;

                हाल DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        अवरोध;

                शेष:
                        वापस true;
                }

                अगर (!FLAGS_SET(flags, f))
                        वापस true;
        }

        वापस FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

पूर्णांक readdir_all(पूर्णांक dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = शून्य;
        DirectoryEntries *nde;
        पूर्णांक r;

        निश्चित(dir_fd >= 0);

        अगर ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(दुरत्व(DirectoryEntries, बफर) <= SIZE_MAX - DIRENT_आकार_MAX * 8);
        de = दो_स्मृति(दुरत्व(DirectoryEntries, बफर) + DIRENT_आकार_MAX * 8);
        अगर (!de)
                वापस -ENOMEM;

        de->बफर_आकार = 0;
        क्रम (;;) {
                माप_प्रकार bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                निश्चित(MALLOC_SIZEOF_SAFE(de) >= दुरत्व(DirectoryEntries, बफर));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - दुरत्व(DirectoryEntries, बफर), (माप_प्रकार) SSIZE_MAX);
                निश्चित(bs > de->बफर_आकार);

                n = getdents64(dir_fd, (काष्ठा dirent*) ((uint8_t*) de->बफर + de->बफर_आकार), bs - de->बफर_आकार);
                अगर (n < 0)
                        वापस -त्रुटि_सं;
                अगर (n == 0)
                        अवरोध;

                msan_unpoison((uint8_t*) de->बफर + de->बफर_आकार, n);

                de->बफर_आकार += n;

                अगर (de->बफर_आकार < bs - DIRENT_आकार_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        जारी;

                अगर (bs >= SSIZE_MAX - दुरत्व(DirectoryEntries, बफर))
                        वापस -EFBIG;
                bs = bs >= (SSIZE_MAX - दुरत्व(DirectoryEntries, बफर))/2 ? SSIZE_MAX - दुरत्व(DirectoryEntries, बफर) : bs * 2;

                nde = पुनः_स्मृति(de, bs);
                अगर (!nde)
                        वापस -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        काष्ठा dirent *entry;
        FOREACH_DIRENT_IN_बफर(entry, de->बफर, de->बफर_आकार) {
                अगर (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        अगर (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                जारी;
                        अगर (r < 0)
                                वापस r;
                }

                अगर (ignore_dirent(entry, flags))
                        जारी;

                de->n_entries++;
        }

        माप_प्रकार sz, j;

        sz = ALIGN(दुरत्व(DirectoryEntries, बफर) + de->बफर_आकार);
        अगर (!INC_SAFE(&sz, माप(काष्ठा dirent*) * de->n_entries))
                वापस -ENOMEM;

        nde = पुनः_स्मृति(de, sz);
        अगर (!nde)
                वापस -ENOMEM;
        de = nde;

        de->entries = (काष्ठा dirent**) ((uint8_t*) de + ALIGN(दुरत्व(DirectoryEntries, बफर) + de->बफर_आकार));

        j = 0;
        FOREACH_DIRENT_IN_बफर(entry, de->बफर, de->बफर_आकार) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                अगर (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        जारी;

                अगर (ignore_dirent(entry, flags))
                        जारी;

                de->entries[j++] = entry;
        }
        निश्चित(j == de->n_entries);

        अगर (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        अगर (ret)
                *ret = TAKE_PTR(de);

        वापस 0;
}

पूर्णांक readdir_all_at(पूर्णांक fd, स्थिर अक्षर *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ पूर्णांक dir_fd = -EBADF;

        निश्चित(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        अगर (dir_fd < 0)
                वापस dir_fd;

        वापस readdir_all(dir_fd, flags, ret);
}

पूर्णांक recurse_dir(
                पूर्णांक dir_fd,
                स्थिर अक्षर *path,
                अचिन्हित statx_mask,
                अचिन्हित n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                व्योम *userdata) {

        _cleanup_free_ DirectoryEntries *de = शून्य;
        काष्ठा statx मूल_sx;
        पूर्णांक r;

        निश्चित(dir_fd >= 0);
        निश्चित(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        निश्चित((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        अगर (n_depth_max == 0)
                वापस -EOVERFLOW;
        अगर (n_depth_max == अच_पूर्णांक_उच्च) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        अगर (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                अगर (statx_mask != 0) {
                        अगर (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &मूल_sx) < 0)
                                वापस -त्रुटि_सं;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         शून्य, /* we have no dirent */
                         statx_mask != 0 ? &मूल_sx : शून्य,
                         userdata);
                अगर (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        वापस 0;
                अगर (r != RECURSE_DIR_CONTINUE)
                        वापस r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        अगर (r < 0)
                वापस r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                काष्ठा dirent *i = *entry;
                _cleanup_close_ पूर्णांक inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ अक्षर *joined = शून्य;
                काष्ठा statx sx;
                bool sx_valid = false;
                स्थिर अक्षर *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                अगर (path) {
                        joined = path_join(path, i->d_name);
                        अगर (!joined)
                                वापस -ENOMEM;

                        p = joined;
                } अन्यथा
                        p = i->d_name;

                अगर (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        अगर (subdir_fd < 0) {
                                अगर (त्रुटि_सं == ENOENT) /* Vanished by now, go for next file immediately */
                                        जारी;

                                /* If it is a subdir but we failed to open it, then fail */
                                अगर (!IN_SET(त्रुटि_सं, ENOTDIR, ELOOP)) {
                                        log_debug_errno(त्रुटि_सं, "Failed to open directory '%s': %m", p);

                                        निश्चित(त्रुटि_सं <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ शून्य,
                                                 userdata);
                                        अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                अवरोध;
                                        अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                वापस r;

                                        जारी;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } अन्यथा {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                अगर (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        अगर (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                वापस -त्रुटि_सं;

                                        sx_valid = true;
                                }
                        }
                }

                अगर (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        अगर (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                अगर (inode_fd < 0) {
                                        अगर (त्रुटि_सं == ENOENT) /* Vanished by now, go for next file immediately */
                                                जारी;

                                        log_debug_errno(त्रुटि_सं, "Failed to open directory entry '%s': %m", p);

                                        निश्चित(त्रुटि_सं <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ शून्य,
                                                 userdata);
                                        अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                अवरोध;
                                        अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                वापस r;

                                        जारी;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                अगर (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        वापस -त्रुटि_सं;

                                निश्चित(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                अगर (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        अगर (subdir_fd < 0)
                                                वापस subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } अन्यथा अगर (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                अगर (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        अगर (त्रुटि_सं == ENOENT) /* Vanished by now? Go for next file immediately */
                                                जारी;

                                        log_debug_errno(त्रुटि_सं, "Failed to stat directory entry '%s': %m", p);

                                        निश्चित(त्रुटि_सं <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + त्रुटि_सं,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ शून्य,
                                                 userdata);
                                        अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                अवरोध;
                                        अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                वापस r;

                                        जारी;
                                }

                                निश्चित(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                अगर (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ शून्य,
                                                 userdata);
                                        अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                अवरोध;
                                        अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                वापस r;

                                        जारी;
                                }
                        }
                }

                अगर (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        अगर (sx.stx_mask & STATX_TYPE) {
                                निश्चित((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        अगर (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                अगर (subdir_fd >= 0) {
                        अगर (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                अगर (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_मूल))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_मूल);
                                अन्यथा {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        अगर (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                अगर (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : शून्य, /* only pass sx if user asked for it */
                                                 userdata);
                                        अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                अवरोध;
                                        अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                वापस r;

                                        जारी;
                                }
                        }

                        अगर (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : शून्य, /* only pass sx if user asked for it */
                                         userdata);
                                अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        अवरोध;
                                अगर (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        वापस r;

                                जारी;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : शून्य, /* only pass sx if user asked for it */
                                 userdata);
                        अगर (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                अगर (r != 0)
                                        वापस r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : शून्य, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } अन्यथा
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : शून्य, /* only pass sx if user asked for it */
                                 userdata);

                अगर (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        अवरोध;

                अगर (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        पूर्णांक f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        अगर (unlinkat(dir_fd, i->d_name, f) < 0) {
                                अगर (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        वापस -त्रुटि_सं;

                                log_debug_errno(त्रुटि_सं, "Unable to remove '%s', ignoring: %m", p);
                        }
                } अन्यथा अगर (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        वापस r;
        }

        अगर (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         शून्य, /* we have no dirent */
                         statx_mask != 0 ? &मूल_sx : शून्य,
                         userdata);
                अगर (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        वापस r;
        }

        वापस 0;
}

पूर्णांक recurse_dir_at(
                पूर्णांक atfd,
                स्थिर अक्षर *path,
                अचिन्हित statx_mask,
                अचिन्हित n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                व्योम *userdata) {

        _cleanup_close_ पूर्णांक fd = -EBADF;

        निश्चित(atfd >= 0 || atfd == AT_FDCWD);
        निश्चित(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        वापस recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
