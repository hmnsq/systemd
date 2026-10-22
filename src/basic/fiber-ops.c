/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਅਨ੍ਤਰ੍ਭਾਵ <poll.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <threads.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fiber-ops.h"

ਸਥਿਰ thread_local ਅਚਲ FiberOps *fiber_ops = ਨਿਰਾ;

bool fiber_ops_is_ਸੈੱਟ(ਖਾਲੀ) {
        ਵਾਪਸ fiber_ops != ਨਿਰਾ;
}

ਖਾਲੀ fiber_ops_ਸੈੱਟ(ਅਚਲ FiberOps *ops) {
        fiber_ops = ops;
}

ਪੂਰਨ fiber_ops_ppoll(ਬਣਤਰ pollfd *fds, size_t n_fds, ਅਚਲ ਬਣਤਰ timespec *timeout, ਅਚਲ sigset_t *sigmask) {
        ਜੇ (fiber_ops)
                ਵਾਪਸ fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        ਵਾਪਸ RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(ਪੂਰਨ fd, ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2) {
        ਜੇ (fiber_ops)
                ਵਾਪਸ fiber_ops->ਪਠ(fd, buf, ਗਿਣਤੀ_2);

        ssize_t n = ਪਠ(fd, buf, ਗਿਣਤੀ_2);
        ਵਾਪਸ n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *buf, size_t ਗਿਣਤੀ_2) {
        ਜੇ (fiber_ops)
                ਵਾਪਸ fiber_ops->ਲਿਖ(fd, buf, ਗਿਣਤੀ_2);

        ਵਾਪਸ RET_NERRNO(ਲਿਖ(fd, buf, ਗਿਣਤੀ_2));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fiber_ops);

        ਵਾਪਸ fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ਪ੍ਰਤਿਜ੍ਞਾ(fiber_ops);

        ਵਾਪਸ fiber_ops->timeout_unref(timer);
}
