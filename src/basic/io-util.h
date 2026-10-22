/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

ಪೂರ್ಣಾಙ್ಕ flush_fd(ಪೂರ್ಣಾಙ್ಕ fd);

ssize_t loop_read(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll);
ಪೂರ್ಣಾಙ್ಕ loop_read_exact(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll);

ಪೂರ್ಣಾಙ್ಕ loop_write_full(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ loop_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t nbytes) {
        ಪ್ರತ್ಯಾಗಮ loop_write_full(fd, buf, nbytes, 0);
}

ಪೂರ್ಣಾಙ್ಕ pwritev_full(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
ಪೂರ್ಣಾಙ್ಕ pwrite_full(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

ಪೂರ್ಣಾಙ್ಕ pipe_eof(ಪೂರ್ಣಾಙ್ಕ fd);

ಪೂರ್ಣಾಙ್ಕ ppoll_usec_full(ಸಂರಚನಾ pollfd *fds, size_t n_fds, usec_t timeout, ಧ್ರುವ sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ ppoll_usec(ಸಂರಚನಾ pollfd *fds, size_t n_fds, usec_t timeout) {
        ಪ್ರತ್ಯಾಗಮ ppoll_usec_full(fds, n_fds, timeout, ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ fd_wait_for_event(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ event, usec_t timeout);

ssize_t sparse_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *p, size_t sz, size_t run_length);

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ಪ್ರತ್ಯಾಗಮ (l >> 63) == 0;
}

ಸ್ಥಿರ ಅನ್ತಃಸ್ಥ bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ಯದಿ (l == UINT64_MAX)
                ಪ್ರತ್ಯಾಗಮ true;

        ಪ್ರತ್ಯಾಗಮ FILE_SIZE_VALID(l);
}
