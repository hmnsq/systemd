/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <signal.h>
#അന്തര്ഭാവ <sys/mman.h>

#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "sigbus.h"
#അന്തര്ഭാവ "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

സ്ഥിതം ഘടന sigaction old_sigaction;
സ്ഥിതം unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

സ്ഥിതം ശൂന്യം* അസ്ഥിര sigbus_queue[SIGBUS_QUEUE_MAX];
സ്ഥിതം അസ്ഥിര sig_atomic_t n_sigbus_queue = 0;

സ്ഥിതം ശൂന്യം sigbus_തള്ളുക(ശൂന്യം *addr) {
        പ്രതിജ്ഞാ(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                ശൂന്യം *tmp = ശൂന്യ;
                എങ്കിൽ (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        തിരികെ;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        വേണ്ടി (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                എങ്കിൽ (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        തിരികെ;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                എങ്കിൽ (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        തിരികെ;
        }
}

പൂർണ്ണസംഖ്യ sigbus_എടുക്കുക(ശൂന്യം **ret) {
        പ്രതിജ്ഞാ(ret);

        വേണ്ടി (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                എങ്കിൽ (_likely_(c == 0))
                        തിരികെ 0;

                എങ്കിൽ (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        തിരികെ -EOVERFLOW;

                വേണ്ടി (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        ശൂന്യം *addr;

                        addr = sigbus_queue[u];
                        എങ്കിൽ (!addr)
                                തുടരുക;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        എങ്കിൽ (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, ശൂന്യ, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                തിരികെ 1;
                        }
                }
        }
}

സ്ഥിതം ശൂന്യം sigbus_handler(പൂർണ്ണസംഖ്യ sn, siginfo_t *si, ശൂന്യം *data) {
        unsigned long ul;
        ശൂന്യം *aligned;

        പ്രതിജ്ഞാ(sn == SIGBUS);
        പ്രതിജ്ഞാ(si);

        എങ്കിൽ (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, ശൂന്യ) >= 0);
                propagate_signal(sn, si);
                തിരികെ;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (ശൂന്യം*) ul;

        /* Let's remember which address failed */
        sigbus_തള്ളുക(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

ശൂന്യം sigbus_install(ശൂന്യം) {
        സ്ഥിതം സ്ഥിരം ഘടന sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (ശൂന്യം) page_size();

        n_installed++;

        എങ്കിൽ (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        തിരികെ;
}

ശൂന്യം sigbus_reset(ശൂന്യം) {

        എങ്കിൽ (n_installed <= 0)
                തിരികെ;

        n_installed--;

        എങ്കിൽ (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, ശൂന്യ) >= 0);

        തിരികെ;
}
