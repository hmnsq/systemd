/* SPDX-License-Identifier: LGPL-2.1-or-later */
#निर्देशः once

#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव "forward.h"

स्थिर अन्तःस्थ शून्य umaskp(mode_t *u) {
        umask(*u);
}

#परिभाषय _cleanup_umask_ _cleanup_(umaskp)

/* We make use of the fact here that the umask() syscall uses only the lower 9 bits of mode_t, although
 * mode_t has space for the file type in the bits further up. We simply OR in the file type mask S_IFMT to
 * distinguish the first and the second iteration of the WITH_UMASK() loop, so that we can run the first one,
 * and exit on the second. */

assert_cc((S_IFMT & 0777) == 0);

#परिभाषय WITH_UMASK(आवरणम्_3)                                            \
        प्रति (_cleanup_umask_ mode_t _saved_umask_ = umask(आवरणम्_3) | S_IFMT; \
             FLAGS_समुच्चयः(_saved_umask_, S_IFMT);                          \
             _saved_umask_ &= 0777)

#परिभाषय खण्डम्_WITH_UMASK(आवरणम्_3) \
        अप्रयुक्तम् _cleanup_umask_ mode_t _saved_umask_ = umask(आवरणम्_3);
