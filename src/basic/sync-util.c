/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "alloc-util.h"
#include "errno-util.h"
#include "fd-util.h"
#include "fs-util.h"
#include "log.h"
#include "path-util.h"
#include "string-util.h"
#include "sync-util.h"

පූර්ණ_සංඛ්‍යා fsync_directory_of_file(පූර්ණ_සංඛ්‍යා fd) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා dfd = -EBADF;
        sa_nracanaa stat st;
        පූර්ණ_සංඛ්‍යා r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        නම් (fstat(fd, &st) < 0)
                ආපසු -errno;

        නම් (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                නම් (dfd < 0)
                        ආපසු -errno;

        } නැතහොත් නම් (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                නම් (r < 0)
                        ආපසු r;
                නම් (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ආපසු -ENOTTY;
        }

        නම් (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                නම් (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        නම් (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ආපසු 0;

                        ආපසු r;
                }

                නම් (!path_is_absolute(path))
                        ආපසු -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                නම් (dfd < 0)
                        ආපසු dfd;
        }

        ආපසු RET_NERRNO(fsync(dfd));
}

පූර්ණ_සංඛ්‍යා fsync_full(පූර්ණ_සංඛ්‍යා fd) {
        පූර්ණ_සංඛ්‍යා r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        නම් (r < 0) /* Return earlier error */
                ආපසු r;
        නම් (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ආපසු 0;
        ආපසු q;
}

පූර්ණ_සංඛ්‍යා fsync_path_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා opened_fd = -EBADF;
        පූර්ණ_සංඛ්‍යා fd;

        නම් (isempty(path)) {
                නම් (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        නම් (opened_fd < 0)
                                ආපසු -errno;

                        fd = opened_fd;
                } නැතහොත්
                        fd = at_fd;
        } නැතහොත් {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                නම් (opened_fd < 0)
                        ආපසු -errno;

                fd = opened_fd;
        }

        ආපසු RET_NERRNO(fsync(fd));
}

පූර්ණ_සංඛ්‍යා fsync_parent_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා opened_fd = -EBADF;

        නම් (isempty(path)) {
                නම් (at_fd != AT_FDCWD)
                        ආපසු fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                නම් (opened_fd < 0)
                        ආපසු -errno;

                ආපසු RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        නම් (opened_fd < 0)
                ආපසු -errno;

        ආපසු fsync_directory_of_file(opened_fd);
}

පූර්ණ_සංඛ්‍යා fsync_path_and_parent_at(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා opened_fd = -EBADF;

        නම් (isempty(path)) {
                නම් (at_fd != AT_FDCWD)
                        ආපසු fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } නැතහොත්
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        නම් (opened_fd < 0)
                ආපසු -errno;

        ආපසු fsync_full(opened_fd);
}

පූර්ණ_සංඛ්‍යා syncfs_path(පූර්ණ_සංඛ්‍යා at_fd, const var_na *path) {
        _cleanup_close_ පූර්ණ_සංඛ්‍යා fd = -EBADF;

        නම් (isempty(path)) {
                නම් (at_fd != AT_FDCWD)
                        ආපසු RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } නැතහොත්
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        නම් (fd < 0)
                ආපසු -errno;

        ආපසු RET_NERRNO(syncfs(fd));
}
