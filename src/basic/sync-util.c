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
        ᯀᯮᯝ (fstat(fd, &st) < 0)
                ᯀᯮᯞ -errno;

        ᯀᯮᯝ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ᯀᯮᯝ (dfd < 0)
                        ᯀᯮᯞ -errno;

        } ᯀᯮᯝᯂᯪ ᯀᯮᯝ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ᯀᯮᯝ (r < 0)
                        ᯀᯮᯞ r;
                ᯀᯮᯝ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ᯀᯮᯞ -ENOTTY;
        }

        ᯀᯮᯝ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ᯀᯮᯝ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ᯀᯮᯝ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ᯀᯮᯞ 0;

                        ᯀᯮᯞ r;
                }

                ᯀᯮᯝ (!path_is_absolute(path))
                        ᯀᯮᯞ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ᯀᯮᯝ (dfd < 0)
                        ᯀᯮᯞ dfd;
        }

        ᯀᯮᯞ RET_NERRNO(fsync(dfd));
}

puur_naa_nka fsync_full(puur_naa_nka fd) {
        puur_naa_nka r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ᯀᯮᯝ (r < 0) /* Return earlier error */
                ᯀᯮᯞ r;
        ᯀᯮᯝ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ᯀᯮᯞ 0;
        ᯀᯮᯞ q;
}

puur_naa_nka fsync_path_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        puur_naa_nka fd;

        ᯀᯮᯝ (isempty(path)) {
                ᯀᯮᯝ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ᯀᯮᯝ (opened_fd < 0)
                                ᯀᯮᯞ -errno;

                        fd = opened_fd;
                } ᯀᯮᯝᯂᯪ
                        fd = at_fd;
        } ᯀᯮᯝᯂᯪ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ᯀᯮᯝ (opened_fd < 0)
                        ᯀᯮᯞ -errno;

                fd = opened_fd;
        }

        ᯀᯮᯞ RET_NERRNO(fsync(fd));
}

puur_naa_nka fsync_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᯀᯮᯝ (isempty(path)) {
                ᯀᯮᯝ (at_fd != AT_FDCWD)
                        ᯀᯮᯞ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ᯀᯮᯝ (opened_fd < 0)
                        ᯀᯮᯞ -errno;

                ᯀᯮᯞ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ᯀᯮᯝ (opened_fd < 0)
                ᯀᯮᯞ -errno;

        ᯀᯮᯞ fsync_directory_of_file(opened_fd);
}

puur_naa_nka fsync_path_and_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᯀᯮᯝ (isempty(path)) {
                ᯀᯮᯝ (at_fd != AT_FDCWD)
                        ᯀᯮᯞ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᯀᯮᯝᯂᯪ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ᯀᯮᯝ (opened_fd < 0)
                ᯀᯮᯞ -errno;

        ᯀᯮᯞ fsync_full(opened_fd);
}

puur_naa_nka syncfs_path(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        ᯀᯮᯝ (isempty(path)) {
                ᯀᯮᯝ (at_fd != AT_FDCWD)
                        ᯀᯮᯞ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᯀᯮᯝᯂᯪ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ᯀᯮᯝ (fd < 0)
                ᯀᯮᯞ -errno;

        ᯀᯮᯞ RET_NERRNO(syncfs(fd));
}
