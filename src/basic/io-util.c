/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <sys/epoll.h>          /* IWYU pragma: keep */
#അന്തര്ഭാവ <time.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fiber-ops.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "iovec-util.h"
#അന്തര്ഭാവ "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        തിരികെ (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        തിരികെ (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

പൂർണ്ണസംഖ്യ flush_fd(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ എണ്ണം = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        വേണ്ടി (;;) {
                അക്ഷരം buf[LINE_MAX];
                ssize_t l;
                പൂർണ്ണസംഖ്യ r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                എങ്കിൽ (r == -EINTR)
                        തുടരുക;
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        തിരികെ എണ്ണം;

                l = പഠ(fd, buf, വലുപ്പം(buf));
                എങ്കിൽ (l < 0) {
                        എങ്കിൽ (errno == EINTR)
                                തുടരുക;
                        എങ്കിൽ (errno == EAGAIN)
                                തിരികെ എണ്ണം;

                        തിരികെ -errno;
                } അല്ലെങ്കിൽ എങ്കിൽ (l == 0)
                        തിരികെ എണ്ണം;

                എങ്കിൽ (l > INT_MAX-എണ്ണം) /* On overflow terminate */
                        തിരികെ INT_MAX;

                എണ്ണം += (പൂർണ്ണസംഖ്യ) l;
        }
}

