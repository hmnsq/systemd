/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "fs-util.h"
#অন্তর্ভাৱ "log.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "sync-util.h"

পূর্ণাঙ্ক fsync_directory_of_file(পূর্ণাঙ্ক fd) {
        _cleanup_close_ পূর্ণাঙ্ক dfd = -EBADF;
        সংরচনা stat st;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        যদি (fstat(fd, &st) < 0)
                ঘূৰাই -errno;

        যদি (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                যদি (dfd < 0)
                        ঘূৰাই -errno;

        } অন্যথা যদি (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                যদি (r < 0)
                        ঘূৰাই r;
                যদি (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ঘূৰাই -ENOTTY;
        }

        যদি (dfd < 0) {
                _cleanup_free_ ৱর্ণ *path = রিক্ত;

                r = fd_get_path(fd, &path);
                যদি (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        যদি (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ঘূৰাই 0;

                        ঘূৰাই r;
                }

                যদি (!path_is_absolute(path))
                        ঘূৰাই -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                যদি (dfd < 0)
                        ঘূৰাই dfd;
        }

        ঘূৰাই RET_NERRNO(fsync(dfd));
}

পূর্ণাঙ্ক fsync_full(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        যদি (r < 0) /* Return earlier error */
                ঘূৰাই r;
        যদি (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ঘূৰাই 0;
        ঘূৰাই q;
}

পূর্ণাঙ্ক fsync_path_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path) {
        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;
        পূর্ণাঙ্ক fd;

        যদি (isempty(path)) {
                যদি (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        যদি (opened_fd < 0)
                                ঘূৰাই -errno;

                        fd = opened_fd;
                } অন্যথা
                        fd = at_fd;
        } অন্যথা {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                যদি (opened_fd < 0)
                        ঘূৰাই -errno;

                fd = opened_fd;
        }

        ঘূৰাই RET_NERRNO(fsync(fd));
}

পূর্ণাঙ্ক fsync_parent_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path) {
        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ঘূৰাই fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                যদি (opened_fd < 0)
                        ঘূৰাই -errno;

                ঘূৰাই RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        যদি (opened_fd < 0)
                ঘূৰাই -errno;

        ঘূৰাই fsync_directory_of_file(opened_fd);
}

পূর্ণাঙ্ক fsync_path_and_parent_at(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path) {
        _cleanup_close_ পূর্ণাঙ্ক opened_fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ঘূৰাই fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } অন্যথা
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        যদি (opened_fd < 0)
                ঘূৰাই -errno;

        ঘূৰাই fsync_full(opened_fd);
}

পূর্ণাঙ্ক syncfs_path(পূর্ণাঙ্ক at_fd, ধ্রুৱ ৱর্ণ *path) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ঘূৰাই RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } অন্যথা
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                ঘূৰাই -errno;

        ঘূৰাই RET_NERRNO(syncfs(fd));
}
