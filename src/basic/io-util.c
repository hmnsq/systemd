/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <poll.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <sys/epoll.h>          /* IWYU pragma: keep */
#అన్తర్భావ <time.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "fiber-ops.h"
#అన్తర్భావ "io-util.h"
#అన్తర్భావ "iovec-util.h"
#అన్తర్భావ "time-util.h"

uint32_t poll_events_గమ్యం_epoll(uint32_t events) {
        తిరిగి (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_గమ్యం_poll(uint32_t events) {
        తిరిగి (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

పూర్ణాంకం flush_fd(పూర్ణాంకం fd) {
        పూర్ణాంకం లెక్క = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        కొరకు (;;) {
                అక్షరం buf[LINE_MAX];
                ssize_t l;
                పూర్ణాంకం r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ఒకవేళ (r == -EINTR)
                        కొనసాగు;
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        తిరిగి లెక్క;

                l = పఠ(fd, buf, పరిమాణం(buf));
                ఒకవేళ (l < 0) {
                        ఒకవేళ (errno == EINTR)
                                కొనసాగు;
                        ఒకవేళ (errno == EAGAIN)
                                తిరిగి లెక్క;

                        తిరిగి -errno;
                } లేకపోతే ఒకవేళ (l == 0)
                        తిరిగి లెక్క;

                ఒకవేళ (l > గరిష్ఠ_పూర్ణాంకం-లెక్క) /* On overflow terminate */
                        తిరిగి గరిష్ఠ_పూర్ణాంకం;

                లెక్క += (పూర్ణాంకం) l;
        }
}

ssize_t loop_read(పూర్ణాంకం fd, శూన్య *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ప్రతిజ్ఞా(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ఒకవేళ (nbytes > (size_t) SSIZE_MAX)
                తిరిగి -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        పూర్ణాంకం flags = 0;
        ఒకవేళ (fiber_ops_is_సమితి() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ఒకవేళ (flags < 0)
                        తిరిగి -errno;
        }

        చేయి {
                ssize_t k;

                ఒకవేళ (fiber_ops_is_సమితి() && (do_poll || !FLAGS_సమితి(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ఒకవేళ (k < 0)
                                తిరిగి n > 0 ? n : k;
                } లేకపోతే {
                        k = పఠ(fd, p, nbytes);
                        ఒకవేళ (k < 0) {
                                ఒకవేళ (errno == EINTR)
                                        కొనసాగు;

                                ఒకవేళ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (శూన్య) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        కొనసాగు;
                                }

                                తిరిగి n > 0 ? n : -errno;
                        }
                }

                ఒకవేళ (k == 0)
                        తిరిగి n;

                ప్రతిజ్ఞా((size_t) k <= nbytes);
                ప్రతిజ్ఞా(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } అయితే (nbytes > 0);

        తిరిగి n;
}

పూర్ణాంకం loop_read_exact(పూర్ణాంకం fd, శూన్య *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ఒకవేళ (n < 0)
                తిరిగి (పూర్ణాంకం) n;
        ఒకవేళ ((size_t) n != nbytes)
                తిరిగి -EIO;

        తిరిగి 0;
}

పూర్ణాంకం loop_write_నిండిన(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t nbytes, usec_t timeout) {
        స్థిరరాశి uint8_t *p;
        usec_t చివర;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(buf || nbytes == 0);

        ఒకవేళ (nbytes == 0) {
                స్థిర స్థిరరాశి dummy_t dummy[0];
                assert_cc(పరిమాణం(dummy) == 0);
                p = (స్థిరరాశి శూన్య*) dummy; /* Some valid pointer, in case NULL was specified */
        } లేకపోతే {
                ఒకవేళ (nbytes == SIZE_MAX)
                        nbytes = అక్షరశ్రేణి_పొడవు(buf);
                లేకపోతే ఒకవేళ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        తిరిగి -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        పూర్ణాంకం flags = 0;
        ఒకవేళ (fiber_ops_is_సమితి() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ఒకవేళ (flags < 0)
                        తిరిగి -errno;
        }

        ఒకవేళ (fiber_ops_is_సమితి() && !FLAGS_సమితి(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_సమితి(timeout) ? timeout : USEC_INFINITY);

                అయితే (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ఒకవేళ (k < 0)
                                తిరిగి (పూర్ణాంకం) k;
                        ఒకవేళ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                తిరిగి -EIO;

                        ప్రతిజ్ఞా((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                తిరిగి 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        చివర = timestamp_is_సమితి(timeout) ? usec_కూడు(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        చేయి {
                ssize_t k;

                k = లిఖ(fd, p, nbytes);
                ఒకవేళ (k < 0) {
                        ఒకవేళ (errno == EINTR)
                                కొనసాగు;

                        ఒకవేళ (errno != EAGAIN || timeout == 0)
                                తిరిగి -errno;

                        usec_t wait_for;

                        ఒకవేళ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        లేకపోతే {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ఒకవేళ (t >= చివర)
                                        తిరిగి -ETIME;

                                wait_for = usec_sub_unsigned(చివర, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ఒకవేళ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                కొనసాగు;
                        ఒకవేళ (r < 0)
                                తిరిగి r;
                        ఒకవేళ (r == 0)
                                తిరిగి -ETIME;
                        కొనసాగు;
                }

                ఒకవేళ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        తిరిగి -EIO;

                ప్రతిజ్ఞా((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } అయితే (nbytes > 0);

        తిరిగి 0;
}

పూర్ణాంకం pwritev_నిండిన(పూర్ణాంకం fd, నిర్మాణం iovec *iovec, size_t n, uint64_t స్థానభ్రంశం, uint64_t *ret_written) {
        uint64_t written = 0;
        పూర్ణాంకం r = 0;

        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        కొరకు (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                అయితే (i < n && !iovec_is_సమితి(iovec + i))
                        i++;
                ఒకవేళ (i >= n)
                        ఆపు;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, స్థానభ్రంశం + written);
                ఒకవేళ (k < 0) {
                        ఒకవేళ (errno == EINTR)
                                కొనసాగు;

                        r = -errno;
                        ఆపు;
                }
                ఒకవేళ (k == 0) {
                        r = -EIO;
                        ఆపు;
                }

                written += k;
                (శూన్య) iovec_inc_many(iovec + i, m, k);
        }

        ఒకవేళ (ret_written)
                *ret_written = written;
        తిరిగి r;
}

పూర్ణాంకం pwrite_నిండిన(పూర్ణాంకం fd, స్థిరరాశి శూన్య *buf, size_t nbytes, uint64_t స్థానభ్రంశం) {
        ప్రతిజ్ఞా(fd >= 0);
        ప్రతిజ్ఞా(buf || nbytes == 0);

        తిరిగి pwritev_నిండిన(fd, &IOVEC_MAKE((శూన్య*) buf, nbytes), 1, స్థానభ్రంశం, /* ret_written= */ ఖాళీ);
}

పూర్ణాంకం pipe_eof(పూర్ణాంకం fd) {
        పూర్ణాంకం r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ఒకవేళ (r <= 0)
                తిరిగి r;

        తిరిగి !!(r & POLLHUP);
}

పూర్ణాంకం ppoll_usec_నిండిన(నిర్మాణం pollfd *fds, size_t n_fds, usec_t timeout, స్థిరరాశి sigset_t *ss) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fds || n_fds == 0);

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

        ఒకవేళ (n_fds == 0 && timeout == 0)
                తిరిగి 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ఖాళీ : TIMESPEC_STORE(timeout), ss);
        ఒకవేళ (r <= 0)
                తిరిగి r;

        కొరకు (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ఒకవేళ (fds[i].revents == 0)
                        కొనసాగు;
                ఒకవేళ (fds[i].revents & POLLNVAL)
                        తిరిగి -EBADF;
                n--;
        }

        తిరిగి r;
}

పూర్ణాంకం fd_wait_for_event(పూర్ణాంకం fd, పూర్ణాంకం event, usec_t timeout) {
        నిర్మాణం pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        పూర్ణాంకం r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ఒకవేళ (r <= 0)
                తిరిగి r;

        తిరిగి pollfd.revents;
}

స్థిర size_t nul_నిడివి(స్థిరరాశి uint8_t *p, size_t sz) {
        size_t n = 0;

        ప్రతిజ్ఞా(p);

        అయితే (sz > 0) {
                ఒకవేళ (*p != 0)
                        ఆపు;

                n++;
                p++;
                sz--;
        }

        తిరిగి n;
}

ssize_t sparse_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *p, size_t sz, size_t నిరంతరం_నిడివి) {
        స్థిరరాశి uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        అయితే (q < e) {
                size_t n;

                n = nul_నిడివి(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ఒకవేళ ((n > నిరంతరం_నిడివి) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ఒకవేళ (q > w) {
                                l = లిఖ(fd, w, q - w);
                                ఒకవేళ (l < 0)
                                        తిరిగి -errno;
                                ఒకవేళ (l != q -w)
                                        తిరిగి -EIO;
                        }

                        ఒకవేళ (lseek(fd, n, SEEK_CUR) < 0)
                                తిరిగి -errno;

                        q += n;
                        w = q;
                } లేకపోతే ఒకవేళ (n > 0)
                        q += n;
                లేకపోతే
                        q++;
        }

        ఒకవేళ (q > w) {
                l = లిఖ(fd, w, q - w);
                ఒకవేళ (l < 0)
                        తిరిగి -errno;
                ఒకవేళ (l != q - w)
                        తిరిగి -EIO;
        }

        తిరిగి q - (స్థిరరాశి uint8_t*) p;
}
