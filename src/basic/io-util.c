/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <fcntl.h>
#समावेश <poll.h>
#समावेश <stdio.h>
#समावेश <string.h>
#समावेश <sys/epoll.h>          /* IWYU pragma: keep */
#समावेश <time.h>
#समावेश <unistd.h>

#समावेश "errno-util.h"
#समावेश "fiber-ops.h"
#समावेश "io-util.h"
#समावेश "iovec-util.h"
#समावेश "time-util.h"

uint32_t poll_events_to_epoll(uint32_t events) {
        वापस (events & POLLIN    ? EPOLLIN    : 0) |
               (events & POLLOUT   ? EPOLLOUT   : 0) |
               (events & POLLPRI   ? EPOLLPRI   : 0) |
               (events & POLLERR   ? EPOLLERR   : 0) |
               (events & POLLHUP   ? EPOLLHUP   : 0) |
               (events & POLLRDHUP ? EPOLLRDHUP : 0);
}

uint32_t epoll_events_to_poll(uint32_t events) {
        वापस (events & EPOLLIN    ? POLLIN    : 0) |
               (events & EPOLLOUT   ? POLLOUT   : 0) |
               (events & EPOLLPRI   ? POLLPRI   : 0) |
               (events & EPOLLERR   ? POLLERR   : 0) |
               (events & EPOLLHUP   ? POLLHUP   : 0) |
               (events & EPOLLRDHUP ? POLLRDHUP : 0);
}

पूर्णांक flush_fd(पूर्णांक fd) {
        पूर्णांक गिनती = 0;

        /* Read from the specified file descriptor, until POLLIN is not set anymore, throwing away everything
         * read. Note that some file descriptors (notable IP sockets) will trigger POLLIN even when no data can be read
         * (due to IP packet checksum mismatches), hence this function is only safe to be non-blocking if the fd used
         * was set to non-blocking too. */

        क्रम (;;) {
                अक्षर buf[LINE_MAX];
                ssize_t l;
                पूर्णांक r;

                r = fd_wait_for_event(fd, POLLIN, 0);
                अगर (r == -EINTR)
                        जारी;
                अगर (r < 0)
                        वापस r;
                अगर (r == 0)
                        वापस गिनती;

                l = पढ़ो(fd, buf, माप(buf));
                अगर (l < 0) {
                        अगर (त्रुटि_सं == EINTR)
                                जारी;
                        अगर (त्रुटि_सं == EAGAIN)
                                वापस गिनती;

                        वापस -त्रुटि_सं;
                } अन्यथा अगर (l == 0)
                        वापस गिनती;

                अगर (l > पूर्णांक_उच्च-गिनती) /* On overflow terminate */
                        वापस पूर्णांक_उच्च;

                गिनती += (पूर्णांक) l;
        }
}

ssize_t loop_read(पूर्णांक fd, व्योम *buf, माप_प्रकार nbytes, bool do_poll) {
        uint8_t *p = ASSERT_PTR(buf);
        ssize_t n = 0;

        निश्चित(fd >= 0);

        /* If called with nbytes == 0, let's call read() at least once, to validate the operation */

        अगर (nbytes > (माप_प्रकार) SSIZE_MAX)
                वापस -EINVAL;

        /* do_poll == false means "don't wait, return what we have if EAGAIN". If the fd is already
         * non-blocking, read() can't block the thread, so the non-fiber path satisfies that semantic
         * correctly even from a fiber. Only use the fiber path when the fd is blocking (where read()
         * would otherwise block the entire event loop). */
        पूर्णांक flags = 0;
        अगर (fiber_ops_is_set() && !do_poll) {
                flags = fcntl(fd, F_GETFL);
                अगर (flags < 0)
                        वापस -त्रुटि_सं;
        }

        करो {
                ssize_t k;

                अगर (fiber_ops_is_set() && (do_poll || !FLAGS_SET(flags, O_NONBLOCK))) {
                        /* On a fiber the read op suspends on EAGAIN until data is available, so we don't
                         * need a separate poll step or the do_poll knob. */
                        k = fiber_ops_read(fd, p, nbytes);
                        अगर (k < 0)
                                वापस n > 0 ? n : k;
                } अन्यथा {
                        k = पढ़ो(fd, p, nbytes);
                        अगर (k < 0) {
                                अगर (त्रुटि_सं == EINTR)
                                        जारी;

                                अगर (त्रुटि_सं == EAGAIN && do_poll) {

                                        /* We knowingly ignore any return value here,
                                         * and expect that any error/EOF is reported
                                         * via read() */

                                        (व्योम) fd_wait_for_event(fd, POLLIN, USEC_INFINITY);
                                        जारी;
                                }

                                वापस n > 0 ? n : -त्रुटि_सं;
                        }
                }

                अगर (k == 0)
                        वापस n;

                निश्चित((माप_प्रकार) k <= nbytes);
                निश्चित(k <= SSIZE_MAX - n);

                p += k;
                nbytes -= k;
                n += k;
        } जबतक (nbytes > 0);

        वापस n;
}

