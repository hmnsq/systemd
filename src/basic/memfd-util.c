/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdio.h>
#ضمن <sys/stat.h>
#ضمن <unistd.h>

#ضمن "alloc-util.h"
#ضمن "errno-util.h"
#ضمن "fd-util.h"
#ضمن "io-util.h"
#ضمن "memfd-util.h"
#ضمن "process-util.h"
#ضمن "string-util.h"

صحيح memfd_create_wrapper(ثابت حرف *name, unsigned mode) {
        unsigned mode_compat;
        صحيح mfd;

        أكد(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ܐܢ (mfd != -EINVAL)
                ܦܢܐ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ܐܢ (mode == mode_compat)
                ܦܢܐ mfd;

        ܦܢܐ RET_NERRNO(memfd_create(name, mode_compat));
}

صحيح memfd_new_full(ثابت حرف *name, unsigned extra_flags) {
        _cleanup_free_ حرف *g = عدم;
        صحيح r;

        ܐܢ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ حرف *comm = عدم;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ܐܢ (r < 0)
                        ܦܢܐ r;

                ܐܢ (isempty(comm))
                        name = "sd";
                ܐܚܪܢܐ {
                        g = strjoin("sd-", comm);
                        ܐܢ (!g)
                                ܦܢܐ -ENOMEM;

                        name = g;
                }
        }

        ܦܢܐ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ساكن صحيح memfd_add_seals(صحيح fd, unsigned seals) {
        أكد(fd >= 0);

        ܦܢܐ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

ساكن صحيح memfd_get_seals(صحيح fd, unsigned *ret_seals) {
        صحيح r;

        أكد(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܐܢ (ret_seals)
                *ret_seals = r;
        ܦܢܐ 0;
}

صحيح memfd_set_sealed(صحيح fd) {
        ܦܢܐ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_get_sealed(صحيح fd) {
        unsigned seals;
        صحيح r;

        r = memfd_get_seals(fd, &seals);
        ܐܢ (r < 0)
                ܦܢܐ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ܦܢܐ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_set_sealed_gracefully(صحيح fd) {
        صحيح r;

        r = memfd_set_sealed(fd);
        ܐܢ (r != -EPERM)
                ܦܢܐ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ܐܢ (r < 0)
                ܦܢܐ r;
        ܐܢ (r == 0)
                ܦܢܐ -EPERM; /* Propagate the original error. */
        ܦܢܐ 0;
}

صحيح memfd_get_size(صحيح fd, uint64_t *ret) {
        بنية stat stat;

        أكد(fd >= 0);
        أكد(ret);

        ܐܢ (fstat(fd, &stat) < 0)
                ܦܢܐ -errno;

        *ret = stat.st_size;
        ܦܢܐ 0;
}

صحيح memfd_set_size(صحيح fd, uint64_t sz) {
        أكد(fd >= 0);

        ܦܢܐ RET_NERRNO(ftruncate(fd, sz));
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *data, size_t sz) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(data || sz == 0);

        ܐܢ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ܐܢ (fd < 0)
                ܦܢܐ fd;

        ܐܢ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ܐܢ (r < 0)
                        ܦܢܐ r;
        }

        r = memfd_set_sealed(fd);
        ܐܢ (r < 0)
                ܦܢܐ r;

        ܦܢܐ TAKE_FD(fd);
}
