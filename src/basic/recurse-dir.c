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

static 整数 sort_func(struct dirent * const *a, struct dirent * const *b) {
        assert(a);
        assert(b);

        返回 strcmp((*a)->d_name, (*b)->d_name);
}

static bool ignore_dirent(const struct dirent *de, RecurseDirFlags flags) {
        assert(de);

        /* Depending on flag either ignore everything starting with ".", or just "." itself and ".." */

        如果 ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) {
                RecurseDirFlags f;

                switch (de->d_type) {

                case DT_DIR:
                        f = RECURSE_DIR_MUST_BE_DIRECTORY;
                        break;

                case DT_REG:
                        f = RECURSE_DIR_MUST_BE_REGULAR;
                        break;

                case DT_LNK:
                        f = RECURSE_DIR_MUST_BE_SYMLINK;
                        break;

                case DT_SOCK:
                        f = RECURSE_DIR_MUST_BE_SOCKET;
                        break;

                default:
                        返回 true;
                }

                如果 (!FLAGS_SET(flags, f))
                        返回 true;
        }

        返回 FLAGS_SET(flags, RECURSE_DIR_IGNORE_DOT) ?
                de->d_name[0] == '.' :
                dot_or_dot_dot(de->d_name);
}

整数 readdir_all(整数 dir_fd, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_free_ DirectoryEntries *de = NULL;
        DirectoryEntries *nde;
        整数 r;

        assert(dir_fd >= 0);

        如果 ((flags & _RECURSE_DIR_MUST_BE_MASK) != 0) /* We need the type to validate it */
                flags |= RECURSE_DIR_ENSURE_TYPE;

        /* Returns an array with pointers to "struct dirent" directory entries, optionally sorted.
         *
         * Start with space for up to 8 directory entries. We expect at least 2 ("." + ".."), hence hopefully
         * 8 will cover most cases comprehensively. (Note that most likely a lot more entries will actually
         * fit in the buffer, given we calculate maximum file name length here.) */
        /* Silence static analyzers */
        assert_cc(offsetof(DirectoryEntries, buffer) <= SIZE_MAX - DIRENT_SIZE_MAX * 8);
        de = malloc(offsetof(DirectoryEntries, buffer) + DIRENT_SIZE_MAX * 8);
        如果 (!de)
                返回 -ENOMEM;

        de->buffer_size = 0;
        为 (;;) {
                size_t bs;
                ssize_t n;

                /* Silence static analyzers, MALLOC_SIZEOF_SAFE is at least as large as the allocation */
                assert(MALLOC_SIZEOF_SAFE(de) >= offsetof(DirectoryEntries, buffer));
                bs = MIN(MALLOC_SIZEOF_SAFE(de) - offsetof(DirectoryEntries, buffer), (size_t) SSIZE_MAX);
                assert(bs > de->buffer_size);

                n = getdents64(dir_fd, (struct dirent*) ((uint8_t*) de->buffer + de->buffer_size), bs - de->buffer_size);
                如果 (n < 0)
                        返回 -errno;
                如果 (n == 0)
                        break;

                msan_unpoison((uint8_t*) de->buffer + de->buffer_size, n);

                de->buffer_size += n;

                如果 (de->buffer_size < bs - DIRENT_SIZE_MAX) /* Still room for one more entry, then try to
                                                             * fill it up without growing the structure. */
                        continue;

                如果 (bs >= SSIZE_MAX - offsetof(DirectoryEntries, buffer))
                        返回 -EFBIG;
                bs = bs >= (SSIZE_MAX - offsetof(DirectoryEntries, buffer))/2 ? SSIZE_MAX - offsetof(DirectoryEntries, buffer) : bs * 2;

                nde = realloc(de, bs);
                如果 (!nde)
                        返回 -ENOMEM;
                de = nde;
        }

        de->n_entries = 0;
        struct dirent *entry;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                如果 (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE)) {
                        r = dirent_ensure_type(dir_fd, entry);
                        如果 (r == -ENOENT)
                                /* dentry gone by now? no problem, let's just suppress it */
                                continue;
                        如果 (r < 0)
                                返回 r;
                }

                如果 (ignore_dirent(entry, flags))
                        continue;

                de->n_entries++;
        }

        size_t sz, j;

        sz = ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size);
        如果 (!INC_SAFE(&sz, sizeof(struct dirent*) * de->n_entries))
                返回 -ENOMEM;

        nde = realloc(de, sz);
        如果 (!nde)
                返回 -ENOMEM;
        de = nde;

        de->entries = (struct dirent**) ((uint8_t*) de + ALIGN(offsetof(DirectoryEntries, buffer) + de->buffer_size));

        j = 0;
        FOREACH_DIRENT_IN_BUFFER(entry, de->buffer, de->buffer_size) {
                /* If d_type == DT_UNKNOWN that means we failed to ensure the type in the earlier loop and
                 * didn't include the dentry in de->n_entries and as such should skip it here as well. */
                如果 (FLAGS_SET(flags, RECURSE_DIR_ENSURE_TYPE) && entry->d_type == DT_UNKNOWN)
                        continue;

                如果 (ignore_dirent(entry, flags))
                        continue;

                de->entries[j++] = entry;
        }
        assert(j == de->n_entries);

        如果 (FLAGS_SET(flags, RECURSE_DIR_SORT))
                typesafe_qsort(de->entries, de->n_entries, sort_func);

        如果 (ret)
                *ret = TAKE_PTR(de);

        返回 0;
}

