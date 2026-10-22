/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ umaskp(mode_t *u) {
        umask(*u);
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WITH_UMASK(ਨਕਾਬ_3)                                            \
        ਲਈ (_cleanup_umask_ mode_t _saved_umask_ = umask(ਨਕਾਬ_3) | S_IFMT; \
             FLAGS_ਸੈੱਟ(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਖੰਡ_WITH_UMASK(ਨਕਾਬ_3) \
        ਅਣਵਰਤਿਆ _cleanup_umask_ mode_t _saved_umask_ = umask(ਨਕਾਬ_3);
