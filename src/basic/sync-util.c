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

صحیح fsync_directory_of_file(صحیح fd) {
        _cleanup_close_ صحیح dfd = -EBADF;
        ساختار stat st;
        صحیح r;

        أكد(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        اگر (fstat(fd, &st) < 0)
                بازگشت -errno;

        اگر (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                اگر (dfd < 0)
                        بازگشت -errno;

        } وگرنه اگر (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                اگر (r < 0)
                        بازگشت r;
                اگر (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        بازگشت -ENOTTY;
        }

        اگر (dfd < 0) {
                _cleanup_free_ نویسه *path = پوچ;

                r = fd_get_path(fd, &path);
                اگر (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        اگر (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                بازگشت 0;

                        بازگشت r;
                }

                اگر (!path_is_absolute(path))
                        بازگشت -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                اگر (dfd < 0)
                        بازگشت dfd;
        }

        بازگشت RET_NERRNO(fsync(dfd));
}

صحیح fsync_full(صحیح fd) {
        صحیح r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        اگر (r < 0) /* Return earlier error */
                بازگشت r;
        اگر (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                بازگشت 0;
        بازگشت q;
}

صحیح fsync_path_at(صحیح at_fd, ثابت نویسه *path) {
        _cleanup_close_ صحیح opened_fd = -EBADF;
        صحیح fd;

        اگر (isempty(path)) {
                اگر (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        اگر (opened_fd < 0)
                                بازگشت -errno;

                        fd = opened_fd;
                } وگرنه
                        fd = at_fd;
        } وگرنه {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                اگر (opened_fd < 0)
                        بازگشت -errno;

                fd = opened_fd;
        }

        بازگشت RET_NERRNO(fsync(fd));
}

صحیح fsync_parent_at(صحیح at_fd, ثابت نویسه *path) {
        _cleanup_close_ صحیح opened_fd = -EBADF;

        اگر (isempty(path)) {
                اگر (at_fd != AT_FDCWD)
                        بازگشت fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                اگر (opened_fd < 0)
                        بازگشت -errno;

                بازگشت RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        اگر (opened_fd < 0)
                بازگشت -errno;

        بازگشت fsync_directory_of_file(opened_fd);
}

صحیح fsync_path_and_parent_at(صحیح at_fd, ثابت نویسه *path) {
        _cleanup_close_ صحیح opened_fd = -EBADF;

        اگر (isempty(path)) {
                اگر (at_fd != AT_FDCWD)
                        بازگشت fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وگرنه
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        اگر (opened_fd < 0)
                بازگشت -errno;

        بازگشت fsync_full(opened_fd);
}

صحیح syncfs_path(صحیح at_fd, ثابت نویسه *path) {
        _cleanup_close_ صحیح fd = -EBADF;

        اگر (isempty(path)) {
                اگر (at_fd != AT_FDCWD)
                        بازگشت RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وگرنه
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        اگر (fd < 0)
                بازگشت -errno;

        بازگشت RET_NERRNO(syncfs(fd));
}
