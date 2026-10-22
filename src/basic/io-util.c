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
        ཕྱིར (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        ཕྱིར (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

ཧྲིལ flush_fd(ཧྲིལ fd) {
        ཧྲིལ count = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        ཆེད་དུ (;;) {
                var_na buf[LINE_MAX];
                ssize_t l;
                ཧྲིལ r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                གལ་ཏེ (r == -EINTR)
                        continue;
                གལ་ཏེ (r < 0)
                        ཕྱིར r;
                གལ་ཏེ (r == 0)
                        ཕྱིར count;

                l = read(fd, buf, parimaa_na(buf));
                གལ་ཏེ (l < 0) {
                        གལ་ཏེ (errno == EINTR)
                                continue;
                        གལ་ཏེ (errno == EAGAIN)
                                ཕྱིར count;

                        ཕྱིར -errno;
                } དེ་མིན གལ་ཏེ (l == 0)
                        ཕྱིར count;

                གལ་ཏེ (l > INT_MAX-count) /* On overflow terminate */
                        ཕྱིར INT_MAX;

                count += (ཧྲིལ) l;
        }
}

ssize_t loop_read(ཧྲིལ fd, void *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        assert(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        གལ་ཏེ (nbytes > (size_t) SSIZE_MAX)
                ཕྱིར -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        ཧྲིལ flags = 0;
        གལ་ཏེ (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                གལ་ཏེ (flags < 0)
                        ཕྱིར -errno;
        }

        do {
                ssize_t k;

                གལ་ཏེ (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        གལ་ཏེ (k < 0)
                                ཕྱིར n > 0 ? n : k;
                } དེ་མིན {
                        k = read(fd, p, nbytes);
                        གལ་ཏེ (k < 0) {
                                གལ་ཏེ (errno == EINTR)
                                        continue;

                                གལ་ཏེ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (void) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        continue;
                                }

                                ཕྱིར n > 0 ? n : -errno;
                        }
                }

                གལ་ཏེ (k == 0)
                        ཕྱིར n;

                assert((size_t) k <= nbytes);
                assert(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } སྐབས (nbytes > 0);

        ཕྱིར n;
}

ཧྲིལ loop_read_exact(ཧྲིལ fd, void *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        གལ་ཏེ (n < 0)
                ཕྱིར (ཧྲིལ) n;
        གལ་ཏེ ((size_t) n != nbytes)
                ཕྱིར -EIO;

        ཕྱིར 0;
}

ཧྲིལ loop_write_full(ཧྲིལ fd, const void *buf, size_t nbytes, usec_t timeout) {
        const uint8_t *p;
        usec_t end;
        ཧྲིལ r;

        assert(fd >= 0);
        assert(buf || nbytes == 0);

        གལ་ཏེ (nbytes == 0) {
                static const dummy_t dummy[0];
                assert_cc(parimaa_na(dummy) == 0);
                p = (const void*) dummy; /* Some valid pointer, in case NULL was specified */
        } དེ་མིན {
                གལ་ཏེ (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                དེ་མིན གལ་ཏེ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ཕྱིར -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        ཧྲིལ flags = 0;
        གལ་ཏེ (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                གལ་ཏེ (flags < 0)
                        ཕྱིར -errno;
        }

        གལ་ཏེ (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                སྐབས (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        གལ་ཏེ (k < 0)
                                ཕྱིར (ཧྲིལ) k;
                        གལ་ཏེ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ཕྱིར -EIO;

                        assert((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ཕྱིར 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        do {
                ssize_t k;

                k = write(fd, p, nbytes);
                གལ་ཏེ (k < 0) {
                        གལ་ཏེ (errno == EINTR)
                                continue;

                        གལ་ཏེ (errno != EAGAIN || timeout == 0)
                                ཕྱིར -errno;

                        usec_t wait_for;

                        གལ་ཏེ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        དེ་མིན {
                                usec_t t = now(CLOCK_MONOTONIC);
                                གལ་ཏེ (t >= end)
                                        ཕྱིར -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        གལ་ཏེ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                continue;
                        གལ་ཏེ (r < 0)
                                ཕྱིར r;
                        གལ་ཏེ (r == 0)
                                ཕྱིར -ETIME;
                        continue;
                }

                གལ་ཏེ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ཕྱིར -EIO;

                assert((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } སྐབས (nbytes > 0);

        ཕྱིར 0;
}

ཧྲིལ pwritev_full(ཧྲིལ fd, sa_nracanaa iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        ཧྲིལ r = 0;

        assert(fd >= 0);
        assert(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        ཆེད་དུ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                སྐབས (i < n && !iovec_is_set(iovec + i))
                        i++;
                གལ་ཏེ (i >= n)
                        bha_nga;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                གལ་ཏེ (k < 0) {
                        གལ་ཏེ (errno == EINTR)
                                continue;

                        r = -errno;
                        bha_nga;
                }
                གལ་ཏེ (k == 0) {
                        r = -EIO;
                        bha_nga;
                }

                written += k;
                (void) iovec_inc_many(iovec + i, m, k);
        }

        གལ་ཏེ (ret_written)
                *ret_written = written;
        ཕྱིར r;
}

ཧྲིལ pwrite_full(ཧྲིལ fd, const void *buf, size_t nbytes, uint64_t offset) {
        assert(fd >= 0);
        assert(buf || nbytes == 0);

        ཕྱིར pwritev_full(fd, &IOVEC_MAKE((void*) buf, nbytes), 1, offset, /* ret_written= */ NULL);
}

ཧྲིལ pipe_eof(ཧྲིལ fd) {
        ཧྲིལ r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        གལ་ཏེ (r <= 0)
                ཕྱིར r;

        ཕྱིར !!(r & POLLHUP);
}

ཧྲིལ ppoll_usec_full(sa_nracanaa pollfd *fds, size_t n_fds, usec_t timeout, const sigset_t *ss) {
        ཧྲིལ r;

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

        གལ་ཏེ (n_fds == 0 && timeout == 0)
                ཕྱིར 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? NULL : TIMESPEC_STORE(timeout), ss);
        གལ་ཏེ (r <= 0)
                ཕྱིར r;

        ཆེད་དུ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                གལ་ཏེ (fds[i].revents == 0)
                        continue;
                གལ་ཏེ (fds[i].revents & POLLNVAL)
                        ཕྱིར -EBADF;
                n--;
        }

        ཕྱིར r;
}

ཧྲིལ fd_wait_for_event(ཧྲིལ fd, ཧྲིལ event, usec_t timeout) {
        sa_nracanaa pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        ཧྲིལ r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        གལ་ཏེ (r <= 0)
                ཕྱིར r;

        ཕྱིར pollfd.revents;
}

static size_t nul_length(const uint8_t *p, size_t sz) {
        size_t n = 0;

        assert(p);

        སྐབས (sz > 0) {
                གལ་ཏེ (*p != 0)
                        bha_nga;

                n++;
                p++;
                sz--;
        }

        ཕྱིར n;
}

ssize_t sparse_write(ཧྲིལ fd, const void *p, size_t sz, size_t run_length) {
        const uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        སྐབས (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                གལ་ཏེ ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        གལ་ཏེ (q > w) {
                                l = write(fd, w, q - w);
                                གལ་ཏེ (l < 0)
                                        ཕྱིར -errno;
                                གལ་ཏེ (l != q -w)
                                        ཕྱིར -EIO;
                        }

                        གལ་ཏེ (lseek(fd, n, SEEK_CUR) < 0)
                                ཕྱིར -errno;

                        q += n;
                        w = q;
                } དེ་མིན གལ་ཏེ (n > 0)
                        q += n;
                དེ་མིན
                        q++;
        }

        གལ་ཏེ (q > w) {
                l = write(fd, w, q - w);
                གལ་ཏེ (l < 0)
                        ཕྱིར -errno;
                གལ་ཏེ (l != q - w)
                        ཕྱིར -EIO;
        }

        ཕྱིར q - (const uint8_t*) p;
}
