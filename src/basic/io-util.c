/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <sys/epoll.h>          /* IWYU pragma: keep */
#அந்தர்பாவ <time.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fiber-ops.h"
#அந்தர்பாவ "io-util.h"
#அந்தர்பாவ "iovec-util.h"
#அந்தர்பாவ "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        திரும்பு (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        திரும்பு (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

முழு flush_fd(முழு fd) {
        முழு மொத்தம் = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        க்கு (;;) {
                எழுத்து buf[LINE_MAX];
                ssize_t l;
                முழு r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                எனில் (r == -EINTR)
                        தொடர்;
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        திரும்பு மொத்தம்;

                l = பட(fd, buf, அளவு(buf));
                எனில் (l < 0) {
                        எனில் (errno == EINTR)
                                தொடர்;
                        எனில் (errno == EAGAIN)
                                திரும்பு மொத்தம்;

                        திரும்பு -errno;
                } இல்லையெனில் எனில் (l == 0)
                        திரும்பு மொத்தம்;

                எனில் (l > INT_MAX-மொத்தம்) /* On overflow terminate */
                        திரும்பு INT_MAX;

                மொத்தம் += (முழு) l;
        }
}

ssize_t loop_read(முழு fd, வெறுமை *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        ப்ரதிஜ்ஞா(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        எனில் (nbytes > (size_t) SSIZE_MAX)
                திரும்பு -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        முழு flags = 0;
        எனில் (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                எனில் (flags < 0)
                        திரும்பு -errno;
        }

        செய் {
                ssize_t k;

                எனில் (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        எனில் (k < 0)
                                திரும்பு n > 0 ? n : k;
                } இல்லையெனில் {
                        k = பட(fd, p, nbytes);
                        எனில் (k < 0) {
                                எனில் (errno == EINTR)
                                        தொடர்;

                                எனில் (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (வெறுமை) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        தொடர்;
                                }

                                திரும்பு n > 0 ? n : -errno;
                        }
                }

                எனில் (k == 0)
                        திரும்பு n;

                ப்ரதிஜ்ஞா((size_t) k <= nbytes);
                ப்ரதிஜ்ஞா(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } போது (nbytes > 0);

        திரும்பு n;
}

முழு loop_read_exact(முழு fd, வெறுமை *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        எனில் (n < 0)
                திரும்பு (முழு) n;
        எனில் ((size_t) n != nbytes)
                திரும்பு -EIO;

        திரும்பு 0;
}

முழு loop_write_full(முழு fd, மாறிலி வெறுமை *buf, size_t nbytes, usec_t timeout) {
        மாறிலி uint8_t *p;
        usec_t end;
        முழு r;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(buf || nbytes == 0);

        எனில் (nbytes == 0) {
                நிலையான மாறிலி dummy_t dummy[0];
                assert_cc(அளவு(dummy) == 0);
                p = (மாறிலி வெறுமை*) dummy; /* Some valid pointer, in case NULL was specified */
        } இல்லையெனில் {
                எனில் (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                இல்லையெனில் எனில் (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        திரும்பு -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        முழு flags = 0;
        எனில் (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                எனில் (flags < 0)
                        திரும்பு -errno;
        }

        எனில் (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                போது (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        எனில் (k < 0)
                                திரும்பு (முழு) k;
                        எனில் (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                திரும்பு -EIO;

                        ப்ரதிஜ்ஞா((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                திரும்பு 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        செய் {
                ssize_t k;

                k = லிக(fd, p, nbytes);
                எனில் (k < 0) {
                        எனில் (errno == EINTR)
                                தொடர்;

                        எனில் (errno != EAGAIN || timeout == 0)
                                திரும்பு -errno;

                        usec_t wait_for;

                        எனில் (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        இல்லையெனில் {
                                usec_t t = now(CLOCK_MONOTONIC);
                                எனில் (t >= end)
                                        திரும்பு -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        எனில் (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                தொடர்;
                        எனில் (r < 0)
                                திரும்பு r;
                        எனில் (r == 0)
                                திரும்பு -ETIME;
                        தொடர்;
                }

                எனில் (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        திரும்பு -EIO;

                ப்ரதிஜ்ஞா((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } போது (nbytes > 0);

        திரும்பு 0;
}

முழு pwritev_full(முழு fd, கட்டமைப்பு iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        முழு r = 0;

        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        க்கு (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                போது (i < n && !iovec_is_set(iovec + i))
                        i++;
                எனில் (i >= n)
                        நிறுத்து;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                எனில் (k < 0) {
                        எனில் (errno == EINTR)
                                தொடர்;

                        r = -errno;
                        நிறுத்து;
                }
                எனில் (k == 0) {
                        r = -EIO;
                        நிறுத்து;
                }

                written += k;
                (வெறுமை) iovec_inc_many(iovec + i, m, k);
        }

        எனில் (ret_written)
                *ret_written = written;
        திரும்பு r;
}

முழு pwrite_full(முழு fd, மாறிலி வெறுமை *buf, size_t nbytes, uint64_t offset) {
        ப்ரதிஜ்ஞா(fd >= 0);
        ப்ரதிஜ்ஞா(buf || nbytes == 0);

        திரும்பு pwritev_full(fd, &IOVEC_MAKE((வெறுமை*) buf, nbytes), 1, offset, /* ret_written= */ வெற்று);
}

முழு pipe_eof(முழு fd) {
        முழு r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        எனில் (r <= 0)
                திரும்பு r;

        திரும்பு !!(r & POLLHUP);
}

முழு ppoll_usec_full(கட்டமைப்பு pollfd *fds, size_t n_fds, usec_t timeout, மாறிலி sigset_t *ss) {
        முழு r;

        ப்ரதிஜ்ஞா(fds || n_fds == 0);

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

        எனில் (n_fds == 0 && timeout == 0)
                திரும்பு 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? வெற்று : TIMESPEC_STORE(timeout), ss);
        எனில் (r <= 0)
                திரும்பு r;

        க்கு (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                எனில் (fds[i].revents == 0)
                        தொடர்;
                எனில் (fds[i].revents & POLLNVAL)
                        திரும்பு -EBADF;
                n--;
        }

        திரும்பு r;
}

முழு fd_wait_for_event(முழு fd, முழு event, usec_t timeout) {
        கட்டமைப்பு pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        முழு r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        எனில் (r <= 0)
                திரும்பு r;

        திரும்பு pollfd.revents;
}

நிலையான size_t nul_length(மாறிலி uint8_t *p, size_t sz) {
        size_t n = 0;

        ப்ரதிஜ்ஞா(p);

        போது (sz > 0) {
                எனில் (*p != 0)
                        நிறுத்து;

                n++;
                p++;
                sz--;
        }

        திரும்பு n;
}

ssize_t sparse_write(முழு fd, மாறிலி வெறுமை *p, size_t sz, size_t run_length) {
        மாறிலி uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        போது (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                எனில் ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        எனில் (q > w) {
                                l = லிக(fd, w, q - w);
                                எனில் (l < 0)
                                        திரும்பு -errno;
                                எனில் (l != q -w)
                                        திரும்பு -EIO;
                        }

                        எனில் (lseek(fd, n, SEEK_CUR) < 0)
                                திரும்பு -errno;

                        q += n;
                        w = q;
                } இல்லையெனில் எனில் (n > 0)
                        q += n;
                இல்லையெனில்
                        q++;
        }

        எனில் (q > w) {
                l = லிக(fd, w, q - w);
                எனில் (l < 0)
                        திரும்பு -errno;
                எனில் (l != q - w)
                        திரும்பு -EIO;
        }

        திரும்பு q - (மாறிலி uint8_t*) p;
}
