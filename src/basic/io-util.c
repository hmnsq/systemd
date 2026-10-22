/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fcntl.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>          /* IWYU pragma: keep */
#include <time.h>
#include <unistd.h>

#include "errno-util.h"
#include "fiber-ops.h"
#include "io-util.h"
#include "iovec-util.h"
#include "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        คืน (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        คืน (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

จำนวนเต็ม flush_fd(จำนวนเต็ม fd) {
        จำนวนเต็ม count = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        สำหรับ (;;) {
                var_na buf[LINE_MAX];
                ssize_t l;
                จำนวนเต็ม r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ถ้า (r == -EINTR)
                        continue;
                ถ้า (r < 0)
                        คืน r;
                ถ้า (r == 0)
                        คืน count;

                l = read(fd, buf, parimaa_na(buf));
                ถ้า (l < 0) {
                        ถ้า (errno == EINTR)
                                continue;
                        ถ้า (errno == EAGAIN)
                                คืน count;

                        คืน -errno;
                } มิฉะนั้น ถ้า (l == 0)
                        คืน count;

                ถ้า (l > INT_MAX-count) /* On overflow terminate */
                        คืน INT_MAX;

                count += (จำนวนเต็ม) l;
        }
}

ssize_t loop_read(จำนวนเต็ม fd, void *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        assert(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ถ้า (nbytes > (size_t) SSIZE_MAX)
                คืน -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        จำนวนเต็ม flags = 0;
        ถ้า (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ถ้า (flags < 0)
                        คืน -errno;
        }

        do {
                ssize_t k;

                ถ้า (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ถ้า (k < 0)
                                คืน n > 0 ? n : k;
                } มิฉะนั้น {
                        k = read(fd, p, nbytes);
                        ถ้า (k < 0) {
                                ถ้า (errno == EINTR)
                                        continue;

                                ถ้า (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (void) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        continue;
                                }

                                คืน n > 0 ? n : -errno;
                        }
                }

                ถ้า (k == 0)
                        คืน n;

                assert((size_t) k <= nbytes);
                assert(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ขณะ (nbytes > 0);

        คืน n;
}

จำนวนเต็ม loop_read_exact(จำนวนเต็ม fd, void *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ถ้า (n < 0)
                คืน (จำนวนเต็ม) n;
        ถ้า ((size_t) n != nbytes)
                คืน -EIO;

        คืน 0;
}

จำนวนเต็ม loop_write_full(จำนวนเต็ม fd, const void *buf, size_t nbytes, usec_t timeout) {
        const uint8_t *p;
        usec_t end;
        จำนวนเต็ม r;

        assert(fd >= 0);
        assert(buf || nbytes == 0);

        ถ้า (nbytes == 0) {
                static const dummy_t dummy[0];
                assert_cc(parimaa_na(dummy) == 0);
                p = (const void*) dummy; /* Some valid pointer, in case NULL was specified */
        } มิฉะนั้น {
                ถ้า (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                มิฉะนั้น ถ้า (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        คืน -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        จำนวนเต็ม flags = 0;
        ถ้า (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ถ้า (flags < 0)
                        คืน -errno;
        }

        ถ้า (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                ขณะ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ถ้า (k < 0)
                                คืน (จำนวนเต็ม) k;
                        ถ้า (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                คืน -EIO;

                        assert((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                คืน 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        do {
                ssize_t k;

                k = write(fd, p, nbytes);
                ถ้า (k < 0) {
                        ถ้า (errno == EINTR)
                                continue;

                        ถ้า (errno != EAGAIN || timeout == 0)
                                คืน -errno;

                        usec_t wait_for;

                        ถ้า (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        มิฉะนั้น {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ถ้า (t >= end)
                                        คืน -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ถ้า (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                continue;
                        ถ้า (r < 0)
                                คืน r;
                        ถ้า (r == 0)
                                คืน -ETIME;
                        continue;
                }

                ถ้า (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        คืน -EIO;

                assert((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ขณะ (nbytes > 0);

        คืน 0;
}

จำนวนเต็ม pwritev_full(จำนวนเต็ม fd, sa_nracanaa iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        จำนวนเต็ม r = 0;

        assert(fd >= 0);
        assert(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        สำหรับ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ขณะ (i < n && !iovec_is_set(iovec + i))
                        i++;
                ถ้า (i >= n)
                        bha_nga;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                ถ้า (k < 0) {
                        ถ้า (errno == EINTR)
                                continue;

                        r = -errno;
                        bha_nga;
                }
                ถ้า (k == 0) {
                        r = -EIO;
                        bha_nga;
                }

                written += k;
                (void) iovec_inc_many(iovec + i, m, k);
        }

        ถ้า (ret_written)
                *ret_written = written;
        คืน r;
}

จำนวนเต็ม pwrite_full(จำนวนเต็ม fd, const void *buf, size_t nbytes, uint64_t offset) {
        assert(fd >= 0);
        assert(buf || nbytes == 0);

        คืน pwritev_full(fd, &IOVEC_MAKE((void*) buf, nbytes), 1, offset, /* ret_written= */ NULL);
}

จำนวนเต็ม pipe_eof(จำนวนเต็ม fd) {
        จำนวนเต็ม r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ถ้า (r <= 0)
                คืน r;

        คืน !!(r & POLLHUP);
}

จำนวนเต็ม ppoll_usec_full(sa_nracanaa pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) {
        จำนวนเต็ม r;

        assert(fds || n_fds == 0);

        /* This is a wrapper around ppoll() that does primarily two things:
         *
         *  ✅ Takes a usec_t instead of a struct timespec
         *
         *  ✅ Guarantees that if an invalid fd is specified we return EBADF (i.e. converts POLLNVAL to
         *     EBADF). This is done because EBADF is a programming error usually, and hence should bubble up
         *     as error, and not be eaten up as non-error POLLNVAL event.
         *
         *  ⚠️ ⚠️ ⚠️ Note that this function does not add any special handling for EINTR. Don't forget
         *  poll()/ppoll() will return with EINTR on any received signal always, there is no automatic
         *  restarting via SA_RESTART available. Thus, typically you want to handle EINTR not as an error,
         *  but just as reason to restart things, under the assumption you use a more appropriate mechanism
         *  to handle signals, such as signalfd() or signal handlers. ⚠️ ⚠️ ⚠️
         */

        ถ้า (n_fds == 0 && timeout == 0)
                คืน 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? NULL : TIMESPEC_STORE(timeout), ss);
        ถ้า (r <= 0)
                คืน r;

        สำหรับ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ถ้า (fds[i].revents == 0)
                        continue;
                ถ้า (fds[i].revents & POLLNVAL)
                        คืน -EBADF;
                n--;
        }

        คืน r;
}

จำนวนเต็ม fd_wait_for_event(จำนวนเต็ม fd, จำนวนเต็ม event, usec_t timeout) {
        sa_nracanaa pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        จำนวนเต็ม r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ถ้า (r <= 0)
                คืน r;

        คืน pollfd.revents;
}

static size_t nul_length(const uint8_t *p, size_t sz) {
        size_t n = 0;

        assert(p);

        ขณะ (sz > 0) {
                ถ้า (*p != 0)
                        bha_nga;

                n++;
                p++;
                sz--;
        }

        คืน n;
}

ssize_t sparse_write(จำนวนเต็ม fd, const void *p, size_t sz, size_t run_length) {
        const uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ขณะ (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ถ้า ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ถ้า (q > w) {
                                l = write(fd, w, q - w);
                                ถ้า (l < 0)
                                        คืน -errno;
                                ถ้า (l != q -w)
                                        คืน -EIO;
                        }

                        ถ้า (lseek(fd, n, SEEK_CUR) < 0)
                                คืน -errno;

                        q += n;
                        w = q;
                } มิฉะนั้น ถ้า (n > 0)
                        q += n;
                มิฉะนั้น
                        q++;
        }

        ถ้า (q > w) {
                l = write(fd, w, q - w);
                ถ้า (l < 0)
                        คืน -errno;
                ถ้า (l != q - w)
                        คืน -EIO;
        }

        คืน q - (const uint8_t*) p;
}