पूर्णांक loop_read_exact(पूर्णांक fd, व्योम *buf, माप_प्रकार nbytes, bool do_poll) {
        ssize_t n;

        n = loop_read(fd, buf, nbytes, do_poll);
        अगर (n < 0)
                वापस (पूर्णांक) n;
        अगर ((माप_प्रकार) n != nbytes)
                वापस -EIO;

        वापस 0;
}

पूर्णांक loop_write_full(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार nbytes, usec_t timeout) {
        स्थिर uint8_t *p;
        usec_t end;
        पूर्णांक r;

        निश्चित(fd >= 0);
        निश्चित(buf || nbytes == 0);

        अगर (nbytes == 0) {
                अटल स्थिर dummy_t dummy[0];
                assert_cc(माप(dummy) == 0);
                p = (स्थिर व्योम*) dummy; /* Some valid pointer, in case NULL was specified */
        } अन्यथा {
                अगर (nbytes == SIZE_MAX)
                        nbytes = म_माप(buf);
                अन्यथा अगर (_unlikely_(nbytes > (माप_प्रकार) SSIZE_MAX))
                        वापस -EINVAL;

                p = buf;
        }

        /* timeout == 0 means "don't wait, return -EAGAIN if not ready". If the fd is already
         * non-blocking, write() can't block the thread, so the non-fiber path satisfies that
         * semantic correctly even from a fiber. Only use the fiber path when the fd is blocking
         * (where write() would otherwise block the entire event loop). */
        पूर्णांक flags = 0;
        अगर (fiber_ops_is_set() && timeout == 0) {
                flags = fcntl(fd, F_GETFL);
                अगर (flags < 0)
                        वापस -त्रुटि_सं;
        }

        अगर (fiber_ops_is_set() && !FLAGS_SET(flags, O_NONBLOCK)) {
                /* On a fiber the write op suspends on EAGAIN until the fd is writable; honor the
                 * caller's timeout via a deadline scope. */
                FIBER_OPS_TIMEOUT(timestamp_is_set(timeout) ? timeout : USEC_INFINITY);

                जबतक (nbytes > 0) {
                        ssize_t k = fiber_ops_write(fd, p, nbytes);
                        अगर (k < 0)
                                वापस (पूर्णांक) k;
                        अगर (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                                वापस -EIO;

                        निश्चित((माप_प्रकार) k <= nbytes);
                        p += k;
                        nbytes -= k;
                }

                वापस 0;
        }

        /* When timeout is 0 or USEC_INFINITY this is not used. But we initialize it to a sensible value. */
        end = timestamp_is_set(timeout) ? usec_add(now(CLOCK_MONOTONIC), timeout) : USEC_INFINITY;

        करो {
                ssize_t k;

                k = ग_लिखो(fd, p, nbytes);
                अगर (k < 0) {
                        अगर (त्रुटि_सं == EINTR)
                                जारी;

                        अगर (त्रुटि_सं != EAGAIN || timeout == 0)
                                वापस -त्रुटि_सं;

                        usec_t wait_for;

                        अगर (timeout == USEC_INFINITY)
                                wait_for = USEC_INFINITY;
                        अन्यथा {
                                usec_t t = now(CLOCK_MONOTONIC);
                                अगर (t >= end)
                                        वापस -ETIME;

                                wait_for = usec_sub_unsigned(end, t);
                        }

                        r = fd_wait_for_event(fd, POLLOUT, wait_for);
                        अगर (timeout == USEC_INFINITY || ERRNO_IS_NEG_TRANSIENT(r))
                                /* If timeout == USEC_INFINITY we knowingly ignore any return value
                                 * here, and expect that any error/EOF is reported via write() */
                                जारी;
                        अगर (r < 0)
                                वापस r;
                        अगर (r == 0)
                                वापस -ETIME;
                        जारी;
                }

                अगर (_unlikely_(nbytes > 0 && k == 0)) /* Can't really happen */
                        वापस -EIO;

                निश्चित((माप_प्रकार) k <= nbytes);

                p += k;
                nbytes -= k;
        } जबतक (nbytes > 0);

        वापस 0;
}

पूर्णांक pwritev_full(पूर्णांक fd, काष्ठा iovec *iovec, माप_प्रकार n, uint64_t offset, uint64_t *ret_written) {
        uint64_t written = 0;
        पूर्णांक r = 0;

        निश्चित(fd >= 0);
        निश्चित(iovec || n == 0);

        /* Writes all of the iovecs at the offset. The iovecs are advanced past what was written, and
         * 'ret_written' tells how much that was, also if the write fails halfway. */

        क्रम (माप_प्रकार i = 0;;) {
                माप_प्रकार m;
                ssize_t k;

                जबतक (i < n && !iovec_is_set(iovec + i))
                        i++;
                अगर (i >= n)
                        अवरोध;

                m = MIN(n - i, (माप_प्रकार) IOV_MAX);
                k = pwritev(fd, iovec + i, m, offset + written);
                अगर (k < 0) {
                        अगर (त्रुटि_सं == EINTR)
                                जारी;

                        r = -त्रुटि_सं;
                        अवरोध;
                }
                अगर (k == 0) {
                        r = -EIO;
                        अवरोध;
                }

                written += k;
                (व्योम) iovec_inc_many(iovec + i, m, k);
        }

        अगर (ret_written)
                *ret_written = written;
        वापस r;
}

पूर्णांक pwrite_full(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार nbytes, uint64_t offset) {
        निश्चित(fd >= 0);
        निश्चित(buf || nbytes == 0);

        वापस pwritev_full(fd, &IOVEC_MAKE((व्योम*) buf, nbytes), 1, offset, /* ret_written= */ शून्य);
}

पूर्णांक pipe_eof(पूर्णांक fd) {
        पूर्णांक r;

        r = fd_wait_for_event(fd, POLLIN, 0);
        अगर (r <= 0)
                वापस r;

        वापस !!(r & POLLHUP);
}

पूर्णांक ppoll_usec_full(काष्ठा pollfd *fds, माप_प्रकार n_fds, usec_t timeout, स्थिर sigset_t *ss) {
        पूर्णांक r;

        निश्चित(fds || n_fds == 0);

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

        अगर (n_fds == 0 && timeout == 0)
                वापस 0;

        r = fiber_ops_ppoll(fds, n_fds, timeout == USEC_INFINITY ? शून्य : TIMESPEC_STORE(timeout), ss);
        अगर (r <= 0)
                वापस r;

        क्रम (माप_प्रकार i = 0, n = r; i < n_fds && n > 0; i++) {
                अगर (fds[i].revents == 0)
                        जारी;
                अगर (fds[i].revents & POLLNVAL)
                        वापस -EBADF;
                n--;
        }

        वापस r;
}

पूर्णांक fd_wait_for_event(पूर्णांक fd, पूर्णांक event, usec_t timeout) {
        काष्ठा pollfd pollfd = {
                .fd = fd,
                .events = event,
        };
        पूर्णांक r;

        /* ⚠️ ⚠️ ⚠️ Keep in mind you almost certainly want to handle -EINTR gracefully in the caller, see
         * ppoll_usec() above! ⚠️ ⚠️ ⚠️ */

        r = ppoll_usec(&pollfd, 1, timeout);
        अगर (r <= 0)
                वापस r;

        वापस pollfd.revents;
}

अटल माप_प्रकार nul_लंबाई(स्थिर uint8_t *p, माप_प्रकार sz) {
        माप_प्रकार n = 0;

        निश्चित(p);

        जबतक (sz > 0) {
                अगर (*p != 0)
                        अवरोध;

                n++;
                p++;
                sz--;
        }

        वापस n;
}

ssize_t sparse_write(पूर्णांक fd, स्थिर व्योम *p, माप_प्रकार sz, माप_प्रकार run_लंबाई) {
        स्थिर uint8_t *q, *w, *e;
        ssize_t l;

        q = w = p;
        e = q + sz;
        जबतक (q < e) {
                माप_प्रकार n;

                n = nul_लंबाई(q, e - q);

                /* If there are more than the specified run length of
                 * NUL bytes, or if this is the beginning or the end
                 * of the buffer, then seek instead of write */
                अगर ((n > run_लंबाई) ||
                    (n > 0 && q == p) ||
                    (n > 0 && q + n >= e)) {
                        अगर (q > w) {
                                l = ग_लिखो(fd, w, q - w);
                                अगर (l < 0)
                                        वापस -त्रुटि_सं;
                                अगर (l != q -w)
                                        वापस -EIO;
                        }

                        अगर (lseek(fd, n, प्रस्तुत_से) < 0)
                                वापस -त्रुटि_सं;

                        q += n;
                        w = q;
                } अन्यथा अगर (n > 0)
                        q += n;
                अन्यथा
                        q++;
        }

        अगर (q > w) {
                l = ग_लिखो(fd, w, q - w);
                अगर (l < 0)
                        वापस -त्रुटि_सं;
                अगर (l != q - w)
                        वापस -EIO;
        }

        वापस q - (स्थिर uint8_t*) p;
}
