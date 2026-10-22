/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <signal.h>
#ضمن <sys/mman.h>

#ضمن "memory-util.h"
#ضمن "sigbus.h"
#ضمن "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

ساكن بنية sigaction old_sigaction;
ساكن unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ساكن فراغ* متطاير sigbus_queue[SIGBUS_QUEUE_MAX];
ساكن متطاير sig_atomic_t n_sigbus_queue = 0;

ساكن فراغ sigbus_push(فراغ *addr) {
        أكد(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                فراغ *tmp = عدم;
                ܐܢ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        ܦܢܐ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        لكل (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ܐܢ (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        ܦܢܐ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ܐܢ (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ܦܢܐ;
        }
}

صحيح sigbus_pop(فراغ **ret) {
        أكد(ret);

        لكل (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ܐܢ (_likely_(c == 0))
                        ܦܢܐ 0;

                ܐܢ (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        ܦܢܐ -EOVERFLOW;

                لكل (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        فراغ *addr;

                        addr = sigbus_queue[u];
                        ܐܢ (!addr)
                                استمر;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ܐܢ (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, عدم, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ܦܢܐ 1;
                        }
                }
        }
}

ساكن فراغ sigbus_handler(صحيح sn, siginfo_t *si, فراغ *data) {
        unsigned long ul;
        فراغ *aligned;

        أكد(sn == SIGBUS);
        أكد(si);

        ܐܢ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, عدم) >= 0);
                propagate_signal(sn, si);
                ܦܢܐ;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (فراغ*) ul;

        /* Let's remember which address failed */
        sigbus_push(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

فراغ sigbus_install(فراغ) {
        ساكن ثابت بنية sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (فراغ) page_size();

        n_installed++;

        ܐܢ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ܦܢܐ;
}

فراغ sigbus_reset(فراغ) {

        ܐܢ (n_installed <= 0)
                ܦܢܐ;

        n_installed--;

        ܐܢ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, عدم) >= 0);

        ܦܢܐ;
}
