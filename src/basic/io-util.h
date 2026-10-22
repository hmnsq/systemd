/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#അന്തര്ഭാവ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

പൂർണ്ണസംഖ്യ flush_fd(പൂർണ്ണസംഖ്യ fd);

ssize_t loop_read(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t nbytes, bool do_poll);
പൂർണ്ണസംഖ്യ loop_read_exact(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t nbytes, bool do_poll);

പൂർണ്ണസംഖ്യ loop_write_full(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ loop_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t nbytes) {
        തിരികെ loop_write_full(fd, buf, nbytes, 0);
}

പൂർണ്ണസംഖ്യ pwritev_full(പൂർണ്ണസംഖ്യ fd, ഘടന iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
പൂർണ്ണസംഖ്യ pwrite_full(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

പൂർണ്ണസംഖ്യ pipe_eof(പൂർണ്ണസംഖ്യ fd);

പൂർണ്ണസംഖ്യ ppoll_usec_full(ഘടന pollfd *fds, size_t n_fds, usec_t timeout, സ്ഥിരം sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) സ്ഥിതം വരിയിൽ പൂർണ്ണസംഖ്യ ppoll_usec(ഘടന pollfd *fds, size_t n_fds, usec_t timeout) {
        തിരികെ ppoll_usec_full(fds, n_fds, timeout, ശൂന്യ);
}

പൂർണ്ണസംഖ്യ fd_wait_for_event(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ event, usec_t timeout);

ssize_t sparse_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *p, size_t sz, size_t run_length);

സ്ഥിതം വരിയിൽ bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        തിരികെ (l >> 63) == 0;
}

സ്ഥിതം വരിയിൽ bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        എങ്കിൽ (l == UINT64_MAX)
                തിരികെ true;

        തിരികെ FILE_SIZE_VALID(l);
}
