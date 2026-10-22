/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_గమ్యం_epoll(uint32_t events);
uint32_t epoll_events_గమ్యం_poll(uint32_t events);

పూర్ణాంకం flush_fd(పూర్ణాంకం fd);

ssize_t loop_read(పూర్ణాంకం fd, శూన్య *buf, size_t nbytes, bool do_poll);
పూర్ణాంకం loop_read_exact(పూర్ణాంకం fd, శూన్య *buf, size_t nbytes, bool do_poll);

పూర్ణాంకం loop_write_నిండిన(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
స్థిర అంతఃవరుస పూర్ణాంకం loop_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t nbytes) {
        తిరిగి loop_write_నిండిన(fd, buf, nbytes, 0);
}

పూర్ణాంకం pwritev_నిండిన(పూర్ణాంకం fd, నిర్మాణం iovec *iovec, size_t n, uint64_t స్థానభ్రంశం, uint64_t *ret_written);
పూర్ణాంకం pwrite_నిండిన(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t nbytes, uint64_t స్థానభ్రంశం) _nonnull_if_nonzero_(2, 3);

పూర్ణాంకం pipe_eof(పూర్ణాంకం fd);

పూర్ణాంకం ppoll_usec_నిండిన(నిర్మాణం pollfd *fds, size_t n_fds, usec_t timeout, స్థిరరాశి sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) స్థిర అంతఃవరుస పూర్ణాంకం ppoll_usec(నిర్మాణం pollfd *fds, size_t n_fds, usec_t timeout) {
        తిరిగి ppoll_usec_నిండిన(fds, n_fds, timeout, ఖాళీ);
}

పూర్ణాంకం fd_wait_for_event(పూర్ణాంకం fd, పూర్ణాంకం event, usec_t timeout);

ssize_t sparse_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *p, size_t sz, size_t నిరంతరం_నిడివి);

స్థిర అంతఃవరుస bool FILE_కొలత_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        తిరిగి (l >> 63) == 0;
}

స్థిర అంతఃవరుస bool FILE_కొలత_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ఒకవేళ (l == UINT64_MAX)
                తిరిగి true;

        తిరిగి FILE_కొలత_VALID(l);
}
