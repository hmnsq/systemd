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

số_nguyên fsync_directory_of_file(số_nguyên fd) {
        _cleanup_close_ số_nguyên dfd = -EBADF;
        struct stat st;
        số_nguyên r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        nếu (fstat(fd, &st) < 0)
                trả_về -errno;

        nếu (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                nếu (dfd < 0)
                        trả_về -errno;

        } không_thì nếu (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                nếu (r < 0)
                        trả_về r;
                nếu (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        trả_về -ENOTTY;
        }

        nếu (dfd < 0) {
                _cleanup_free_ char *path = NULL;

                r = fd_get_path(fd, &path);
                nếu (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        nếu (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                trả_về 0;

                        trả_về r;
                }

                nếu (!path_is_absolute(path))
                        trả_về -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                nếu (dfd < 0)
                        trả_về dfd;
        }

        trả_về RET_NERRNO(fsync(dfd));
}

số_nguyên fsync_full(số_nguyên fd) {
        số_nguyên r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        nếu (r < 0) /* Return earlier error */
                trả_về r;
        nếu (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                trả_về 0;
        trả_về q;
}

số_nguyên fsync_path_at(số_nguyên at_fd, const char *path) {
        _cleanup_close_ số_nguyên opened_fd = -EBADF;
        số_nguyên fd;

        nếu (isempty(path)) {
                nếu (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        nếu (opened_fd < 0)
                                trả_về -errno;

                        fd = opened_fd;
                } không_thì
                        fd = at_fd;
        } không_thì {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                nếu (opened_fd < 0)
                        trả_về -errno;

                fd = opened_fd;
        }

        trả_về RET_NERRNO(fsync(fd));
}

số_nguyên fsync_parent_at(số_nguyên at_fd, const char *path) {
        _cleanup_close_ số_nguyên opened_fd = -EBADF;

        nếu (isempty(path)) {
                nếu (at_fd != AT_FDCWD)
                        trả_về fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                nếu (opened_fd < 0)
                        trả_về -errno;

                trả_về RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        nếu (opened_fd < 0)
                trả_về -errno;

        trả_về fsync_directory_of_file(opened_fd);
}

số_nguyên fsync_path_and_parent_at(số_nguyên at_fd, const char *path) {
        _cleanup_close_ số_nguyên opened_fd = -EBADF;

        nếu (isempty(path)) {
                nếu (at_fd != AT_FDCWD)
                        trả_về fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } không_thì
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        nếu (opened_fd < 0)
                trả_về -errno;

        trả_về fsync_full(opened_fd);
}

số_nguyên syncfs_path(số_nguyên at_fd, const char *path) {
        _cleanup_close_ số_nguyên fd = -EBADF;

        nếu (isempty(path)) {
                nếu (at_fd != AT_FDCWD)
                        trả_về RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } không_thì
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        nếu (fd < 0)
                trả_về -errno;

        trả_về RET_NERRNO(syncfs(fd));
}
