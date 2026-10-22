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

uint32_t poll_events_ಗಮ್ಯ_epoll(uint32_t events) {
        ಹಿಂತಿರುಗಿ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_ಗಮ್ಯ_poll(uint32_t events) {
        ಹಿಂತಿರುಗಿ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

ಪೂರ್ಣಾಂಕ flush_fd(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        ಗಾಗಿ (;;) {
                ಅಕ್ಷರ buf[LINE_MAX];
                ssize_t l;
                ಪೂರ್ಣಾಂಕ r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ಆದರೆ (r == -EINTR)
                        ಮುಂದುವರಿ;
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
                ಆದರೆ (r == 0)
                        ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;

                l = ಪಠ(fd, buf, ಗಾತ್ರ(buf));
                ಆದರೆ (l < 0) {
                        ಆದರೆ (errno == EINTR)
                                ಮುಂದುವರಿ;
                        ಆದರೆ (errno == EAGAIN)
                                ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;

                        ಹಿಂತಿರುಗಿ -errno;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (l == 0)
                        ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;

                ಆದರೆ (l > ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ-ಎಣಿಕೆ_2) /* On overflow terminate */
                        ಹಿಂತಿರುಗಿ ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ;

                ಎಣಿಕೆ_2 += (ಪೂರ್ಣಾಂಕ) l;
        }
}

ssize_t loop_read(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ಆದರೆ (nbytes > (size_t) SSIZE_MAX)
                ಹಿಂತಿರುಗಿ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        ಪೂರ್ಣಾಂಕ flags = 0;
        ಆದರೆ (fiber_ops_is_ಗಣ() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ಆದರೆ (flags < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಮಾಡು {
                ssize_t k;

                ಆದರೆ (fiber_ops_is_ಗಣ() && (do_poll || !FLAGS_ಗಣ(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ಆದರೆ (k < 0)
                                ಹಿಂತಿರುಗಿ n > 0 ? n : k;
                } ಇಲ್ಲದಿದ್ದರೆ {
                        k = ಪಠ(fd, p, nbytes);
                        ಆದರೆ (k < 0) {
                                ಆದರೆ (errno == EINTR)
                                        ಮುಂದುವರಿ;

                                ಆದರೆ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (ಶೂನ್ಯ) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        ಮುಂದುವರಿ;
                                }

                                ಹಿಂತಿರುಗಿ n > 0 ? n : -errno;
                        }
                }

                ಆದರೆ (k == 0)
                        ಹಿಂತಿರುಗಿ n;

                ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);
                ಪ್ರತಿಜ್ಞಾ(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ಆಗ (nbytes > 0);

        ಹಿಂತಿರುಗಿ n;
}

ಪೂರ್ಣಾಂಕ loop_read_exact(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ಆದರೆ (n < 0)
                ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) n;
        ಆದರೆ ((size_t) n != nbytes)
                ಹಿಂತಿರುಗಿ -EIO;

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ loop_write_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t nbytes, usec_t timeout) {
        ಸ್ಥಿರಾಂಕ uint8_t *p;
        usec_t ಕೊನೆ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(buf || nbytes == 0);

        ಆದರೆ (nbytes == 0) {
                ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ dummy_t dummy[0];
                assert_cc(ಗಾತ್ರ(dummy) == 0);
                p = (ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ*) dummy; /* Some valid pointer, in case NULL was specified */
        } ಇಲ್ಲದಿದ್ದರೆ {
                ಆದರೆ (nbytes == SIZE_MAX)
                        nbytes = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(buf);
                ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ಹಿಂತಿರುಗಿ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        ಪೂರ್ಣಾಂಕ flags = 0;
        ಆದರೆ (fiber_ops_is_ಗಣ() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ಆದರೆ (flags < 0)
                        ಹಿಂತಿರುಗಿ -errno;
        }

        ಆದರೆ (fiber_ops_is_ಗಣ() && !FLAGS_ಗಣ(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_ಗಣ(timeout) ? timeout : USEC_INFINITY);

                ಆಗ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ಆದರೆ (k < 0)
                                ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ) k;
                        ಆದರೆ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ಹಿಂತಿರುಗಿ -EIO;

                        ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ಹಿಂತಿರುಗಿ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        ಕೊನೆ = timestamp_is_ಗಣ(timeout) ? usec_ಕೂಡಿಸು(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        ಮಾಡು {
                ssize_t k;

                k = ಲಿಖ(fd, p, nbytes);
                ಆದರೆ (k < 0) {
                        ಆದರೆ (errno == EINTR)
                                ಮುಂದುವರಿ;

                        ಆದರೆ (errno != EAGAIN || timeout == 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        usec_t wait_for;

                        ಆದರೆ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ಇಲ್ಲದಿದ್ದರೆ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ಆದರೆ (t >= ಕೊನೆ)
                                        ಹಿಂತಿರುಗಿ -ETIME;

                                wait_for = usec_sub_unsigned(ಕೊನೆ, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ಆದರೆ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                ಮುಂದುವರಿ;
                        ಆದರೆ (r < 0)
                                ಹಿಂತಿರುಗಿ r;
                        ಆದರೆ (r == 0)
                                ಹಿಂತಿರುಗಿ -ETIME;
                        ಮುಂದುವರಿ;
                }

                ಆದರೆ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ಹಿಂತಿರುಗಿ -EIO;

                ಪ್ರತಿಜ್ಞಾ((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ಆಗ (nbytes > 0);

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ pwritev_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ರಚನೆ iovec *iovec, size_t n, uint64_t ಸ್ಥಾನಾಂತರ, uint64_t *ret_written) {
        uint64_t written = 0;
        ಪೂರ್ಣಾಂಕ r = 0;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        ಗಾಗಿ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ಆಗ (i < n && !iovec_is_ಗಣ(iovec + i))
                        i++;
                ಆದರೆ (i >= n)
                        ನಿಲ್ಲಿಸು;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, ಸ್ಥಾನಾಂತರ + written);
                ಆದರೆ (k < 0) {
                        ಆದರೆ (errno == EINTR)
                                ಮುಂದುವರಿ;

                        r = -errno;
                        ನಿಲ್ಲಿಸು;
                }
                ಆದರೆ (k == 0) {
                        r = -EIO;
                        ನಿಲ್ಲಿಸು;
                }

                written += k;
                (ಶೂನ್ಯ) iovec_inc_many(iovec + i, m, k);
        }

        ಆದರೆ (ret_written)
                *ret_written = written;
        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ pwrite_ತುಂಬಿದ(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *buf, size_t nbytes, uint64_t ಸ್ಥಾನಾಂತರ) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(buf || nbytes == 0);

        ಹಿಂತಿರುಗಿ pwritev_ತುಂಬಿದ(fd, &IOVEC_MAKE((ಶೂನ್ಯ*) buf, nbytes), 1, ಸ್ಥಾನಾಂತರ, /* ret_written= */ ಖಾಲಿ);
}

ಪೂರ್ಣಾಂಕ pipe_eof(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ !!(r & POLLHUP);
}

ಪೂರ್ಣಾಂಕ ppoll_usec_ತುಂಬಿದ(ರಚನೆ pollfd *fds, size_t n_fds, usec_t timeout, ಸ್ಥಿರಾಂಕ sigset_t *ss) {
        ಪೂರ್ಣಾಂಕ r;

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

        ಆದರೆ (n_fds == 0 && timeout == 0)
                ಹಿಂತಿರುಗಿ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ಖಾಲಿ : TIMESPEC_STORE(timeout), ss);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        ಗಾಗಿ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ಆದರೆ (fds[i].revents == 0)
                        ಮುಂದುವರಿ;
                ಆದರೆ (fds[i].revents & POLLNVAL)
                        ಹಿಂತಿರುಗಿ -EBADF;
                n--;
        }

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ fd_wait_for_event(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ event, usec_t timeout) {
        ರಚನೆ pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        ಪೂರ್ಣಾಂಕ r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ಆದರೆ (r <= 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ pollfd.revents;
}

ಸ್ಥಿರ size_t nul_ಉದ್ದಳತೆ(ಸ್ಥಿರಾಂಕ uint8_t *p, size_t sz) {
        size_t n = 0;

        ಪ್ರತಿಜ್ಞಾ(p);

        ಆಗ (sz > 0) {
                ಆದರೆ (*p != 0)
                        ನಿಲ್ಲಿಸು;

                n++;
                p++;
                sz--;
        }

        ಹಿಂತಿರುಗಿ n;
}

ssize_t sparse_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p, size_t sz, size_t ಸತತ_ಉದ್ದಳತೆ) {
        ಸ್ಥಿರಾಂಕ uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ಆಗ (q < e) {
                size_t n;

                n = nul_ಉದ್ದಳತೆ(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ಆದರೆ ((n > ಸತತ_ಉದ್ದಳತೆ) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ಆದರೆ (q > w) {
                                l = ಲಿಖ(fd, w, q - w);
                                ಆದರೆ (l < 0)
                                        ಹಿಂತಿರುಗಿ -errno;
                                ಆದರೆ (l != q -w)
                                        ಹಿಂತಿರುಗಿ -EIO;
                        }

                        ಆದರೆ (lseek(fd, n, SEEK_CUR) < 0)
                                ಹಿಂತಿರುಗಿ -errno;

                        q += n;
                        w = q;
                } ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n > 0)
                        q += n;
                ಇಲ್ಲದಿದ್ದರೆ
                        q++;
        }

        ಆದರೆ (q > w) {
                l = ಲಿಖ(fd, w, q - w);
                ಆದರೆ (l < 0)
                        ಹಿಂತಿರುಗಿ -errno;
                ಆದರೆ (l != q - w)
                        ಹಿಂತಿರುಗಿ -EIO;
        }

        ಹಿಂತಿರುಗಿ q - (ಸ್ಥಿರಾಂಕ uint8_t*) p;
}
