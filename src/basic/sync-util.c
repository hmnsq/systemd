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

ကိန်းပြည့် fsync_directory_of_file(ကိန်းပြည့် fd) {
        _cleanup_close_ ကိန်းပြည့် dfd = -EBADF;
        sa_nracanaa stat st;
        ကိန်းပြည့် r;

        assert(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        အကယ်၍ (fstat(fd, &st) < 0)
                ပြန်ပေး -errno;

        အကယ်၍ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                အကယ်၍ (dfd < 0)
                        ပြန်ပေး -errno;

        } မဟုတ်ပါက အကယ်၍ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;
                အကယ်၍ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        ပြန်ပေး -ENOTTY;
        }

        အကယ်၍ (dfd < 0) {
                _cleanup_free_ var_na *path = NULL;

                r = fd_get_path(fd, &path);
                အကယ်၍ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        အကယ်၍ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                ပြန်ပေး 0;

                        ပြန်ပေး r;
                }

                အကယ်၍ (!path_is_absolute(path))
                        ပြန်ပေး -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                အကယ်၍ (dfd < 0)
                        ပြန်ပေး dfd;
        }

        ပြန်ပေး RET_NERRNO(fsync(dfd));
}

ကိန်းပြည့် fsync_full(ကိန်းပြည့် fd) {
        ကိန်းပြည့် r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        အကယ်၍ (r < 0) /* Return earlier error */
                ပြန်ပေး r;
        အကယ်၍ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                ပြန်ပေး 0;
        ပြန်ပေး q;
}

ကိန်းပြည့် fsync_path_at(ကိန်းပြည့် at_fd, const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် opened_fd = -EBADF;
        ကိန်းပြည့် fd;

        အကယ်၍ (isempty(path)) {
                အကယ်၍ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        အကယ်၍ (opened_fd < 0)
                                ပြန်ပေး -errno;

                        fd = opened_fd;
                } မဟုတ်ပါက
                        fd = at_fd;
        } မဟုတ်ပါက {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                အကယ်၍ (opened_fd < 0)
                        ပြန်ပေး -errno;

                fd = opened_fd;
        }

        ပြန်ပေး RET_NERRNO(fsync(fd));
}

ကိန်းပြည့် fsync_parent_at(ကိန်းပြည့် at_fd, const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် opened_fd = -EBADF;

        အကယ်၍ (isempty(path)) {
                အကယ်၍ (at_fd != AT_FDCWD)
                        ပြန်ပေး fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                အကယ်၍ (opened_fd < 0)
                        ပြန်ပေး -errno;

                ပြန်ပေး RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        အကယ်၍ (opened_fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး fsync_directory_of_file(opened_fd);
}

ကိန်းပြည့် fsync_path_and_parent_at(ကိန်းပြည့် at_fd, const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် opened_fd = -EBADF;

        အကယ်၍ (isempty(path)) {
                အကယ်၍ (at_fd != AT_FDCWD)
                        ပြန်ပေး fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } မဟုတ်ပါက
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        အကယ်၍ (opened_fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး fsync_full(opened_fd);
}

ကိန်းပြည့် syncfs_path(ကိန်းပြည့် at_fd, const var_na *path) {
        _cleanup_close_ ကိန်းပြည့် fd = -EBADF;

        အကယ်၍ (isempty(path)) {
                အကယ်၍ (at_fd != AT_FDCWD)
                        ပြန်ပေး RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } မဟုတ်ပါက
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        အကယ်၍ (fd < 0)
                ပြန်ပေး -errno;

        ပြန်ပေး RET_NERRNO(syncfs(fd));
}
