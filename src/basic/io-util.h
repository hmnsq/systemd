/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_ਮੰਜ਼ਿਲ_epoll(uint32_t events);
uint32_t epoll_events_ਮੰਜ਼ਿਲ_poll(uint32_t events);

ਪੂਰਨ flush_fd(ਪੂਰਨ fd);

ssize_t loop_read(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t nbytes, bool do_poll);
ਪੂਰਨ loop_read_exact(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t nbytes, bool do_poll);

ਪੂਰਨ loop_write_ਭਰਿਆ(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ loop_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t nbytes) {
        ਵਾਪਸ loop_write_ਭਰਿਆ(fd, buf, nbytes, 0);
}

ਪੂਰਨ pwritev_ਭਰਿਆ(ਪੂਰਨ fd, ਬਣਤਰ iovec *iovec, size_t n, uint64_t ਵਿੱਥ, uint64_t *ret_written);
ਪੂਰਨ pwrite_ਭਰਿਆ(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t nbytes, uint64_t ਵਿੱਥ) _nonnull_if_nonzero_(2, 3);

ਪੂਰਨ pipe_eof(ਪੂਰਨ fd);

ਪੂਰਨ ppoll_usec_ਭਰਿਆ(ਬਣਤਰ pollfd *fds, size_t n_fds, usec_t timeout, ਅਚਲ sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ppoll_usec(ਬਣਤਰ pollfd *fds, size_t n_fds, usec_t timeout) {
        ਵਾਪਸ ppoll_usec_ਭਰਿਆ(fds, n_fds, timeout, ਨਿਰਾ);
}

ਪੂਰਨ fd_wait_for_event(ਪੂਰਨ fd, ਪੂਰਨ event, usec_t timeout);

ssize_t sparse_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *p, size_t sz, size_t ਲੜੀਵਾਰ_ਲੰਮਾਈ);

ਸਥਿਰ ਸਤਰਅੰਦਰ bool FILE_ਮਾਪ_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ਵਾਪਸ (l >> 63) == 0;
}

ਸਥਿਰ ਸਤਰਅੰਦਰ bool FILE_ਮਾਪ_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ਜੇ (l == UINT64_MAX)
                ਵਾਪਸ true;

        ਵਾਪਸ FILE_ਮਾਪ_VALID(l);
}
