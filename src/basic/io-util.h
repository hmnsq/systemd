/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_to_epoll(uint32_t events);
uint32_t epoll_events_to_poll(uint32_t events);

පූර්ණ_සංඛ්‍යා flush_fd(පූර්ණ_සංඛ්‍යා fd);

ssize_t loop_read(පූර්ණ_සංඛ්‍යා fd, void *buf, size_t nbytes, bool do_poll);
පූර්ණ_සංඛ්‍යා loop_read_exact(පූර්ණ_සංඛ්‍යා fd, void *buf, size_t nbytes, bool do_poll);

පූර්ණ_සංඛ්‍යා loop_write_full(පූර්ණ_සංඛ්‍යා fd, const void *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
static anta_hstha පූර්ණ_සංඛ්‍යා loop_write(පූර්ණ_සංඛ්‍යා fd, const void *buf, size_t nbytes) {
        ආපසු loop_write_full(fd, buf, nbytes, 0);
}

පූර්ණ_සංඛ්‍යා pwritev_full(පූර්ණ_සංඛ්‍යා fd, sa_nracanaa iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written);
පූර්ණ_සංඛ්‍යා pwrite_full(පූර්ණ_සංඛ්‍යා fd, const void *buf, size_t nbytes, uint64_t offset) _nonnull_if_nonzero_(2, 3);

පූර්ණ_සංඛ්‍යා pipe_eof(පූර්ණ_සංඛ්‍යා fd);

පූර්ණ_සංඛ්‍යා ppoll_usec_full(sa_nracanaa pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) static anta_hstha පූර්ණ_සංඛ්‍යා ppoll_usec(sa_nracanaa pollfd *fds, size_t n_fds, usec_t timeout) {
        ආපසු ppoll_usec_full(fds, n_fds, timeout, NULL);
}

පූර්ණ_සංඛ්‍යා fd_wait_for_event(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා event, usec_t timeout);

ssize_t sparse_write(පූර්ණ_සංඛ්‍යා fd, const void *p, size_t sz, size_t run_length);

static anta_hstha bool FILE_SIZE_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        ආපසු (l >> 63) == 0;
}

static anta_hstha bool FILE_SIZE_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        නම් (l == UINT64_MAX)
                ආපසු true;

        ආපසු FILE_SIZE_VALID(l);
}
