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

цео fsync_directory_of_file(цео fd) {
        _cleanup_close_ цео dfd = -EBADF;
        struct stat st;
        цео r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ако (fstat(fd, &st) < 0)
                врати -errno;

        ако (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ако (dfd < 0)
                        врати -errno;

        } инако ако (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ако (r < 0)
                        врати r;
                ако (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        врати -ENOTTY;
        }

        ако (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                ако (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ако (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                врати 0;

                        врати r;
                }

                ако (!path_is_absolute(path))
                        врати -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ако (dfd < 0)
                        врати dfd;
        }

        врати RET_NERRNO(fsync(dfd));
}

цео fsync_full(цео fd) {
        цео r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ако (r < 0) /* Return earlier error */
                врати r;
        ако (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                врати 0;
        врати q;
}

цео fsync_path_at(цео at_fd, const char *path) {
        _cleanup_close_ цео opened_fd = -EBADF;
        цео fd;

        ако (isempty(path)) {
                ако (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ако (opened_fd < 0)
                                врати -errno;

                        fd = opened_fd;
                } инако
                        fd = at_fd;
        } инако {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ако (opened_fd < 0)
                        врати -errno;

                fd = opened_fd;
        }

        врати RET_NERRNO(fsync(fd));
}

цео fsync_parent_at(цео at_fd, const char *path) {
        _cleanup_close_ цео opened_fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        врати fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ако (opened_fd < 0)
                        врати -errno;

                врати RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ако (opened_fd < 0)
                врати -errno;

        врати fsync_directory_of_file(opened_fd);
}

цео fsync_path_and_parent_at(цео at_fd, const char *path) {
        _cleanup_close_ цео opened_fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        врати fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } инако
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ако (opened_fd < 0)
                врати -errno;

        врати fsync_full(opened_fd);
}

цео syncfs_path(цео at_fd, const char *path) {
        _cleanup_close_ цео fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        врати RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } инако
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ако (fd < 0)
                врати -errno;

        врати RET_NERRNO(syncfs(fd));
}
