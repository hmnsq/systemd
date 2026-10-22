/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "fs-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "sync-util.h"

ಪೂರ್ಣಾಙ್ಕ fsync_directory_of_file(ಪೂರ್ಣಾಙ್ಕ fd) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ dfd = -EBADF;
        ಸಂರಚನಾ stat st;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ಯದಿ (fstat(fd, &st) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಯದಿ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ಯದಿ (dfd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

        } ಅನ್ಯಥಾ ಯದಿ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ಪ್ರತ್ಯಾಗಮ -ENOTTY;
        }

        ಯದಿ (dfd < 0) {
                _cleanup_free_ ವರ್ಣ *path = ರಿಕ್ತ;

                r = fd_get_path(fd, &path);
                ಯದಿ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ಯದಿ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ಪ್ರತ್ಯಾಗಮ 0;

                        ಪ್ರತ್ಯಾಗಮ r;
                }

                ಯದಿ (!path_is_absolute(path))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ಯದಿ (dfd < 0)
                        ಪ್ರತ್ಯಾಗಮ dfd;
        }

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fsync(dfd));
}

ಪೂರ್ಣಾಙ್ಕ fsync_full(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ಯದಿ (r < 0) /* Return earlier error */
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ಪ್ರತ್ಯಾಗಮ 0;
        ಪ್ರತ್ಯಾಗಮ q;
}

ಪೂರ್ಣಾಙ್ಕ fsync_path_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ fd;

        ಯದಿ (isempty(path)) {
                ಯದಿ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ಯದಿ (opened_fd < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        fd = opened_fd;
                } ಅನ್ಯಥಾ
                        fd = at_fd;
        } ಅನ್ಯಥಾ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ಯದಿ (opened_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                fd = opened_fd;
        }

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fsync(fd));
}

ಪೂರ್ಣಾಙ್ಕ fsync_parent_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;

        ಯದಿ (isempty(path)) {
                ಯದಿ (at_fd != AT_FDCWD)
                        ಪ್ರತ್ಯಾಗಮ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ಯದಿ (opened_fd < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;

                ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ಯದಿ (opened_fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ fsync_directory_of_file(opened_fd);
}

ಪೂರ್ಣಾಙ್ಕ fsync_path_and_parent_at(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ opened_fd = -EBADF;

        ಯದಿ (isempty(path)) {
                ಯದಿ (at_fd != AT_FDCWD)
                        ಪ್ರತ್ಯಾಗಮ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ಅನ್ಯಥಾ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ಯದಿ (opened_fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ fsync_full(opened_fd);
}

ಪೂರ್ಣಾಙ್ಕ syncfs_path(ಪೂರ್ಣಾಙ್ಕ at_fd, ಧ್ರುವ ವರ್ಣ *path) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;

        ಯದಿ (isempty(path)) {
                ಯದಿ (at_fd != AT_FDCWD)
                        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ಅನ್ಯಥಾ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(syncfs(fd));
}
