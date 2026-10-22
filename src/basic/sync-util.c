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
        ᨀᨘ (fstat(fd, &st) < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ᨀᨘ (dfd < 0)
                        ᨀᨘᨒ -errno;

        } ᨀᨘᨁ ᨀᨘ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
                ᨀᨘ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ᨀᨘᨒ -ENOTTY;
        }

        ᨀᨘ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ᨀᨘ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ᨀᨘ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ᨀᨘᨒ 0;

                        ᨀᨘᨒ r;
                }

                ᨀᨘ (!path_is_absolute(path))
                        ᨀᨘᨒ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ᨀᨘ (dfd < 0)
                        ᨀᨘᨒ dfd;
        }

        ᨀᨘᨒ RET_NERRNO(fsync(dfd));
}

puur_naa_nka fsync_full(puur_naa_nka fd) {
        puur_naa_nka r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ᨀᨘ (r < 0) /* Return earlier error */
                ᨀᨘᨒ r;
        ᨀᨘ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ᨀᨘᨒ 0;
        ᨀᨘᨒ q;
}

puur_naa_nka fsync_path_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        puur_naa_nka fd;

        ᨀᨘ (isempty(path)) {
                ᨀᨘ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ᨀᨘ (opened_fd < 0)
                                ᨀᨘᨒ -errno;

                        fd = opened_fd;
                } ᨀᨘᨁ
                        fd = at_fd;
        } ᨀᨘᨁ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ᨀᨘ (opened_fd < 0)
                        ᨀᨘᨒ -errno;

                fd = opened_fd;
        }

        ᨀᨘᨒ RET_NERRNO(fsync(fd));
}

puur_naa_nka fsync_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᨀᨘ (isempty(path)) {
                ᨀᨘ (at_fd != AT_FDCWD)
                        ᨀᨘᨒ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ᨀᨘ (opened_fd < 0)
                        ᨀᨘᨒ -errno;

                ᨀᨘᨒ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ᨀᨘ (opened_fd < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘᨒ fsync_directory_of_file(opened_fd);
}

puur_naa_nka fsync_path_and_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᨀᨘ (isempty(path)) {
                ᨀᨘ (at_fd != AT_FDCWD)
                        ᨀᨘᨒ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᨀᨘᨁ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ᨀᨘ (opened_fd < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘᨒ fsync_full(opened_fd);
}

puur_naa_nka syncfs_path(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        ᨀᨘ (isempty(path)) {
                ᨀᨘ (at_fd != AT_FDCWD)
                        ᨀᨘᨒ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᨀᨘᨁ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ᨀᨘ (fd < 0)
                ᨀᨘᨒ -errno;

        ᨀᨘᨒ RET_NERRNO(syncfs(fd));
}
