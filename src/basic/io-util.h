/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

পূর্ণাঙ্ক flush_fd(পূর্ণাঙ্ক fd);

ssize_t loop_read(পূর্ণাঙ্ক fd, শূন্য *buf, size_t nbytes, bool do_poll);
পূর্ণাঙ্ক loop_read_exact(পূর্ণাঙ্ক fd, শূন্য *buf, size_t nbytes, bool do_poll);

পূর্ণাঙ্ক loop_write_full(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
স্থির অন্তঃস্থ পূর্ণাঙ্ক loop_write(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t nbytes) {
        প্রত্যাগম loop_write_full(fd, buf, nbytes, 0);
}

পূর্ণাঙ্ক pwritev_full(পূর্ণাঙ্ক fd, সংরচনা iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
পূর্ণাঙ্ক pwrite_full(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

পূর্ণাঙ্ক pipe_eof(পূর্ণাঙ্ক fd);

পূর্ণাঙ্ক ppoll_usec_full(সংরচনা pollfd *fds, size_t n_fds, usec_t timeout, ধ্রুব sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) স্থির অন্তঃস্থ পূর্ণাঙ্ক ppoll_usec(সংরচনা pollfd *fds, size_t n_fds, usec_t timeout) {
        প্রত্যাগম ppoll_usec_full(fds, n_fds, timeout, রিক্ত);
}

পূর্ণাঙ্ক fd_wait_for_event(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক event, usec_t timeout);

ssize_t sparse_write(পূর্ণাঙ্ক fd, ধ্রুব শূন্য *p, size_t sz, size_t run_length);

স্থির অন্তঃস্থ bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        প্রত্যাগম (l >> 63) == 0;
}

স্থির অন্তঃস্থ bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        যদি (l == UINT64_MAX)
                প্রত্যাগম true;

        প্রত্যাগম FILE_SIZE_VALID(l);
}
