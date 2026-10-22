/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <signal.h>
#شامل <sys/mman.h>

#شامل "memory-util.h"
#شامل "sigbus.h"
#شامل "signal-util.h"

#تعریف_کرو SIGBUS_قطار_MAX 64

ساکن ساخت sigaction old_sigaction;
ساکن غیرعلامتی n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ساکن خالی* متلون sigbus_قطار[SIGBUS_قطار_MAX];
ساکن متلون sig_atomic_t n_sigbus_قطار = 0;

ساکن خالی sigbus_ڈالو(خالی *addr) {
        أكد(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_قطار) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                خالی *tmp = تہی;
                اگر (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_قطار, 1, __ATOMIC_SEQ_CST);
                        واپس;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        برائے (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_قطار;

                اگر (c > SIGBUS_قطار_MAX) /* already overflowed */
                        واپس;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                اگر (__atomic_compare_exchange_n(&n_sigbus_قطار, &c, c + SIGBUS_قطار_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        واپس;
        }
}

صحیح sigbus_نکالو(خالی **ret) {
        أكد(ret);

        برائے (;;) {
                غیرعلامتی u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_قطار;

                اگر (_likely_(c == 0))
                        واپس 0;

                اگر (_unlikely_(c > SIGBUS_قطار_MAX))
                        واپس -EOVERFLOW;

                برائے (u = 0; u < SIGBUS_قطار_MAX; u++) {
                        خالی *addr;

                        addr = sigbus_قطار[u];
                        اگر (!addr)
                                جاری;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        اگر (__atomic_compare_exchange_n(&sigbus_قطار[u], &addr, تہی, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_قطار, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                واپس 1;
                        }
                }
        }
}

ساکن خالی sigbus_handler(صحیح sn, siginfo_t *si, خالی *ڈیٹا) {
        غیرعلامتی طویل ul;
        خالی *aligned;

        أكد(sn == SIGBUS);
        أكد(si);

        اگر (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, تہی) >= 0);
                propagate_signal(sn, si);
                واپس;
        }

        ul = (غیرعلامتی طویل) si->si_addr;
        ul = ul / صفحہ_سائز();
        ul = ul * صفحہ_سائز();
        aligned = (خالی*) ul;

        /* Let's remember which address failed */
        sigbus_ڈالو(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, صفحہ_سائز(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

خالی sigbus_install(خالی) {
        ساکن ثابت ساخت sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (خالی) صفحہ_سائز();

        n_installed++;

        اگر (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        واپس;
}

خالی sigbus_reset(خالی) {

        اگر (n_installed <= 0)
                واپس;

        n_installed--;

        اگر (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, تہی) >= 0);

        واپس;
}
