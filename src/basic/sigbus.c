/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <signal.h>
#ଅନ୍ତର୍ଭାଵ <sys/mman.h>

#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "sigbus.h"
#ଅନ୍ତର୍ଭାଵ "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

ସ୍ଥିର ସଂରଚନା sigaction old_sigaction;
ସ୍ଥିର unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

ସ୍ଥିର ଶୂନ୍ୟ* ଅସ୍ଥିର sigbus_queue[SIGBUS_QUEUE_MAX];
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t n_sigbus_queue = 0;

ସ୍ଥିର ଶୂନ୍ୟ sigbus_ଠେଲ(ଶୂନ୍ୟ *addr) {
        ପ୍ରତିଜ୍ଞା(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                ଶୂନ୍ୟ *tmp = ଖାଲି;
                ଯଦି (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        ଫେରନ୍ତୁ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        ପାଇଁ (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ଯଦି (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        ଫେରନ୍ତୁ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                ଯଦି (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        ଫେରନ୍ତୁ;
        }
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା sigbus_କାଢ଼(ଶୂନ୍ୟ **ret) {
        ପ୍ରତିଜ୍ଞା(ret);

        ପାଇଁ (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                ଯଦି (_likely_(c == 0))
                        ଫେରନ୍ତୁ 0;

                ଯଦି (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        ଫେରନ୍ତୁ -EOVERFLOW;

                ପାଇଁ (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        ଶୂନ୍ୟ *addr;

                        addr = sigbus_queue[u];
                        ଯଦି (!addr)
                                ଜାରି;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        ଯଦି (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, ଖାଲି, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                ଫେରନ୍ତୁ 1;
                        }
                }
        }
}

ସ୍ଥିର ଶୂନ୍ୟ sigbus_handler(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sn, siginfo_t *si, ଶୂନ୍ୟ *data) {
        unsigned long ul;
        ଶୂନ୍ୟ *aligned;

        ପ୍ରତିଜ୍ଞା(sn == SIGBUS);
        ପ୍ରତିଜ୍ଞା(si);

        ଯଦି (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ଖାଲି) >= 0);
                propagate_signal(sn, si);
                ଫେରନ୍ତୁ;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (ଶୂନ୍ୟ*) ul;

        /* Let's remember which address failed */
        sigbus_ଠେଲ(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

ଶୂନ୍ୟ sigbus_install(ଶୂନ୍ୟ) {
        ସ୍ଥିର ଅଚଳ ସଂରଚନା sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (ଶୂନ୍ୟ) page_size();

        n_installed++;

        ଯଦି (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        ଫେରନ୍ତୁ;
}

ଶୂନ୍ୟ sigbus_reset(ଶୂନ୍ୟ) {

        ଯଦି (n_installed <= 0)
                ଫେରନ୍ତୁ;

        n_installed--;

        ଯଦି (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ଖାଲି) >= 0);

        ଫେରନ୍ତୁ;
}
