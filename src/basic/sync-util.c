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

ཧྲིལ fsync_directory_of_file(ཧྲིལ fd) {
        _cleanup_close_ ཧྲིལ dfd = -EBADF;
        sa_nracanaa stat st;
        ཧྲིལ r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        གལ་ཏེ (fstat(fd, &st) < 0)
                ཕྱིར -errno;

        གལ་ཏེ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                གལ་ཏེ (dfd < 0)
                        ཕྱིར -errno;

        } དེ་མིན གལ་ཏེ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ཕྱིར -ENOTTY;
        }

        གལ་ཏེ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                གལ་ཏེ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        གལ་ཏེ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ཕྱིར 0;

                        ཕྱིར r;
                }

                གལ་ཏེ (!path_is_absolute(path))
                        ཕྱིར -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                གལ་ཏེ (dfd < 0)
                        ཕྱིར dfd;
        }

        ཕྱིར RET_NERRNO(fsync(dfd));
}

ཧྲིལ fsync_full(ཧྲིལ fd) {
        ཧྲིལ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        གལ་ཏེ (r < 0) /* Return earlier error */
                ཕྱིར r;
        གལ་ཏེ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ཕྱིར 0;
        ཕྱིར q;
}

ཧྲིལ fsync_path_at(ཧྲིལ at_fd, const var_na *path) {
        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;
        ཧྲིལ fd;

        གལ་ཏེ (isempty(path)) {
                གལ་ཏེ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        གལ་ཏེ (opened_fd < 0)
                                ཕྱིར -errno;

                        fd = opened_fd;
                } དེ་མིན
                        fd = at_fd;
        } དེ་མིན {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                གལ་ཏེ (opened_fd < 0)
                        ཕྱིར -errno;

                fd = opened_fd;
        }

        ཕྱིར RET_NERRNO(fsync(fd));
}

ཧྲིལ fsync_parent_at(ཧྲིལ at_fd, const var_na *path) {
        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;

        གལ་ཏེ (isempty(path)) {
                གལ་ཏེ (at_fd != AT_FDCWD)
                        ཕྱིར fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                གལ་ཏེ (opened_fd < 0)
                        ཕྱིར -errno;

                ཕྱིར RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        གལ་ཏེ (opened_fd < 0)
                ཕྱིར -errno;

        ཕྱིར fsync_directory_of_file(opened_fd);
}

ཧྲིལ fsync_path_and_parent_at(ཧྲིལ at_fd, const var_na *path) {
        _cleanup_close_ ཧྲིལ opened_fd = -EBADF;

        གལ་ཏེ (isempty(path)) {
                གལ་ཏེ (at_fd != AT_FDCWD)
                        ཕྱིར fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } དེ་མིན
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        གལ་ཏེ (opened_fd < 0)
                ཕྱིར -errno;

        ཕྱིར fsync_full(opened_fd);
}

ཧྲིལ syncfs_path(ཧྲིལ at_fd, const var_na *path) {
        _cleanup_close_ ཧྲིལ fd = -EBADF;

        གལ་ཏེ (isempty(path)) {
                གལ་ཏེ (at_fd != AT_FDCWD)
                        ཕྱིར RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } དེ་མིན
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        གལ་ཏེ (fd < 0)
                ཕྱིར -errno;

        ཕྱིར RET_NERRNO(syncfs(fd));
}
