/* SPDX-License-Identifier: LGPL-2.1-or-later */
#அந்தர்பாவ <poll.h>
#அந்தர்பாவ <threads.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "fiber-ops.h"

நிலையான thread_local மாறிலி FiberOps *fiber_ops = வெற்று;

bool fiber_ops_is_set(வெறுமை) {
        திரும்பு fiber_ops != வெற்று;
}

வெறுமை fiber_ops_set(மாறிலி FiberOps *ops) {
        fiber_ops = ops;
}

முழு fiber_ops_ppoll(கட்டமைப்பு pollfd *fds, size_t n_fds, மாறிலி கட்டமைப்பு timespec *timeout, மாறிலி sigset_t *sigmask) {
        எனில் (fiber_ops)
                திரும்பு fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        திரும்பு RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(முழு fd, வெறுமை *buf, size_t மொத்தம்) {
        எனில் (fiber_ops)
                திரும்பு fiber_ops->பட(fd, buf, மொத்தம்);

        ssize_t n = பட(fd, buf, மொத்தம்);
        திரும்பு n < 0 ? -errno : n;
}

ssize_t fiber_ops_write(முழு fd, மாறிலி வெறுமை *buf, size_t மொத்தம்) {
        எனில் (fiber_ops)
                திரும்பு fiber_ops->லிக(fd, buf, மொத்தம்);

        திரும்பு RET_NERRNO(லிக(fd, buf, மொத்தம்));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        ப்ரதிஜ்ஞா(fiber_ops);

        திரும்பு fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        ப்ரதிஜ்ஞா(fiber_ops);

        திரும்பு fiber_ops->timeout_unref(timer);
}
