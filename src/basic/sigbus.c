/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <signal.h>
#समावेश <sys/mman.h>

#समावेश "memory-util.h"
#समावेश "sigbus.h"
#समावेश "signal-util.h"

#घोषणा SIGBUS_QUEUE_MAX 64

अटल काष्ठा sigaction old_sigaction;
अटल अचिन्हित n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

अटल व्योम* अस्थिर sigbus_queue[SIGBUS_QUEUE_MAX];
अटल अस्थिर संक_पूर्ण_प्रकार n_sigbus_queue = 0;

अटल व्योम sigbus_धकेलो(व्योम *addr) {
        निश्चित(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                व्योम *tmp = शून्य;
                अगर (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        वापस;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        क्रम (;;) {
                संक_पूर्ण_प्रकार c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                अगर (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        वापस;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                अगर (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        वापस;
        }
}

पूर्णांक sigbus_निकालो(व्योम **ret) {
        निश्चित(ret);

        क्रम (;;) {
                अचिन्हित u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                अगर (_likely_(c == 0))
                        वापस 0;

                अगर (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        वापस -EOVERFLOW;

                क्रम (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        व्योम *addr;

                        addr = sigbus_queue[u];
                        अगर (!addr)
                                जारी;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        अगर (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, शून्य, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                वापस 1;
                        }
                }
        }
}

अटल व्योम sigbus_handler(पूर्णांक sn, siginfo_t *si, व्योम *तथ्य) {
        अचिन्हित दीर्घ ul;
        व्योम *aligned;

        निश्चित(sn == SIGBUS);
        निश्चित(si);

        अगर (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, शून्य) >= 0);
                propagate_signal(sn, si);
                वापस;
        }

        ul = (अचिन्हित दीर्घ) si->si_addr;
        ul = ul / page_आकार();
        ul = ul * page_आकार();
        aligned = (व्योम*) ul;

        /* Let's remember which address failed */
        sigbus_धकेलो(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_आकार(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

व्योम sigbus_install(व्योम) {
        अटल स्थिर काष्ठा sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (व्योम) page_आकार();

        n_installed++;

        अगर (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        वापस;
}

व्योम sigbus_reset(व्योम) {

        अगर (n_installed <= 0)
                वापस;

        n_installed--;

        अगर (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, शून्य) >= 0);

        वापस;
}
