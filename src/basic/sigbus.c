/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <signal.h>
#అన్తర్భావ <sys/mman.h>

#అన్తర్భావ "memory-util.h"
#అన్తర్భావ "sigbus.h"
#అన్తర్భావ "signal-util.h"

#నిర్వచించు SIGBUS_వరుస_MAX 64

స్థిర నిర్మాణం sigaction old_sigaction;
స్థిర గుర్తులేని n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

స్థిర శూన్య* అస్థిర sigbus_వరుస[SIGBUS_వరుస_MAX];
స్థిర అస్థిర sig_atomic_t n_sigbus_వరుస = 0;

స్థిర శూన్య sigbus_నెట్టు(శూన్య *addr) {
        ప్రతిజ్ఞా(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_వరుస) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                శూన్య *tmp = ఖాళీ;
                ఒకవేళ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_వరుస, 1, __ATOMIC_SEQ_CST);
                        తిరిగి;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        కొరకు (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_వరుస;

                ఒకవేళ (c > SIGBUS_వరుస_MAX) /* already overflowed */
                        తిరిగి;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ఒకవేళ (__atomic_compare_exchange_n(&n_sigbus_వరుస, &c, c + SIGBUS_వరుస_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        తిరిగి;
        }
}

పూర్ణాంకం sigbus_తీయి(శూన్య **ret) {
        ప్రతిజ్ఞా(ret);

        కొరకు (;;) {
                గుర్తులేని u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_వరుస;

                ఒకవేళ (_likely_(c == 0))
                        తిరిగి 0;

                ఒకవేళ (_unlikely_(c > SIGBUS_వరుస_MAX))
                        తిరిగి -EOVERFLOW;

                కొరకు (u = 0; u < SIGBUS_వరుస_MAX; u++) {
                        శూన్య *addr;

                        addr = sigbus_వరుస[u];
                        ఒకవేళ (!addr)
                                కొనసాగు;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ఒకవేళ (__atomic_compare_exchange_n(&sigbus_వరుస[u], &addr, ఖాళీ, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_వరుస, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                తిరిగి 1;
                        }
                }
        }
}

స్థిర శూన్య sigbus_handler(పూర్ణాంకం sn, siginfo_t *si, శూన్య *దత్తాంశం) {
        గుర్తులేని దీర్ఘ ul;
        శూన్య *aligned;

        ప్రతిజ్ఞా(sn == SIGBUS);
        ప్రతిజ్ఞా(si);

        ఒకవేళ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ఖాళీ) >= 0);
                propagate_signal(sn, si);
                తిరిగి;
        }

        ul = (గుర్తులేని దీర్ఘ) si->si_addr;
        ul = ul / పుట_కొలత();
        ul = ul * పుట_కొలత();
        aligned = (శూన్య*) ul;

        /* Let's remember which address failed */
        sigbus_నెట్టు(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, పుట_కొలత(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

శూన్య sigbus_install(శూన్య) {
        స్థిర స్థిరరాశి నిర్మాణం sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (శూన్య) పుట_కొలత();

        n_installed++;

        ఒకవేళ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        తిరిగి;
}

శూన్య sigbus_reset(శూన్య) {

        ఒకవేళ (n_installed <= 0)
                తిరిగి;

        n_installed--;

        ఒకవేళ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ఖాళీ) >= 0);

        తిరిగి;
}
