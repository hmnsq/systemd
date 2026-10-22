/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <fcntl.h>
#شامل <poll.h>
#شامل <stdio.h>
#شامل <string.h>
#شامل <sys/epoll.h>          /* IWYU pragma: keep */
#شامل <time.h>
#شامل <unistd.h>

#شامل "errno-util.h"
#شامل "fiber-ops.h"
#شامل "io-util.h"
#شامل "iovec-util.h"
#شامل "time-util.h"

uint32_t poll_events_مقصد_epoll(uint32_t events) {
        واپس (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_مقصد_poll(uint32_t events) {
        واپس (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

صحیح flush_fd(صحیح fd) {
        صحیح گنتی = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        برائے (;;) {
                حرف buf[LINE_MAX];
                ssize_t l;
                صحیح r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                اگر (r == -EINTR)
                        جاری;
                اگر (r < 0)
                        واپس r;
                اگر (r == 0)
                        واپس گنتی;

                l = اقرأ(fd, buf, حجم(buf));
                اگر (l < 0) {
                        اگر (errno == EINTR)
                                جاری;
                        اگر (errno == EAGAIN)
                                واپس گنتی;

                        واپس -errno;
                } ورنہ اگر (l == 0)
                        واپس گنتی;

                اگر (l > زیادہ_سے_زیادہ_عدد-گنتی) /* On overflow terminate */
                        واپس زیادہ_سے_زیادہ_عدد;

                گنتی += (صحیح) l;
        }
}

ssize_t loop_read(صحیح fd, خالی *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        أكد(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        اگر (nbytes > (size_t) SSIZE_MAX)
                واپس -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        صحیح flags = 0;
        اگر (fiber_ops_is_سیٹ() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                اگر (flags < 0)
                        واپس -errno;
        }

        کرو {
                ssize_t k;

                اگر (fiber_ops_is_سیٹ() && (do_poll || !FLAGS_سیٹ(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        اگر (k < 0)
                                واپس n > 0 ? n : k;
                } ورنہ {
                        k = اقرأ(fd, p, nbytes);
                        اگر (k < 0) {
                                اگر (errno == EINTR)
                                        جاری;

                                اگر (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (خالی) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        جاری;
                                }

                                واپس n > 0 ? n : -errno;
                        }
                }

                اگر (k == 0)
                        واپس n;

                أكد((size_t) k <= nbytes);
                أكد(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } جب (nbytes > 0);

        واپس n;
}

صحیح loop_read_exact(صحیح fd, خالی *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        اگر (n < 0)
                واپس (صحیح) n;
        اگر ((size_t) n != nbytes)
                واپس -EIO;

        واپس 0;
}

صحیح loop_write_بھرا(صحیح fd, ثابت خالی *buf, size_t nbytes, usec_t timeout) {
        ثابت uint8_t *p;
        usec_t انتہا;
        صحیح r;

        أكد(fd >= 0);
        أكد(buf || nbytes == 0);

        اگر (nbytes == 0) {
                ساکن ثابت dummy_t dummy[0];
                assert_cc(حجم(dummy) == 0);
                p = (ثابت خالی*) dummy; /* Some valid pointer, in case NULL was specified */
        } ورنہ {
                اگر (nbytes == SIZE_MAX)
                        nbytes = لڑی_لمبائی(buf);
                ورنہ اگر (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        واپس -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        صحیح flags = 0;
        اگر (fiber_ops_is_سیٹ() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                اگر (flags < 0)
                        واپس -errno;
        }

        اگر (fiber_ops_is_سیٹ() && !FLAGS_سیٹ(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_سیٹ(timeout) ? timeout : USEC_INFINITY);

                جب (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        اگر (k < 0)
                                واپس (صحیح) k;
                        اگر (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                واپس -EIO;

                        أكد((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                واپس 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        انتہا = timestamp_is_سیٹ(timeout) ? usec_جوڑو(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        کرو {
                ssize_t k;

                k = اكتب(fd, p, nbytes);
                اگر (k < 0) {
                        اگر (errno == EINTR)
                                جاری;

                        اگر (errno != EAGAIN || timeout == 0)
                                واپس -errno;

                        usec_t wait_for;

                        اگر (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ورنہ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                اگر (t >= انتہا)
                                        واپس -ETIME;

                                wait_for = usec_sub_unsigned(انتہا, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        اگر (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                جاری;
                        اگر (r < 0)
                                واپس r;
                        اگر (r == 0)
                                واپس -ETIME;
                        جاری;
                }

                اگر (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        واپس -EIO;

                أكد((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } جب (nbytes > 0);

        واپس 0;
}

صحیح pwritev_بھرا(صحیح fd, ساخت iovec *iovec, size_t n, uint64_t انحراف, uint64_t *ret_written) {
        uint64_t written = 0;
        صحیح r = 0;

        أكد(fd >= 0);
        أكد(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        برائے (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                جب (i < n && !iovec_is_سیٹ(iovec + i))
                        i++;
                اگر (i >= n)
                        توڑو;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, انحراف + written);
                اگر (k < 0) {
                        اگر (errno == EINTR)
                                جاری;

                        r = -errno;
                        توڑو;
                }
                اگر (k == 0) {
                        r = -EIO;
                        توڑو;
                }

                written += k;
                (خالی) iovec_inc_many(iovec + i, m, k);
        }

        اگر (ret_written)
                *ret_written = written;
        واپس r;
}

صحیح pwrite_بھرا(صحیح fd, ثابت خالی *buf, size_t nbytes, uint64_t انحراف) {
        أكد(fd >= 0);
        أكد(buf || nbytes == 0);

        واپس pwritev_بھرا(fd, &IOVEC_MAKE((خالی*) buf, nbytes), 1, انحراف, /* ret_written= */ تہی);
}

صحیح pipe_eof(صحیح fd) {
        صحیح r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        اگر (r <= 0)
                واپس r;

        واپس !!(r & POLLHUP);
}

صحیح ppoll_usec_بھرا(ساخت pollfd *fds, size_t n_fds, usec_t timeout, ثابت sigset_t *ss) {
        صحیح r;

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

        اگر (n_fds == 0 && timeout == 0)
                واپس 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? تہی : TIMESPEC_STORE(timeout), ss);
        اگر (r <= 0)
                واپس r;

        برائے (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                اگر (fds[i].revents == 0)
                        جاری;
                اگر (fds[i].revents & POLLNVAL)
                        واپس -EBADF;
                n--;
        }

        واپس r;
}

صحیح fd_wait_for_event(صحیح fd, صحیح event, usec_t timeout) {
        ساخت pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        صحیح r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        اگر (r <= 0)
                واپس r;

        واپس pollfd.revents;
}

ساکن size_t nul_لمبائی(ثابت uint8_t *p, size_t sz) {
        size_t n = 0;

        أكد(p);

        جب (sz > 0) {
                اگر (*p != 0)
                        توڑو;

                n++;
                p++;
                sz--;
        }

        واپس n;
}

ssize_t sparse_write(صحیح fd, ثابت خالی *p, size_t sz, size_t تسلسل_لمبائی) {
        ثابت uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        جب (q < e) {
                size_t n;

                n = nul_لمبائی(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                اگر ((n > تسلسل_لمبائی) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        اگر (q > w) {
                                l = اكتب(fd, w, q - w);
                                اگر (l < 0)
                                        واپس -errno;
                                اگر (l != q -w)
                                        واپس -EIO;
                        }

                        اگر (lseek(fd, n, SEEK_CUR) < 0)
                                واپس -errno;

                        q += n;
                        w = q;
                } ورنہ اگر (n > 0)
                        q += n;
                ورنہ
                        q++;
        }

        اگر (q > w) {
                l = اكتب(fd, w, q - w);
                اگر (l < 0)
                        واپس -errno;
                اگر (l != q - w)
                        واپس -EIO;
        }

        واپس q - (ثابت uint8_t*) p;
}
