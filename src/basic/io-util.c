/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <poll.h>
#અન્તર્ભાવ <stdio.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <sys/epoll.h>          /* IWYU pragma: keep */
#અન્તર્ભાવ <time.h>
#અન્તર્ભાવ <unistd.h>

#અન્તર્ભાવ "errno-util.h"
#અન્તર્ભાવ "fiber-ops.h"
#અન્તર્ભાવ "io-util.h"
#અન્તર્ભાવ "iovec-util.h"
#અન્તર્ભાવ "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        પાછા (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        પાછા (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

પૂર્ણાંક flush_fd(પૂર્ણાંક fd) {
        પૂર્ણાંક ગણતરી = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        માટે (;;) {
                અક્ષર buf[LINE_MAX];
                ssize_t l;
                પૂર્ણાંક r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                જો (r == -EINTR)
                        ચાલુ;
                જો (r < 0)
                        પાછા r;
                જો (r == 0)
                        પાછા ગણતરી;

                l = પઠ(fd, buf, કદ(buf));
                જો (l < 0) {
                        જો (errno == EINTR)
                                ચાલુ;
                        જો (errno == EAGAIN)
                                પાછા ગણતરી;

                        પાછા -errno;
                } નહીંતર જો (l == 0)
                        પાછા ગણતરી;

                જો (l > INT_MAX-ગણતરી) /* On overflow terminate */
                        પાછા INT_MAX;

                ગણતરી += (પૂર્ણાંક) l;
        }
}

