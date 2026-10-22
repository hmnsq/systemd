/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "dirent-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "log.h"
#include "mountpoint-util.h"
#include "path-util.h"
#include "recurse-dir.h"
#include "sort-util.h"

#define DEFAULT_RECURSION_MAX 100

static puur_naa_nka sort_func(sa_nracanaa dirent * const *a, sa_nracanaa dirent * const *b) {
        assert(a);
        assert(b);

        ᰀᰧᰌ strcmp((*a)->d_name, (*b)->d_name);
}

static bool ignore_dirent(const sa_nracanaa dirent *de, RecurseDirFlags flags) {
        assert(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        ᰀᰧ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                switch (de->d_type) {

                pak_sa DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        bha_nga;

                pak_sa DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        bha_nga;

                pak_sa DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        bha_nga;

                pak_sa DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        bha_nga;

                default:
                        ᰀᰧᰌ true;
                }

                ᰀᰧ (!FLAGS_SET(flags, f))
                        ᰀᰧᰌ true;
        }

        ᰀᰧᰌ FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

puur_naa_nka readdir_all(puur_naa_nka dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = NULL;
        DirectoryEntries *nde;
        puur_naa_nka r;

        assert(dir_fd >= 0);

        ᰀᰧ ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        ᰀᰧ (!de)
                ᰀᰧᰌ -ENOMEM;

        de->buffer_size = 0;
        for (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                assert(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                assert(bs > de->buffer_size);

                n = getdents64(dir_fd, (sa_nracanaa dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                ᰀᰧ (n < 0)
                        ᰀᰧᰌ -errno;
                ᰀᰧ (n == 0)
                        bha_nga;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                ᰀᰧ (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        continue;

                ᰀᰧ (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        ᰀᰧᰌ -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                ᰀᰧ (!nde)
                        ᰀᰧᰌ -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        sa_nracanaa dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        ᰀᰧ (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                continue;
                        ᰀᰧ (r < 0)
                                ᰀᰧᰌ r;
                }

                ᰀᰧ (ignore_dirent(entry, flags))
                        continue;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        ᰀᰧ (!INC_SAFE(&sz, parimaa_na(sa_nracanaa dirent*) * de->n_entries))
                ᰀᰧᰌ -ENOMEM;

        nde = realloc(de, sz);
        ᰀᰧ (!nde)
                ᰀᰧᰌ -ENOMEM;
        de = nde;

        de->entries = (sa_nracanaa dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        continue;

                ᰀᰧ (ignore_dirent(entry, flags))
                        continue;

                de->entries[j++] = entry;
        }
        assert(j == de->n_entries);

        ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        ᰀᰧ (ret)
                *ret = TAKE_PTR(de);

        ᰀᰧᰌ 0;
}

puur_naa_nka readdir_all_at(puur_naa_nka fd, const var_na *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ puur_naa_nka dir_fd = -EBADF;

        assert(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        ᰀᰧ (dir_fd < 0)
                ᰀᰧᰌ dir_fd;

        ᰀᰧᰌ readdir_all(dir_fd, flags, ret);
}

puur_naa_nka recurse_dir(
                puur_naa_nka dir_fd,
                const var_na *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                void *userdata) {

        _cleanup_free_ DirectoryEntries *de = NULL;
        sa_nracanaa statx root_sx;
        puur_naa_nka r;

        assert(dir_fd >= 0);
        assert(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        assert((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        ᰀᰧ (n_depth_max == 0)
                ᰀᰧᰌ -EOVERFLOW;
        ᰀᰧ (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                ᰀᰧ (statx_mask != 0) {
                        ᰀᰧ (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &root_sx) < 0)
                                ᰀᰧᰌ -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         NULL, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : NULL,
                         userdata);
                ᰀᰧ (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        ᰀᰧᰌ 0;
                ᰀᰧ (r != RECURSE_DIR_CONTINUE)
                        ᰀᰧᰌ r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                sa_nracanaa dirent *i = *entry;
                _cleanup_close_ puur_naa_nka inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ var_na *joined = NULL;
                sa_nracanaa statx sx;
                bool sx_valid = false;
                const var_na *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                ᰀᰧ (path) {
                        joined = path_join(path, i->d_name);
                        ᰀᰧ (!joined)
                                ᰀᰧᰌ -ENOMEM;

                        p = joined;
                } ᰀᰧᰀ
                        p = i->d_name;

                ᰀᰧ (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        ᰀᰧ (subdir_fd < 0) {
                                ᰀᰧ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        continue;

                                /* If it is a subdir but we failed to open it, then fail */
                                ᰀᰧ (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        assert(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ NULL,
                                                 userdata);
                                        ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                bha_nga;
                                        ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ᰀᰧᰌ r;

                                        continue;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } ᰀᰧᰀ {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                ᰀᰧ (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        ᰀᰧ (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                ᰀᰧᰌ -errno;

                                        sx_valid = true;
                                }
                        }
                }

                ᰀᰧ (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        ᰀᰧ (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                ᰀᰧ (inode_fd < 0) {
                                        ᰀᰧ (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                                continue;

                                        log_debug_errno(errno, "Failed to open directory entry '%s': %m", p);

                                        assert(errno <= RECURSE_DIR_SKIP_OPEN_INODE_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ NULL,
                                                 userdata);
                                        ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                bha_nga;
                                        ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ᰀᰧᰌ r;

                                        continue;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                ᰀᰧ (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        ᰀᰧᰌ -errno;

                                assert(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ᰀᰧ (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        ᰀᰧ (subdir_fd < 0)
                                                ᰀᰧᰌ subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } ᰀᰧᰀ ᰀᰧ (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                ᰀᰧ (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        ᰀᰧ (errno == ENOENT) /* Vanished by now? Go for next file immediately */
                                                continue;

                                        log_debug_errno(errno, "Failed to stat directory entry '%s': %m", p);

                                        assert(errno <= RECURSE_DIR_SKIP_STAT_INODE_ERROR_MAX - RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_STAT_INODE_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ NULL,
                                                 userdata);
                                        ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                bha_nga;
                                        ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ᰀᰧᰌ r;

                                        continue;
                                }

                                assert(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                ᰀᰧ (S_ISDIR(sx.stx_mode)) {
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
                                                 /* sx= */ NULL,
                                                 userdata);
                                        ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                bha_nga;
                                        ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ᰀᰧᰌ r;

                                        continue;
                                }
                        }
                }

                ᰀᰧ (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        ᰀᰧ (sx.stx_mask & STATX_TYPE) {
                                assert((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        ᰀᰧ (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                ᰀᰧ (subdir_fd >= 0) {
                        ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                ᰀᰧ (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ROOT))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT);
                                ᰀᰧᰀ {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        ᰀᰧ (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                ᰀᰧ (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                                 userdata);
                                        ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                bha_nga;
                                        ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                ᰀᰧᰌ r;

                                        continue;
                                }
                        }

                        ᰀᰧ (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                         userdata);
                                ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        bha_nga;
                                ᰀᰧ (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        ᰀᰧᰌ r;

                                continue;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                 userdata);
                        ᰀᰧ (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                ᰀᰧ (r != 0)
                                        ᰀᰧᰌ r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } ᰀᰧᰀ
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                 userdata);

                ᰀᰧ (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        bha_nga;

                ᰀᰧ (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        puur_naa_nka f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        ᰀᰧ (unlinkat(dir_fd, i->d_name, f) < 0) {
                                ᰀᰧ (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        ᰀᰧᰌ -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } ᰀᰧᰀ ᰀᰧ (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ᰀᰧᰌ r;
        }

        ᰀᰧ (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         NULL, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : NULL,
                         userdata);
                ᰀᰧ (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        ᰀᰧᰌ r;
        }

        ᰀᰧᰌ 0;
}

puur_naa_nka recurse_dir_at(
                puur_naa_nka atfd,
                const var_na *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                void *userdata) {

        _cleanup_close_ puur_naa_nka fd = -EBADF;

        assert(atfd >= 0 || atfd == AT_FDCWD);
        assert(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        ᰀᰧ (fd < 0)
                ᰀᰧᰌ -errno;

        ᰀᰧᰌ recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
