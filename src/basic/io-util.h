/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#অন্তর্ভাব "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

পূর্ণসংখ্যা flush_fd(পূর্ণসংখ্যা fd);

ssize_t loop_read(পূর্ণসংখ্যা fd, শূন্য *buf, size_t nbytes, bool do_poll);
পূর্ণসংখ্যা loop_read_exact(পূর্ণসংখ্যা fd, শূন্য *buf, size_t nbytes, bool do_poll);

পূর্ণসংখ্যা loop_write_full(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
স্থির অন্তঃরেখ পূর্ণসংখ্যা loop_write(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t nbytes) {
        ফেরত loop_write_full(fd, buf, nbytes, 0);
}

পূর্ণসংখ্যা pwritev_full(পূর্ণসংখ্যা fd, কাঠামো iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
পূর্ণসংখ্যা pwrite_full(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

পূর্ণসংখ্যা pipe_eof(পূর্ণসংখ্যা fd);

পূর্ণসংখ্যা ppoll_usec_full(কাঠামো pollfd *fds, size_t n_fds, usec_t timeout, ধ্রুবক sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) স্থির অন্তঃরেখ পূর্ণসংখ্যা ppoll_usec(কাঠামো pollfd *fds, size_t n_fds, usec_t timeout) {
        ফেরত ppoll_usec_full(fds, n_fds, timeout, ফাঁকা);
}

পূর্ণসংখ্যা fd_wait_for_event(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা event, usec_t timeout);

ssize_t sparse_write(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *p, size_t sz, size_t run_দৈর্ঘ্য);

স্থির অন্তঃরেখ bool FILE_আকার_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ফেরত (l >> 63) == 0;
}

স্থির অন্তঃরেখ bool FILE_আকার_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        যদি (l == UINT64_MAX)
                ফেরত true;

        ফেরত FILE_আকার_VALID(l);
}
