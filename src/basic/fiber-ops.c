/* SPDX-License-Identifier: LGPL-2.1-or-later */
#समावेश <poll.h>
#समावेश <threads.h>
#समावेश <unistd.h>

#समावेश "errno-util.h"
#समावेश "fiber-ops.h"

अटल thread_local स्थिर FiberOps *fiber_ops = शून्य;

bool fiber_ops_is_set(व्योम) {
        वापस fiber_ops != शून्य;
}

व्योम fiber_ops_set(स्थिर FiberOps *ops) {
        fiber_ops = ops;
}

पूर्णांक fiber_ops_ppoll(काष्ठा pollfd *fds, माप_प्रकार n_fds, स्थिर काष्ठा timespec *timeout, स्थिर sigset_t *sigmask) {
        अगर (fiber_ops)
                वापस fiber_ops->ppoll(fds, n_fds, timeout, sigmask);

        वापस RET_NERRNO(ppoll(fds, n_fds, timeout, sigmask));
}

ssize_t fiber_ops_read(पूर्णांक fd, व्योम *buf, माप_प्रकार गिनती) {
        अगर (fiber_ops)
                वापस fiber_ops->पढ़ो(fd, buf, गिनती);

        ssize_t n = पढ़ो(fd, buf, गिनती);
        वापस n < 0 ? -त्रुटि_सं : n;
}

ssize_t fiber_ops_write(पूर्णांक fd, स्थिर व्योम *buf, माप_प्रकार गिनती) {
        अगर (fiber_ops)
                वापस fiber_ops->ग_लिखो(fd, buf, गिनती);

        वापस RET_NERRNO(ग_लिखो(fd, buf, गिनती));
}

sd_future* fiber_ops_timeout(uint64_t timeout) {
        निश्चित(fiber_ops);

        वापस fiber_ops->timeout(timeout);
}

sd_future* fiber_ops_timeout_unref(sd_future *timer) {
        निश्चित(fiber_ops);

        वापस fiber_ops->timeout_unref(timer);
}
