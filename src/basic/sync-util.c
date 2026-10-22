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

цяло fsync_directory_of_file(цяло fd) {
        _cleanup_close_ цяло dfd = -EBADF;
        struct stat st;
        цяло r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ако (fstat(fd, &st) < 0)
                върни -errno;

        ако (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ако (dfd < 0)
                        върни -errno;

        } иначе ако (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ако (r < 0)
                        върни r;
                ако (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        върни -ENOTTY;
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
                                върни 0;

                        върни r;
                }

                ако (!path_is_absolute(path))
                        върни -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ако (dfd < 0)
                        върни dfd;
        }

        върни RET_NERRNO(fsync(dfd));
}

цяло fsync_full(цяло fd) {
        цяло r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ако (r < 0) /* Return earlier error */
                върни r;
        ако (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                върни 0;
        върни q;
}

цяло fsync_path_at(цяло at_fd, const char *path) {
        _cleanup_close_ цяло opened_fd = -EBADF;
        цяло fd;

        ако (isempty(path)) {
                ако (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ако (opened_fd < 0)
                                върни -errno;

                        fd = opened_fd;
                } иначе
                        fd = at_fd;
        } иначе {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ако (opened_fd < 0)
                        върни -errno;

                fd = opened_fd;
        }

        върни RET_NERRNO(fsync(fd));
}

цяло fsync_parent_at(цяло at_fd, const char *path) {
        _cleanup_close_ цяло opened_fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        върни fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ако (opened_fd < 0)
                        върни -errno;

                върни RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ако (opened_fd < 0)
                върни -errno;

        върни fsync_directory_of_file(opened_fd);
}

цяло fsync_path_and_parent_at(цяло at_fd, const char *path) {
        _cleanup_close_ цяло opened_fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        върни fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } иначе
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ако (opened_fd < 0)
                върни -errno;

        върни fsync_full(opened_fd);
}

цяло syncfs_path(цяло at_fd, const char *path) {
        _cleanup_close_ цяло fd = -EBADF;

        ако (isempty(path)) {
                ако (at_fd != AT_FDCWD)
                        върни RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } иначе
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ако (fd < 0)
                върни -errno;

        върни RET_NERRNO(syncfs(fd));
}
