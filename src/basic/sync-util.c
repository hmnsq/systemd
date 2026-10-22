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

ಪೂರ್ಣಾಂಕ fsync_directory_ದ_file(ಪೂರ್ಣಾಂಕ fd) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ dfd = -EBADF;
        ರಚನೆ stat st;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ಆದರೆ (fstat(fd, &st) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಆದರೆ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ಆದರೆ (dfd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

        } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ಹಿಂತಿರುಗಿ -ENOTTY;
        }

        ಆದರೆ (dfd < 0) {
                _cleanup_free_ ಅಕ್ಷರ *ಮಾರ್ಗ = ಖಾಲಿ;

                r = fd_get_ಮಾರ್ಗ(fd, &ಮಾರ್ಗ);
                ಆದರೆ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ಆದರೆ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ಹಿಂತಿರುಗಿ 0;

                        ಹಿಂತಿರುಗಿ r;
                }

                ಆದರೆ (!ಮಾರ್ಗ_is_absolute(ಮಾರ್ಗ))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                dfd = open_ಪೋಷಕ(ಮಾರ್ಗ, O_CLOEXEC|O_NOFOLLOW, 0);
                ಆದರೆ (dfd < 0)
                        ಹಿಂತಿರುಗಿ dfd;
        }

        ಹಿಂತಿರುಗಿ RET_NERRNO(fsync(dfd));
}

ಪೂರ್ಣಾಂಕ fsync_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_ದ_file(fd);
        ಆದರೆ (r < 0) /* Return earlier error */
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ಹಿಂತಿರುಗಿ 0;
        ಹಿಂತಿರುಗಿ q;
}

ಪೂರ್ಣಾಂಕ fsync_ಮಾರ್ಗ_at(ಪೂರ್ಣಾಂಕ at_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;
        ಪೂರ್ಣಾಂಕ fd;

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಆದರೆ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ಆದರೆ (opened_fd < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        fd = opened_fd;
                } ಇಲ್ಲದಿದ್ದರೆ
                        fd = at_fd;
        } ಇಲ್ಲದಿದ್ದರೆ {
                opened_fd = openat(at_fd, ಮಾರ್ಗ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ಆದರೆ (opened_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                fd = opened_fd;
        }

        ಹಿಂತಿರುಗಿ RET_NERRNO(fsync(fd));
}

ಪೂರ್ಣಾಂಕ fsync_ಪೋಷಕ_at(ಪೂರ್ಣಾಂಕ at_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಆದರೆ (at_fd != AT_FDCWD)
                        ಹಿಂತಿರುಗಿ fsync_directory_ದ_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ಆದರೆ (opened_fd < 0)
                        ಹಿಂತಿರುಗಿ -errno;

                ಹಿಂತಿರುಗಿ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, ಮಾರ್ಗ, O_ಮಾರ್ಗ|O_CLOEXEC|O_NOFOLLOW);
        ಆದರೆ (opened_fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ fsync_directory_ದ_file(opened_fd);
}

ಪೂರ್ಣಾಂಕ fsync_ಮಾರ್ಗ_and_ಪೋಷಕ_at(ಪೂರ್ಣಾಂಕ at_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ opened_fd = -EBADF;

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಆದರೆ (at_fd != AT_FDCWD)
                        ಹಿಂತಿರುಗಿ fsync_ತುಂಬಿದ(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ಇಲ್ಲದಿದ್ದರೆ
                opened_fd = openat(at_fd, ಮಾರ್ಗ, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ಆದರೆ (opened_fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ fsync_ತುಂಬಿದ(opened_fd);
}

ಪೂರ್ಣಾಂಕ syncfs_ಮಾರ್ಗ(ಪೂರ್ಣಾಂಕ at_fd, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;

        ಆದರೆ (isempty(ಮಾರ್ಗ)) {
                ಆದರೆ (at_fd != AT_FDCWD)
                        ಹಿಂತಿರುಗಿ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ಇಲ್ಲದಿದ್ದರೆ
                fd = openat(at_fd, ಮಾರ್ಗ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ -errno;

        ಹಿಂತಿರುಗಿ RET_NERRNO(syncfs(fd));
}
