/* SPDX-License-Identifier: LGPL-2.1-or-later */
#അന്തര്ഭാവ <poll.h>
#അന്തര്ഭാവ <threads.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fiber-ops.h"

സ്ഥിതം thread_local സ്ഥിരം FiberOps *fiber_ops = ശൂന്യ;

bool fiber_ops_is_set(ശൂന്യം) {
        തിരികെ fiber_ops != ശൂന്യ;
}

ശൂന്യം fiber_ops_set(സ്ഥിരം FiberOps *ops) {
        fiber_ops = ops;
}

പൂർണ്ണസംഖ്യ fiber_ops_ppoll(ഘടന pollfd *fds, size_t n_fds, സ്ഥിരം ഘടന timespec *timeout, സ്ഥിരം sigset_t *sigmask) {
        എങ്കിൽ (fiber_ops)
                തിരികെ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        തിരികെ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buf, size_t എണ്ണം) {
        എങ്കിൽ (fiber_ops)
                തിരികെ fiber_ops->പഠ(fd, buf, എണ്ണം);

        ssize_t n = പഠ(fd, buf, എണ്ണം);
        തിരികെ n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buf, size_t എണ്ണം) {
        എങ്കിൽ (fiber_ops)
                തിരികെ fiber_ops->ലിഖ(fd, buf, എണ്ണം);

        തിരികെ RET_NERRNO(ലിഖ(fd, buf, എണ്ണം));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        പ്രതിജ്ഞാ(fiber_ops);

        തിരികെ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        പ്രതിജ്ഞാ(fiber_ops);

        തിരികെ fiber_ops->timeout_unref(timer);
}
