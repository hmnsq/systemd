/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <signal.h>
#অন্তর্ভাব <sys/mman.h>

#অন্তর্ভাব "memory-util.h"
#অন্তর্ভাব "sigbus.h"
#অন্তর্ভাব "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

স্থির কাঠামো sigaction old_sigaction;
স্থির unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

স্থির শূন্য* অস্থির sigbus_queue[SIGBUS_QUEUE_MAX];
স্থির অস্থির sig_atomic_t n_sigbus_queue = 0;

স্থির শূন্য sigbus_ঠেলো(শূন্য *addr) {
        প্রতিজ্ঞা(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                শূন্য *tmp = ফাঁকা;
                যদি (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        ফেরত;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        জন্য (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                যদি (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        ফেরত;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                যদি (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ফেরত;
        }
}

পূর্ণসংখ্যা sigbus_বের_করো(শূন্য **ret) {
        প্রতিজ্ঞা(ret);

        জন্য (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                যদি (_likely_(c == 0))
                        ফেরত 0;

                যদি (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        ফেরত -EOVERFLOW;

                জন্য (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        শূন্য *addr;

                        addr = sigbus_queue[u];
                        যদি (!addr)
                                চালু;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        যদি (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, ফাঁকা, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ফেরত 1;
                        }
                }
        }
}

স্থির শূন্য sigbus_handler(পূর্ণসংখ্যা sn, siginfo_t *si, শূন্য *তথ্য) {
        unsigned long ul;
        শূন্য *aligned;

        প্রতিজ্ঞা(sn == SIGBUS);
        প্রতিজ্ঞা(si);

        যদি (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ফাঁকা) >= 0);
                propagate_signal(sn, si);
                ফেরত;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_আকার();
        ul = ul * page_আকার();
        aligned = (শূন্য*) ul;

        /* Let's remember which address failed */
        sigbus_ঠেলো(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_আকার(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

শূন্য sigbus_install(শূন্য) {
        স্থির ধ্রুবক কাঠামো sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (শূন্য) page_আকার();

        n_installed++;

        যদি (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ফেরত;
}

শূন্য sigbus_reset(শূন্য) {

        যদি (n_installed <= 0)
                ফেরত;

        n_installed--;

        যদি (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ফাঁকা) >= 0);

        ফেরত;
}