ssize_t loop_read(પૂર્ણાંક fd, શૂન્ય *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        પ્રતિજ્ઞા(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        જો (nbytes > (size_t) SSIZE_MAX)
                પાછા -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        પૂર્ણાંક flags = 0;
        જો (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                જો (flags < 0)
                        પાછા -errno;
        }

        કરો {
                ssize_t k;

                જો (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        જો (k < 0)
                                પાછા n > 0 ? n : k;
                } નહીંતર {
                        k = પઠ(fd, p, nbytes);
                        જો (k < 0) {
                                જો (errno == EINTR)
                                        ચાલુ;

                                જો (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (શૂન્ય) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        ચાલુ;
                                }

                                પાછા n > 0 ? n : -errno;
                        }
                }

                જો (k == 0)
                        પાછા n;

                પ્રતિજ્ઞા((size_t) k <= nbytes);
                પ્રતિજ્ઞા(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } જ્યારે (nbytes > 0);

        પાછા n;
}

પૂર્ણાંક loop_read_exact(પૂર્ણાંક fd, શૂન્ય *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        જો (n < 0)
                પાછા (પૂર્ણાંક) n;
        જો ((size_t) n != nbytes)
                પાછા -EIO;

        પાછા 0;
}

પૂર્ણાંક loop_write_full(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t nbytes, usec_t timeout) {
        અચલ uint8_t *p;
        usec_t end;
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(buf || nbytes == 0);

        જો (nbytes == 0) {
                સ્થિર અચલ dummy_t dummy[0];
                assert_cc(કદ(dummy) == 0);
                p = (અચલ શૂન્ય*) dummy; /* Some valid pointer, in case NULL was specified */
        } નહીંતર {
                જો (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                નહીંતર જો (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        પાછા -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        પૂર્ણાંક flags = 0;
        જો (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                જો (flags < 0)
                        પાછા -errno;
        }

        જો (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                જ્યારે (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        જો (k < 0)
                                પાછા (પૂર્ણાંક) k;
                        જો (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                પાછા -EIO;

                        પ્રતિજ્ઞા((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                પાછા 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        કરો {
                ssize_t k;

                k = લિખ(fd, p, nbytes);
                જો (k < 0) {
                        જો (errno == EINTR)
                                ચાલુ;

                        જો (errno != EAGAIN || timeout == 0)
                                પાછા -errno;

                        usec_t wait_for;

                        જો (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        નહીંતર {
                                usec_t t = now(CLOCK_MONOTONIC);
                                જો (t >= end)
                                        પાછા -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        જો (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                ચાલુ;
                        જો (r < 0)
                                પાછા r;
                        જો (r == 0)
                                પાછા -ETIME;
                        ચાલુ;
                }

                જો (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        પાછા -EIO;

                પ્રતિજ્ઞા((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } જ્યારે (nbytes > 0);

        પાછા 0;
}

પૂર્ણાંક pwritev_full(પૂર્ણાંક fd, સંરચના iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        પૂર્ણાંક r = 0;

        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        માટે (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                જ્યારે (i < n && !iovec_is_set(iovec + i))
                        i++;
                જો (i >= n)
                        તોડો;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                જો (k < 0) {
                        જો (errno == EINTR)
                                ચાલુ;

                        r = -errno;
                        તોડો;
                }
                જો (k == 0) {
                        r = -EIO;
                        તોડો;
                }

                written += k;
                (શૂન્ય) iovec_inc_many(iovec + i, m, k);
        }

        જો (ret_written)
                *ret_written = written;
        પાછા r;
}

પૂર્ણાંક pwrite_full(પૂર્ણાંક fd, અચલ શૂન્ય *buf, size_t nbytes, uint64_t offset) {
        પ્રતિજ્ઞા(fd >= 0);
        પ્રતિજ્ઞા(buf || nbytes == 0);

        પાછા pwritev_full(fd, &IOVEC_MAKE((શૂન્ય*) buf, nbytes), 1, offset, /* ret_written= */ ખાલી);
}

પૂર્ણાંક pipe_eof(પૂર્ણાંક fd) {
        પૂર્ણાંક r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        જો (r <= 0)
                પાછા r;

        પાછા !!(r & POLLHUP);
}

પૂર્ણાંક ppoll_usec_full(સંરચના pollfd *fds, size_t n_fds, usec_t timeout, અચલ sigset_t *ss) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fds || n_fds == 0);

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

        જો (n_fds == 0 && timeout == 0)
                પાછા 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ખાલી : TIMESPEC_STORE(timeout), ss);
        જો (r <= 0)
                પાછા r;

        માટે (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                જો (fds[i].revents == 0)
                        ચાલુ;
                જો (fds[i].revents & POLLNVAL)
                        પાછા -EBADF;
                n--;
        }

        પાછા r;
}

પૂર્ણાંક fd_wait_for_event(પૂર્ણાંક fd, પૂર્ણાંક event, usec_t timeout) {
        સંરચના pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        પૂર્ણાંક r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        જો (r <= 0)
                પાછા r;

        પાછા pollfd.revents;
}

સ્થિર size_t nul_length(અચલ uint8_t *p, size_t sz) {
        size_t n = 0;

        પ્રતિજ્ઞા(p);

        જ્યારે (sz > 0) {
                જો (*p != 0)
                        તોડો;

                n++;
                p++;
                sz--;
        }

        પાછા n;
}

ssize_t sparse_write(પૂર્ણાંક fd, અચલ શૂન્ય *p, size_t sz, size_t run_length) {
        અચલ uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        જ્યારે (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                જો ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        જો (q > w) {
                                l = લિખ(fd, w, q - w);
                                જો (l < 0)
                                        પાછા -errno;
                                જો (l != q -w)
                                        પાછા -EIO;
                        }

                        જો (lseek(fd, n, SEEK_CUR) < 0)
                                પાછા -errno;

                        q += n;
                        w = q;
                } નહીંતર જો (n > 0)
                        q += n;
                નહીંતર
                        q++;
        }

        જો (q > w) {
                l = લિખ(fd, w, q - w);
                જો (l < 0)
                        પાછા -errno;
                જો (l != q - w)
                        પાછા -EIO;
        }

        પાછા q - (અચલ uint8_t*) p;
}
