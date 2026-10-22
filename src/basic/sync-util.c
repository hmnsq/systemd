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

정수 fsync_directory_of_file(정수 fd) {
        _cleanup_close_ 정수 dfd = -EBADF;
        struct stat st;
        정수 r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        만약 (fstat(fd, &st) < 0)
                반환 -errno;

        만약 (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                만약 (dfd < 0)
                        반환 -errno;

        } 아니면 만약 (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                만약 (r < 0)
                        반환 r;
                만약 (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        반환 -ENOTTY;
        }

        만약 (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                만약 (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        만약 (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                반환 0;

                        반환 r;
                }

                만약 (!path_is_absolute(path))
                        반환 -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                만약 (dfd < 0)
                        반환 dfd;
        }

        반환 RET_NERRNO(fsync(dfd));
}

정수 fsync_full(정수 fd) {
        정수 r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        만약 (r < 0) /* Return earlier error */
                반환 r;
        만약 (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                반환 0;
        반환 q;
}

정수 fsync_path_at(정수 at_fd, const char *path) {
        _cleanup_close_ 정수 opened_fd = -EBADF;
        정수 fd;

        만약 (isempty(path)) {
                만약 (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        만약 (opened_fd < 0)
                                반환 -errno;

                        fd = opened_fd;
                } 아니면
                        fd = at_fd;
        } 아니면 {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                만약 (opened_fd < 0)
                        반환 -errno;

                fd = opened_fd;
        }

        반환 RET_NERRNO(fsync(fd));
}

정수 fsync_parent_at(정수 at_fd, const char *path) {
        _cleanup_close_ 정수 opened_fd = -EBADF;

        만약 (isempty(path)) {
                만약 (at_fd != AT_FDCWD)
                        반환 fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                만약 (opened_fd < 0)
                        반환 -errno;

                반환 RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        만약 (opened_fd < 0)
                반환 -errno;

        반환 fsync_directory_of_file(opened_fd);
}

정수 fsync_path_and_parent_at(정수 at_fd, const char *path) {
        _cleanup_close_ 정수 opened_fd = -EBADF;

        만약 (isempty(path)) {
                만약 (at_fd != AT_FDCWD)
                        반환 fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } 아니면
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        만약 (opened_fd < 0)
                반환 -errno;

        반환 fsync_full(opened_fd);
}

정수 syncfs_path(정수 at_fd, const char *path) {
        _cleanup_close_ 정수 fd = -EBADF;

        만약 (isempty(path)) {
                만약 (at_fd != AT_FDCWD)
                        반환 RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } 아니면
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        만약 (fd < 0)
                반환 -errno;

        반환 RET_NERRNO(syncfs(fd));
}
