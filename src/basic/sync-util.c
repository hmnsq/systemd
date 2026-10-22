/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "log.h"
#شامل "path-util.h"
#شامل "string-util.h"
#شامل "sync-util.h"

صحیح fsync_directory_کا_file(صحیح fd) {
        _cleanup_close_ صحیح dfd = -EBADF;
        ساخت stat st;
        صحیح r;

        أكد(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        اگر (fstat(fd, &st) < 0)
                واپس -errno;

        اگر (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                اگر (dfd < 0)
                        واپس -errno;

        } ورنہ اگر (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                اگر (r < 0)
                        واپس r;
                اگر (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        واپس -ENOTTY;
        }

        اگر (dfd < 0) {
                _cleanup_free_ حرف *راستہ = تہی;

                r = fd_get_راستہ(fd, &راستہ);
                اگر (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        اگر (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                واپس 0;

                        واپس r;
                }

                اگر (!راستہ_is_absolute(راستہ))
                        واپس -EINVAL;

                dfd = open_والد(راستہ, O_CLOEXEC|O_NOFOLLOW, 0);
                اگر (dfd < 0)
                        واپس dfd;
        }

        واپس RET_NERRNO(fsync(dfd));
}

صحیح fsync_بھرا(صحیح fd) {
        صحیح r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_کا_file(fd);
        اگر (r < 0) /* Return earlier error */
                واپس r;
        اگر (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                واپس 0;
        واپس q;
}

صحیح fsync_راستہ_at(صحیح at_fd, ثابت حرف *راستہ) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        صحیح fd;

        اگر (isempty(راستہ)) {
                اگر (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        اگر (opened_fd < 0)
                                واپس -errno;

                        fd = opened_fd;
                } ورنہ
                        fd = at_fd;
        } ورنہ {
                opened_fd = openat(at_fd, راستہ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                اگر (opened_fd < 0)
                        واپس -errno;

                fd = opened_fd;
        }

        واپس RET_NERRNO(fsync(fd));
}

صحیح fsync_والد_at(صحیح at_fd, ثابت حرف *راستہ) {
        _cleanup_close_ صحیح opened_fd = -EBADF;

        اگر (isempty(راستہ)) {
                اگر (at_fd != AT_FDCWD)
                        واپس fsync_directory_کا_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                اگر (opened_fd < 0)
                        واپس -errno;

                واپس RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, راستہ, O_راستہ|O_CLOEXEC|O_NOFOLLOW);
        اگر (opened_fd < 0)
                واپس -errno;

        واپس fsync_directory_کا_file(opened_fd);
}

صحیح fsync_راستہ_and_والد_at(صحیح at_fd, ثابت حرف *راستہ) {
        _cleanup_close_ صحیح opened_fd = -EBADF;

        اگر (isempty(راستہ)) {
                اگر (at_fd != AT_FDCWD)
                        واپس fsync_بھرا(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ورنہ
                opened_fd = openat(at_fd, راستہ, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        اگر (opened_fd < 0)
                واپس -errno;

        واپس fsync_بھرا(opened_fd);
}

صحیح syncfs_راستہ(صحیح at_fd, ثابت حرف *راستہ) {
        _cleanup_close_ صحیح fd = -EBADF;

        اگر (isempty(راستہ)) {
                اگر (at_fd != AT_FDCWD)
                        واپس RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ورنہ
                fd = openat(at_fd, راستہ, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                واپس -errno;

        واپس RET_NERRNO(syncfs(fd));
}
