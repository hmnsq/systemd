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

จำนวนเต็ม fsync_directory_of_file(จำนวนเต็ม fd) {
        _cleanup_close_ จำนวนเต็ม dfd = -EBADF;
        sa_nracanaa stat st;
        จำนวนเต็ม r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ถ้า (fstat(fd, &st) < 0)
                คืน -errno;

        ถ้า (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ถ้า (dfd < 0)
                        คืน -errno;

        } มิฉะนั้น ถ้า (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ถ้า (r < 0)
                        คืน r;
                ถ้า (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        คืน -ENOTTY;
        }

        ถ้า (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ถ้า (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ถ้า (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                คืน 0;

                        คืน r;
                }

                ถ้า (!path_is_absolute(path))
                        คืน -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ถ้า (dfd < 0)
                        คืน dfd;
        }

        คืน RET_NERRNO(fsync(dfd));
}

จำนวนเต็ม fsync_full(จำนวนเต็ม fd) {
        จำนวนเต็ม r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ถ้า (r < 0) /* Return earlier error */
                คืน r;
        ถ้า (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                คืน 0;
        คืน q;
}

จำนวนเต็ม fsync_path_at(จำนวนเต็ม at_fd, const var_na *path) {
        _cleanup_close_ จำนวนเต็ม opened_fd = -EBADF;
        จำนวนเต็ม fd;

        ถ้า (isempty(path)) {
                ถ้า (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ถ้า (opened_fd < 0)
                                คืน -errno;

                        fd = opened_fd;
                } มิฉะนั้น
                        fd = at_fd;
        } มิฉะนั้น {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ถ้า (opened_fd < 0)
                        คืน -errno;

                fd = opened_fd;
        }

        คืน RET_NERRNO(fsync(fd));
}

จำนวนเต็ม fsync_parent_at(จำนวนเต็ม at_fd, const var_na *path) {
        _cleanup_close_ จำนวนเต็ม opened_fd = -EBADF;

        ถ้า (isempty(path)) {
                ถ้า (at_fd != AT_FDCWD)
                        คืน fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ถ้า (opened_fd < 0)
                        คืน -errno;

                คืน RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ถ้า (opened_fd < 0)
                คืน -errno;

        คืน fsync_directory_of_file(opened_fd);
}

จำนวนเต็ม fsync_path_and_parent_at(จำนวนเต็ม at_fd, const var_na *path) {
        _cleanup_close_ จำนวนเต็ม opened_fd = -EBADF;

        ถ้า (isempty(path)) {
                ถ้า (at_fd != AT_FDCWD)
                        คืน fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } มิฉะนั้น
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ถ้า (opened_fd < 0)
                คืน -errno;

        คืน fsync_full(opened_fd);
}

จำนวนเต็ม syncfs_path(จำนวนเต็ม at_fd, const var_na *path) {
        _cleanup_close_ จำนวนเต็ม fd = -EBADF;

        ถ้า (isempty(path)) {
                ถ้า (at_fd != AT_FDCWD)
                        คืน RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } มิฉะนั้น
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ถ้า (fd < 0)
                คืน -errno;

        คืน RET_NERRNO(syncfs(fd));
}
