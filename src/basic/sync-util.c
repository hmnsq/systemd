/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sys/stat.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "sync-util.h"

పూర్ణాంకం fsync_directory_యొక్క_file(పూర్ణాంకం fd) {
        _cleanup_close_ పూర్ణాంకం dfd = -EBADF;
        నిర్మాణం stat st;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        ఒకవేళ (fstat(fd, &st) < 0)
                తిరిగి -errno;

        ఒకవేళ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                ఒకవేళ (dfd < 0)
                        తిరిగి -errno;

        } లేకపోతే ఒకవేళ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        తిరిగి -ENOTTY;
        }

        ఒకవేళ (dfd < 0) {
                _cleanup_free_ అక్షరం *మార్గం = ఖాళీ;

                r = fd_get_మార్గం(fd, &మార్గం);
                ఒకవేళ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        ఒకవేళ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                తిరిగి 0;

                        తిరిగి r;
                }

                ఒకవేళ (!మార్గం_is_absolute(మార్గం))
                        తిరిగి -EINVAL;

                dfd = open_మాతృ(మార్గం, O_CLOEXEC|O_NOFOLLOW, 0);
                ఒకవేళ (dfd < 0)
                        తిరిగి dfd;
        }

        తిరిగి RET_NERRNO(fsync(dfd));
}

పూర్ణాంకం fsync_నిండిన(పూర్ణాంకం fd) {
        పూర్ణాంకం r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_యొక్క_file(fd);
        ఒకవేళ (r < 0) /* Return earlier error */
                తిరిగి r;
        ఒకవేళ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                తిరిగి 0;
        తిరిగి q;
}

పూర్ణాంకం fsync_మార్గం_at(పూర్ణాంకం at_fd, స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;
        పూర్ణాంకం fd;

        ఒకవేళ (isempty(మార్గం)) {
                ఒకవేళ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        ఒకవేళ (opened_fd < 0)
                                తిరిగి -errno;

                        fd = opened_fd;
                } లేకపోతే
                        fd = at_fd;
        } లేకపోతే {
                opened_fd = openat(at_fd, మార్గం, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                ఒకవేళ (opened_fd < 0)
                        తిరిగి -errno;

                fd = opened_fd;
        }

        తిరిగి RET_NERRNO(fsync(fd));
}

పూర్ణాంకం fsync_మాతృ_at(పూర్ణాంకం at_fd, స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;

        ఒకవేళ (isempty(మార్గం)) {
                ఒకవేళ (at_fd != AT_FDCWD)
                        తిరిగి fsync_directory_యొక్క_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                ఒకవేళ (opened_fd < 0)
                        తిరిగి -errno;

                తిరిగి RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, మార్గం, O_మార్గం|O_CLOEXEC|O_NOFOLLOW);
        ఒకవేళ (opened_fd < 0)
                తిరిగి -errno;

        తిరిగి fsync_directory_యొక్క_file(opened_fd);
}

పూర్ణాంకం fsync_మార్గం_and_మాతృ_at(పూర్ణాంకం at_fd, స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం opened_fd = -EBADF;

        ఒకవేళ (isempty(మార్గం)) {
                ఒకవేళ (at_fd != AT_FDCWD)
                        తిరిగి fsync_నిండిన(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } లేకపోతే
                opened_fd = openat(at_fd, మార్గం, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        ఒకవేళ (opened_fd < 0)
                తిరిగి -errno;

        తిరిగి fsync_నిండిన(opened_fd);
}

పూర్ణాంకం syncfs_మార్గం(పూర్ణాంకం at_fd, స్థిరరాశి అక్షరం *మార్గం) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;

        ఒకవేళ (isempty(మార్గం)) {
                ఒకవేళ (at_fd != AT_FDCWD)
                        తిరిగి RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } లేకపోతే
                fd = openat(at_fd, మార్గం, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        ఒకవేళ (fd < 0)
                తిరిగి -errno;

        తిరిగి RET_NERRNO(syncfs(fd));
}
