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
        ᮚᮦᮔ᮪ (fstat(fd, &st) < 0)
                ᮝᮜᮤ -errno;

        ᮚᮦᮔ᮪ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ᮚᮦᮔ᮪ (dfd < 0)
                        ᮝᮜᮤ -errno;

        } ᮜᮤᮚ ᮚᮦᮔ᮪ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;
                ᮚᮦᮔ᮪ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ᮝᮜᮤ -ENOTTY;
        }

        ᮚᮦᮔ᮪ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                ᮚᮦᮔ᮪ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ᮚᮦᮔ᮪ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ᮝᮜᮤ 0;

                        ᮝᮜᮤ r;
                }

                ᮚᮦᮔ᮪ (!path_is_absolute(path))
                        ᮝᮜᮤ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ᮚᮦᮔ᮪ (dfd < 0)
                        ᮝᮜᮤ dfd;
        }

        ᮝᮜᮤ RET_NERRNO(fsync(dfd));
}

puur_naa_nka fsync_full(puur_naa_nka fd) {
        puur_naa_nka r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ᮚᮦᮔ᮪ (r < 0) /* Return earlier error */
                ᮝᮜᮤ r;
        ᮚᮦᮔ᮪ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ᮝᮜᮤ 0;
        ᮝᮜᮤ q;
}

puur_naa_nka fsync_path_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;
        puur_naa_nka fd;

        ᮚᮦᮔ᮪ (isempty(path)) {
                ᮚᮦᮔ᮪ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ᮚᮦᮔ᮪ (opened_fd < 0)
                                ᮝᮜᮤ -errno;

                        fd = opened_fd;
                } ᮜᮤᮚ
                        fd = at_fd;
        } ᮜᮤᮚ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ᮚᮦᮔ᮪ (opened_fd < 0)
                        ᮝᮜᮤ -errno;

                fd = opened_fd;
        }

        ᮝᮜᮤ RET_NERRNO(fsync(fd));
}

puur_naa_nka fsync_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᮚᮦᮔ᮪ (isempty(path)) {
                ᮚᮦᮔ᮪ (at_fd != AT_FDCWD)
                        ᮝᮜᮤ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ᮚᮦᮔ᮪ (opened_fd < 0)
                        ᮝᮜᮤ -errno;

                ᮝᮜᮤ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ᮚᮦᮔ᮪ (opened_fd < 0)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ fsync_directory_of_file(opened_fd);
}

puur_naa_nka fsync_path_and_parent_at(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka opened_fd = -EBADF;

        ᮚᮦᮔ᮪ (isempty(path)) {
                ᮚᮦᮔ᮪ (at_fd != AT_FDCWD)
                        ᮝᮜᮤ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᮜᮤᮚ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ᮚᮦᮔ᮪ (opened_fd < 0)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ fsync_full(opened_fd);
}

puur_naa_nka syncfs_path(puur_naa_nka at_fd, const var_na *path) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;

        ᮚᮦᮔ᮪ (isempty(path)) {
                ᮚᮦᮔ᮪ (at_fd != AT_FDCWD)
                        ᮝᮜᮤ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ᮜᮤᮚ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ᮚᮦᮔ᮪ (fd < 0)
                ᮝᮜᮤ -errno;

        ᮝᮜᮤ RET_NERRNO(syncfs(fd));
}
