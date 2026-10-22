/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#ଅନ୍ତର୍ଭାଵ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flush_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ssize_t loop_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t nbytes, bool do_poll);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_read_exact(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t nbytes, bool do_poll);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_write_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t nbytes) {
        ଫେରନ୍ତୁ loop_write_full(fd, buf, nbytes, 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pwritev_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା pwrite_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pipe_eof(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd);

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ppoll_usec_full(ସଂରଚନା pollfd *fds, size_t n_fds, usec_t timeout, ଅଚଳ sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ସ୍ଥିର ଧାଡ଼ିଭିତରେ ପୂର୍ଣ୍ଣସଂଖ୍ୟା ppoll_usec(ସଂରଚନା pollfd *fds, size_t n_fds, usec_t timeout) {
        ଫେରନ୍ତୁ ppoll_usec_full(fds, n_fds, timeout, ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_wait_for_event(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, usec_t timeout);

ssize_t sparse_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *p, size_t sz, size_t run_length);

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ଫେରନ୍ତୁ (l >> 63) == 0;
}

ସ୍ଥିର ଧାଡ଼ିଭିତରେ bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        ଯଦି (l == UINT64_MAX)
                ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ FILE_SIZE_VALID(l);
}
