/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sync-util.h"

ਪੂਰਨ fsync_directory_ਦਾ_file(ਪੂਰਨ fd) {
        _cleanup_close_ ਪੂਰਨ dfd = -EBADF;
        ਬਣਤਰ stat st;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ਜੇ (fstat(fd, &st) < 0)
                ਵਾਪਸ -errno;

        ਜੇ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ਜੇ (dfd < 0)
                        ਵਾਪਸ -errno;

        } ਨਹੀਂਤਾਂ ਜੇ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ਵਾਪਸ -ENOTTY;
        }

        ਜੇ (dfd < 0) {
                _cleanup_free_ ਅੱਖਰ *ਰਸਤਾ = ਨਿਰਾ;

                r = fd_get_ਰਸਤਾ(fd, &ਰਸਤਾ);
                ਜੇ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ਜੇ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ਵਾਪਸ 0;

                        ਵਾਪਸ r;
                }

                ਜੇ (!ਰਸਤਾ_is_absolute(ਰਸਤਾ))
                        ਵਾਪਸ -EINVAL;

                dfd = open_ਮਾਪਾ(ਰਸਤਾ, O_CLOEXEC|O_NOFOLLOW, 0);
                ਜੇ (dfd < 0)
                        ਵਾਪਸ dfd;
        }

        ਵਾਪਸ RET_NERRNO(fsync(dfd));
}

ਪੂਰਨ fsync_ਭਰਿਆ(ਪੂਰਨ fd) {
        ਪੂਰਨ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_ਦਾ_file(fd);
        ਜੇ (r < 0) /* Return earlier error */
                ਵਾਪਸ r;
        ਜੇ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ਵਾਪਸ 0;
        ਵਾਪਸ q;
}

ਪੂਰਨ fsync_ਰਸਤਾ_at(ਪੂਰਨ at_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;
        ਪੂਰਨ fd;

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਜੇ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ਜੇ (opened_fd < 0)
                                ਵਾਪਸ -errno;

                        fd = opened_fd;
                } ਨਹੀਂਤਾਂ
                        fd = at_fd;
        } ਨਹੀਂਤਾਂ {
                opened_fd = openat(at_fd, ਰਸਤਾ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ਜੇ (opened_fd < 0)
                        ਵਾਪਸ -errno;

                fd = opened_fd;
        }

        ਵਾਪਸ RET_NERRNO(fsync(fd));
}

ਪੂਰਨ fsync_ਮਾਪਾ_at(ਪੂਰਨ at_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਜੇ (at_fd != AT_FDCWD)
                        ਵਾਪਸ fsync_directory_ਦਾ_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ਜੇ (opened_fd < 0)
                        ਵਾਪਸ -errno;

                ਵਾਪਸ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, ਰਸਤਾ, O_ਰਸਤਾ|O_CLOEXEC|O_NOFOLLOW);
        ਜੇ (opened_fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ fsync_directory_ਦਾ_file(opened_fd);
}

ਪੂਰਨ fsync_ਰਸਤਾ_and_ਮਾਪਾ_at(ਪੂਰਨ at_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ opened_fd = -EBADF;

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਜੇ (at_fd != AT_FDCWD)
                        ਵਾਪਸ fsync_ਭਰਿਆ(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ਨਹੀਂਤਾਂ
                opened_fd = openat(at_fd, ਰਸਤਾ, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ਜੇ (opened_fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ fsync_ਭਰਿਆ(opened_fd);
}

ਪੂਰਨ syncfs_ਰਸਤਾ(ਪੂਰਨ at_fd, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਜੇ (at_fd != AT_FDCWD)
                        ਵਾਪਸ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ਨਹੀਂਤਾਂ
                fd = openat(at_fd, ਰਸਤਾ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ਜੇ (fd < 0)
                ਵਾਪਸ -errno;

        ਵਾਪਸ RET_NERRNO(syncfs(fd));
}
