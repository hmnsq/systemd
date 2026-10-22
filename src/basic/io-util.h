/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव "forward.h"

/* On most architectures POLL* and EPOLL* share numeric values, but sparc allocates POLLRDHUP at
 * 0x800 while EPOLLRDHUP is 0x2000 everywhere. Always go through these helpers when crossing
 * between poll() and epoll() event masks. Bits outside the mapped set (EPOLLET, EPOLLONESHOT,
 * POLLNVAL, …) are dropped. */
uint32_t poll_events_गन्तव्यम्_epoll(uint32_t events);
uint32_t epoll_events_गन्तव्यम्_poll(uint32_t events);

पूर्णाङ्क flush_fd(पूर्णाङ्क fd);

ssize_t loop_read(पूर्णाङ्क fd, शून्य *buf, size_t nbytes, bool do_poll);
पूर्णाङ्क loop_read_exact(पूर्णाङ्क fd, शून्य *buf, size_t nbytes, bool do_poll);

पूर्णाङ्क loop_write_पूर्णम्(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t nbytes, usec_t timeout) _nonnull_if_nonzero_(2, 3);
स्थिर अन्तःस्थ पूर्णाङ्क loop_write(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t nbytes) {
        प्रत्यागम loop_write_पूर्णम्(fd, buf, nbytes, 0);
}

पूर्णाङ्क pwritev_पूर्णम्(पूर्णाङ्क fd, संरचना iovec *iovec, size_t n, uint64_t विस्थापनम्, uint64_t *ret_written);
पूर्णाङ्क pwrite_पूर्णम्(पूर्णाङ्क fd, ध्रुव शून्य *buf, size_t nbytes, uint64_t विस्थापनम्) _nonnull_if_nonzero_(2, 3);

पूर्णाङ्क pipe_eof(पूर्णाङ्क fd);

पूर्णाङ्क ppoll_usec_पूर्णम्(संरचना pollfd *fds, size_t n_fds, usec_t timeout, ध्रुव sigset_t *ss) _nonnull_if_nonzero_(1, 2);
_nonnull_if_nonzero_(1, 2) स्थिर अन्तःस्थ पूर्णाङ्क ppoll_usec(संरचना pollfd *fds, size_t n_fds, usec_t timeout) {
        प्रत्यागम ppoll_usec_पूर्णम्(fds, n_fds, timeout, रिक्त);
}

पूर्णाङ्क fd_wait_for_event(पूर्णाङ्क fd, पूर्णाङ्क event, usec_t timeout);

ssize_t sparse_write(पूर्णाङ्क fd, ध्रुव शून्य *p, size_t sz, size_t सततम्_दीर्घता);

स्थिर अन्तःस्थ bool FILE_आकार_VALID(uint64_t l) {
        /* ftruncate() and friends take an unsigned file size, but actually cannot deal with file sizes larger than
         * 2^63 since the kernel internally handles it as signed value. This call allows checking for this early. */

        प्रत्यागम (l >> 63) == 0;
}

स्थिर अन्तःस्थ bool FILE_आकार_VALID_OR_INFINITY(uint64_t l) {

        /* Same as above, but allows one extra value: -1 as indication for infinity. */

        यदि (l == UINT64_MAX)
                प्रत्यागम true;

        प्रत्यागम FILE_आकार_VALID(l);
}