整数 readdir_all_at(整数 fd, const char *path, RecurseDirFlags flags, DirectoryEntries **ret) {
        _cleanup_close_ 整数 dir_fd = -EBADF;

        assert(fd >= 0 || fd == AT_FDCWD);

        dir_fd = xopenat(fd, path, O_DIRECTORY|O_CLOEXEC);
        如果 (dir_fd < 0)
                返回 dir_fd;

        返回 readdir_all(dir_fd, flags, ret);
}

整数 recurse_dir(
                整数 dir_fd,
                const char *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                void *userdata) {

        _cleanup_free_ DirectoryEntries *de = NULL;
        struct statx root_sx;
        整数 r;

        assert(dir_fd >= 0);
        assert(func);

        /* We cannot descend into dirs if we are supposed to ignore them */
        assert((flags & _RECURSE_DIR_MUST_BE_MASK) == 0 || FLAGS_SET(flags, RECURSE_DIR_MUST_BE_DIRECTORY));

        /* This is a lot like ftw()/nftw(), but a lot more modern, i.e. built around openat()/statx()/O_PATH,
         * and under the assumption that fds are not as 'expensive' as they used to be. */

        如果 (n_depth_max == 0)
                返回 -EOVERFLOW;
        如果 (n_depth_max == UINT_MAX) /* special marker for "default" */
                n_depth_max = DEFAULT_RECURSION_MAX;

        如果 (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {
                如果 (statx_mask != 0) {
                        如果 (statx(dir_fd, "", AT_EMPTY_PATH, statx_mask, &root_sx) < 0)
                                返回 -errno;
                }

                r = func(RECURSE_DIR_ENTER,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         NULL, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : NULL,
                         userdata);
                如果 (IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY))
                        返回 0;
                如果 (r != RECURSE_DIR_CONTINUE)
                        返回 r;
        }

        /* Mask out RECURSE_DIR_ENSURE_TYPE so we can do it ourselves and avoid an extra statx() call. */
        r = readdir_all(dir_fd, flags & ~RECURSE_DIR_ENSURE_TYPE, &de);
        如果 (r < 0)
                返回 r;

        FOREACH_ARRAY(entry, de->entries, de->n_entries) {
                struct dirent *i = *entry;
                _cleanup_close_ 整数 inode_fd = -EBADF, subdir_fd = -EBADF;
                _cleanup_free_ char *joined = NULL;
                struct statx sx;
                bool sx_valid = false;
                const char *p;

                /* For each directory entry we'll do one of the following:
                 *
                 * 1) If the entry refers to a directory, we'll open it as O_DIRECTORY 'subdir_fd' and then statx() the opened directory via that new fd (if requested)
                 * 2) Otherwise, if RECURSE_DIR_INODE_FD is set we'll open it as O_PATH 'inode_fd' and then statx() the opened inode via that new fd (if requested)
                 * 3) Otherwise, we'll statx() the directory entry via the directory fd we are currently looking at (if requested)
                 */

                如果 (path) {
                        joined = path_join(path, i->d_name);
                        如果 (!joined)
                                返回 -ENOMEM;

                        p = joined;
                } 否则
                        p = i->d_name;

                如果 (IN_SET(i->d_type, DT_UNKNOWN, DT_DIR)) {
                        subdir_fd = openat(dir_fd, i->d_name, O_DIRECTORY|O_NOFOLLOW|O_CLOEXEC);
                        如果 (subdir_fd < 0) {
                                如果 (errno == ENOENT) /* Vanished by now, go for next file immediately */
                                        continue;

                                /* If it is a subdir but we failed to open it, then fail */
                                如果 (!IN_SET(errno, ENOTDIR, ELOOP)) {
                                        log_debug_errno(errno, "Failed to open directory '%s': %m", p);

                                        assert(errno <= RECURSE_DIR_SKIP_OPEN_DIR_ERROR_MAX - RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE);

                                        r = func(RECURSE_DIR_SKIP_OPEN_DIR_ERROR_BASE + errno,
                                                 p,
                                                 dir_fd,
                                                 /* inode_fd= */ -EBADF,
                                                 i,
                                                 /* sx= */ NULL,
                                                 userdata);
                                        如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                break;
                                        如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                返回 r;

                                        continue;
                                }

                                /* If it's not a subdir, then let's handle it like a regular inode below */

                        } 否则 {
                                /* If we managed to get a DIR* off the inode, it's definitely a directory. */
                                i->d_type = DT_DIR;

                                如果 (statx_mask != 0 || (flags & RECURSE_DIR_SAME_MOUNT)) {
                                        如果 (statx(subdir_fd, "", AT_EMPTY_PATH, statx_mask, &sx) < 0)
                                                返回 -errno;

                                        sx_valid = true;
                                }
                        }
                }

                如果 (subdir_fd < 0) {
                        /* It's not a subdirectory. */

                        如果 (flags & RECURSE_DIR_INODE_FD) {

                                inode_fd = openat(dir_fd, i->d_name, O_PATH|O_NOFOLLOW|O_CLOEXEC);
                                如果 (inode_fd < 0) {
                                        如果 (errno == ENOENT) /* Vanished by now, go for next file immediately */
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
                                        如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                break;
                                        如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                返回 r;

                                        continue;
                                }

                                /* If we open the inode, then verify it's actually a non-directory, like we
                                 * assume. Let's guarantee that we never pass statx data of a directory where
                                 * caller expects a non-directory */

                                如果 (statx(inode_fd, "", AT_EMPTY_PATH, statx_mask | STATX_TYPE, &sx) < 0)
                                        返回 -errno;

                                assert(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                如果 (S_ISDIR(sx.stx_mode)) {
                                        /* What? It's a directory now? Then someone must have quickly
                                         * replaced it. Let's handle that gracefully: convert it to a
                                         * directory fd — which should be riskless now that we pinned the
                                         * inode. */

                                        subdir_fd = fd_reopen(inode_fd, O_DIRECTORY|O_CLOEXEC);
                                        如果 (subdir_fd < 0)
                                                返回 subdir_fd;

                                        inode_fd = safe_close(inode_fd);
                                }

                        } 否则 如果 (statx_mask != 0 || (i->d_type == DT_UNKNOWN && (flags & RECURSE_DIR_ENSURE_TYPE))) {

                                如果 (statx(dir_fd, i->d_name, AT_SYMLINK_NOFOLLOW, statx_mask | STATX_TYPE, &sx) < 0) {
                                        如果 (errno == ENOENT) /* Vanished by now? Go for next file immediately */
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
                                        如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                break;
                                        如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                返回 r;

                                        continue;
                                }

                                assert(sx.stx_mask & STATX_TYPE);
                                sx_valid = true;

                                如果 (S_ISDIR(sx.stx_mode)) {
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
                                        如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                break;
                                        如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                返回 r;

                                        continue;
                                }
                        }
                }

                如果 (sx_valid) {
                        /* Copy over the data we acquired through statx() if we acquired any */
                        如果 (sx.stx_mask & STATX_TYPE) {
                                assert((subdir_fd < 0) == !S_ISDIR(sx.stx_mode));
                                i->d_type = IFTODT(sx.stx_mode);
                        }

                        如果 (sx.stx_mask & STATX_INO)
                                i->d_ino = sx.stx_ino;
                }

                如果 (subdir_fd >= 0) {
                        如果 (FLAGS_SET(flags, RECURSE_DIR_SAME_MOUNT)) {
                                bool is_mount;

                                如果 (sx_valid && FLAGS_SET(sx.stx_attributes_mask, STATX_ATTR_MOUNT_ROOT))
                                        is_mount = FLAGS_SET(sx.stx_attributes, STATX_ATTR_MOUNT_ROOT);
                                否则 {
                                        r = is_mount_point_at(dir_fd, i->d_name, /* flags= */ 0);
                                        如果 (r < 0)
                                                log_debug_errno(r, "Failed to determine whether %s is a submount, assuming not: %m", p);

                                        is_mount = r > 0;
                                }

                                如果 (is_mount) {
                                        r = func(RECURSE_DIR_SKIP_MOUNT,
                                                 p,
                                                 dir_fd,
                                                 subdir_fd,
                                                 i,
                                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                                 userdata);
                                        如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                                break;
                                        如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                                返回 r;

                                        continue;
                                }
                        }

                        如果 (n_depth_max <= 1) {
                                /* When we reached max depth, generate a special event */

                                r = func(RECURSE_DIR_SKIP_DEPTH,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                         userdata);
                                如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                                        break;
                                如果 (!IN_SET(r, RECURSE_DIR_CONTINUE, RECURSE_DIR_SKIP_ENTRY))
                                        返回 r;

                                continue;
                        }

                        r = func(RECURSE_DIR_ENTER,
                                 p,
                                 dir_fd,
                                 subdir_fd,
                                 i,
                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                 userdata);
                        如果 (r == RECURSE_DIR_CONTINUE) {
                                r = recurse_dir(subdir_fd,
                                                p,
                                                statx_mask,
                                                n_depth_max - 1,
                                                flags & ~RECURSE_DIR_TOPLEVEL, /* we already called the callback for this entry */
                                                func,
                                                userdata);
                                如果 (r != 0)
                                        返回 r;

                                r = func(RECURSE_DIR_LEAVE,
                                         p,
                                         dir_fd,
                                         subdir_fd,
                                         i,
                                         statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                         userdata);
                        }
                } 否则
                        /* Non-directory inode */
                        r = func(RECURSE_DIR_ENTRY,
                                 p,
                                 dir_fd,
                                 inode_fd,
                                 i,
                                 statx_mask != 0 ? &sx : NULL, /* only pass sx if user asked for it */
                                 userdata);

                如果 (r == RECURSE_DIR_LEAVE_DIRECTORY)
                        break;

                如果 (IN_SET(r, RECURSE_DIR_UNLINK, RECURSE_DIR_UNLINK_GRACEFUL)) {
                        整数 f = subdir_fd >= 0 ? AT_REMOVEDIR : 0;

                        /* Close inodes before we try to delete them */
                        subdir_fd = safe_close(subdir_fd);
                        inode_fd = safe_close(inode_fd);

                        如果 (unlinkat(dir_fd, i->d_name, f) < 0) {
                                如果 (r != RECURSE_DIR_UNLINK_GRACEFUL)
                                        返回 -errno;

                                log_debug_errno(errno, "Unable to remove '%s', ignoring: %m", p);
                        }
                } 否则 如果 (!IN_SET(r, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        返回 r;
        }

        如果 (FLAGS_SET(flags, RECURSE_DIR_TOPLEVEL)) {

                r = func(RECURSE_DIR_LEAVE,
                         path,
                         -EBADF, /* we have no parent fd */
                         dir_fd,
                         NULL, /* we have no dirent */
                         statx_mask != 0 ? &root_sx : NULL,
                         userdata);
                如果 (!IN_SET(r, RECURSE_DIR_LEAVE_DIRECTORY, RECURSE_DIR_SKIP_ENTRY, RECURSE_DIR_CONTINUE))
                        返回 r;
        }

        返回 0;
}

整数 recurse_dir_at(
                整数 atfd,
                const char *path,
                unsigned statx_mask,
                unsigned n_depth_max,
                RecurseDirFlags flags,
                recurse_dir_func_t func,
                void *userdata) {

        _cleanup_close_ 整数 fd = -EBADF;

        assert(atfd >= 0 || atfd == AT_FDCWD);
        assert(func);

        fd = openat(atfd, path ?: ".", O_DIRECTORY|O_CLOEXEC);
        如果 (fd < 0)
                返回 -errno;

        返回 recurse_dir(fd, path, statx_mask, n_depth_max, flags, func, userdata);
}
