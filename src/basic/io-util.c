/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <poll.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <sys/epoll.h>          /* IWYU pragma: keep */
#अन्तर्भाव <time.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "fiber-ops.h"
#अन्तर्भाव "io-util.h"
#अन्तर्भाव "iovec-util.h"
#अन्तर्भाव "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        प्रत्यागम (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        प्रत्यागम (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

पूर्णाङ्क flush_fd(पूर्णाङ्क fd) {
        पूर्णाङ्क गिनती = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        प्रति (;;) {
                वर्ण buf[LINE_MAX];
                ssize_t l;
                पूर्णाङ्क r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                यदि (r == -EINTR)
                        अनुवर्त;
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        प्रत्यागम गिनती;

                l = पठ(fd, buf, परिमाण(buf));
                यदि (l < 0) {
                        यदि (त्रुटि_सं == EINTR)
                                अनुवर्त;
                        यदि (त्रुटि_सं == EAGAIN)
                                प्रत्यागम गिनती;

                        प्रत्यागम -त्रुटि_सं;
                } अन्यथा यदि (l == 0)
                        प्रत्यागम गिनती;

                यदि (l > पूर्णांक_उच्च-गिनती) /* On overflow terminate */
                        प्रत्यागम पूर्णांक_उच्च;

                गिनती += (पूर्णाङ्क) l;
        }
}

