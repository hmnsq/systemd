/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#અન્તર્ભાવ "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

પૂર્ણાંક flush_fd(પૂર્ણાંક fd);

ssize_t loop_read(પૂર્ણાંક fd, શૂન્ય *buf, size_t nbytes, bool do_poll);
પૂર્ણાંક loop_read_exact(પૂર્ણાંક fd, શૂન્ય *buf, size_t nbytes, bool do_poll);

પૂર્ણાંક loop_write_full(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
સ્થિર અંતઃરેખ પૂર્ણાંક loop_write(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t nbytes) {
        પાછા loop_write_full(fd, buf, nbytes, 0);
}

પૂર્ણાંક pwritev_full(પૂર્ણાંક fd, સંરચના iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
પૂર્ણાંક pwrite_full(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

પૂર્ણાંક pipe_eof(પૂર્ણાંક fd);

પૂર્ણાંક ppoll_usec_full(સંરચના pollfd *fds, size_t n_fds, usec_t timeout, અચલ sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) સ્થિર અંતઃરેખ પૂર્ણાંક ppoll_usec(સંરચના pollfd *fds, size_t n_fds, usec_t timeout) {
        પાછા ppoll_usec_full(fds, n_fds, timeout, ખાલી);
}

પૂર્ણાંક fd_wait_for_event(પૂર્ણાંક fd, પૂર્ણાંક event, usec_t timeout);

ssize_t sparse_write(પૂર્ણાંક fd, અચલ શૂન્ય *p, size_t sz, size_t run_length);

સ્થિર અંતઃરેખ bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        પાછા (l >> 63) == 0;
}

સ્થિર અંતઃરેખ bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        જો (l == UINT64_MAX)
                પાછા true;

        પાછા FILE_SIZE_VALID(l);
}
