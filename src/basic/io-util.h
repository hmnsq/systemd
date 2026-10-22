/* SPDX-License-Identifier: LGPL-2.1-or-later */
#आशय once

#समावेश "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

पूर्णांक flush_fd(पूर्णांक fd);

ssize_t loop_read(पूर्णांक fd, व्योम *buf, माप_प्रकार nbytes, bool do_poll);
पूर्णांक loop_read_exact(पूर्णांक fd, व्योम *buf, माप_प्रकार nbytes, bool do_poll);

पूर्णांक loop_write_full(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
अटल अंतरभूत पूर्णांक loop_write(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार nbytes) {
        वापस loop_write_full(fd, buf, nbytes, 0);
}

पूर्णांक pwritev_full(पूर्णांक fd, काष्ठा iovec *iovec, माप_प्रकार n, uint64_t offset, uint64_t *ret_written);
पूर्णांक pwrite_full(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

पूर्णांक pipe_eof(पूर्णांक fd);

पूर्णांक ppoll_usec_full(काष्ठा pollfd *fds, माप_प्रकार n_fds, usec_t timeout, स्थिर sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) अटल अंतरभूत पूर्णांक ppoll_usec(काष्ठा pollfd *fds, माप_प्रकार n_fds, usec_t timeout) {
        वापस ppoll_usec_full(fds, n_fds, timeout, शून्य);
}

पूर्णांक fd_wait_for_event(पूर्णांक fd, पूर्णांक event, usec_t timeout);

ssize_t sparse_write(पूर्णांक fd, स्थिर व्योम *p, माप_प्रकार sz, माप_प्रकार run_लंबाई);

अटल अंतरभूत bool FILE_आकार_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        वापस (l >> 63) == 0;
}

अटल अंतरभूत bool FILE_आकार_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        अगर (l == UINT64_MAX)
                वापस true;

        वापस FILE_आकार_VALID(l);
}
