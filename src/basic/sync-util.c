/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <sys/stat.h>
#समावेश <unistd.h>

#समावेश "alloc-util.h"
#समावेश "errno-util.h"
#समावेश "fd-util.h"
#समावेश "fs-util.h"
#समावेश "log.h"
#समावेश "path-util.h"
#समावेश "string-util.h"
#समावेश "sync-util.h"

पूर्णांक fsync_directory_of_file(पूर्णांक fd) {
        _cleanup_close_ पूर्णांक dfd = -EBADF;
        काष्ठा stat st;
        पूर्णांक r;

        निश्चित(fd >= 0);

        /* We only reasonably can do this for regular files and directories, or for O_PATH fds, hence check
         * for the inode type first */
        अगर (ख_स्थिति(fd, &st) < 0)
                वापस -त्रुटि_सं;

        अगर (S_ISDIR(st.st_mode)) {
                dfd = openat(fd, "..", O_RDONLY|O_DIRECTORY|O_CLOEXEC, 0);
                अगर (dfd < 0)
                        वापस -त्रुटि_सं;

        } अन्यथा अगर (!S_ISREG(st.st_mode)) { /* Regular files are OK regardless if O_PATH or not, for all other
                                            * types check O_PATH flag */
                r = fd_is_opath(fd);
                अगर (r < 0)
                        वापस r;
                अगर (!r) /* If O_PATH this refers to the inode in the fs, in which case we can sensibly do
                         * what is requested. Otherwise this refers to a socket, fifo or device node, where
                         * the concept of a containing directory doesn't make too much sense. */
                        वापस -ENOTTY;
        }

        अगर (dfd < 0) {
                _cleanup_free_ अक्षर *path = शून्य;

                r = fd_get_path(fd, &path);
                अगर (r < 0) {
                        log_debug_errno(r, "Failed to query /proc/self/fd/%d%s: %m",
                                        fd,
                                        r == -ENOSYS ? ", ignoring" : "");

                        अगर (r == -ENOSYS)
                                /* If /proc is not available, we're most likely running in some
                                 * chroot environment, and syncing the directory is not very
                                 * important in that case. Let's just silently do nothing. */
                                वापस 0;

                        वापस r;
                }

                अगर (!path_is_absolute(path))
                        वापस -EINVAL;

                dfd = open_parent(path, O_CLOEXEC|O_NOFOLLOW, 0);
                अगर (dfd < 0)
                        वापस dfd;
        }

        वापस RET_NERRNO(fsync(dfd));
}

पूर्णांक fsync_full(पूर्णांक fd) {
        पूर्णांक r, q;

        /* Sync both the file and the directory */

        r = RET_NERRNO(fsync(fd));

        q = fsync_directory_of_file(fd);
        अगर (r < 0) /* Return earlier error */
                वापस r;
        अगर (q == -ENOTTY) /* Ignore if the 'fd' refers to a block device or so which doesn't really have a
                           * parent dir */
                वापस 0;
        वापस q;
}

पूर्णांक fsync_path_at(पूर्णांक at_fd, स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;
        पूर्णांक fd;

        अगर (isempty(path)) {
                अगर (at_fd == AT_FDCWD) {
                        opened_fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                        अगर (opened_fd < 0)
                                वापस -त्रुटि_सं;

                        fd = opened_fd;
                } अन्यथा
                        fd = at_fd;
        } अन्यथा {
                opened_fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
                अगर (opened_fd < 0)
                        वापस -त्रुटि_सं;

                fd = opened_fd;
        }

        वापस RET_NERRNO(fsync(fd));
}

पूर्णांक fsync_parent_at(पूर्णांक at_fd, स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;

        अगर (isempty(path)) {
                अगर (at_fd != AT_FDCWD)
                        वापस fsync_directory_of_file(at_fd);

                opened_fd = खोलो("..", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
                अगर (opened_fd < 0)
                        वापस -त्रुटि_सं;

                वापस RET_NERRNO(fsync(opened_fd));
        }

        opened_fd = openat(at_fd, path, O_PATH|O_CLOEXEC|O_NOFOLLOW);
        अगर (opened_fd < 0)
                वापस -त्रुटि_सं;

        वापस fsync_directory_of_file(opened_fd);
}

पूर्णांक fsync_path_and_parent_at(पूर्णांक at_fd, स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक opened_fd = -EBADF;

        अगर (isempty(path)) {
                अगर (at_fd != AT_FDCWD)
                        वापस fsync_full(at_fd);

                opened_fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                opened_fd = openat(at_fd, path, O_RDONLY|O_NOFOLLOW|O_NONBLOCK|O_CLOEXEC);
        अगर (opened_fd < 0)
                वापस -त्रुटि_सं;

        वापस fsync_full(opened_fd);
}

पूर्णांक syncfs_path(पूर्णांक at_fd, स्थिर अक्षर *path) {
        _cleanup_close_ पूर्णांक fd = -EBADF;

        अगर (isempty(path)) {
                अगर (at_fd != AT_FDCWD)
                        वापस RET_NERRNO(syncfs(at_fd));

                fd = खोलो(".", O_RDONLY|O_DIRECTORY|O_CLOEXEC);
        } अन्यथा
                fd = openat(at_fd, path, O_RDONLY|O_CLOEXEC|O_NONBLOCK);
        अगर (fd < 0)
                वापस -त्रुटि_सं;

        वापस RET_NERRNO(syncfs(fd));
}
