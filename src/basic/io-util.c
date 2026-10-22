/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <poll.h>
#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <sys/epoll.h>          /* IWYU pragma: keep */
#ଅନ୍ତର୍ଭାଵ <time.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fiber-ops.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "iovec-util.h"
#ଅନ୍ତର୍ଭାଵ "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        ଫେରନ୍ତୁ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        ଫେରନ୍ତୁ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା flush_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        ପାଇଁ (;;) {
                ଅକ୍ଷର buf[LINE_MAX];
                ssize_t l;
                ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                ଯଦି (r == -EINTR)
                        ଜାରି;
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
                ଯଦି (r == 0)
                        ଫେରନ୍ତୁ ଗଣନା_2;

                l = ପଠ(fd, buf, ଆକାର(buf));
                ଯଦି (l < 0) {
                        ଯଦି (errno == EINTR)
                                ଜାରି;
                        ଯଦି (errno == EAGAIN)
                                ଫେରନ୍ତୁ ଗଣନା_2;

                        ଫେରନ୍ତୁ -errno;
                } ଅନ୍ୟଥା ଯଦି (l == 0)
                        ଫେରନ୍ତୁ ଗଣନା_2;

                ଯଦି (l > INT_MAX-ଗଣନା_2) /* On overflow terminate */
                        ଫେରନ୍ତୁ INT_MAX;

                ଗଣନା_2 += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) l;
        }
}

