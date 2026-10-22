/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "log.h"
#ضمن "path-util.h"
#ضمن "string-util.h"
#ضمن "sync-util.h"

صحيح fsync_directory_د_file(صحيح fd) {
        _cleanup_close_ صحيح dfd = -EBADF;
        بنية stat st;
        صحيح r;

        أكد(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        إذا (fstat(fd, &st) < 0)
                أرجع -errno;

        إذا (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                إذا (dfd < 0)
                        أرجع -errno;

        } وإلا إذا (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                إذا (r < 0)
                        أرجع r;
                إذا (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        أرجع -ENOTTY;
        }

        إذا (dfd < 0) {
                _cleanup_free_ حرف *لار = عدم;

                r = fd_get_لار(fd, &لار);
                إذا (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        إذا (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                أرجع 0;

                        أرجع r;
                }

                إذا (!لار_is_absolute(لار))
                        أرجع -EINVAL;

                dfd = open_پلار(لار, O_CLOEXEC|O_NOFOLLOW, 0);
                إذا (dfd < 0)
                        أرجع dfd;
        }

        أرجع RET_NERRNO(fsync(dfd));
}

صحيح fsync_ډک(صحيح fd) {
        صحيح r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_د_file(fd);
        إذا (r < 0) /* Return earlier error */
                أرجع r;
        إذا (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                أرجع 0;
        أرجع q;
}

صحيح fsync_لار_at(صحيح at_fd, ثابت حرف *لار) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        صحيح fd;

        إذا (isempty(لار)) {
                إذا (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        إذا (opened_fd < 0)
                                أرجع -errno;

                        fd = opened_fd;
                } وإلا
                        fd = at_fd;
        } وإلا {
                opened_fd = openat(at_fd, لار, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                إذا (opened_fd < 0)
                        أرجع -errno;

                fd = opened_fd;
        }

        أرجع RET_NERRNO(fsync(fd));
}

صحيح fsync_پلار_at(صحيح at_fd, ثابت حرف *لار) {
        _cleanup_close_ صحيح opened_fd = -EBADF;

        إذا (isempty(لار)) {
                إذا (at_fd != AT_FDCWD)
                        أرجع fsync_directory_د_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                إذا (opened_fd < 0)
                        أرجع -errno;

                أرجع RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, لار, O_لار|O_CLOEXEC|O_NOFOLLOW);
        إذا (opened_fd < 0)
                أرجع -errno;

        أرجع fsync_directory_د_file(opened_fd);
}

صحيح fsync_لار_and_پلار_at(صحيح at_fd, ثابت حرف *لار) {
        _cleanup_close_ صحيح opened_fd = -EBADF;

        إذا (isempty(لار)) {
                إذا (at_fd != AT_FDCWD)
                        أرجع fsync_ډک(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وإلا
                opened_fd = openat(at_fd, لار, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        إذا (opened_fd < 0)
                أرجع -errno;

        أرجع fsync_ډک(opened_fd);
}

صحيح syncfs_لار(صحيح at_fd, ثابت حرف *لار) {
        _cleanup_close_ صحيح fd = -EBADF;

        إذا (isempty(لار)) {
                إذا (at_fd != AT_FDCWD)
                        أرجع RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وإلا
                fd = openat(at_fd, لار, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        إذا (fd < 0)
                أرجع -errno;

        أرجع RET_NERRNO(syncfs(fd));
}
