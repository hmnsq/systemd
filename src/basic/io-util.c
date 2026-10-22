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
        परत (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        परत (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

पूर्णांक flush_fd(पूर्णांक fd) {
        पूर्णांक मोजणी = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        साठी (;;) {
                अक्षर buf[LINE_MAX];
                ssize_t l;
                पूर्णांक r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                जर (r == -EINTR)
                        सुरूठेवा;
                जर (r < 0)
                        परत r;
                जर (r == 0)
                        परत मोजणी;

                l = पठ(fd, buf, आकार(buf));
                जर (l < 0) {
                        जर (त्रुटि_सं == EINTR)
                                सुरूठेवा;
                        जर (त्रुटि_सं == EAGAIN)
                                परत मोजणी;

                        परत -त्रुटि_सं;
                } अन्यथा जर (l == 0)
                        परत मोजणी;

                जर (l > पूर्णांक_उच्च-मोजणी) /* On overflow terminate */
                        परत पूर्णांक_उच्च;

                मोजणी += (पूर्णांक) l;
        }
}

ssize_t loop_read(पूर्णांक fd, शून्य *buf, माप_प्रकार nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        प्रतिज्ञा(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        जर (nbytes > (माप_प्रकार) SSIZE_MAX)
                परत -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        पूर्णांक flags = 0;
        जर (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                जर (flags < 0)
                        परत -त्रुटि_सं;
        }

        करा {
                ssize_t k;

                जर (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        जर (k < 0)
                                परत n > 0 ? n : k;
                } अन्यथा {
                        k = पठ(fd, p, nbytes);
                        जर (k < 0) {
                                जर (त्रुटि_सं == EINTR)
                                        सुरूठेवा;

                                जर (त्रुटि_सं == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (शून्य) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        सुरूठेवा;
                                }

                                परत n > 0 ? n : -त्रुटि_सं;
                        }
                }

                जर (k == 0)
                        परत n;

                प्रतिज्ञा((माप_प्रकार) k <= nbytes);
                प्रतिज्ञा(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } पर्यंत (nbytes > 0);

        परत n;
}

पूर्णांक loop_read_exact(पूर्णांक fd, शून्य *buf, माप_प्रकार nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        जर (n < 0)
                परत (पूर्णांक) n;
        जर ((माप_प्रकार) n != nbytes)
                परत -EIO;

        परत 0;
}

पूर्णांक loop_write_full(पूर्णांक fd, स्थिरांक शून्य *buf, माप_प्रकार nbytes, usec_t timeout) {
        स्थिरांक uint8_t *p;
        usec_t end;
        पूर्णांक r;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(buf || nbytes == 0);

        जर (nbytes == 0) {
                स्थिर स्थिरांक dummy_t dummy[0];
                assert_cc(आकार(dummy) == 0);
                p = (स्थिरांक शून्य*) dummy; /* Some valid pointer, in case NULL was specified */
        } अन्यथा {
                जर (nbytes == SIZE_MAX)
                        nbytes = म_माप(buf);
                अन्यथा जर (_unlikely_(nbytes > (माप_प्रकार) SSIZE_MAX))
                        परत -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        पूर्णांक flags = 0;
        जर (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                जर (flags < 0)
                        परत -त्रुटि_सं;
        }

        जर (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                पर्यंत (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        जर (k < 0)
                                परत (पूर्णांक) k;
                        जर (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                परत -EIO;

                        प्रतिज्ञा((माप_प्रकार) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                परत 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        करा {
                ssize_t k;

                k = लिख(fd, p, nbytes);
                जर (k < 0) {
                        जर (त्रुटि_सं == EINTR)
                                सुरूठेवा;

                        जर (त्रुटि_सं != EAGAIN || timeout == 0)
                                परत -त्रुटि_सं;

                        usec_t wait_for;

                        जर (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        अन्यथा {
                                usec_t t = now(CLOCK_MONOTONIC);
                                जर (t >= end)
                                        परत -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        जर (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                सुरूठेवा;
                        जर (r < 0)
                                परत r;
                        जर (r == 0)
                                परत -ETIME;
                        सुरूठेवा;
                }

                जर (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        परत -EIO;

                प्रतिज्ञा((माप_प्रकार) k <= nbytes);

                p += k;
                nbytes -= k;
        } पर्यंत (nbytes > 0);

        परत 0;
}

पूर्णांक pwritev_full(पूर्णांक fd, संरचना iovec *iovec, माप_प्रकार n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        पूर्णांक r = 0;

        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        साठी (माप_प्रकार i = 0;;) {
                माप_प्रकार m;
                ssize_t k;

                पर्यंत (i < n && !iovec_is_set(iovec + i))
                        i++;
                जर (i >= n)
                        थांबा;

                m = MIN(n - i, (माप_प्रकार) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                जर (k < 0) {
                        जर (त्रुटि_सं == EINTR)
                                सुरूठेवा;

                        r = -त्रुटि_सं;
                        थांबा;
                }
                जर (k == 0) {
                        r = -EIO;
                        थांबा;
                }

                written += k;
                (शून्य) iovec_inc_many(iovec + i, m, k);
        }

        जर (ret_written)
                *ret_written = written;
        परत r;
}

पूर्णांक pwrite_full(पूर्णांक fd, स्थिरांक शून्य *buf, माप_प्रकार nbytes, uint64_t offset) {
        प्रतिज्ञा(fd >= 0);
        प्रतिज्ञा(buf || nbytes == 0);

        परत pwritev_full(fd, &IOVEC_MAKE((शून्य*) buf, nbytes), 1, offset, /* ret_written= */ रिक्त);
}

पूर्णांक pipe_eof(पूर्णांक fd) {
        पूर्णांक r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        जर (r <= 0)
                परत r;

        परत !!(r & POLLHUP);
}

पूर्णांक ppoll_usec_full(संरचना pollfd *fds, माप_प्रकार n_fds, usec_t timeout, स्थिरांक sigset_t *ss) {
        पूर्णांक r;

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

        जर (n_fds == 0 && timeout == 0)
                परत 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? रिक्त : TIMESPEC_STORE(timeout), ss);
        जर (r <= 0)
                परत r;

        साठी (माप_प्रकार i = 0, n = r; i < n_fds && n > 0; i++) {
                जर (fds[i].revents == 0)
                        सुरूठेवा;
                जर (fds[i].revents & POLLNVAL)
                        परत -EBADF;
                n--;
        }

        परत r;
}

पूर्णांक fd_wait_for_event(पूर्णांक fd, पूर्णांक event, usec_t timeout) {
        संरचना pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        पूर्णांक r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        जर (r <= 0)
                परत r;

        परत pollfd.revents;
}

स्थिर माप_प्रकार nul_length(स्थिरांक uint8_t *p, माप_प्रकार sz) {
        माप_प्रकार n = 0;

        प्रतिज्ञा(p);

        पर्यंत (sz > 0) {
                जर (*p != 0)
                        थांबा;

                n++;
                p++;
                sz--;
        }

        परत n;
}

ssize_t sparse_write(पूर्णांक fd, स्थिरांक शून्य *p, माप_प्रकार sz, माप_प्रकार run_length) {
        स्थिरांक uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        पर्यंत (q < e) {
                माप_प्रकार n;

                n = nul_length(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                जर ((n > run_length) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        जर (q > w) {
                                l = लिख(fd, w, q - w);
                                जर (l < 0)
                                        परत -त्रुटि_सं;
                                जर (l != q -w)
                                        परत -EIO;
                        }

                        जर (lseek(fd, n, प्रस्तुत_से) < 0)
                                परत -त्रुटि_सं;

                        q += n;
                        w = q;
                } अन्यथा जर (n > 0)
                        q += n;
                अन्यथा
                        q++;
        }

        जर (q > w) {
                l = लिख(fd, w, q - w);
                जर (l < 0)
                        परत -त्रुटि_सं;
                जर (l != q - w)
                        परत -EIO;
        }

        परत q - (स्थिरांक uint8_t*) p;
}
