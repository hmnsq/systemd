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
        вернуть (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        вернуть (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

целое flush_fd(целое fd) {
        целое count = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        для (;;) {
                char buf[LINE_MAX];
                ssize_t l;
                целое r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                если (r == -EINTR)
                        continue;
                если (r < 0)
                        вернуть r;
                если (r == 0)
                        вернуть count;

                l = read(fd, buf, sizeof(buf));
                если (l < 0) {
                        если (errno == EINTR)
                                continue;
                        если (errno == EAGAIN)
                                вернуть count;

                        вернуть -errno;
                } иначе если (l == 0)
                        вернуть count;

                если (l > INT_MAX-count) /* On overflow terminate */
                        вернуть INT_MAX;

                count += (целое) l;
        }
}

ssize_t loop_read(целое fd, void *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        assert(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        если (nbytes > (size_t) SSIZE_MAX)
                вернуть -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        целое flags = 0;
        если (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                если (flags < 0)
                        вернуть -errno;
        }

        do {
                ssize_t k;

                если (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        если (k < 0)
                                вернуть n > 0 ? n : k;
                } иначе {
                        k = read(fd, p, nbytes);
                        если (k < 0) {
                                если (errno == EINTR)
                                        continue;

                                если (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (void) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        continue;
                                }

                                вернуть n > 0 ? n : -errno;
                        }
                }

                если (k == 0)
                        вернуть n;

                assert((size_t) k <= nbytes);
                assert(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } пока (nbytes > 0);

        вернуть n;
}

целое loop_read_exact(целое fd, void *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        если (n < 0)
                вернуть (целое) n;
        если ((size_t) n != nbytes)
                вернуть -EIO;

        вернуть 0;
}

целое loop_write_full(целое fd, const void *buf, size_t nbytes, usec_t timeout) {
        const uint8_t *p;
        usec_t end;
        целое r;

        assert(fd >= 0);
        assert(buf || nbytes == 0);

        если (nbytes == 0) {
                static const dummy_t dummy[0];
                assert_cc(sizeof(dummy) == 0);
                p = (const void*) dummy; /* Some valid pointer, in case NULL was specified */
        } иначе {
                если (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                иначе если (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        вернуть -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        целое flags = 0;
        если (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                если (flags < 0)
                        вернуть -errno;
        }

        если (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                пока (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        если (k < 0)
                                вернуть (целое) k;
                        если (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                вернуть -EIO;

                        assert((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                вернуть 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        do {
                ssize_t k;

                k = write(fd, p, nbytes);
                если (k < 0) {
                        если (errno == EINTR)
                                continue;

                        если (errno != EAGAIN || timeout == 0)
                                вернуть -errno;

                        usec_t wait_for;

                        если (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        иначе {
                                usec_t t = now(CLOCK_MONOTONIC);
                                если (t >= end)
                                        вернуть -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        если (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                continue;
                        если (r < 0)
                                вернуть r;
                        если (r == 0)
                                вернуть -ETIME;
                        continue;
                }

                если (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        вернуть -EIO;

                assert((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } пока (nbytes > 0);

        вернуть 0;
}

целое pwritev_full(целое fd, struct iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        целое r = 0;

        assert(fd >= 0);
        assert(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        для (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                пока (i < n && !iovec_is_set(iovec + i))
                        i++;
                если (i >= n)
                        break;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                если (k < 0) {
                        если (errno == EINTR)
                                continue;

                        r = -errno;
                        break;
                }
                если (k == 0) {
                        r = -EIO;
                        break;
                }

                written += k;
                (void) iovec_inc_many(iovec + i, m, k);
        }

        если (ret_written)
                *ret_written = written;
        вернуть r;
}

целое pwrite_full(целое fd, const void *buf, size_t nbytes, uint64_t offset) {
        assert(fd >= 0);
        assert(buf || nbytes == 0);

        вернуть pwritev_full(fd, &IOVEC_MAKE((void*) buf, nbytes), 1, offset, /* ret_written= */ NULL);
}

целое pipe_eof(целое fd) {
        целое r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        если (r <= 0)
                вернуть r;

        вернуть !!(r & POLLHUP);
}

целое ppoll_usec_full(struct pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) {
        целое r;

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

        если (n_fds == 0 && timeout == 0)
                вернуть 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? NULL : TIMESPEC_STORE(timeout), ss);
        если (r <= 0)
                вернуть r;

        для (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                если (fds[i].revents == 0)
                        continue;
                если (fds[i].revents & POLLNVAL)
                        вернуть -EBADF;
                n--;
        }

        вернуть r;
}

целое fd_wait_for_event(целое fd, целое event, usec_t timeout) {
        struct pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        целое r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        если (r <= 0)
                вернуть r;

        вернуть pollfd.revents;
}

static size_t nul_length(const uint8_t *p, size_t sz) {
        size_t n = 0;

        assert(p);

        пока (sz > 0) {
                если (*p != 0)
                        break;

                n++;
                p++;
                sz--;
        }

        вернуть n;
}

ssize_t sparse_write(целое fd, const void *p, size_t sz, size_t run_length) {
        const uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        пока (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                если ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        если (q > w) {
                                l = write(fd, w, q - w);
                                если (l < 0)
                                        вернуть -errno;
                                если (l != q -w)
                                        вернуть -EIO;
                        }

                        если (lseek(fd, n, SEEK_CUR) < 0)
                                вернуть -errno;

                        q += n;
                        w = q;
                } иначе если (n > 0)
                        q += n;
                иначе
                        q++;
        }

        если (q > w) {
                l = write(fd, w, q - w);
                если (l < 0)
                        вернуть -errno;
                если (l != q - w)
                        вернуть -EIO;
        }

        вернуть q - (const uint8_t*) p;
}
