/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ہدایت once

#ضمن "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_منزل_epoll(uint32_t events);
uint32_t epoll_events_منزل_poll(uint32_t events);

صحيح flush_fd(صحيح fd);

ssize_t loop_read(صحيح fd, فراغ *buf, size_t nbytes, bool do_poll);
صحيح loop_read_exact(صحيح fd, فراغ *buf, size_t nbytes, bool do_poll);

صحيح loop_write_بھریا(صحيح fd, ثابت فراغ *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
ساكن مضمن صحيح loop_write(صحيح fd, ثابت فراغ *buf, size_t nbytes) {
        أرجع loop_write_بھریا(fd, buf, nbytes, 0);
}

صحيح pwritev_بھریا(صحيح fd, بنية iovec *iovec, size_t n, uint64_t وتھ, uint64_t *ret_written);
صحيح pwrite_بھریا(صحيح fd, ثابت فراغ *buf, size_t nbytes, uint64_t وتھ) _nonnull_if_nonzero_(2, 3);

صحيح pipe_eof(صحيح fd);

صحيح ppoll_usec_بھریا(بنية pollfd *fds, size_t n_fds, usec_t timeout, ثابت sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) ساكن مضمن صحيح ppoll_usec(بنية pollfd *fds, size_t n_fds, usec_t timeout) {
        أرجع ppoll_usec_بھریا(fds, n_fds, timeout, عدم);
}

صحيح fd_wait_for_event(صحيح fd, صحيح event, usec_t timeout);

ssize_t sparse_write(صحيح fd, ثابت فراغ *p, size_t sz, size_t لگاتار_لمائی);

ساكن مضمن bool FILE_ناپ_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        أرجع (l >> 63) == 0;
}

ساكن مضمن bool FILE_ناپ_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        إذا (l == UINT64_MAX)
                أرجع true;

        أرجع FILE_ناپ_VALID(l);
}
