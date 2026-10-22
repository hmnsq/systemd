/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "fs-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "sync-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_directory_of_file(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfd = -EBADF;
        ସଂରଚନା stat st;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ଯଦି (fstat(fd, &st) < 0)
                ଫେରନ୍ତୁ -errno;

        ଯଦି (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ଯଦି (dfd < 0)
                        ଫେରନ୍ତୁ -errno;

        } ଅନ୍ୟଥା ଯଦି (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ଫେରନ୍ତୁ -ENOTTY;
        }

        ଯଦି (dfd < 0) {
                _cleanup_free_ ଅକ୍ଷର *path = ଖାଲି;

                r = fd_get_path(fd, &path);
                ଯଦି (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ଯଦି (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ଫେରନ୍ତୁ 0;

                        ଫେରନ୍ତୁ r;
                }

                ଯଦି (!path_is_absolute(path))
                        ଫେରନ୍ତୁ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                ଯଦି (dfd < 0)
                        ଫେରନ୍ତୁ dfd;
        }

        ଫେରନ୍ତୁ RET_NERRNO(fsync(dfd));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        ଯଦି (r < 0) /* Return earlier error */
                ଫେରନ୍ତୁ r;
        ଯଦି (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ଫେରନ୍ତୁ 0;
        ଫେରନ୍ତୁ q;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_path_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

        ଯଦି (isempty(path)) {
                ଯଦି (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ଯଦି (opened_fd < 0)
                                ଫେରନ୍ତୁ -errno;

                        fd = opened_fd;
                } ଅନ୍ୟଥା
                        fd = at_fd;
        } ଅନ୍ୟଥା {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ଯଦି (opened_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                fd = opened_fd;
        }

        ଫେରନ୍ତୁ RET_NERRNO(fsync(fd));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;

        ଯଦି (isempty(path)) {
                ଯଦି (at_fd != AT_FDCWD)
                        ଫେରନ୍ତୁ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ଯଦି (opened_fd < 0)
                        ଫେରନ୍ତୁ -errno;

                ଫେରନ୍ତୁ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        ଯଦି (opened_fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ fsync_directory_of_file(opened_fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fsync_path_and_parent_at(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା opened_fd = -EBADF;

        ଯଦି (isempty(path)) {
                ଯଦି (at_fd != AT_FDCWD)
                        ଫେରନ୍ତୁ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ଅନ୍ୟଥା
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ଯଦି (opened_fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ fsync_full(opened_fd);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା syncfs_path(ପୂର୍ଣ୍ଣସଂଖ୍ୟା at_fd, ଅଚଳ ଅକ୍ଷର *path) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;

        ଯଦି (isempty(path)) {
                ଯଦି (at_fd != AT_FDCWD)
                        ଫେରନ୍ତୁ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } ଅନ୍ୟଥା
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ -errno;

        ଫେରନ୍ତୁ RET_NERRNO(syncfs(fd));
}