ssize_t loop_read(पूर्णाङ्क fd, शून्य *buf, माप_प्रकार nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        प्रतिज्ञा(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        यदि (nbytes > (माप_प्रकार) SSIZE_MAX)
                प्रत्यागम -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        पूर्णाङ्क flags = 0;
        यदि (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                यदि (flags < 0)
                        प्रत्यागम -त्रुटि_सं;
        }

        कुरु {
                ssize_t k;

                यदि (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        यदि (k < 0)
                                प्रत्यागम n > 0 ? n : k;
                } अन्यथा {
                        k = पठ(fd, p, nbytes);
                        यदि (k < 0) {
                                यदि (त्रुटि_सं == EINTR)
                                        अनुवर्त;

                                यदि (त्रुटि_सं == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (शून्य) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        अनुवर्त;
                                }

                                प्रत्यागम n > 0 ? n : -त्रुटि_सं;
                        }
                }

                यदि (k == 0)
                        प्रत्यागम n;

                प्रतिज्ञा((माप_प्रकार) k <= nbytes);
                प्रतिज्ञा(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } यावत् (nbytes > 0);

        प्रत्यागम n;
}

पूर्णाङ्क loop_read_exact(पूर्णाङ्क fd, शून्य *buf, माप_प्रकार nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        यदि (n < 0)
                प्रत्यागम (पूर्णाङ्क) n;
        यदि ((माप_प्रकार) n != nbytes)
                प्रत्यागम -EIO;

        प्रत्यागम 0;
}

पूर्णाङ्क loop_write_full(पूर्णाङ्क fd, ध्रुव शून्य *buf, माप_प्रकार nbytes, usec_t timeout) {
        ध्रुव uint8_t *p;
        usec_t end;
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(buf || nbytes == 0);

        यदि (nbytes == 0) {
                स्थिर ध्रुव dummy_t dummy[0];
                assert_cc(परिमाण(dummy) == 0);
                p = (ध्रुव शून्य*) dummy; /* Some valid pointer, in case NULL was specified */
        } अन्यथा {
                यदि (nbytes == SIZE_MAX)
                        nbytes = म_माप(buf);
                अन्यथा यदि (_unlikely_(nbytes > (माप_प्रकार) SSIZE_MAX))
                        प्रत्यागम -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        पूर्णाङ्क flags = 0;
        यदि (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                यदि (flags < 0)
                        प्रत्यागम -त्रुटि_सं;
        }

        यदि (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                यावत् (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        यदि (k < 0)
                                प्रत्यागम (पूर्णाङ्क) k;
                        यदि (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                प्रत्यागम -EIO;

                        प्रतिज्ञा((माप_प्रकार) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                प्रत्यागम 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        कुरु {
                ssize_t k;

                k = लिख(fd, p, nbytes);
                यदि (k < 0) {
                        यदि (त्रुटि_सं == EINTR)
                                अनुवर्त;

                        यदि (त्रुटि_सं != EAGAIN || timeout == 0)
                                प्रत्यागम -त्रुटि_सं;

                        usec_t wait_for;

                        यदि (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        अन्यथा {
                                usec_t t = now(CLOCK_MONOTONIC);
                                यदि (t >= end)
                                        प्रत्यागम -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        यदि (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                अनुवर्त;
                        यदि (r < 0)
                                प्रत्यागम r;
                        यदि (r == 0)
                                प्रत्यागम -ETIME;
                        अनुवर्त;
                }

                यदि (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        प्रत्यागम -EIO;

                प्रतिज्ञा((माप_प्रकार) k <= nbytes);

                p += k;
                nbytes -= k;
        } यावत् (nbytes > 0);

        प्रत्यागम 0;
}

पूर्णाङ्क pwritev_full(पूर्णाङ्क fd, संरचना iovec *iovec, माप_प्रकार n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        पूर्णाङ्क r = 0;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        प्रति (माप_प्रकार i = 0;;) {
                माप_प्रकार m;
                ssize_t k;

                यावत् (i < n && !iovec_is_set(iovec + i))
                        i++;
                यदि (i >= n)
                        भङ्ग;

                m = MIN(n - i, (माप_प्रकार) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                यदि (k < 0) {
                        यदि (त्रुटि_सं == EINTR)
                                अनुवर्त;

                        r = -त्रुटि_सं;
                        भङ्ग;
                }
                यदि (k == 0) {
                        r = -EIO;
                        भङ्ग;
                }

                written += k;
                (शून्य) iovec_inc_many(iovec + i, m, k);
        }

        यदि (ret_written)
                *ret_written = written;
        प्रत्यागम r;
}

पूर्णाङ्क pwrite_full(पूर्णाङ्क fd, ध्रुव शून्य *buf, माप_प्रकार nbytes, uint64_t offset) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(buf || nbytes == 0);

        प्रत्यागम pwritev_full(fd, &IOVEC_MAKE((शून्य*) buf, nbytes), 1, offset, /* ret_written= */ रिक्त);
}

पूर्णाङ्क pipe_eof(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        यदि (r <= 0)
                प्रत्यागम r;

        प्रत्यागम !!(r & POLLHUP);
}

पूर्णाङ्क ppoll_usec_full(संरचना pollfd *fds, माप_प्रकार n_fds, usec_t timeout, ध्रुव sigset_t *ss) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fds || n_fds == 0);

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

        यदि (n_fds == 0 && timeout == 0)
                प्रत्यागम 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? रिक्त : TIMESPEC_STORE(timeout), ss);
        यदि (r <= 0)
                प्रत्यागम r;

        प्रति (माप_प्रकार i = 0, n = r; i < n_fds && n > 0; i++) {
                यदि (fds[i].revents == 0)
                        अनुवर्त;
                यदि (fds[i].revents & POLLNVAL)
                        प्रत्यागम -EBADF;
                n--;
        }

        प्रत्यागम r;
}

पूर्णाङ्क fd_wait_for_event(पूर्णाङ्क fd, पूर्णाङ्क event, usec_t timeout) {
        संरचना pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        पूर्णाङ्क r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        यदि (r <= 0)
                प्रत्यागम r;

        प्रत्यागम pollfd.revents;
}

स्थिर माप_प्रकार nul_length(ध्रुव uint8_t *p, माप_प्रकार sz) {
        माप_प्रकार n = 0;

        प्रतिज्ञा(p);

        यावत् (sz > 0) {
                यदि (*p != 0)
                        भङ्ग;

                n++;
                p++;
                sz--;
        }

        प्रत्यागम n;
}

ssize_t sparse_write(पूर्णाङ्क fd, ध्रुव शून्य *p, माप_प्रकार sz, माप_प्रकार run_length) {
        ध्रुव uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        यावत् (q < e) {
                माप_प्रकार n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                यदि ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        यदि (q > w) {
                                l = लिख(fd, w, q - w);
                                यदि (l < 0)
                                        प्रत्यागम -त्रुटि_सं;
                                यदि (l != q -w)
                                        प्रत्यागम -EIO;
                        }

                        यदि (lseek(fd, n, प्रस्तुत_से) < 0)
                                प्रत्यागम -त्रुटि_सं;

                        q += n;
                        w = q;
                } अन्यथा यदि (n > 0)
                        q += n;
                अन्यथा
                        q++;
        }

        यदि (q > w) {
                l = लिख(fd, w, q - w);
                यदि (l < 0)
                        प्रत्यागम -त्रुटि_सं;
                यदि (l != q - w)
                        प्रत्यागम -EIO;
        }

        प्रत्यागम q - (ध्रुव uint8_t*) p;
}
