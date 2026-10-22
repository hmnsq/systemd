/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

цео flush_fd(цео fd);

ssize_t loop_read(цео fd, void *buf, size_t nbytes, bool do_poll);
цео loop_read_exact(цео fd, void *buf, size_t nbytes, bool do_poll);

цео loop_write_full(цео fd, const void *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
static inline цео loop_write(цео fd, const void *buf, size_t nbytes) {
        врати loop_write_full(fd, buf, nbytes, 0);
}

цео pwritev_full(цео fd, struct iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
цео pwrite_full(цео fd, const void *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

цео pipe_eof(цео fd);

цео ppoll_usec_full(struct pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) static inline цео ppoll_usec(struct pollfd *fds, size_t n_fds, usec_t timeout) {
        врати ppoll_usec_full(fds, n_fds, timeout, NULL);
}

цео fd_wait_for_event(цео fd, цео event, usec_t timeout);

ssize_t sparse_write(цео fd, const void *p, size_t sz, size_t run_length);

static inline bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        врати (l >> 63) == 0;
}

static inline bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ако (l == UINT64_MAX)
                врати true;

        врати FILE_SIZE_VALID(l);
}
