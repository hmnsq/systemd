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

صحيح fsync_directory_of_file(صحيح fd) {
        _cleanup_close_ صحيح dfd = -EBADF;
        بنية stat st;
        صحيح r;

        أكد(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        𐡄𐡍 (fstat(fd, &st) < 0)
                أرجع -errno;

        𐡄𐡍 (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                𐡄𐡍 (dfd < 0)
                        أرجع -errno;

        } وإلا 𐡄𐡍 (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                𐡄𐡍 (r < 0)
                        أرجع r;
                𐡄𐡍 (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        أرجع -ENOTTY;
        }

        𐡄𐡍 (dfd < 0) {
                _cleanup_free_ حرف *path = عدم;

                r = fd_get_path(fd, &path);
                𐡄𐡍 (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        𐡄𐡍 (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                أرجع 0;

                        أرجع r;
                }

                𐡄𐡍 (!path_is_absolute(path))
                        أرجع -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                𐡄𐡍 (dfd < 0)
                        أرجع dfd;
        }

        أرجع RET_NERRNO(fsync(dfd));
}

صحيح fsync_full(صحيح fd) {
        صحيح r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        𐡄𐡍 (r < 0) /* Return earlier error */
                أرجع r;
        𐡄𐡍 (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                أرجع 0;
        أرجع q;
}

صحيح fsync_path_at(صحيح at_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح opened_fd = -EBADF;
        صحيح fd;

        𐡄𐡍 (isempty(path)) {
                𐡄𐡍 (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        𐡄𐡍 (opened_fd < 0)
                                أرجع -errno;

                        fd = opened_fd;
                } وإلا
                        fd = at_fd;
        } وإلا {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                𐡄𐡍 (opened_fd < 0)
                        أرجع -errno;

                fd = opened_fd;
        }

        أرجع RET_NERRNO(fsync(fd));
}

صحيح fsync_parent_at(صحيح at_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح opened_fd = -EBADF;

        𐡄𐡍 (isempty(path)) {
                𐡄𐡍 (at_fd != AT_FDCWD)
                        أرجع fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                𐡄𐡍 (opened_fd < 0)
                        أرجع -errno;

                أرجع RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        𐡄𐡍 (opened_fd < 0)
                أرجع -errno;

        أرجع fsync_directory_of_file(opened_fd);
}

صحيح fsync_path_and_parent_at(صحيح at_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح opened_fd = -EBADF;

        𐡄𐡍 (isempty(path)) {
                𐡄𐡍 (at_fd != AT_FDCWD)
                        أرجع fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وإلا
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        𐡄𐡍 (opened_fd < 0)
                أرجع -errno;

        أرجع fsync_full(opened_fd);
}

صحيح syncfs_path(صحيح at_fd, ثابت حرف *path) {
        _cleanup_close_ صحيح fd = -EBADF;

        𐡄𐡍 (isempty(path)) {
                𐡄𐡍 (at_fd != AT_FDCWD)
                        أرجع RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } وإلا
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        𐡄𐡍 (fd < 0)
                أرجع -errno;

        أرجع RET_NERRNO(syncfs(fd));
}
