/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <signal.h>
#அந்தர்பாவ <sys/mman.h>

#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "sigbus.h"
#அந்தர்பாவ "signal-util.h"

#define SIGBUS_QUEUE_MAX 64

நிலையான கட்டமைப்பு sigaction old_sigaction;
நிலையான unsigned n_installed = 0;

/* We maintain a fixed size list of page addresses that triggered a
   SIGBUS. We access with list with atomic operations, so that we
   don't have to deal with locks between signal handler and main
   programs in possibly multiple threads. */

நிலையான வெறுமை* நிலையற்ற sigbus_queue[SIGBUS_QUEUE_MAX];
நிலையான நிலையற்ற sig_atomic_t n_sigbus_queue = 0;

நிலையான வெறுமை sigbus_தள்ளு(வெறுமை *addr) {
        ப்ரதிஜ்ஞா(addr);

        /* Find a free place, increase the number of entries and leave, if we can */
        FOREACH_ELEMENT(u, sigbus_queue) {
                /* OK to initialize this here since we haven't started the atomic ops yet */
                வெறுமை *tmp = வெற்று;
                எனில் (__atomic_compare_exchange_n(u, &tmp, addr, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                        __atomic_fetch_add(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                        திரும்பு;
                }
        }

        /* If we can't, make sure the queue size is out of bounds, to
         * mark it as overflowed */
        க்கு (;;) {
                sig_atomic_t c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                எனில் (c > SIGBUS_QUEUE_MAX) /* already overflowed */
                        திரும்பு;

                /* OK if we clobber c here, since we either immediately return
                 * or it will be immediately reinitialized on next loop */
                எனில் (__atomic_compare_exchange_n(&n_sigbus_queue, &c, c + SIGBUS_QUEUE_MAX, false,
                                                __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
                        திரும்பு;
        }
}

முழு sigbus_எடு(வெறுமை **ret) {
        ப்ரதிஜ்ஞா(ret);

        க்கு (;;) {
                unsigned u, c;

                __atomic_thread_fence(__ATOMIC_SEQ_CST);
                c = n_sigbus_queue;

                எனில் (_likely_(c == 0))
                        திரும்பு 0;

                எனில் (_unlikely_(c > SIGBUS_QUEUE_MAX))
                        திரும்பு -EOVERFLOW;

                க்கு (u = 0; u < SIGBUS_QUEUE_MAX; u++) {
                        வெறுமை *addr;

                        addr = sigbus_queue[u];
                        எனில் (!addr)
                                தொடர்;

                        /* OK if we clobber addr here, since we either immediately return
                         * or it will be immediately reinitialized on next loop */
                        எனில் (__atomic_compare_exchange_n(&sigbus_queue[u], &addr, வெற்று, false,
                                                        __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST)) {
                                __atomic_fetch_sub(&n_sigbus_queue, 1, __ATOMIC_SEQ_CST);
                                /* If we successfully entered this if condition, addr won't
                                 * have been modified since its assignment, so safe to use it */
                                *ret = addr;
                                திரும்பு 1;
                        }
                }
        }
}

நிலையான வெறுமை sigbus_handler(முழு sn, siginfo_t *si, வெறுமை *data) {
        unsigned long ul;
        வெறுமை *aligned;

        ப்ரதிஜ்ஞா(sn == SIGBUS);
        ப்ரதிஜ்ஞா(si);

        எனில் (si->si_code != BUS_ADRERR || !si->si_addr) {
                assert_se(sigaction(SIGBUS, &old_sigaction, வெற்று) >= 0);
                propagate_signal(sn, si);
                திரும்பு;
        }

        ul = (unsigned long) si->si_addr;
        ul = ul / page_size();
        ul = ul * page_size();
        aligned = (வெறுமை*) ul;

        /* Let's remember which address failed */
        sigbus_தள்ளு(aligned);

        /* Replace mapping with an anonymous page, so that the
         * execution can continue, however with a zeroed out page */
        assert_se(mmap(aligned, page_size(), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED, -1, 0) == aligned);
}

வெறுமை sigbus_install(வெறுமை) {
        நிலையான மாறிலி கட்டமைப்பு sigaction sa = {
                .sa_sigaction = sigbus_handler,
                .sa_flags = SA_SIGINFO,
        };

        /* make sure that sysconf() is not called from a signal handler because
        * it is not guaranteed to be async-signal-safe since POSIX.1-2008 */
        (வெறுமை) page_size();

        n_installed++;

        எனில் (n_installed == 1)
                assert_se(sigaction(SIGBUS, &sa, &old_sigaction) >= 0);

        திரும்பு;
}

வெறுமை sigbus_reset(வெறுமை) {

        எனில் (n_installed <= 0)
                திரும்பு;

        n_installed--;

        எனில் (n_installed == 0)
                assert_se(sigaction(SIGBUS, &old_sigaction, வெற்று) >= 0);

        திரும்பு;
}
