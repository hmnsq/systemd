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

पूर्णांक fsync_directory_of_file(पूर्णांक fd) {
        _cleanup_close_ पूर्णांक dfd = -EBADF;
        संरचना stat st;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        जर (ख_स्थिति(fd, &st) < 0)
                परत -त्रुटि_सं;

        जर (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                जर (dfd < 0)
                        परत -त्रुटि_सं;

        } अन्यथा जर (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                जर (r < 0)
                        परत r;
                जर (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        परत -ENOTTY;
        }

        जर (dfd < 0) {
                _cleanup_free_ अक्षर *path = रिक्त;

                r = fd_get_path(fd, &path);
                जर (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        जर (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                परत 0;

                        परत r;
                }

                जर (!path_is_absolute(path))
                        परत -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                जर (dfd < 0)
                        परत dfd;
        }

        परत RET_NERRNO(fsync(dfd));
}

पूर्णांक fsync_full(पूर्णांक fd) {
        पूर्णांक r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        जर (r < 0) /* Return earlier error */
                परत r;
        जर (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                परत 0;
        परत q;
}

पूर्णांक fsync_path_at(पूर्णांक at_fd, स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        पूर्णांक fd;

        जर (isempty(path)) {
                जर (at_fd == AT_FDCWD) {
                        opened_fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        जर (opened_fd < 0)
                                परत -त्रुटि_सं;

                        fd = opened_fd;
                } अन्यथा
                        fd = at_fd;
        } अन्यथा {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                जर (opened_fd < 0)
                        परत -त्रुटि_सं;

                fd = opened_fd;
        }

        परत RET_NERRNO(fsync(fd));
}

पूर्णांक fsync_parent_at(पूर्णांक at_fd, स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;

        जर (isempty(path)) {
                जर (at_fd != AT_FDCWD)
                        परत fsync_directory_of_file(at_fd);

                opened_fd = खोलो("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                जर (opened_fd < 0)
                        परत -त्रुटि_सं;

                परत RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        जर (opened_fd < 0)
                परत -त्रुटि_सं;

        परत fsync_directory_of_file(opened_fd);
}

पूर्णांक fsync_path_and_parent_at(पूर्णांक at_fd, स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;

        जर (isempty(path)) {
                जर (at_fd != AT_FDCWD)
                        परत fsync_full(at_fd);

                opened_fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        जर (opened_fd < 0)
                परत -त्रुटि_सं;

        परत fsync_full(opened_fd);
}

पूर्णांक syncfs_path(पूर्णांक at_fd, स्थिरांक अक्षर *path) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        जर (isempty(path)) {
                जर (at_fd != AT_FDCWD)
                        परत RET_NERRNO(syncfs(at_fd));

                fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        जर (fd < 0)
                परत -त्रुटि_सं;

        परत RET_NERRNO(syncfs(fd));
}
