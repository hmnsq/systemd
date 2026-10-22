/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <poll.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <sys/epoll.h>          /* IWYU pragma: keep */
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fiber-ops.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "iovec-util.h"
#ಅನ್ತರ್ಭಾವ "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        ಪ್ರತ್ಯಾಗಮ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        ಪ್ರತ್ಯಾಗಮ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

ಪೂರ್ಣಾಙ್ಕ flush_fd(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        ಪ್ರತಿ (;;) {
                ವರ್ಣ buf[LINE_MAX];
                ssize_t l;
                ಪೂರ್ಣಾಙ್ಕ r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ಯದಿ (r == -EINTR)
                        ಅನುವರ್ತ;
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
                ಯದಿ (r == 0)
                        ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;

                l = ಪಠ(fd, buf, ಪರಿಮಾಣ(buf));
                ಯದಿ (l < 0) {
                        ಯದಿ (errno == EINTR)
                                ಅನುವರ್ತ;
                        ಯದಿ (errno == EAGAIN)
                                ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;

                        ಪ್ರತ್ಯಾಗಮ -errno;
                } ಅನ್ಯಥಾ ಯದಿ (l == 0)
                        ಪ್ರತ್ಯಾಗಮ ಎಣಿಕೆ;

                ಯದಿ (l > INT_MAX-ಎಣಿಕೆ) /* On overflow terminate */
                        ಪ್ರತ್ಯಾಗಮ INT_MAX;

                ಎಣಿಕೆ += (ಪೂರ್ಣಾಙ್ಕ) l;
        }
}

