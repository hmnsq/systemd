/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "sync-util.h"

പൂർണ്ണസംഖ്യ fsync_directory_of_file(പൂർണ്ണസംഖ്യ fd) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ dfd = -EBADF;
        ഘടന stat st;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        എങ്കിൽ (fstat(fd, &st) < 0)
                തിരികെ -errno;

        എങ്കിൽ (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                എങ്കിൽ (dfd < 0)
                        തിരികെ -errno;

        } അല്ലെങ്കിൽ എങ്കിൽ (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        തിരികെ -ENOTTY;
        }

        എങ്കിൽ (dfd < 0) {
                _cleanup_free_ അക്ഷരം *path = ശൂന്യ;

                r = fd_get_path(fd, &path);
                എങ്കിൽ (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        എങ്കിൽ (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                തിരികെ 0;

                        തിരികെ r;
                }

                എങ്കിൽ (!path_is_absolute(path))
                        തിരികെ -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                എങ്കിൽ (dfd < 0)
                        തിരികെ dfd;
        }

        തിരികെ RET_NERRNO(fsync(dfd));
}

പൂർണ്ണസംഖ്യ fsync_full(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        എങ്കിൽ (r < 0) /* Return earlier error */
                തിരികെ r;
        എങ്കിൽ (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                തിരികെ 0;
        തിരികെ q;
}

പൂർണ്ണസംഖ്യ fsync_path_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;
        പൂർണ്ണസംഖ്യ fd;

        എങ്കിൽ (isempty(path)) {
                എങ്കിൽ (at_fd == AT_FDCWD) {
                        opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        എങ്കിൽ (opened_fd < 0)
                                തിരികെ -errno;

                        fd = opened_fd;
                } അല്ലെങ്കിൽ
                        fd = at_fd;
        } അല്ലെങ്കിൽ {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                എങ്കിൽ (opened_fd < 0)
                        തിരികെ -errno;

                fd = opened_fd;
        }

        തിരികെ RET_NERRNO(fsync(fd));
}

പൂർണ്ണസംഖ്യ fsync_parent_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;

        എങ്കിൽ (isempty(path)) {
                എങ്കിൽ (at_fd != AT_FDCWD)
                        തിരികെ fsync_directory_of_file(at_fd);

                opened_fd = open("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                എങ്കിൽ (opened_fd < 0)
                        തിരികെ -errno;

                തിരികെ RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        എങ്കിൽ (opened_fd < 0)
                തിരികെ -errno;

        തിരികെ fsync_directory_of_file(opened_fd);
}

പൂർണ്ണസംഖ്യ fsync_path_and_parent_at(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ opened_fd = -EBADF;

        എങ്കിൽ (isempty(path)) {
                എങ്കിൽ (at_fd != AT_FDCWD)
                        തിരികെ fsync_full(at_fd);

                opened_fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } അല്ലെങ്കിൽ
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        എങ്കിൽ (opened_fd < 0)
                തിരികെ -errno;

        തിരികെ fsync_full(opened_fd);
}

പൂർണ്ണസംഖ്യ syncfs_path(പൂർണ്ണസംഖ്യ at_fd, സ്ഥിരം അക്ഷരം *path) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;

        എങ്കിൽ (isempty(path)) {
                എങ്കിൽ (at_fd != AT_FDCWD)
                        തിരികെ RET_NERRNO(syncfs(at_fd));

                fd = open(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } അല്ലെങ്കിൽ
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        എങ്കിൽ (fd < 0)
                തിരികെ -errno;

        തിരികെ RET_NERRNO(syncfs(fd));
}
