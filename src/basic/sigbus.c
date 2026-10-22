/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/mman.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "memory-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sigbus.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "signal-util.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SIGBUS_ਕਤਾਰ_MAX 64

ਸਥਿਰ ਬਣਤਰ sigaction old_sigaction;
ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ਸਥਿਰ ਖਾਲੀ* ਅਸਥਿਰ sigbus_ਕਤਾਰ[SIGBUS_ਕਤਾਰ_MAX];
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t n_sigbus_ਕਤਾਰ = 0;

ਸਥਿਰ ਖਾਲੀ sigbus_ਧੱਕੋ(ਖਾਲੀ *addr) {
        ਪ੍ਰਤਿਜ੍ਞਾ(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_ਕਤਾਰ) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                ਖਾਲੀ *tmp = ਨਿਰਾ;
                ਜੇ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_ਕਤਾਰ, 1, __ATOMIC_SEQ_CST);
                        ਵਾਪਸ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        ਲਈ (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_ਕਤਾਰ;

                ਜੇ (c > SIGBUS_ਕਤਾਰ_MAX) /* already overflowed */
                        ਵਾਪਸ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ਜੇ (__atomic_compare_exchange_n(&n_sigbus_ਕਤਾਰ, &c, c + SIGBUS_ਕਤਾਰ_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ਵਾਪਸ;
        }
}

ਪੂਰਨ sigbus_ਕੱਢੋ(ਖਾਲੀ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਲਈ (;;) {
                ਬਿਨਾਨਿਸ਼ਾਨੀ u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_ਕਤਾਰ;

                ਜੇ (_likely_(c == 0))
                        ਵਾਪਸ 0;

                ਜੇ (_unlikely_(c > SIGBUS_ਕਤਾਰ_MAX))
                        ਵਾਪਸ -EOVERFLOW;

                ਲਈ (u = 0; u < SIGBUS_ਕਤਾਰ_MAX; u++) {
                        ਖਾਲੀ *addr;

                        addr = sigbus_ਕਤਾਰ[u];
                        ਜੇ (!addr)
                                ਜਾਰੀ;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ਜੇ (__atomic_compare_exchange_n(&sigbus_ਕਤਾਰ[u], &addr, ਨਿਰਾ, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_ਕਤਾਰ, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ਵਾਪਸ 1;
                        }
                }
        }
}

ਸਥਿਰ ਖਾਲੀ sigbus_handler(ਪੂਰਨ sn, siginfo_t *si, ਖਾਲੀ *ਅੰਕੜੇ) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ul;
        ਖਾਲੀ *aligned;

        ਪ੍ਰਤਿਜ੍ਞਾ(sn == SIGBUS);
        ਪ੍ਰਤਿਜ੍ਞਾ(si);

        ਜੇ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ਨਿਰਾ) >= 0);
                propagate_signal(sn, si);
                ਵਾਪਸ;
        }

        ul = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ) si->si_addr;
        ul = ul / ਪੰਨਾ_ਮਾਪ();
        ul = ul * ਪੰਨਾ_ਮਾਪ();
        aligned = (ਖਾਲੀ*) ul;

        /* Let's remember which address failed */
        sigbus_ਧੱਕੋ(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, ਪੰਨਾ_ਮਾਪ(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

ਖਾਲੀ sigbus_install(ਖਾਲੀ) {
        ਸਥਿਰ ਅਚਲ ਬਣਤਰ sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (ਖਾਲੀ) ਪੰਨਾ_ਮਾਪ();

        n_installed++;

        ਜੇ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ਵਾਪਸ;
}

ਖਾਲੀ sigbus_reset(ਖਾਲੀ) {

        ਜੇ (n_installed <= 0)
                ਵਾਪਸ;

        n_installed--;

        ਜੇ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ਨਿਰਾ) >= 0);

        ਵਾਪਸ;
}