ssize_t loop_read(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ಯದಿ (nbytes > (size_t) SSIZE_MAX)
                ಪ್ರತ್ಯಾಗಮ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        ಪೂರ್ಣಾಙ್ಕ flags = 0;
        ಯದಿ (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ಯದಿ (flags < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಕುರು {
                ssize_t k;

                ಯದಿ (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ಯದಿ (k < 0)
                                ಪ್ರತ್ಯಾಗಮ n > 0 ? n : k;
                } ಅನ್ಯಥಾ {
                        k = ಪಠ(fd, p, nbytes);
                        ಯದಿ (k < 0) {
                                ಯದಿ (errno == EINTR)
                                        ಅನುವರ್ತ;

                                ಯದಿ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (ಶೂನ್ಯ) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        ಅನುವರ್ತ;
                                }

                                ಪ್ರತ್ಯಾಗಮ n > 0 ? n : -errno;
                        }
                }

                ಯದಿ (k == 0)
                        ಪ್ರತ್ಯಾಗಮ n;

                ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);
                ಪ್ರತಿಜ್ಞಾ(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ಯಾವತ್ (nbytes > 0);

        ಪ್ರತ್ಯಾಗಮ n;
}

ಪೂರ್ಣಾಙ್ಕ loop_read_exact(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ಯದಿ (n < 0)
                ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) n;
        ಯದಿ ((size_t) n != nbytes)
                ಪ್ರತ್ಯಾಗಮ -EIO;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ loop_write_full(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t nbytes, usec_t timeout) {
        ಧ್ರುವ uint8_t *p;
        usec_t end;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(buf || nbytes == 0);

        ಯದಿ (nbytes == 0) {
                ಸ್ಥಿರ ಧ್ರುವ dummy_t dummy[0];
                assert_cc(ಪರಿಮಾಣ(dummy) == 0);
                p = (ಧ್ರುವ ಶೂನ್ಯ*) dummy; /* Some valid pointer, in case NULL was specified */
        } ಅನ್ಯಥಾ {
                ಯದಿ (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                ಅನ್ಯಥಾ ಯದಿ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ಪ್ರತ್ಯಾಗಮ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        ಪೂರ್ಣಾಙ್ಕ flags = 0;
        ಯದಿ (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ಯದಿ (flags < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
        }

        ಯದಿ (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                ಯಾವತ್ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ಯದಿ (k < 0)
                                ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ) k;
                        ಯದಿ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ಪ್ರತ್ಯಾಗಮ -EIO;

                        ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ಪ್ರತ್ಯಾಗಮ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        ಕುರು {
                ssize_t k;

                k = ಲಿಖ(fd, p, nbytes);
                ಯದಿ (k < 0) {
                        ಯದಿ (errno == EINTR)
                                ಅನುವರ್ತ;

                        ಯದಿ (errno != EAGAIN || timeout == 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        usec_t wait_for;

                        ಯದಿ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ಅನ್ಯಥಾ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ಯದಿ (t >= end)
                                        ಪ್ರತ್ಯಾಗಮ -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ಯದಿ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                ಅನುವರ್ತ;
                        ಯದಿ (r < 0)
                                ಪ್ರತ್ಯಾಗಮ r;
                        ಯದಿ (r == 0)
                                ಪ್ರತ್ಯಾಗಮ -ETIME;
                        ಅನುವರ್ತ;
                }

                ಯದಿ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ಪ್ರತ್ಯಾಗಮ -EIO;

                ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ಯಾವತ್ (nbytes > 0);

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ pwritev_full(ಪೂರ್ಣಾಙ್ಕ fd, ಸಂರಚನಾ iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        ಪೂರ್ಣಾಙ್ಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        ಪ್ರತಿ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ಯಾವತ್ (i < n && !iovec_is_set(iovec + i))
                        i++;
                ಯದಿ (i >= n)
                        ಭಙ್ಗ;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                ಯದಿ (k < 0) {
                        ಯದಿ (errno == EINTR)
                                ಅನುವರ್ತ;

                        r = -errno;
                        ಭಙ್ಗ;
                }
                ಯದಿ (k == 0) {
                        r = -EIO;
                        ಭಙ್ಗ;
                }

                written += k;
                (ಶೂನ್ಯ) iovec_inc_many(iovec + i, m, k);
        }

        ಯದಿ (ret_written)
                *ret_written = written;
        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ pwrite_full(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buf, size_t nbytes, uint64_t offset) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(buf || nbytes == 0);

        ಪ್ರತ್ಯಾಗಮ pwritev_full(fd, &IOVEC_MAKE((ಶೂನ್ಯ*) buf, nbytes), 1, offset, /* ret_written= */ ರಿಕ್ತ);
}

ಪೂರ್ಣಾಙ್ಕ pipe_eof(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ !!(r & POLLHUP);
}

ಪೂರ್ಣಾಙ್ಕ ppoll_usec_full(ಸಂರಚನಾ pollfd *fds, size_t n_fds, usec_t timeout, ಧ್ರುವ sigset_t *ss) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fds || n_fds == 0);

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

        ಯದಿ (n_fds == 0 && timeout == 0)
                ಪ್ರತ್ಯಾಗಮ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ರಿಕ್ತ : TIMESPEC_STORE(timeout), ss);
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತಿ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ಯದಿ (fds[i].revents == 0)
                        ಅನುವರ್ತ;
                ಯದಿ (fds[i].revents & POLLNVAL)
                        ಪ್ರತ್ಯಾಗಮ -EBADF;
                n--;
        }

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ fd_wait_for_event(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ event, usec_t timeout) {
        ಸಂರಚನಾ pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        ಪೂರ್ಣಾಙ್ಕ r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ಯದಿ (r <= 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ pollfd.revents;
}

ಸ್ಥಿರ size_t nul_length(ಧ್ರುವ uint8_t *p, size_t sz) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಯಾವತ್ (sz > 0) {
                ಯದಿ (*p != 0)
                        ಭಙ್ಗ;

                n++;
                p++;
                sz--;
        }

        ಪ್ರತ್ಯಾಗಮ n;
}

ssize_t sparse_write(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *p, size_t sz, size_t run_length) {
        ಧ್ರುವ uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ಯಾವತ್ (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ಯದಿ ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ಯದಿ (q > w) {
                                l = ಲಿಖ(fd, w, q - w);
                                ಯದಿ (l < 0)
                                        ಪ್ರತ್ಯಾಗಮ -errno;
                                ಯದಿ (l != q -w)
                                        ಪ್ರತ್ಯಾಗಮ -EIO;
                        }

                        ಯದಿ (lseek(fd, n, SEEK_CUR) < 0)
                                ಪ್ರತ್ಯಾಗಮ -errno;

                        q += n;
                        w = q;
                } ಅನ್ಯಥಾ ಯದಿ (n > 0)
                        q += n;
                ಅನ್ಯಥಾ
                        q++;
        }

        ಯದಿ (q > w) {
                l = ಲಿಖ(fd, w, q - w);
                ಯದಿ (l < 0)
                        ಪ್ರತ್ಯಾಗಮ -errno;
                ಯದಿ (l != q - w)
                        ಪ್ರತ್ಯಾಗಮ -EIO;
        }

        ಪ್ರತ್ಯಾಗಮ q - (ಧ್ರುವ uint8_t*) p;
}
