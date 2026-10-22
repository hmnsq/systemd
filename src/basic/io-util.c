/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <fcntl.h>
#ضمن <poll.h>
#ضمن <stdio.h>
#ضمن <string.h>
#ضمن <sys/epoll.h>          /* IWYU pragma: keep */
#ضمن <time.h>
#ضمن <unistd.h>

#ضمن "errno-util.h"
#ضمن "fiber-ops.h"
#ضمن "io-util.h"
#ضمن "iovec-util.h"
#ضمن "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        ܦܢܐ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        ܦܢܐ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

صحيح flush_fd(صحيح fd) {
        صحيح count = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        لكل (;;) {
                حرف buf[LINE_MAX];
                ssize_t l;
                صحيح r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ܐܢ (r == -EINTR)
                        استمر;
                ܐܢ (r < 0)
                        ܦܢܐ r;
                ܐܢ (r == 0)
                        ܦܢܐ count;

                l = اقرأ(fd, buf, حجم(buf));
                ܐܢ (l < 0) {
                        ܐܢ (errno == EINTR)
                                استمر;
                        ܐܢ (errno == EAGAIN)
                                ܦܢܐ count;

                        ܦܢܐ -errno;
                } ܐܚܪܢܐ ܐܢ (l == 0)
                        ܦܢܐ count;

                ܐܢ (l > INT_MAX-count) /* On overflow terminate */
                        ܦܢܐ INT_MAX;

                count += (صحيح) l;
        }
}

