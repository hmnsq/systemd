/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "sync-util.h"

পূর্ণসংখ্যা fsync_directory_of_file(পূর্ণসংখ্যা fd) {
        _cleanup_close_ পূর্ণসংখ্যা dfd = -EBADF;
        কাঠামো stat st;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        যদি (fstat(fd, &st) < 0)
                ফেরত -errno;

        যদি (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                যদি (dfd < 0)
                        ফেরত -errno;

        } অন্যথা যদি (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                যদি (r < 0)
                        ফেরত r;
                যদি (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ফেরত -ENOTTY;
        }

        যদি (dfd < 0) {
                _cleanup_free_ অক্ষর *path = ফাঁকা;

                r = fd_get_path(fd, &path);
                যদি (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        যদি (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ফেরত 0;

                        ফেরত r;
                }

                যদি (!path_is_absolute(path))
                        ফেরত -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                যদি (dfd < 0)
                        ফেরত dfd;
        }

        ফেরত RET_NERRNO(fsync(dfd));
}

পূর্ণসংখ্যা fsync_full(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        যদি (r < 0) /* Return earlier error */
                ফেরত r;
        যদি (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ফেরত 0;
        ফেরত q;
}

পূর্ণসংখ্যা fsync_path_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;
        পূর্ণসংখ্যা fd;

        যদি (isempty(path)) {
                যদি (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        যদি (opened_fd < 0)
                                ফেরত -errno;

                        fd = opened_fd;
                } অন্যথা
                        fd = at_fd;
        } অন্যথা {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                যদি (opened_fd < 0)
                        ফেরত -errno;

                fd = opened_fd;
        }

        ফেরত RET_NERRNO(fsync(fd));
}

পূর্ণসংখ্যা fsync_parent_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ফেরত fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                যদি (opened_fd < 0)
                        ফেরত -errno;

                ফেরত RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        যদি (opened_fd < 0)
                ফেরত -errno;

        ফেরত fsync_directory_of_file(opened_fd);
}

পূর্ণসংখ্যা fsync_path_and_parent_at(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা opened_fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ফেরত fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } অন্যথা
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        যদি (opened_fd < 0)
                ফেরত -errno;

        ফেরত fsync_full(opened_fd);
}

পূর্ণসংখ্যা syncfs_path(পূর্ণসংখ্যা at_fd, ধ্রুবক অক্ষর *path) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;

        যদি (isempty(path)) {
                যদি (at_fd != AT_FDCWD)
                        ফেরত RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } অন্যথা
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        যদি (fd < 0)
                ফেরত -errno;

        ফেরত RET_NERRNO(syncfs(fd));
}
