/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <poll.h>
#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <sys/epoll.h>          /* IWYU pragma: keep */
#অন্তর্ভাব <time.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fiber-ops.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "iovec-util.h"
#অন্তর্ভাব "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        ফেরত (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        ফেরত (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

পূর্ণসংখ্যা flush_fd(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা গুনতি = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        জন্য (;;) {
                অক্ষর buf[LINE_MAX];
                ssize_t l;
                পূর্ণসংখ্যা r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                যদি (r == -EINTR)
                        চালু;
                যদি (r < 0)
                        ফেরত r;
                যদি (r == 0)
                        ফেরত গুনতি;

                l = পঠ(fd, buf, আকার(buf));
                যদি (l < 0) {
                        যদি (errno == EINTR)
                                চালু;
                        যদি (errno == EAGAIN)
                                ফেরত গুনতি;

                        ফেরত -errno;
                } অন্যথা যদি (l == 0)
                        ফেরত গুনতি;

                যদি (l > INT_MAX-গুনতি) /* On overflow terminate */
                        ফেরত INT_MAX;

                গুনতি += (পূর্ণসংখ্যা) l;
        }
}

ssize_t loop_read(পূর্ণসংখ্যা fd, শূন্য *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        প্রতিজ্ঞা(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        যদি (nbytes > (size_t) SSIZE_MAX)
                ফেরত -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        পূর্ণসংখ্যা flags = 0;
        যদি (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                যদি (flags < 0)
                        ফেরত -errno;
        }

        করো {
                ssize_t k;

                যদি (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        যদি (k < 0)
                                ফেরত n > 0 ? n : k;
                } অন্যথা {
                        k = পঠ(fd, p, nbytes);
                        যদি (k < 0) {
                                যদি (errno == EINTR)
                                        চালু;

                                যদি (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (শূন্য) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        চালু;
                                }

                                ফেরত n > 0 ? n : -errno;
                        }
                }

                যদি (k == 0)
                        ফেরত n;

                প্রতিজ্ঞা((size_t) k <= nbytes);
                প্রতিজ্ঞা(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } যখন (nbytes > 0);

        ফেরত n;
}

পূর্ণসংখ্যা loop_read_exact(পূর্ণসংখ্যা fd, শূন্য *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        যদি (n < 0)
                ফেরত (পূর্ণসংখ্যা) n;
        যদি ((size_t) n != nbytes)
                ফেরত -EIO;

        ফেরত 0;
}

পূর্ণসংখ্যা loop_write_full(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t nbytes, usec_t timeout) {
        ধ্রুবক uint8_t *p;
        usec_t end;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(buf || nbytes == 0);

        যদি (nbytes == 0) {
                স্থির ধ্রুবক dummy_t dummy[0];
                assert_cc(আকার(dummy) == 0);
                p = (ধ্রুবক শূন্য*) dummy; /* Some valid pointer, in case NULL was specified */
        } অন্যথা {
                যদি (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                অন্যথা যদি (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        ফেরত -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        পূর্ণসংখ্যা flags = 0;
        যদি (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                যদি (flags < 0)
                        ফেরত -errno;
        }

        যদি (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                যখন (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        যদি (k < 0)
                                ফেরত (পূর্ণসংখ্যা) k;
                        যদি (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                ফেরত -EIO;

                        প্রতিজ্ঞা((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                ফেরত 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        করো {
                ssize_t k;

                k = লিখ(fd, p, nbytes);
                যদি (k < 0) {
                        যদি (errno == EINTR)
                                চালু;

                        যদি (errno != EAGAIN || timeout == 0)
                                ফেরত -errno;

                        usec_t wait_for;

                        যদি (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        অন্যথা {
                                usec_t t = now(CLOCK_MONOTONIC);
                                যদি (t >= end)
                                        ফেরত -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        যদি (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                চালু;
                        যদি (r < 0)
                                ফেরত r;
                        যদি (r == 0)
                                ফেরত -ETIME;
                        চালু;
                }

                যদি (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        ফেরত -EIO;

                প্রতিজ্ঞা((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } যখন (nbytes > 0);

        ফেরত 0;
}

পূর্ণসংখ্যা pwritev_full(পূর্ণসংখ্যা fd, কাঠামো iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        পূর্ণসংখ্যা r = 0;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        জন্য (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                যখন (i < n && !iovec_is_set(iovec + i))
                        i++;
                যদি (i >= n)
                        বিরতি;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                যদি (k < 0) {
                        যদি (errno == EINTR)
                                চালু;

                        r = -errno;
                        বিরতি;
                }
                যদি (k == 0) {
                        r = -EIO;
                        বিরতি;
                }

                written += k;
                (শূন্য) iovec_inc_many(iovec + i, m, k);
        }

        যদি (ret_written)
                *ret_written = written;
        ফেরত r;
}

পূর্ণসংখ্যা pwrite_full(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *buf, size_t nbytes, uint64_t offset) {
        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(buf || nbytes == 0);

        ফেরত pwritev_full(fd, &IOVEC_MAKE((শূন্য*) buf, nbytes), 1, offset, /* ret_written= */ ফাঁকা);
}

পূর্ণসংখ্যা pipe_eof(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        যদি (r <= 0)
                ফেরত r;

        ফেরত !!(r & POLLHUP);
}

পূর্ণসংখ্যা ppoll_usec_full(কাঠামো pollfd *fds, size_t n_fds, usec_t timeout, ধ্রুবক sigset_t *ss) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fds || n_fds == 0);

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

        যদি (n_fds == 0 && timeout == 0)
                ফেরত 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ফাঁকা : TIMESPEC_STORE(timeout), ss);
        যদি (r <= 0)
                ফেরত r;

        জন্য (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                যদি (fds[i].revents == 0)
                        চালু;
                যদি (fds[i].revents & POLLNVAL)
                        ফেরত -EBADF;
                n--;
        }

        ফেরত r;
}

পূর্ণসংখ্যা fd_wait_for_event(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা event, usec_t timeout) {
        কাঠামো pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        পূর্ণসংখ্যা r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        যদি (r <= 0)
                ফেরত r;

        ফেরত pollfd.revents;
}

স্থির size_t nul_দৈর্ঘ্য(ধ্রুবক uint8_t *p, size_t sz) {
        size_t n = 0;

        প্রতিজ্ঞা(p);

        যখন (sz > 0) {
                যদি (*p != 0)
                        বিরতি;

                n++;
                p++;
                sz--;
        }

        ফেরত n;
}

ssize_t sparse_write(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *p, size_t sz, size_t run_দৈর্ঘ্য) {
        ধ্রুবক uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        যখন (q < e) {
                size_t n;

                n = nul_দৈর্ঘ্য(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                যদি ((n > run_দৈর্ঘ্য) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        যদি (q > w) {
                                l = লিখ(fd, w, q - w);
                                যদি (l < 0)
                                        ফেরত -errno;
                                যদি (l != q -w)
                                        ফেরত -EIO;
                        }

                        যদি (lseek(fd, n, SEEK_CUR) < 0)
                                ফেরত -errno;

                        q += n;
                        w = q;
                } অন্যথা যদি (n > 0)
                        q += n;
                অন্যথা
                        q++;
        }

        যদি (q > w) {
                l = লিখ(fd, w, q - w);
                যদি (l < 0)
                        ফেরত -errno;
                যদি (l != q - w)
                        ফেরত -EIO;
        }

        ফেরত q - (ধ্রুবক uint8_t*) p;
}
