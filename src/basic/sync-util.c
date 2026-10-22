/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sys/stat.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fd-util.h"
#અન્તર્ભાવ "fs-util.h"
#અન્તર્ભાવ "log.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "sync-util.h"

પૂર્ણાંક fsync_directory_of_file(પૂર્ણાંક fd) {
        _cleanup_close_ પૂર્ણાંક dfd = -EBADF;
        સંરચના stat st;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        જો (fstat(fd, &st) < 0)
                પાછા -errno;

        જો (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                જો (dfd < 0)
                        પાછા -errno;

        } નહીંતર જો (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                જો (r < 0)
                        પાછા r;
                જો (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        પાછા -ENOTTY;
        }

        જો (dfd < 0) {
                _cleanup_free_ અક્ષર *path = ખાલી;

                r = fd_get_path(fd, &path);
                જો (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        જો (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                પાછા 0;

                        પાછા r;
                }

                જો (!path_is_absolute(path))
                        પાછા -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                જો (dfd < 0)
                        પાછા dfd;
        }

        પાછા RET_NERRNO(fsync(dfd));
}

પૂર્ણાંક fsync_full(પૂર્ણાંક fd) {
        પૂર્ણાંક r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        જો (r < 0) /* Return earlier error */
                પાછા r;
        જો (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                પાછા 0;
        પાછા q;
}

પૂર્ણાંક fsync_path_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;
        પૂર્ણાંક fd;

        જો (isempty(path)) {
                જો (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        જો (opened_fd < 0)
                                પાછા -errno;

                        fd = opened_fd;
                } નહીંતર
                        fd = at_fd;
        } નહીંતર {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                જો (opened_fd < 0)
                        પાછા -errno;

                fd = opened_fd;
        }

        પાછા RET_NERRNO(fsync(fd));
}

પૂર્ણાંક fsync_parent_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;

        જો (isempty(path)) {
                જો (at_fd != AT_FDCWD)
                        પાછા fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                જો (opened_fd < 0)
                        પાછા -errno;

                પાછા RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        જો (opened_fd < 0)
                પાછા -errno;

        પાછા fsync_directory_of_file(opened_fd);
}

પૂર્ણાંક fsync_path_and_parent_at(પૂર્ણાંક at_fd, અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક opened_fd = -EBADF;

        જો (isempty(path)) {
                જો (at_fd != AT_FDCWD)
                        પાછા fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } નહીંતર
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        જો (opened_fd < 0)
                પાછા -errno;

        પાછા fsync_full(opened_fd);
}

પૂર્ણાંક syncfs_path(પૂર્ણાંક at_fd, અચલ અક્ષર *path) {
        _cleanup_close_ પૂર્ણાંક fd = -EBADF;

        જો (isempty(path)) {
                જો (at_fd != AT_FDCWD)
                        પાછા RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } નહીંતર
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        જો (fd < 0)
                પાછા -errno;

        પાછા RET_NERRNO(syncfs(fd));
}
