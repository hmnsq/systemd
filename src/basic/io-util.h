/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_ಗಮ್ಯ_epoll(uint32_t events);
uint32_t epoll_events_ಗಮ್ಯ_poll(uint32_t events);

ಪೂರ್ಣಾಂಕ flush_fd(ಪೂರ್ಣಾಂಕ fd);

ssize_t loop_read(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll);
ಪೂರ್ಣಾಂಕ loop_read_exact(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll);

ಪೂರ್ಣಾಂಕ loop_write_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ loop_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t nbytes) {
        ಹಿಂತಿರುಗಿ loop_write_ತುಂಬಿದ(fd, buf, nbytes, 0);
}

ಪೂರ್ಣಾಂಕ pwritev_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ iovec *iovec, size_t n, uint64_t ಸ್ಥಾನಾಂತರ, uint64_t *ret_written);
ಪೂರ್ಣಾಂಕ pwrite_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t nbytes, uint64_t ಸ್ಥಾನಾಂತರ) _nonnull_if_nonzero_(2, 3);

ಪೂರ್ಣಾಂಕ pipe_eof(ಪೂರ್ಣಾಂಕ fd);

ಪೂರ್ಣಾಂಕ ppoll_usec_ತುಂಬಿದ(ರಚನೆ pollfd *fds, size_t n_fds, usec_t timeout, ಸ್ಥಿರಾಂಕ sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ppoll_usec(ರಚನೆ pollfd *fds, size_t n_fds, usec_t timeout) {
        ಹಿಂತಿರುಗಿ ppoll_usec_ತುಂಬಿದ(fds, n_fds, timeout, ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ fd_wait_for_event(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ event, usec_t timeout);

ssize_t sparse_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t sz, size_t ಸತತ_ಉದ್ದಳತೆ);

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool FILE_ಗಾತ್ರ_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ಹಿಂತಿರುಗಿ (l >> 63) == 0;
}

ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool FILE_ಗಾತ್ರ_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ಆದರೆ (l == UINT64_MAX)
                ಹಿಂತಿರುಗಿ true;

        ಹಿಂತಿರುಗಿ FILE_ಗಾತ್ರ_VALID(l);
}
