/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "log.h"
#include "path-util.h"
#include "string-util.h"
#include "sync-util.h"

ціле fsync_directory_of_file(ціле fd) {
        _cleanup_close_ ціле dfd = -EBADF;
        struct stat st;
        ціле r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        якщо (fstat(fd, &st) < 0)
                повернути -errno;

        якщо (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                якщо (dfd < 0)
                        повернути -errno;

        } інакше якщо (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                якщо (r < 0)
                        повернути r;
                якщо (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        повернути -ENOTTY;
        }

        якщо (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                якщо (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        якщо (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                повернути 0;

                        повернути r;
                }

                якщо (!path_is_absolute(path))
                        повернути -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                якщо (dfd < 0)
                        повернути dfd;
        }

        повернути RET_NERRNO(fsync(dfd));
}

ціле fsync_full(ціле fd) {
        ціле r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        якщо (r < 0) /* Return earlier error */
                повернути r;
        якщо (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                повернути 0;
        повернути q;
}

ціле fsync_path_at(ціле at_fd, const char *path) {
        _cleanup_close_ ціле opened_fd = -EBADF;
        ціле fd;

        якщо (isempty(path)) {
                якщо (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        якщо (opened_fd < 0)
                                повернути -errno;

                        fd = opened_fd;
                } інакше
                        fd = at_fd;
        } інакше {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                якщо (opened_fd < 0)
                        повернути -errno;

                fd = opened_fd;
        }

        повернути RET_NERRNO(fsync(fd));
}

ціле fsync_parent_at(ціле at_fd, const char *path) {
        _cleanup_close_ ціле opened_fd = -EBADF;

        якщо (isempty(path)) {
                якщо (at_fd != AT_FDCWD)
                        повернути fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                якщо (opened_fd < 0)
                        повернути -errno;

                повернути RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        якщо (opened_fd < 0)
                повернути -errno;

        повернути fsync_directory_of_file(opened_fd);
}

ціле fsync_path_and_parent_at(ціле at_fd, const char *path) {
        _cleanup_close_ ціле opened_fd = -EBADF;

        якщо (isempty(path)) {
                якщо (at_fd != AT_FDCWD)
                        повернути fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } інакше
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        якщо (opened_fd < 0)
                повернути -errno;

        повернути fsync_full(opened_fd);
}

ціле syncfs_path(ціле at_fd, const char *path) {
        _cleanup_close_ ціле fd = -EBADF;

        якщо (isempty(path)) {
                якщо (at_fd != AT_FDCWD)
                        повернути RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } інакше
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        якщо (fd < 0)
                повернути -errno;

        повернути RET_NERRNO(syncfs(fd));
}
