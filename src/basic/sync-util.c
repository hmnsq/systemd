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

整数 fsync_directory_of_file(整数 fd) {
        _cleanup_close_ 整数 dfd = -EBADF;
        struct stat st;
        整数 r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        如果 (fstat(fd, &st) < 0)
                返回 -errno;

        如果 (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                如果 (dfd < 0)
                        返回 -errno;

        } 否则 如果 (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                如果 (r < 0)
                        返回 r;
                如果 (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        返回 -ENOTTY;
        }

        如果 (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                如果 (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        如果 (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                返回 0;

                        返回 r;
                }

                如果 (!path_is_absolute(path))
                        返回 -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                如果 (dfd < 0)
                        返回 dfd;
        }

        返回 RET_NERRNO(fsync(dfd));
}

整数 fsync_full(整数 fd) {
        整数 r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        如果 (r < 0) /* Return earlier error */
                返回 r;
        如果 (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                返回 0;
        返回 q;
}

整数 fsync_path_at(整数 at_fd, const char *path) {
        _cleanup_close_ 整数 opened_fd = -EBADF;
        整数 fd;

        如果 (isempty(path)) {
                如果 (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        如果 (opened_fd < 0)
                                返回 -errno;

                        fd = opened_fd;
                } 否则
                        fd = at_fd;
        } 否则 {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                如果 (opened_fd < 0)
                        返回 -errno;

                fd = opened_fd;
        }

        返回 RET_NERRNO(fsync(fd));
}

整数 fsync_parent_at(整数 at_fd, const char *path) {
        _cleanup_close_ 整数 opened_fd = -EBADF;

        如果 (isempty(path)) {
                如果 (at_fd != AT_FDCWD)
                        返回 fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                如果 (opened_fd < 0)
                        返回 -errno;

                返回 RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        如果 (opened_fd < 0)
                返回 -errno;

        返回 fsync_directory_of_file(opened_fd);
}

整数 fsync_path_and_parent_at(整数 at_fd, const char *path) {
        _cleanup_close_ 整数 opened_fd = -EBADF;

        如果 (isempty(path)) {
                如果 (at_fd != AT_FDCWD)
                        返回 fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } 否则
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        如果 (opened_fd < 0)
                返回 -errno;

        返回 fsync_full(opened_fd);
}

整数 syncfs_path(整数 at_fd, const char *path) {
        _cleanup_close_ 整数 fd = -EBADF;

        如果 (isempty(path)) {
                如果 (at_fd != AT_FDCWD)
                        返回 RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } 否则
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        如果 (fd < 0)
                返回 -errno;

        返回 RET_NERRNO(syncfs(fd));
}
