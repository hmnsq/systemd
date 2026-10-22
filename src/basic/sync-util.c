/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sys/stat.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "sync-util.h"

முழு fsync_directory_of_file(முழு fd) {
        _cleanup_close_ முழு dfd = -EBADF;
        கட்டமைப்பு stat st;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        எனில் (fstat(fd, &st) < 0)
                திரும்பு -errno;

        எனில் (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                எனில் (dfd < 0)
                        திரும்பு -errno;

        } இல்லையெனில் எனில் (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        திரும்பு -ENOTTY;
        }

        எனில் (dfd < 0) {
                _cleanup_free_ எழுத்து *path = வெற்று;

                r = fd_get_path(fd, &path);
                எனில் (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        எனில் (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                திரும்பு 0;

                        திரும்பு r;
                }

                எனில் (!path_is_absolute(path))
                        திரும்பு -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                எனில் (dfd < 0)
                        திரும்பு dfd;
        }

        திரும்பு RET_NERRNO(fsync(dfd));
}

முழு fsync_full(முழு fd) {
        முழு r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        எனில் (r < 0) /* Return earlier error */
                திரும்பு r;
        எனில் (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                திரும்பு 0;
        திரும்பு q;
}

முழு fsync_path_at(முழு at_fd, மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு opened_fd = -EBADF;
        முழு fd;

        எனில் (isempty(path)) {
                எனில் (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        எனில் (opened_fd < 0)
                                திரும்பு -errno;

                        fd = opened_fd;
                } இல்லையெனில்
                        fd = at_fd;
        } இல்லையெனில் {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                எனில் (opened_fd < 0)
                        திரும்பு -errno;

                fd = opened_fd;
        }

        திரும்பு RET_NERRNO(fsync(fd));
}

முழு fsync_parent_at(முழு at_fd, மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு opened_fd = -EBADF;

        எனில் (isempty(path)) {
                எனில் (at_fd != AT_FDCWD)
                        திரும்பு fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                எனில் (opened_fd < 0)
                        திரும்பு -errno;

                திரும்பு RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        எனில் (opened_fd < 0)
                திரும்பு -errno;

        திரும்பு fsync_directory_of_file(opened_fd);
}

முழு fsync_path_and_parent_at(முழு at_fd, மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு opened_fd = -EBADF;

        எனில் (isempty(path)) {
                எனில் (at_fd != AT_FDCWD)
                        திரும்பு fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } இல்லையெனில்
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        எனில் (opened_fd < 0)
                திரும்பு -errno;

        திரும்பு fsync_full(opened_fd);
}

முழு syncfs_path(முழு at_fd, மாறிலி எழுத்து *path) {
        _cleanup_close_ முழு fd = -EBADF;

        எனில் (isempty(path)) {
                எனில் (at_fd != AT_FDCWD)
                        திரும்பு RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } இல்லையெனில்
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        எனில் (fd < 0)
                திரும்பு -errno;

        திரும்பு RET_NERRNO(syncfs(fd));
}
