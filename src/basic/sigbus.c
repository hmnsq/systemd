/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <signal.h>
#ضمن <sys/mman.h>

#ضمن "memory-util.h"
#ضمن "sigbus.h"
#ضمن "signal-util.h"

#تعریف_کن SIGBUS_صف_MAX 64

ساكن بنية sigaction old_sigaction;
ساكن بیعلامت n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ساكن فراغ* متطاير sigbus_صف[SIGBUS_صف_MAX];
ساكن متطاير sig_atomic_t n_sigbus_صف = 0;

ساكن فراغ sigbus_افزودن(فراغ *addr) {
        أكد(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_صف) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                فراغ *tmp = عدم;
                إذا (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_صف, 1, __ATOMIC_SEQ_CST);
                        أرجع;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        لكل (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_صف;

                إذا (c > SIGBUS_صف_MAX) /* already overflowed */
                        أرجع;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                إذا (__atomic_compare_exchange_n(&n_sigbus_صف, &c, c + SIGBUS_صف_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        أرجع;
        }
}

صحيح sigbus_برداشت(فراغ **ret) {
        أكد(ret);

        لكل (;;) {
                بیعلامت u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_صف;

                إذا (_likely_(c == 0))
                        أرجع 0;

                إذا (_unlikely_(c > SIGBUS_صف_MAX))
                        أرجع -EOVERFLOW;

                لكل (u = 0; u < SIGBUS_صف_MAX; u++) {
                        فراغ *addr;

                        addr = sigbus_صف[u];
                        إذا (!addr)
                                استمر;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        إذا (__atomic_compare_exchange_n(&sigbus_صف[u], &addr, عدم, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_صف, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                أرجع 1;
                        }
                }
        }
}

ساكن فراغ sigbus_handler(صحيح sn, siginfo_t *si, فراغ *داده) {
        بیعلامت دراز ul;
        فراغ *aligned;

        أكد(sn == SIGBUS);
        أكد(si);

        إذا (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, عدم) >= 0);
                propagate_signal(sn, si);
                أرجع;
        }

        ul = (بیعلامت دراز) si->si_addr;
        ul = ul / صفحه_اندازه();
        ul = ul * صفحه_اندازه();
        aligned = (فراغ*) ul;

        /* Let's remember which address failed */
        sigbus_افزودن(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, صفحه_اندازه(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

فراغ sigbus_install(فراغ) {
        ساكن ثابت بنية sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (فراغ) صفحه_اندازه();

        n_installed++;

        إذا (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        أرجع;
}

فراغ sigbus_reset(فراغ) {

        إذا (n_installed <= 0)
                أرجع;

        n_installed--;

        إذا (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, عدم) >= 0);

        أرجع;
}