ssize_t loop_read(صحيح fd, فراغ *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        أكد(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ܐܢ (nbytes > (size_t) SSIZE_MAX)
                ܦܢܐ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        صحيح flags = 0;
        ܐܢ (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ܐܢ (flags < 0)
                        ܦܢܐ -errno;
        }

        ܥܒܕ {
                ssize_t k;

                ܐܢ (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ܐܢ (k < 0)
                                ܦܢܐ n > 0 ? n : k;
                } ܐܚܪܢܐ {
                        k = اقرأ(fd, p, nbytes);
                        ܐܢ (k < 0) {
                                ܐܢ (errno == EINTR)
                                        استمر;

                                ܐܢ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (فراغ) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        استمر;
                                }

                                ܦܢܐ n > 0 ? n : -errno;
                        }
                }

                ܐܢ (k == 0)
                        ܦܢܐ n;

                أكد((size_t) k <= nbytes);
                أكد(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ܟܡܐ (nbytes > 0);

        ܦܢܐ n;
}

صحيح loop_read_exact(صحيح fd, فراغ *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ܐܢ (n < 0)
                ܦܢܐ (صحيح) n;
        ܐܢ ((size_t) n != nbytes)
                ܦܢܐ -EIO;

        ܦܢܐ 0;
}

صحيح loop_write_full(صحيح fd, ثابت فراغ *buf, size_t nbytes, usec_t timeout) {
        ثابت uint8_t *p;
        usec_t end;
        صحيح r;

        أكد(fd >= 0);
        أكد(buf || nbytes == 0);

        ܐܢ (nbytes == 0) {
                ساكن ثابت dummy_t dummy[0];
                assert_cc(حجم(dummy) == 0);
                p = (ثابت فراغ*) dummy; /* Some valid pointer, in case NULL was specified */
        } ܐܚܪܢܐ {
                ܐܢ (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                ܐܚܪܢܐ ܐܢ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ܦܢܐ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        صحيح flags = 0;
        ܐܢ (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ܐܢ (flags < 0)
                        ܦܢܐ -errno;
        }

        ܐܢ (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                ܟܡܐ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ܐܢ (k < 0)
                                ܦܢܐ (صحيح) k;
                        ܐܢ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ܦܢܐ -EIO;

                        أكد((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ܦܢܐ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        ܥܒܕ {
                ssize_t k;

                k = اكتب(fd, p, nbytes);
                ܐܢ (k < 0) {
                        ܐܢ (errno == EINTR)
                                استمر;

                        ܐܢ (errno != EAGAIN || timeout == 0)
                                ܦܢܐ -errno;

                        usec_t wait_for;

                        ܐܢ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ܐܚܪܢܐ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ܐܢ (t >= end)
                                        ܦܢܐ -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ܐܢ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                استمر;
                        ܐܢ (r < 0)
                                ܦܢܐ r;
                        ܐܢ (r == 0)
                                ܦܢܐ -ETIME;
                        استمر;
                }

                ܐܢ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ܦܢܐ -EIO;

                أكد((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ܟܡܐ (nbytes > 0);

        ܦܢܐ 0;
}

صحيح pwritev_full(صحيح fd, بنية iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        صحيح r = 0;

        أكد(fd >= 0);
        أكد(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        لكل (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ܟܡܐ (i < n && !iovec_is_set(iovec + i))
                        i++;
                ܐܢ (i >= n)
                        اكسر;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                ܐܢ (k < 0) {
                        ܐܢ (errno == EINTR)
                                استمر;

                        r = -errno;
                        اكسر;
                }
                ܐܢ (k == 0) {
                        r = -EIO;
                        اكسر;
                }

                written += k;
                (فراغ) iovec_inc_many(iovec + i, m, k);
        }

        ܐܢ (ret_written)
                *ret_written = written;
        ܦܢܐ r;
}

صحيح pwrite_full(صحيح fd, ثابت فراغ *buf, size_t nbytes, uint64_t offset) {
        أكد(fd >= 0);
        أكد(buf || nbytes == 0);

        ܦܢܐ pwritev_full(fd, &IOVEC_MAKE((فراغ*) buf, nbytes), 1, offset, /* ret_written= */ عدم);
}

صحيح pipe_eof(صحيح fd) {
        صحيح r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ܐܢ (r <= 0)
                ܦܢܐ r;

        ܦܢܐ !!(r & POLLHUP);
}

صحيح ppoll_usec_full(بنية pollfd *fds, size_t n_fds, usec_t timeout, ثابت sigset_t *ss) {
        صحيح r;

        أكد(fds || n_fds == 0);

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

        ܐܢ (n_fds == 0 && timeout == 0)
                ܦܢܐ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? عدم : TIMESPEC_STORE(timeout), ss);
        ܐܢ (r <= 0)
                ܦܢܐ r;

        لكل (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ܐܢ (fds[i].revents == 0)
                        استمر;
                ܐܢ (fds[i].revents & POLLNVAL)
                        ܦܢܐ -EBADF;
                n--;
        }

        ܦܢܐ r;
}

صحيح fd_wait_for_event(صحيح fd, صحيح event, usec_t timeout) {
        بنية pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        صحيح r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ܐܢ (r <= 0)
                ܦܢܐ r;

        ܦܢܐ pollfd.revents;
}

ساكن size_t nul_length(ثابت uint8_t *p, size_t sz) {
        size_t n = 0;

        أكد(p);

        ܟܡܐ (sz > 0) {
                ܐܢ (*p != 0)
                        اكسر;

                n++;
                p++;
                sz--;
        }

        ܦܢܐ n;
}

ssize_t sparse_write(صحيح fd, ثابت فراغ *p, size_t sz, size_t run_length) {
        ثابت uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ܟܡܐ (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ܐܢ ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ܐܢ (q > w) {
                                l = اكتب(fd, w, q - w);
                                ܐܢ (l < 0)
                                        ܦܢܐ -errno;
                                ܐܢ (l != q -w)
                                        ܦܢܐ -EIO;
                        }

                        ܐܢ (lseek(fd, n, SEEK_CUR) < 0)
                                ܦܢܐ -errno;

                        q += n;
                        w = q;
                } ܐܚܪܢܐ ܐܢ (n > 0)
                        q += n;
                ܐܚܪܢܐ
                        q++;
        }

        ܐܢ (q > w) {
                l = اكتب(fd, w, q - w);
                ܐܢ (l < 0)
                        ܦܢܐ -errno;
                ܐܢ (l != q - w)
                        ܦܢܐ -EIO;
        }

        ܦܢܐ q - (ثابت uint8_t*) p;
}
