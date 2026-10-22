/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/epoll.h>          /* IWYU pragma: keep */
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fiber-ops.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "io-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "iovec-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "time-util.h"

uint32_t poll_events_ਮੰਜ਼ਿਲ_epoll(uint32_t events) {
        ਵਾਪਸ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_ਮੰਜ਼ਿਲ_poll(uint32_t events) {
        ਵਾਪਸ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

ਪੂਰਨ flush_fd(ਪੂਰਨ fd) {
        ਪੂਰਨ ਗਿਣਤੀ_2 = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        ਲਈ (;;) {
                ਅੱਖਰ buf[LINE_MAX];
                ssize_t l;
                ਪੂਰਨ r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ਜੇ (r == -EINTR)
                        ਜਾਰੀ;
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਵਾਪਸ ਗਿਣਤੀ_2;

                l = ਪਠ(fd, buf, ਆਕਾਰ(buf));
                ਜੇ (l < 0) {
                        ਜੇ (errno == EINTR)
                                ਜਾਰੀ;
                        ਜੇ (errno == EAGAIN)
                                ਵਾਪਸ ਗਿਣਤੀ_2;

                        ਵਾਪਸ -errno;
                } ਨਹੀਂਤਾਂ ਜੇ (l == 0)
                        ਵਾਪਸ ਗਿਣਤੀ_2;

                ਜੇ (l > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ-ਗਿਣਤੀ_2) /* On overflow terminate */
                        ਵਾਪਸ ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ;

                ਗਿਣਤੀ_2 += (ਪੂਰਨ) l;
        }
}

ssize_t loop_read(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ਜੇ (nbytes > (size_t) SSIZE_MAX)
                ਵਾਪਸ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        ਪੂਰਨ flags = 0;
        ਜੇ (fiber_ops_is_ਸੈੱਟ() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ਜੇ (flags < 0)
                        ਵਾਪਸ -errno;
        }

        ਕਰੋ {
                ssize_t k;

                ਜੇ (fiber_ops_is_ਸੈੱਟ() && (do_poll || !FLAGS_ਸੈੱਟ(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ਜੇ (k < 0)
                                ਵਾਪਸ n > 0 ? n : k;
                } ਨਹੀਂਤਾਂ {
                        k = ਪਠ(fd, p, nbytes);
                        ਜੇ (k < 0) {
                                ਜੇ (errno == EINTR)
                                        ਜਾਰੀ;

                                ਜੇ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (ਖਾਲੀ) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        ਜਾਰੀ;
                                }

                                ਵਾਪਸ n > 0 ? n : -errno;
                        }
                }

                ਜੇ (k == 0)
                        ਵਾਪਸ n;

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) k <= nbytes);
                ਪ੍ਰਤਿਜ੍ਞਾ(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ਜਦੋਂ (nbytes > 0);

        ਵਾਪਸ n;
}

ਪੂਰਨ loop_read_exact(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ਜੇ (n < 0)
                ਵਾਪਸ (ਪੂਰਨ) n;
        ਜੇ ((size_t) n != nbytes)
                ਵਾਪਸ -EIO;

        ਵਾਪਸ 0;
}

ਪੂਰਨ loop_write_ਭਰਿਆ(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t nbytes, usec_t timeout) {
        ਅਚਲ uint8_t *p;
        usec_t ਸਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf || nbytes == 0);

        ਜੇ (nbytes == 0) {
                ਸਥਿਰ ਅਚਲ dummy_t dummy[0];
                assert_cc(ਆਕਾਰ(dummy) == 0);
                p = (ਅਚਲ ਖਾਲੀ*) dummy; /* Some valid pointer, in case NULL was specified */
        } ਨਹੀਂਤਾਂ {
                ਜੇ (nbytes == SIZE_MAX)
                        nbytes = ਲੜੀ_ਲੰਬਾਈ(buf);
                ਨਹੀਂਤਾਂ ਜੇ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ਵਾਪਸ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        ਪੂਰਨ flags = 0;
        ਜੇ (fiber_ops_is_ਸੈੱਟ() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ਜੇ (flags < 0)
                        ਵਾਪਸ -errno;
        }

        ਜੇ (fiber_ops_is_ਸੈੱਟ() && !FLAGS_ਸੈੱਟ(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_ਸੈੱਟ(timeout) ? timeout : USEC_INFINITY);

                ਜਦੋਂ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ਜੇ (k < 0)
                                ਵਾਪਸ (ਪੂਰਨ) k;
                        ਜੇ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ਵਾਪਸ -EIO;

                        ਪ੍ਰਤਿਜ੍ਞਾ((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ਵਾਪਸ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        ਸਿਰਾ = timestamp_is_ਸੈੱਟ(timeout) ? usec_ਜੋੜੋ(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        ਕਰੋ {
                ssize_t k;

                k = ਲਿਖ(fd, p, nbytes);
                ਜੇ (k < 0) {
                        ਜੇ (errno == EINTR)
                                ਜਾਰੀ;

                        ਜੇ (errno != EAGAIN || timeout == 0)
                                ਵਾਪਸ -errno;

                        usec_t wait_for;

                        ਜੇ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ਨਹੀਂਤਾਂ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ਜੇ (t >= ਸਿਰਾ)
                                        ਵਾਪਸ -ETIME;

                                wait_for = usec_sub_unsigned(ਸਿਰਾ, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ਜੇ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                ਜਾਰੀ;
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;
                        ਜੇ (r == 0)
                                ਵਾਪਸ -ETIME;
                        ਜਾਰੀ;
                }

                ਜੇ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ਵਾਪਸ -EIO;

                ਪ੍ਰਤਿਜ੍ਞਾ((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ਜਦੋਂ (nbytes > 0);

        ਵਾਪਸ 0;
}

ਪੂਰਨ pwritev_ਭਰਿਆ(ਪੂਰਨ fd, ਬਣਤਰ iovec *iovec, size_t n, uint64_t ਵਿੱਥ, uint64_t *ret_written) {
        uint64_t written = 0;
        ਪੂਰਨ r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        ਲਈ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ਜਦੋਂ (i < n && !iovec_is_ਸੈੱਟ(iovec + i))
                        i++;
                ਜੇ (i >= n)
                        ਰੋਕੋ;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, ਵਿੱਥ + written);
                ਜੇ (k < 0) {
                        ਜੇ (errno == EINTR)
                                ਜਾਰੀ;

                        r = -errno;
                        ਰੋਕੋ;
                }
                ਜੇ (k == 0) {
                        r = -EIO;
                        ਰੋਕੋ;
                }

                written += k;
                (ਖਾਲੀ) iovec_inc_many(iovec + i, m, k);
        }

        ਜੇ (ret_written)
                *ret_written = written;
        ਵਾਪਸ r;
}

ਪੂਰਨ pwrite_ਭਰਿਆ(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t nbytes, uint64_t ਵਿੱਥ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fd >= 0);
        ਪ੍ਰਤਿਜ੍ਞਾ(buf || nbytes == 0);

        ਵਾਪਸ pwritev_ਭਰਿਆ(fd, &IOVEC_MAKE((ਖਾਲੀ*) buf, nbytes), 1, ਵਿੱਥ, /* ret_written= */ ਨਿਰਾ);
}

ਪੂਰਨ pipe_eof(ਪੂਰਨ fd) {
        ਪੂਰਨ r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        ਵਾਪਸ !!(r & POLLHUP);
}

ਪੂਰਨ ppoll_usec_ਭਰਿਆ(ਬਣਤਰ pollfd *fds, size_t n_fds, usec_t timeout, ਅਚਲ sigset_t *ss) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fds || n_fds == 0);

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

        ਜੇ (n_fds == 0 && timeout == 0)
                ਵਾਪਸ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ਨਿਰਾ : TIMESPEC_STORE(timeout), ss);
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        ਲਈ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ਜੇ (fds[i].revents == 0)
                        ਜਾਰੀ;
                ਜੇ (fds[i].revents & POLLNVAL)
                        ਵਾਪਸ -EBADF;
                n--;
        }

        ਵਾਪਸ r;
}

ਪੂਰਨ fd_wait_for_event(ਪੂਰਨ fd, ਪੂਰਨ event, usec_t timeout) {
        ਬਣਤਰ pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        ਪੂਰਨ r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        ਵਾਪਸ pollfd.revents;
}

ਸਥਿਰ size_t nul_ਲੰਮਾਈ(ਅਚਲ uint8_t *p, size_t sz) {
        size_t n = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        ਜਦੋਂ (sz > 0) {
                ਜੇ (*p != 0)
                        ਰੋਕੋ;

                n++;
                p++;
                sz--;
        }

        ਵਾਪਸ n;
}

ssize_t sparse_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *p, size_t sz, size_t ਲੜੀਵਾਰ_ਲੰਮਾਈ) {
        ਅਚਲ uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ਜਦੋਂ (q < e) {
                size_t n;

                n = nul_ਲੰਮਾਈ(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ਜੇ ((n > ਲੜੀਵਾਰ_ਲੰਮਾਈ) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ਜੇ (q > w) {
                                l = ਲਿਖ(fd, w, q - w);
                                ਜੇ (l < 0)
                                        ਵਾਪਸ -errno;
                                ਜੇ (l != q -w)
                                        ਵਾਪਸ -EIO;
                        }

                        ਜੇ (lseek(fd, n, SEEK_CUR) < 0)
                                ਵਾਪਸ -errno;

                        q += n;
                        w = q;
                } ਨਹੀਂਤਾਂ ਜੇ (n > 0)
                        q += n;
                ਨਹੀਂਤਾਂ
                        q++;
        }

        ਜੇ (q > w) {
                l = ਲਿਖ(fd, w, q - w);
                ਜੇ (l < 0)
                        ਵਾਪਸ -errno;
                ਜੇ (l != q - w)
                        ਵਾਪਸ -EIO;
        }

        ਵਾਪਸ q - (ਅਚਲ uint8_t*) p;
}
