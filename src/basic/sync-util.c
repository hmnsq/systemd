/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sys/stat.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "sync-util.h"

पूर्णाङ्क fsync_directory_of_file(पूर्णाङ्क fd) {
        _cleanup_close_ पूर्णाङ्क dfd = -EBADF;
        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        जइ (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        जइ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                जइ (dfd < 0)
                        प्रत्यागम -errno;

        } अण्णहा जइ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                जइ (r < 0)
                        प्रत्यागम r;
                जइ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        प्रत्यागम -ENOTTY;
        }

        जइ (dfd < 0) {
                _cleanup_free_ वर्ण *path = रिक्त;

                r = fd_get_path(fd, &path);
                जइ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        जइ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                प्रत्यागम 0;

                        प्रत्यागम r;
                }

                जइ (!path_is_absolute(path))
                        प्रत्यागम -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                जइ (dfd < 0)
                        प्रत्यागम dfd;
        }

        प्रत्यागम RET_NERRNO(fsync(dfd));
}

पूर्णाङ्क fsync_full(पूर्णाङ्क fd) {
        पूर्णाङ्क r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        जइ (r < 0) /* Return earlier error */
                प्रत्यागम r;
        जइ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                प्रत्यागम 0;
        प्रत्यागम q;
}

पूर्णाङ्क fsync_path_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        पूर्णाङ्क fd;

        जइ (isempty(path)) {
                जइ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        जइ (opened_fd < 0)
                                प्रत्यागम -errno;

                        fd = opened_fd;
                } अण्णहा
                        fd = at_fd;
        } अण्णहा {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                जइ (opened_fd < 0)
                        प्रत्यागम -errno;

                fd = opened_fd;
        }

        प्रत्यागम RET_NERRNO(fsync(fd));
}

पूर्णाङ्क fsync_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;

        जइ (isempty(path)) {
                जइ (at_fd != AT_FDCWD)
                        प्रत्यागम fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                जइ (opened_fd < 0)
                        प्रत्यागम -errno;

                प्रत्यागम RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        जइ (opened_fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fsync_directory_of_file(opened_fd);
}

पूर्णाङ्क fsync_path_and_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;

        जइ (isempty(path)) {
                जइ (at_fd != AT_FDCWD)
                        प्रत्यागम fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अण्णहा
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        जइ (opened_fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fsync_full(opened_fd);
}

पूर्णाङ्क syncfs_path(पूर्णाङ्क at_fd, ध्रुव वर्ण *path) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        जइ (isempty(path)) {
                जइ (at_fd != AT_FDCWD)
                        प्रत्यागम RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अण्णहा
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        जइ (fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम RET_NERRNO(syncfs(fd));
}
