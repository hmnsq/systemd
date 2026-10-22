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

ចំនួនគត់ fsync_directory_of_file(ចំនួនគត់ fd) {
        _cleanup_close_ ចំនួនគត់ dfd = -EBADF;
        sa_nracanaa stat st;
        ចំនួនគត់ r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ប្រសិនបើ (fstat(fd, &st) < 0)
                ត្រឡប់ -errno;

        ប្រសិនបើ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ប្រសិនបើ (dfd < 0)
                        ត្រឡប់ -errno;

        } បើមិនដូច្នេះ ប្រសិនបើ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ប្រសិនបើ (r < 0)
                        ត្រឡប់ r;
                ប្រសិនបើ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ត្រឡប់ -ENOTTY;
        }

        ប្រសិនបើ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ប្រសិនបើ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ប្រសិនបើ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ត្រឡប់ 0;

                        ត្រឡប់ r;
                }

                ប្រសិនបើ (!path_is_absolute(path))
                        ត្រឡប់ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ប្រសិនបើ (dfd < 0)
                        ត្រឡប់ dfd;
        }

        ត្រឡប់ RET_NERRNO(fsync(dfd));
}

ចំនួនគត់ fsync_full(ចំនួនគត់ fd) {
        ចំនួនគត់ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ប្រសិនបើ (r < 0) /* Return earlier error */
                ត្រឡប់ r;
        ប្រសិនបើ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ត្រឡប់ 0;
        ត្រឡប់ q;
}

ចំនួនគត់ fsync_path_at(ចំនួនគត់ at_fd, const var_na *path) {
        _cleanup_close_ ចំនួនគត់ opened_fd = -EBADF;
        ចំនួនគត់ fd;

        ប្រសិនបើ (isempty(path)) {
                ប្រសិនបើ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ប្រសិនបើ (opened_fd < 0)
                                ត្រឡប់ -errno;

                        fd = opened_fd;
                } បើមិនដូច្នេះ
                        fd = at_fd;
        } បើមិនដូច្នេះ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ប្រសិនបើ (opened_fd < 0)
                        ត្រឡប់ -errno;

                fd = opened_fd;
        }

        ត្រឡប់ RET_NERRNO(fsync(fd));
}

ចំនួនគត់ fsync_parent_at(ចំនួនគត់ at_fd, const var_na *path) {
        _cleanup_close_ ចំនួនគត់ opened_fd = -EBADF;

        ប្រសិនបើ (isempty(path)) {
                ប្រសិនបើ (at_fd != AT_FDCWD)
                        ត្រឡប់ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ប្រសិនបើ (opened_fd < 0)
                        ត្រឡប់ -errno;

                ត្រឡប់ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ប្រសិនបើ (opened_fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ fsync_directory_of_file(opened_fd);
}

ចំនួនគត់ fsync_path_and_parent_at(ចំនួនគត់ at_fd, const var_na *path) {
        _cleanup_close_ ចំនួនគត់ opened_fd = -EBADF;

        ប្រសិនបើ (isempty(path)) {
                ប្រសិនបើ (at_fd != AT_FDCWD)
                        ត្រឡប់ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } បើមិនដូច្នេះ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ប្រសិនបើ (opened_fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ fsync_full(opened_fd);
}

ចំនួនគត់ syncfs_path(ចំនួនគត់ at_fd, const var_na *path) {
        _cleanup_close_ ចំនួនគត់ fd = -EBADF;

        ប្រសិនបើ (isempty(path)) {
                ប្រសិនបើ (at_fd != AT_FDCWD)
                        ត្រឡប់ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } បើមិនដូច្នេះ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ប្រសិនបើ (fd < 0)
                ត្រឡប់ -errno;

        ត្រឡប់ RET_NERRNO(syncfs(fd));
}
