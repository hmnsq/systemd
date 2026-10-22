/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#شامل "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_مقصد_epoll(uint32_t events);
uint32_t epoll_events_مقصد_poll(uint32_t events);

صحیح flush_fd(صحیح fd);

ssize_t loop_read(صحیح fd, خالی *buf, size_t nbytes, bool do_poll);
صحیح loop_read_exact(صحیح fd, خالی *buf, size_t nbytes, bool do_poll);

صحیح loop_write_بھرا(صحیح fd, ثابت خالی *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ساکن اندرونی صحیح loop_write(صحیح fd, ثابت خالی *buf, size_t nbytes) {
        واپس loop_write_بھرا(fd, buf, nbytes, 0);
}

صحیح pwritev_بھرا(صحیح fd, ساخت iovec *iovec, size_t n, uint64_t انحراف, uint64_t *ret_written);
صحیح pwrite_بھرا(صحیح fd, ثابت خالی *buf, size_t nbytes, uint64_t انحراف) _nonnull_if_nonzero_(2, 3);

صحیح pipe_eof(صحیح fd);

صحیح ppoll_usec_بھرا(ساخت pollfd *fds, size_t n_fds, usec_t timeout, ثابت sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ساکن اندرونی صحیح ppoll_usec(ساخت pollfd *fds, size_t n_fds, usec_t timeout) {
        واپس ppoll_usec_بھرا(fds, n_fds, timeout, تہی);
}

صحیح fd_wait_for_event(صحیح fd, صحیح event, usec_t timeout);

ssize_t sparse_write(صحیح fd, ثابت خالی *p, size_t sz, size_t تسلسل_لمبائی);

ساکن اندرونی bool FILE_سائز_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        واپس (l >> 63) == 0;
}

ساکن اندرونی bool FILE_سائز_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        اگر (l == UINT64_MAX)
                واپس true;

        واپس FILE_سائز_VALID(l);
}
