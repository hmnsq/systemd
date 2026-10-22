/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "fd-util.h"
#ಅನ್ತರ್ಭಾವ "io-util.h"
#ಅನ್ತರ್ಭಾವ "memfd-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"

ಪೂರ್ಣಾಂಕ memfd_create_wrapper(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ) {
        ಚಿಹ್ನೆರಹಿತ ಬಹುಲಕ_compat;
        ಪೂರ್ಣಾಂಕ mfd;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, ಬಹುಲಕ));
        ಆದರೆ (mfd != -EINVAL)
                ಹಿಂತಿರುಗಿ mfd;

        ಬಹುಲಕ_compat = ಬಹುಲಕ & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ಆದರೆ (ಬಹುಲಕ == ಬಹುಲಕ_compat)
                ಹಿಂತಿರುಗಿ mfd;

        ಹಿಂತಿರುಗಿ RET_NERRNO(memfd_create(name, ಬಹುಲಕ_compat));
}

ಪೂರ್ಣಾಂಕ memfd_new_ತುಂಬಿದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಚಿಹ್ನೆರಹಿತ extra_flags) {
        _cleanup_free_ ಅಕ್ಷರ *g = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಆದರೆ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ ಅಕ್ಷರ *comm = ಖಾಲಿ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಆದರೆ (isempty(comm))
                        name = "sd";
                ಇಲ್ಲದಿದ್ದರೆ {
                        g = strjoin("sd-", comm);
                        ಆದರೆ (!g)
                                ಹಿಂತಿರುಗಿ -ENOMEM;

                        name = g;
                }
        }

        ಹಿಂತಿರುಗಿ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ memfd_ಕೂಡಿಸು_seals(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ seals) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಹಿಂತಿರುಗಿ RET_NERRNO(fcntl(fd, F_ಕೂಡಿಸು_SEALS, seals));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ memfd_get_seals(ಪೂರ್ಣಾಂಕ fd, ಚಿಹ್ನೆರಹಿತ *ret_seals) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಆದರೆ (ret_seals)
                *ret_seals = r;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ memfd_ಗಣ_sealed(ಪೂರ್ಣಾಂಕ fd) {
        ಹಿಂತಿರುಗಿ memfd_ಕೂಡಿಸು_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ಪೂರ್ಣಾಂಕ memfd_get_sealed(ಪೂರ್ಣಾಂಕ fd) {
        ಚಿಹ್ನೆರಹಿತ seals;
        ಪೂರ್ಣಾಂಕ r;

        r = memfd_get_seals(fd, &seals);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ಹಿಂತಿರುಗಿ FLAGS_ಗಣ(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ಪೂರ್ಣಾಂಕ memfd_ಗಣ_sealed_gracefully(ಪೂರ್ಣಾಂಕ fd) {
        ಪೂರ್ಣಾಂಕ r;

        r = memfd_ಗಣ_sealed(fd);
        ಆದರೆ (r != -EPERM)
                ಹಿಂತಿರುಗಿ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == 0)
                ಹಿಂತಿರುಗಿ -EPERM; /* Propagate the original error. */
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ memfd_get_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ fd, uint64_t *ret) {
        ರಚನೆ stat stat;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಆದರೆ (fstat(fd, &stat) < 0)
                ಹಿಂತಿರುಗಿ -errno;

        *ret = stat.st_size;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ memfd_ಗಣ_ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ fd, uint64_t sz) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಹಿಂತಿರುಗಿ RET_NERRNO(ftruncate(fd, sz));
}

ಪೂರ್ಣಾಂಕ memfd_new_and_seal(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ದತ್ತಾಂಶ, size_t sz) {
        _cleanup_close_ ಪೂರ್ಣಾಂಕ fd = -EBADF;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ದತ್ತಾಂಶ || sz == 0);

        ಆದರೆ (sz == SIZE_MAX)
                sz = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ದತ್ತಾಂಶ);

        fd = memfd_new_ತುಂಬಿದ(name, MFD_ALLOW_SEALING);
        ಆದರೆ (fd < 0)
                ಹಿಂತಿರುಗಿ fd;

        ಆದರೆ (sz > 0) {
                r = pwrite_ತುಂಬಿದ(fd, ದತ್ತಾಂಶ, sz, 0);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;
        }

        r = memfd_ಗಣ_sealed(fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ TAKE_FD(fd);
}
