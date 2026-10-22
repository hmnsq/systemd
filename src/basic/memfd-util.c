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

ಪೂರ್ಣಾಙ್ಕ memfd_create_wrapper(ಧ್ರುವ ವರ್ಣ *name, unsigned mode) {
        unsigned mode_compat;
        ಪೂರ್ಣಾಙ್ಕ mfd;

        ಪ್ರತಿಜ್ಞಾ(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ಯದಿ (mfd != -EINVAL)
                ಪ್ರತ್ಯಾಗಮ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ಯದಿ (mode == mode_compat)
                ಪ್ರತ್ಯಾಗಮ mfd;

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(memfd_create(name, mode_compat));
}

ಪೂರ್ಣಾಙ್ಕ memfd_new_full(ಧ್ರುವ ವರ್ಣ *name, unsigned extra_flags) {
        _cleanup_free_ ವರ್ಣ *g = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಯದಿ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ ವರ್ಣ *comm = ರಿಕ್ತ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಯದಿ (isempty(comm))
                        name = "sd";
                ಅನ್ಯಥಾ {
                        g = strjoin("sd-", comm);
                        ಯದಿ (!g)
                                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

                        name = g;
                }
        }

        ಪ್ರತ್ಯಾಗಮ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ memfd_add_seals(ಪೂರ್ಣಾಙ್ಕ fd, unsigned seals) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ memfd_get_seals(ಪೂರ್ಣಾಙ್ಕ fd, unsigned *ret_seals) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಯದಿ (ret_seals)
                *ret_seals = r;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ memfd_set_sealed(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪ್ರತ್ಯಾಗಮ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ಪೂರ್ಣಾಙ್ಕ memfd_get_sealed(ಪೂರ್ಣಾಙ್ಕ fd) {
        unsigned seals;
        ಪೂರ್ಣಾಙ್ಕ r;

        r = memfd_get_seals(fd, &seals);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ಪ್ರತ್ಯಾಗಮ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ಪೂರ್ಣಾಙ್ಕ memfd_set_sealed_gracefully(ಪೂರ್ಣಾಙ್ಕ fd) {
        ಪೂರ್ಣಾಙ್ಕ r;

        r = memfd_set_sealed(fd);
        ಯದಿ (r != -EPERM)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == 0)
                ಪ್ರತ್ಯಾಗಮ -EPERM; /* Propagate the original error. */
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ memfd_get_size(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t *ret) {
        ಸಂರಚನಾ stat stat;

        ಪ್ರತಿಜ್ಞಾ(fd >= 0);
        ಪ್ರತಿಜ್ಞಾ(ret);

        ಯದಿ (fstat(fd, &stat) < 0)
                ಪ್ರತ್ಯಾಗಮ -errno;

        *ret = stat.st_size;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ memfd_set_size(ಪೂರ್ಣಾಙ್ಕ fd, uint64_t sz) {
        ಪ್ರತಿಜ್ಞಾ(fd >= 0);

        ಪ್ರತ್ಯಾಗಮ RET_NERRNO(ftruncate(fd, sz));
}

ಪೂರ್ಣಾಙ್ಕ memfd_new_and_seal(ಧ್ರುವ ವರ್ಣ *name, ಧ್ರುವ ಶೂನ್ಯ *data, size_t sz) {
        _cleanup_close_ ಪೂರ್ಣಾಙ್ಕ fd = -EBADF;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(data || sz == 0);

        ಯದಿ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ಯದಿ (fd < 0)
                ಪ್ರತ್ಯಾಗಮ fd;

        ಯದಿ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;
        }

        r = memfd_set_sealed(fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ TAKE_FD(fd);
}