ssize_t loop_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        ଯଦି (nbytes > (size_t) SSIZE_MAX)
                ଫେରନ୍ତୁ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
        ଯଦି (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                ଯଦି (flags < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        କର {
                ssize_t k;

                ଯଦି (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        ଯଦି (k < 0)
                                ଫେରନ୍ତୁ n > 0 ? n : k;
                } ଅନ୍ୟଥା {
                        k = ପଠ(fd, p, nbytes);
                        ଯଦି (k < 0) {
                                ଯଦି (errno == EINTR)
                                        ଜାରି;

                                ଯଦି (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (ଶୂନ୍ୟ) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        ଜାରି;
                                }

                                ଫେରନ୍ତୁ n > 0 ? n : -errno;
                        }
                }

                ଯଦି (k == 0)
                        ଫେରନ୍ତୁ n;

                ପ୍ରତିଜ୍ଞା((size_t) k <= nbytes);
                ପ୍ରତିଜ୍ଞା(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ଯେତେବେଳେ (nbytes > 0);

        ଫେରନ୍ତୁ n;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_read_exact(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        ଯଦି (n < 0)
                ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) n;
        ଯଦି ((size_t) n != nbytes)
                ଫେରନ୍ତୁ -EIO;

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_write_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t nbytes, usec_t timeout) {
        ଅଚଳ uint8_t *p;
        usec_t end;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(buf || nbytes == 0);

        ଯଦି (nbytes == 0) {
                ସ୍ଥିର ଅଚଳ dummy_t dummy[0];
                assert_cc(ଆକାର(dummy) == 0);
                p = (ଅଚଳ ଶୂନ୍ୟ*) dummy; /* Some valid pointer, in case NULL was specified */
        } ଅନ୍ୟଥା {
                ଯଦି (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                ଅନ୍ୟଥା ଯଦି (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ଫେରନ୍ତୁ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = 0;
        ଯଦି (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                ଯଦି (flags < 0)
                        ଫେରନ୍ତୁ -errno;
        }

        ଯଦି (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                ଯେତେବେଳେ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        ଯଦି (k < 0)
                                ଫେରନ୍ତୁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା) k;
                        ଯଦି (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ଫେରନ୍ତୁ -EIO;

                        ପ୍ରତିଜ୍ଞା((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ଫେରନ୍ତୁ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        କର {
                ssize_t k;

                k = ଲିଖ(fd, p, nbytes);
                ଯଦି (k < 0) {
                        ଯଦି (errno == EINTR)
                                ଜାରି;

                        ଯଦି (errno != EAGAIN || timeout == 0)
                                ଫେରନ୍ତୁ -errno;

                        usec_t wait_for;

                        ଯଦି (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        ଅନ୍ୟଥା {
                                usec_t t = now(CLOCK_MONOTONIC);
                                ଯଦି (t >= end)
                                        ଫେରନ୍ତୁ -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        ଯଦି (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                ଜାରି;
                        ଯଦି (r < 0)
                                ଫେରନ୍ତୁ r;
                        ଯଦି (r == 0)
                                ଫେରନ୍ତୁ -ETIME;
                        ଜାରି;
                }

                ଯଦି (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ଫେରନ୍ତୁ -EIO;

                ପ୍ରତିଜ୍ଞା((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ଯେତେବେଳେ (nbytes > 0);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pwritev_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ସଂରଚନା iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r = 0;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        ପାଇଁ (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ଯେତେବେଳେ (i < n && !iovec_is_set(iovec + i))
                        i++;
                ଯଦି (i >= n)
                        ଭାଙ୍ଗ;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                ଯଦି (k < 0) {
                        ଯଦି (errno == EINTR)
                                ଜାରି;

                        r = -errno;
                        ଭାଙ୍ଗ;
                }
                ଯଦି (k == 0) {
                        r = -EIO;
                        ଭାଙ୍ଗ;
                }

                written += k;
                (ଶୂନ୍ୟ) iovec_inc_many(iovec + i, m, k);
        }

        ଯଦି (ret_written)
                *ret_written = written;
        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pwrite_full(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buf, size_t nbytes, uint64_t offset) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(buf || nbytes == 0);

        ଫେରନ୍ତୁ pwritev_full(fd, &IOVEC_MAKE((ଶୂନ୍ୟ*) buf, nbytes), 1, offset, /* ret_written= */ ଖାଲି);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pipe_eof(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ !!(r & POLLHUP);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା ppoll_usec_full(ସଂରଚନା pollfd *fds, size_t n_fds, usec_t timeout, ଅଚଳ sigset_t *ss) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fds || n_fds == 0);

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

        ଯଦି (n_fds == 0 && timeout == 0)
                ଫେରନ୍ତୁ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ଖାଲି : TIMESPEC_STORE(timeout), ss);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        ପାଇଁ (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                ଯଦି (fds[i].revents == 0)
                        ଜାରି;
                ଯଦି (fds[i].revents & POLLNVAL)
                        ଫେରନ୍ତୁ -EBADF;
                n--;
        }

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_wait_for_event(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା event, usec_t timeout) {
        ସଂରଚନା pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        ଯଦି (r <= 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ pollfd.revents;
}

ସ୍ଥିର size_t nul_length(ଅଚଳ uint8_t *p, size_t sz) {
        size_t n = 0;

        ପ୍ରତିଜ୍ଞା(p);

        ଯେତେବେଳେ (sz > 0) {
                ଯଦି (*p != 0)
                        ଭାଙ୍ଗ;

                n++;
                p++;
                sz--;
        }

        ଫେରନ୍ତୁ n;
}

ssize_t sparse_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *p, size_t sz, size_t run_length) {
        ଅଚଳ uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ଯେତେବେଳେ (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                ଯଦି ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        ଯଦି (q > w) {
                                l = ଲିଖ(fd, w, q - w);
                                ଯଦି (l < 0)
                                        ଫେରନ୍ତୁ -errno;
                                ଯଦି (l != q -w)
                                        ଫେରନ୍ତୁ -EIO;
                        }

                        ଯଦି (lseek(fd, n, SEEK_CUR) < 0)
                                ଫେରନ୍ତୁ -errno;

                        q += n;
                        w = q;
                } ଅନ୍ୟଥା ଯଦି (n > 0)
                        q += n;
                ଅନ୍ୟଥା
                        q++;
        }

        ଯଦି (q > w) {
                l = ଲିଖ(fd, w, q - w);
                ଯଦି (l < 0)
                        ଫେରନ୍ତୁ -errno;
                ଯଦି (l != q - w)
                        ଫେରନ୍ତୁ -EIO;
        }

        ଫେରନ୍ତୁ q - (ଅଚଳ uint8_t*) p;
}
