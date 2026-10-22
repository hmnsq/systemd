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

целое fsync_directory_of_file(целое fd) {
        _cleanup_close_ целое dfd = -EBADF;
        struct stat st;
        целое r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        если (fstat(fd, &st) < 0)
                вернуть -errno;

        если (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                если (dfd < 0)
                        вернуть -errno;

        } иначе если (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                если (r < 0)
                        вернуть r;
                если (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        вернуть -ENOTTY;
        }

        если (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                если (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        если (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                вернуть 0;

                        вернуть r;
                }

                если (!path_is_absolute(path))
                        вернуть -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                если (dfd < 0)
                        вернуть dfd;
        }

        вернуть RET_NERRNO(fsync(dfd));
}

целое fsync_full(целое fd) {
        целое r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        если (r < 0) /* Return earlier error */
                вернуть r;
        если (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                вернуть 0;
        вернуть q;
}

целое fsync_path_at(целое at_fd, const char *path) {
        _cleanup_close_ целое opened_fd = -EBADF;
        целое fd;

        если (isempty(path)) {
                если (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        если (opened_fd < 0)
                                вернуть -errno;

                        fd = opened_fd;
                } иначе
                        fd = at_fd;
        } иначе {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                если (opened_fd < 0)
                        вернуть -errno;

                fd = opened_fd;
        }

        вернуть RET_NERRNO(fsync(fd));
}

целое fsync_parent_at(целое at_fd, const char *path) {
        _cleanup_close_ целое opened_fd = -EBADF;

        если (isempty(path)) {
                если (at_fd != AT_FDCWD)
                        вернуть fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                если (opened_fd < 0)
                        вернуть -errno;

                вернуть RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        если (opened_fd < 0)
                вернуть -errno;

        вернуть fsync_directory_of_file(opened_fd);
}

целое fsync_path_and_parent_at(целое at_fd, const char *path) {
        _cleanup_close_ целое opened_fd = -EBADF;

        если (isempty(path)) {
                если (at_fd != AT_FDCWD)
                        вернуть fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } иначе
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        если (opened_fd < 0)
                вернуть -errno;

        вернуть fsync_full(opened_fd);
}

целое syncfs_path(целое at_fd, const char *path) {
        _cleanup_close_ целое fd = -EBADF;

        если (isempty(path)) {
                если (at_fd != AT_FDCWD)
                        вернуть RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } иначе
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        если (fd < 0)
                вернуть -errno;

        вернуть RET_NERRNO(syncfs(fd));
}
