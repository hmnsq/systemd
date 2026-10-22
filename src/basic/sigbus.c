/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <sys/mman.h>

#ಅನ್ತರ್ಭಾವ "memory-util.h"
#ಅನ್ತರ್ಭಾವ "sigbus.h"
#ಅನ್ತರ್ಭಾವ "signal-util.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SIGBUS_ಸರತಿ_MAX 64

ಸ್ಥಿರ ರಚನೆ sigaction old_sigaction;
ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ಸ್ಥಿರ ಶೂನ್ಯ* ಅಸ್ಥಿರ sigbus_ಸರತಿ[SIGBUS_ಸರತಿ_MAX];
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t n_sigbus_ಸರತಿ = 0;

ಸ್ಥಿರ ಶೂನ್ಯ sigbus_ತಳ್ಳು(ಶೂನ್ಯ *addr) {
        ಪ್ರತಿಜ್ಞಾ(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_ಸರತಿ) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                ಶೂನ್ಯ *tmp = ಖಾಲಿ;
                ಆದರೆ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_ಸರತಿ, 1, __ATOMIC_SEQ_CST);
                        ಹಿಂತಿರುಗಿ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        ಗಾಗಿ (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_ಸರತಿ;

                ಆದರೆ (c > SIGBUS_ಸರತಿ_MAX) /* already overflowed */
                        ಹಿಂತಿರುಗಿ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ಆದರೆ (__atomic_compare_exchange_n(&n_sigbus_ಸರತಿ, &c, c + SIGBUS_ಸರತಿ_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ಹಿಂತಿರುಗಿ;
        }
}

ಪೂರ್ಣಾಂಕ sigbus_ತೆಗೆ(ಶೂನ್ಯ **ret) {
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಗಾಗಿ (;;) {
                ಚಿಹ್ನೆರಹಿತ u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_ಸರತಿ;

                ಆದರೆ (_likely_(c == 0))
                        ಹಿಂತಿರುಗಿ 0;

                ಆದರೆ (_unlikely_(c > SIGBUS_ಸರತಿ_MAX))
                        ಹಿಂತಿರುಗಿ -EOVERFLOW;

                ಗಾಗಿ (u = 0; u < SIGBUS_ಸರತಿ_MAX; u++) {
                        ಶೂನ್ಯ *addr;

                        addr = sigbus_ಸರತಿ[u];
                        ಆದರೆ (!addr)
                                ಮುಂದುವರಿ;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ಆದರೆ (__atomic_compare_exchange_n(&sigbus_ಸರತಿ[u], &addr, ಖಾಲಿ, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_ಸರತಿ, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ಹಿಂತಿರುಗಿ 1;
                        }
                }
        }
}

ಸ್ಥಿರ ಶೂನ್ಯ sigbus_handler(ಪೂರ್ಣಾಂಕ sn, siginfo_t *si, ಶೂನ್ಯ *ದತ್ತಾಂಶ) {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ul;
        ಶೂನ್ಯ *aligned;

        ಪ್ರತಿಜ್ಞಾ(sn == SIGBUS);
        ಪ್ರತಿಜ್ಞಾ(si);

        ಆದರೆ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ಖಾಲಿ) >= 0);
                propagate_signal(sn, si);
                ಹಿಂತಿರುಗಿ;
        }

        ul = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ) si->si_addr;
        ul = ul / ಪುಟ_ಗಾತ್ರ();
        ul = ul * ಪುಟ_ಗಾತ್ರ();
        aligned = (ಶೂನ್ಯ*) ul;

        /* Let's remember which address failed */
        sigbus_ತಳ್ಳು(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, ಪುಟ_ಗಾತ್ರ(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

ಶೂನ್ಯ sigbus_install(ಶೂನ್ಯ) {
        ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (ಶೂನ್ಯ) ಪುಟ_ಗಾತ್ರ();

        n_installed++;

        ಆದರೆ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ಹಿಂತಿರುಗಿ;
}

ಶೂನ್ಯ sigbus_reset(ಶೂನ್ಯ) {

        ಆದರೆ (n_installed <= 0)
                ಹಿಂತಿರುಗಿ;

        n_installed--;

        ಆದರೆ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ಖಾಲಿ) >= 0);

        ಹಿಂತಿರುಗಿ;
}