ssize_t loop_read(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        പ്രതിജ്ഞാ(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        എങ്കിൽ (nbytes > (size_t) SSIZE_MAX)
                തിരികെ -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        പൂർണ്ണസംഖ്യ flags = 0;
        എങ്കിൽ (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                എങ്കിൽ (flags < 0)
                        തിരികെ -errno;
        }

        ചെയ്യുക {
                ssize_t k;

                എങ്കിൽ (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        എങ്കിൽ (k < 0)
                                തിരികെ n > 0 ? n : k;
                } അല്ലെങ്കിൽ {
                        k = പഠ(fd, p, nbytes);
                        എങ്കിൽ (k < 0) {
                                എങ്കിൽ (errno == EINTR)
                                        തുടരുക;

                                എങ്കിൽ (errno == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (ശൂന്യം) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        തുടരുക;
                                }

                                തിരികെ n > 0 ? n : -errno;
                        }
                }

                എങ്കിൽ (k == 0)
                        തിരികെ n;

                പ്രതിജ്ഞാ((size_t) k <= nbytes);
                പ്രതിജ്ഞാ(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } ആകുമ്പോൾ (nbytes > 0);

        തിരികെ n;
}

പൂർണ്ണസംഖ്യ loop_read_exact(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        എങ്കിൽ (n < 0)
                തിരികെ (പൂർണ്ണസംഖ്യ) n;
        എങ്കിൽ ((size_t) n != nbytes)
                തിരികെ -EIO;

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ loop_write_full(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t nbytes, usec_t timeout) {
        സ്ഥിരം uint8_t *p;
        usec_t end;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(buf || nbytes == 0);

        എങ്കിൽ (nbytes == 0) {
                സ്ഥിതം സ്ഥിരം dummy_t dummy[0];
                assert_cc(വലുപ്പം(dummy) == 0);
                p = (സ്ഥിരം ശൂന്യം*) dummy; /* Some valid pointer, in case NULL was specified */
        } അല്ലെങ്കിൽ {
                എങ്കിൽ (nbytes == SIZE_MAX)
                        nbytes = strlen(buf);
                അല്ലെങ്കിൽ എങ്കിൽ (_unlikely_(nbytes > (size_t) SSIZE_MAX))
                        തിരികെ -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        പൂർണ്ണസംഖ്യ flags = 0;
        എങ്കിൽ (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                എങ്കിൽ (flags < 0)
                        തിരികെ -errno;
        }

        എങ്കിൽ (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                ആകുമ്പോൾ (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        എങ്കിൽ (k < 0)
                                തിരികെ (പൂർണ്ണസംഖ്യ) k;
                        എങ്കിൽ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                തിരികെ -EIO;

                        പ്രതിജ്ഞാ((size_t) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                തിരികെ 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        ചെയ്യുക {
                ssize_t k;

                k = ലിഖ(fd, p, nbytes);
                എങ്കിൽ (k < 0) {
                        എങ്കിൽ (errno == EINTR)
                                തുടരുക;

                        എങ്കിൽ (errno != EAGAIN || timeout == 0)
                                തിരികെ -errno;

                        usec_t wait_for;

                        എങ്കിൽ (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        അല്ലെങ്കിൽ {
                                usec_t t = now(CLOCK_MONOTONIC);
                                എങ്കിൽ (t >= end)
                                        തിരികെ -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        എങ്കിൽ (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                തുടരുക;
                        എങ്കിൽ (r < 0)
                                തിരികെ r;
                        എങ്കിൽ (r == 0)
                                തിരികെ -ETIME;
                        തുടരുക;
                }

                എങ്കിൽ (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        തിരികെ -EIO;

                പ്രതിജ്ഞാ((size_t) k <= nbytes);

                p += k;
                nbytes -= k;
        } ആകുമ്പോൾ (nbytes > 0);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ pwritev_full(പൂർണ്ണസംഖ്യ fd, ഘടന iovec *iovec, size_t n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        പൂർണ്ണസംഖ്യ r = 0;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        വേണ്ടി (size_t i = 0;;) {
                size_t m;
                ssize_t k;

                ആകുമ്പോൾ (i < n && !iovec_is_set(iovec + i))
                        i++;
                എങ്കിൽ (i >= n)
                        നിർത്തുക;

                m = MIN(n - i, (size_t) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                എങ്കിൽ (k < 0) {
                        എങ്കിൽ (errno == EINTR)
                                തുടരുക;

                        r = -errno;
                        നിർത്തുക;
                }
                എങ്കിൽ (k == 0) {
                        r = -EIO;
                        നിർത്തുക;
                }

                written += k;
                (ശൂന്യം) iovec_inc_many(iovec + i, m, k);
        }

        എങ്കിൽ (ret_written)
                *ret_written = written;
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ pwrite_full(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t nbytes, uint64_t offset) {
        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(buf || nbytes == 0);

        തിരികെ pwritev_full(fd, &IOVEC_MAKE((ശൂന്യം*) buf, nbytes), 1, offset, /* ret_written= */ ശൂന്യ);
}

പൂർണ്ണസംഖ്യ pipe_eof(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        തിരികെ !!(r & POLLHUP);
}

പൂർണ്ണസംഖ്യ ppoll_usec_full(ഘടന pollfd *fds, size_t n_fds, usec_t timeout, സ്ഥിരം sigset_t *ss) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fds || n_fds == 0);

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

        എങ്കിൽ (n_fds == 0 && timeout == 0)
                തിരികെ 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? ശൂന്യ : TIMESPEC_STORE(timeout), ss);
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        വേണ്ടി (size_t i = 0, n = r; i < n_fds && n > 0; i++) {
                എങ്കിൽ (fds[i].revents == 0)
                        തുടരുക;
                എങ്കിൽ (fds[i].revents & POLLNVAL)
                        തിരികെ -EBADF;
                n--;
        }

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ fd_wait_for_event(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ event, usec_t timeout) {
        ഘടന pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        പൂർണ്ണസംഖ്യ r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        തിരികെ pollfd.revents;
}

സ്ഥിതം size_t nul_length(സ്ഥിരം uint8_t *p, size_t sz) {
        size_t n = 0;

        പ്രതിജ്ഞാ(p);

        ആകുമ്പോൾ (sz > 0) {
                എങ്കിൽ (*p != 0)
                        നിർത്തുക;

                n++;
                p++;
                sz--;
        }

        തിരികെ n;
}

ssize_t sparse_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *p, size_t sz, size_t run_length) {
        സ്ഥിരം uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        ആകുമ്പോൾ (q < e) {
                size_t n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                എങ്കിൽ ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        എങ്കിൽ (q > w) {
                                l = ലിഖ(fd, w, q - w);
                                എങ്കിൽ (l < 0)
                                        തിരികെ -errno;
                                എങ്കിൽ (l != q -w)
                                        തിരികെ -EIO;
                        }

                        എങ്കിൽ (lseek(fd, n, SEEK_CUR) < 0)
                                തിരികെ -errno;

                        q += n;
                        w = q;
                } അല്ലെങ്കിൽ എങ്കിൽ (n > 0)
                        q += n;
                അല്ലെങ്കിൽ
                        q++;
        }

        എങ്കിൽ (q > w) {
                l = ലിഖ(fd, w, q - w);
                എങ്കിൽ (l < 0)
                        തിരികെ -errno;
                എങ്കിൽ (l != q - w)
                        തിരികെ -EIO;
        }

        തിരികെ q - (സ്ഥിരം uint8_t*) p;
}
