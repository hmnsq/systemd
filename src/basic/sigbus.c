/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <signal.h>
#अन्तर्भाव <sys/mman.h>

#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "sigbus.h"
#अन्तर्भाव "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

स्थिर संरचना sigaction old_sigaction;
स्थिर अचिन्हित n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

स्थिर शून्य* चञ्चल sigbus_queue[SIGBUS_QUEUE_MAX];
स्थिर चञ्चल संक_पूर्ण_प्रकार n_sigbus_queue = 0;

स्थिर शून्य sigbus_थप(शून्य *addr) {
        प्रतिज्ञा(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                शून्य *tmp = रिक्त;
                यदि (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        फर्काऊ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        लागि (;;) {
                संक_पूर्ण_प्रकार c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                यदि (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        फर्काऊ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                यदि (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        फर्काऊ;
        }
}

पूर्णाङ्क sigbus_झिक(शून्य **ret) {
        प्रतिज्ञा(ret);

        लागि (;;) {
                अचिन्हित u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                यदि (_likely_(c == 0))
                        फर्काऊ 0;

                यदि (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        फर्काऊ -EOVERFLOW;

                लागि (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        शून्य *addr;

                        addr = sigbus_queue[u];
                        यदि (!addr)
                                जारी;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        यदि (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, रिक्त, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                फर्काऊ 1;
                        }
                }
        }
}

स्थिर शून्य sigbus_handler(पूर्णाङ्क sn, siginfo_t *si, शून्य *data) {
        अचिन्हित दीर्घ ul;
        शून्य *aligned;

        प्रतिज्ञा(sn == SIGBUS);
        प्रतिज्ञा(si);

        यदि (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, रिक्त) >= 0);
                propagate_signal(sn, si);
                फर्काऊ;
        }

        ul = (अचिन्हित दीर्घ) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (शून्य*) ul;

        /* Let's remember which address failed */
        sigbus_थप(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

शून्य sigbus_install(शून्य) {
        स्थिर अचर संरचना sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (शून्य) page_size();

        n_installed++;

        यदि (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        फर्काऊ;
}

शून्य sigbus_reset(शून्य) {

        यदि (n_installed <= 0)
                फर्काऊ;

        n_installed--;

        यदि (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, रिक्त) >= 0);

        फर्काऊ;
}
