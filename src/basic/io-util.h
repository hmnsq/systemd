/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#அந்தர்பாவ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

முழு flush_fd(முழு fd);

ssize_t loop_read(முழு fd, வெறுமை *buf, size_t nbytes, bool do_poll);
முழு loop_read_exact(முழு fd, வெறுமை *buf, size_t nbytes, bool do_poll);

முழு loop_write_full(முழு fd, மாறிலி வெறுமை *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
நிலையான உள்வரி முழு loop_write(முழு fd, மாறிலி வெறுமை *buf, size_t nbytes) {
        திரும்பு loop_write_full(fd, buf, nbytes, 0);
}

முழு pwritev_full(முழு fd, கட்டமைப்பு iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
முழு pwrite_full(முழு fd, மாறிலி வெறுமை *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

முழு pipe_eof(முழு fd);

முழு ppoll_usec_full(கட்டமைப்பு pollfd *fds, size_t n_fds, usec_t timeout, மாறிலி sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) நிலையான உள்வரி முழு ppoll_usec(கட்டமைப்பு pollfd *fds, size_t n_fds, usec_t timeout) {
        திரும்பு ppoll_usec_full(fds, n_fds, timeout, வெற்று);
}

முழு fd_wait_for_event(முழு fd, முழு event, usec_t timeout);

ssize_t sparse_write(முழு fd, மாறிலி வெறுமை *p, size_t sz, size_t run_length);

நிலையான உள்வரி bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        திரும்பு (l >> 63) == 0;
}

நிலையான உள்வரி bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        எனில் (l == UINT64_MAX)
                திரும்பு true;

        திரும்பு FILE_SIZE_VALID(l);
}
