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

puur_naa_nka fsync_directory_of_file(puur_naa_nka fd) {
        _cleanup_close_ puur_naa_nka dfd = -EBADF;
        sa_nracanaa stat st;
        puur_naa_nka r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ᬬᬾᬦ᭄ (fstat(fd, &st) < 0)
                ᬯᬮᬶ -errno;

        ᬬᬾᬦ᭄ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ᬬᬾᬦ᭄ (dfd < 0)
                        ᬯᬮᬶ -errno;

        } ᬮᬶᬬ ᬬᬾᬦ᭄ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;
                ᬬᬾᬦ᭄ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ᬯᬮᬶ -ENOTTY;
        }

        ᬬᬾᬦ᭄ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ᬬᬾᬦ᭄ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ᬬᬾᬦ᭄ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ᬯᬮᬶ 0;

                        ᬯᬮᬶ r;
                }

                ᬬᬾᬦ᭄ (!path_is_absolute(path))
                        ᬯᬮᬶ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ᬬᬾᬦ᭄ (dfd < 0)
                        ᬯᬮᬶ dfd;
        }

        ᬯᬮᬶ RET_NERRNO(fsync(dfd));
}

puur_naa_nka fsync_full(puur_naa_nka fd) {
        puur_naa_nka r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ᬬᬾᬦ᭄ (r < 0) /* Return earlier error */
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ᬯᬮᬶ 0;
        ᬯᬮᬶ q;
}

puur_naa_nka fsync_path_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        puur_naa_nka fd;

        ᬬᬾᬦ᭄ (isempty(path)) {
                ᬬᬾᬦ᭄ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ᬬᬾᬦ᭄ (opened_fd < 0)
                                ᬯᬮᬶ -errno;

                        fd = opened_fd;
                } ᬮᬶᬬ
                        fd = at_fd;
        } ᬮᬶᬬ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ᬬᬾᬦ᭄ (opened_fd < 0)
                        ᬯᬮᬶ -errno;

                fd = opened_fd;
        }

        ᬯᬮᬶ RET_NERRNO(fsync(fd));
}

puur_naa_nka fsync_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᬬᬾᬦ᭄ (isempty(path)) {
                ᬬᬾᬦ᭄ (at_fd != AT_FDCWD)
                        ᬯᬮᬶ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ᬬᬾᬦ᭄ (opened_fd < 0)
                        ᬯᬮᬶ -errno;

                ᬯᬮᬶ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ᬬᬾᬦ᭄ (opened_fd < 0)
                ᬯᬮᬶ -errno;

        ᬯᬮᬶ fsync_directory_of_file(opened_fd);
}

puur_naa_nka fsync_path_and_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᬬᬾᬦ᭄ (isempty(path)) {
                ᬬᬾᬦ᭄ (at_fd != AT_FDCWD)
                        ᬯᬮᬶ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᬮᬶᬬ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ᬬᬾᬦ᭄ (opened_fd < 0)
                ᬯᬮᬶ -errno;

        ᬯᬮᬶ fsync_full(opened_fd);
}

puur_naa_nka syncfs_path(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        ᬬᬾᬦ᭄ (isempty(path)) {
                ᬬᬾᬦ᭄ (at_fd != AT_FDCWD)
                        ᬯᬮᬶ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᬮᬶᬬ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ᬬᬾᬦ᭄ (fd < 0)
                ᬯᬮᬶ -errno;

        ᬯᬮᬶ RET_NERRNO(syncfs(fd));
}
