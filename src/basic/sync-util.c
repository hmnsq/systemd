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

पूर्णाङ्क fsync_directory_सम्बन्धि_file(पूर्णाङ्क fd) {
        _cleanup_close_ पूर्णाङ्क dfd = -EBADF;
        संरचना stat st;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        यदि (fstat(fd, &st) < 0)
                प्रत्यागम -errno;

        यदि (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                यदि (dfd < 0)
                        प्रत्यागम -errno;

        } अन्यथा यदि (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        प्रत्यागम -ENOTTY;
        }

        यदि (dfd < 0) {
                _cleanup_free_ वर्ण *मार्गः = रिक्त;

                r = fd_get_मार्गः(fd, &मार्गः);
                यदि (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        यदि (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                प्रत्यागम 0;

                        प्रत्यागम r;
                }

                यदि (!मार्गः_is_absolute(मार्गः))
                        प्रत्यागम -EINVAL;

                dfd = open_parent(मार्गः, O_CLOEXEC|O_NOFOLLOW, 0);
                यदि (dfd < 0)
                        प्रत्यागम dfd;
        }

        प्रत्यागम RET_NERRNO(fsync(dfd));
}

पूर्णाङ्क fsync_पूर्णम्(पूर्णाङ्क fd) {
        पूर्णाङ्क r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_सम्बन्धि_file(fd);
        यदि (r < 0) /* Return earlier error */
                प्रत्यागम r;
        यदि (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                प्रत्यागम 0;
        प्रत्यागम q;
}

पूर्णाङ्क fsync_मार्गः_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;
        पूर्णाङ्क fd;

        यदि (isempty(मार्गः)) {
                यदि (at_fd == AT_FDCWD) {
                        opened_fd = उद्घाटय(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        यदि (opened_fd < 0)
                                प्रत्यागम -errno;

                        fd = opened_fd;
                } अन्यथा
                        fd = at_fd;
        } अन्यथा {
                opened_fd = openat(at_fd, मार्गः, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                यदि (opened_fd < 0)
                        प्रत्यागम -errno;

                fd = opened_fd;
        }

        प्रत्यागम RET_NERRNO(fsync(fd));
}

पूर्णाङ्क fsync_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;

        यदि (isempty(मार्गः)) {
                यदि (at_fd != AT_FDCWD)
                        प्रत्यागम fsync_directory_सम्बन्धि_file(at_fd);

                opened_fd = उद्घाटय("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                यदि (opened_fd < 0)
                        प्रत्यागम -errno;

                प्रत्यागम RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, मार्गः, O_मार्गः|O_CLOEXEC|O_NOFOLLOW);
        यदि (opened_fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fsync_directory_सम्बन्धि_file(opened_fd);
}

पूर्णाङ्क fsync_मार्गः_and_parent_at(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः) {
        _cleanup_close_ पूर्णाङ्क opened_fd = -EBADF;

        यदि (isempty(मार्गः)) {
                यदि (at_fd != AT_FDCWD)
                        प्रत्यागम fsync_पूर्णम्(at_fd);

                opened_fd = उद्घाटय(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                opened_fd = openat(at_fd, मार्गः, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        यदि (opened_fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम fsync_पूर्णम्(opened_fd);
}

पूर्णाङ्क syncfs_मार्गः(पूर्णाङ्क at_fd, ध्रुव वर्ण *मार्गः) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;

        यदि (isempty(मार्गः)) {
                यदि (at_fd != AT_FDCWD)
                        प्रत्यागम RET_NERRNO(syncfs(at_fd));

                fd = उद्घाटय(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                fd = openat(at_fd, मार्गः, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        यदि (fd < 0)
                प्रत्यागम -errno;

        प्रत्यागम RET_NERRNO(syncfs(fd));
}
