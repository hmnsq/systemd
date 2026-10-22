/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h>

#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sigbus.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

ಸ್ಥಿರ ಸಂರಚನಾ sigaction old_sigaction;
ಸ್ಥಿರ unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ಸ್ಥಿರ ಶೂನ್ಯ* ಚಞ್ಚಲ sigbus_queue[SIGBUS_QUEUE_MAX];
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t n_sigbus_queue = 0;

ಸ್ಥಿರ ಶೂನ್ಯ sigbus_ತಳ್ಳು(ಶೂನ್ಯ *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                ಶೂನ್ಯ *tmp = ರಿಕ್ತ;
                ಯದಿ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        ಪ್ರತ್ಯಾಗಮ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        ಪ್ರತಿ (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ಯದಿ (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        ಪ್ರತ್ಯಾಗಮ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ಯದಿ (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ಪ್ರತ್ಯಾಗಮ;
        }
}

ಪೂರ್ಣಾಙ್ಕ sigbus_ತೆಗೆ(ಶೂನ್ಯ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಪ್ರತಿ (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ಯದಿ (_likely_(c == 0))
                        ಪ್ರತ್ಯಾಗಮ 0;

                ಯದಿ (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        ಪ್ರತ್ಯಾಗಮ -EOVERFLOW;

                ಪ್ರತಿ (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        ಶೂನ್ಯ *addr;

                        addr = sigbus_queue[u];
                        ಯದಿ (!addr)
                                ಅನುವರ್ತ;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ಯದಿ (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, ರಿಕ್ತ, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ಪ್ರತ್ಯಾಗಮ 1;
                        }
                }
        }
}

ಸ್ಥಿರ ಶೂನ್ಯ sigbus_handler(ಪೂರ್ಣಾಙ್ಕ sn, siginfo_t *si, ಶೂನ್ಯ *data) {
        unsigned long ul;
        ಶೂನ್ಯ *aligned;

        ಪ್ರತಿಜ್ಞಾ(sn == SIGBUS);
        ಪ್ರತಿಜ್ಞಾ(si);

        ಯದಿ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ರಿಕ್ತ) >= 0);
                propagate_signal(sn, si);
                ಪ್ರತ್ಯಾಗಮ;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (ಶೂನ್ಯ*) ul;

        /* Let's remember which address failed */
        sigbus_ತಳ್ಳು(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

ಶೂನ್ಯ sigbus_install(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (ಶೂನ್ಯ) page_size();

        n_installed++;

        ಯದಿ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ಪ್ರತ್ಯಾಗಮ;
}

ಶೂನ್ಯ sigbus_reset(ಶೂನ್ಯ) {

        ಯದಿ (n_installed <= 0)
                ಪ್ರತ್ಯಾಗಮ;

        n_installed--;

        ಯದಿ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ರಿಕ್ತ) >= 0);

        ಪ್ರತ್ಯಾಗಮ;
}
