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

ຈຳນວນເຕັມ fsync_directory_of_file(ຈຳນວນເຕັມ fd) {
        _cleanup_close_ ຈຳນວນເຕັມ dfd = -EBADF;
        sa_nracanaa stat st;
        ຈຳນວນເຕັມ r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ຖ້າ (fstat(fd, &st) < 0)
                ສົ່ງຄືນ -errno;

        ຖ້າ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ຖ້າ (dfd < 0)
                        ສົ່ງຄືນ -errno;

        } ບໍ່ດັ່ງນັ້ນ ຖ້າ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;
                ຖ້າ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ສົ່ງຄືນ -ENOTTY;
        }

        ຖ້າ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ຖ້າ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ຖ້າ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ສົ່ງຄືນ 0;

                        ສົ່ງຄືນ r;
                }

                ຖ້າ (!path_is_absolute(path))
                        ສົ່ງຄືນ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ຖ້າ (dfd < 0)
                        ສົ່ງຄືນ dfd;
        }

        ສົ່ງຄືນ RET_NERRNO(fsync(dfd));
}

ຈຳນວນເຕັມ fsync_full(ຈຳນວນເຕັມ fd) {
        ຈຳນວນເຕັມ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ຖ້າ (r < 0) /* Return earlier error */
                ສົ່ງຄືນ r;
        ຖ້າ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ສົ່ງຄືນ 0;
        ສົ່ງຄືນ q;
}

ຈຳນວນເຕັມ fsync_path_at(ຈຳນວນເຕັມ at_fd, const var_na *path) {
        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;
        ຈຳນວນເຕັມ fd;

        ຖ້າ (isempty(path)) {
                ຖ້າ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ຖ້າ (opened_fd < 0)
                                ສົ່ງຄືນ -errno;

                        fd = opened_fd;
                } ບໍ່ດັ່ງນັ້ນ
                        fd = at_fd;
        } ບໍ່ດັ່ງນັ້ນ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ຖ້າ (opened_fd < 0)
                        ສົ່ງຄືນ -errno;

                fd = opened_fd;
        }

        ສົ່ງຄືນ RET_NERRNO(fsync(fd));
}

ຈຳນວນເຕັມ fsync_parent_at(ຈຳນວນເຕັມ at_fd, const var_na *path) {
        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;

        ຖ້າ (isempty(path)) {
                ຖ້າ (at_fd != AT_FDCWD)
                        ສົ່ງຄືນ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ຖ້າ (opened_fd < 0)
                        ສົ່ງຄືນ -errno;

                ສົ່ງຄືນ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ຖ້າ (opened_fd < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ fsync_directory_of_file(opened_fd);
}

ຈຳນວນເຕັມ fsync_path_and_parent_at(ຈຳນວນເຕັມ at_fd, const var_na *path) {
        _cleanup_close_ ຈຳນວນເຕັມ opened_fd = -EBADF;

        ຖ້າ (isempty(path)) {
                ຖ້າ (at_fd != AT_FDCWD)
                        ສົ່ງຄືນ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ບໍ່ດັ່ງນັ້ນ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ຖ້າ (opened_fd < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ fsync_full(opened_fd);
}

ຈຳນວນເຕັມ syncfs_path(ຈຳນວນເຕັມ at_fd, const var_na *path) {
        _cleanup_close_ ຈຳນວນເຕັມ fd = -EBADF;

        ຖ້າ (isempty(path)) {
                ຖ້າ (at_fd != AT_FDCWD)
                        ສົ່ງຄືນ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ບໍ່ດັ່ງນັ້ນ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ຖ້າ (fd < 0)
                ສົ່ງຄືນ -errno;

        ສົ່ງຄືນ RET_NERRNO(syncfs(fd));
}
