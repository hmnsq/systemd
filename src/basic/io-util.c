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
        trả_về (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        trả_về (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

số_nguyên flush_fd(số_nguyên fd) {
        số_nguyên count = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        cho (;;) {
                char buf[LINE_MAX];
                ssize_t l;
                số_nguyên r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                nếu (r == -EINTR)
                        continue;
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        trả_về count;

                l = read(fd, buf, sizeof(buf));
                nếu (l < 0) {
                        nếu (errno == EINTR)
                                continue;
                        nếu (errno == EAGAIN)
                                trả_về count;

                        trả_về -errno;
                } không_thì nếu (l == 0)
                        trả_về count;

                nếu (l > INT_MAX-count) /* On overflow terminate */
                        trả_về INT_MAX;

                count += (số_nguyên) l;
        }
}

ssize_t loop_read(số_nguyên fd, void *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        assert(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        nếu (nbytes > (size_t) SSIZE_MAX)
                trả_về -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        số_nguyên flags = 0;
        nếu (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                nếu (flags < 0)
                        trả_về -errno;
        }

        do {
                ssize_t k;

                nếu (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        nếu (k < 0)
                                trả_về n > 0 ? n : k;
                } không_thì {
                        k = read(fd, p, nbytes);
                        nếu (k < 0) {
                                nếu (errno == EINTR)
                                        continue;

                                nếu (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (void) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        continue;
                                }

                                trả_về n > 0 ? n : -errno;
                        }
                }

                nếu (k == 0)
                        trả_về n;

                assert((size_t) k <= nbytes);
                assert(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } trong_khi (nbytes > 0);

        trả_về n;
}

số_nguyên loop_read_exact(số_nguyên fd, void *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        nếu (n < 0)
                trả_về (số_nguyên) n;
        nếu ((size_t) n != nbytes)
                trả_về -EIO;

        trả_về 0;
}

số_nguyên loop_write_full(số_nguyên fd, const void *buf, size_t nbytes, usec_t timeout) {
        const uint8_t *p;
        usec_t end;
        số_nguyên r;

        assert(fd >= 0);
        assert(buf || nbytes == 0);

        nếu (nbytes == 0) {
                static const dummy_t dummy[0];
                assert_cc(sizeof(dummy) == 0);
                p = (const void*) dummy; /* Some valid pointer, in case NULL was specified */
        } không_thì {
                nếu (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                không_thì nếu (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        trả_về -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        số_nguyên flags = 0;
        nếu (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                nếu (flags < 0)
                        trả_về -errno;
        }

        nếu (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                trong_khi (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        nếu (k < 0)
                                trả_về (số_nguyên) k;
                        nếu (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                trả_về -EIO;

                        assert((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                trả_về 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        do {
                ssize_t k;

                k = write(fd, p, nbytes);
                nếu (k < 0) {
                        nếu (errno == EINTR)
                                continue;

                        nếu (errno != EAGAIN || timeout == 0)
                                trả_về -errno;

                        usec_t wait_for;

                        nếu (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        không_thì {
                                usec_t t = now(CLOCK_MONOTONIC);
                                nếu (t >= end)
                                        trả_về -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        nếu (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                continue;
                        nếu (r < 0)
                                trả_về r;
                        nếu (r == 0)
                                trả_về -ETIME;
                        continue;
                }

                nếu (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        trả_về -EIO;

                assert((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } trong_khi (nbytes > 0);

        trả_về 0;
}

số_nguyên pwritev_full(số_nguyên fd, struct iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        số_nguyên r = 0;

        assert(fd >= 0);
        assert(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        cho (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                trong_khi (i < n && !iovec_is_set(iovec + i))
                        i++;
                nếu (i >= n)
                        break;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                nếu (k < 0) {
                        nếu (errno == EINTR)
                                continue;

                        r = -errno;
                        break;
                }
                nếu (k == 0) {
                        r = -EIO;
                        break;
                }

                written += k;
                (void) iovec_inc_many(iovec + i, m, k);
        }

        nếu (ret_written)
                *ret_written = written;
        trả_về r;
}

số_nguyên pwrite_full(số_nguyên fd, const void *buf, size_t nbytes, uint64_t offset) {
        assert(fd >= 0);
        assert(buf || nbytes == 0);

        trả_về pwritev_full(fd, &IOVEC_MAKE((void*) buf, nbytes), 1, offset, /* ret_written= */ NULL);
}

số_nguyên pipe_eof(số_nguyên fd) {
        số_nguyên r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        nếu (r <= 0)
                trả_về r;

        trả_về !!(r & POLLHUP);
}

số_nguyên ppoll_usec_full(struct pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) {
        số_nguyên r;

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

        nếu (n_fds == 0 && timeout == 0)
                trả_về 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? NULL : TIMESPEC_STORE(timeout), ss);
        nếu (r <= 0)
                trả_về r;

        cho (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                nếu (fds[i].revents == 0)
                        continue;
                nếu (fds[i].revents & POLLNVAL)
                        trả_về -EBADF;
                n--;
        }

        trả_về r;
}

số_nguyên fd_wait_for_event(số_nguyên fd, số_nguyên event, usec_t timeout) {
        struct pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        số_nguyên r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        nếu (r <= 0)
                trả_về r;

        trả_về pollfd.revents;
}

static size_t nul_length(const uint8_t *p, size_t sz) {
        size_t n = 0;

        assert(p);

        trong_khi (sz > 0) {
                nếu (*p != 0)
                        break;

                n++;
                p++;
                sz--;
        }

        trả_về n;
}

ssize_t sparse_write(số_nguyên fd, const void *p, size_t sz, size_t run_length) {
        const uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        trong_khi (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                nếu ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        nếu (q > w) {
                                l = write(fd, w, q - w);
                                nếu (l < 0)
                                        trả_về -errno;
                                nếu (l != q -w)
                                        trả_về -EIO;
                        }

                        nếu (lseek(fd, n, SEEK_CUR) < 0)
                                trả_về -errno;

                        q += n;
                        w = q;
                } không_thì nếu (n > 0)
                        q += n;
                không_thì
                        q++;
        }

        nếu (q > w) {
                l = write(fd, w, q - w);
                nếu (l < 0)
                        trả_về -errno;
                nếu (l != q - w)
                        trả_về -EIO;
        }

        trả_về q - (const uint8_t*) p;
}
