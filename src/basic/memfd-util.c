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
        אם (mfd != -EINVAL)
                החזר mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        אם (mode == mode_compat)
                החזר mfd;

        החזר RET_NERRNO(memfd_create(name, mode_compat));
}

صحيح memfd_new_full(ثابت حرف *name, unsigned extra_flags) {
        _cleanup_free_ حرف *g = عدم;
        صحيح r;

        אם (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ حرف *comm = عدم;
                r = pid_get_comm(/* pid= */ 0, &comm);
                אם (r < 0)
                        החזר r;

                אם (isempty(comm))
                        name = "sd";
                אחרת {
                        g = strjoin("sd-", comm);
                        אם (!g)
                                החזר -ENOMEM;

                        name = g;
                }
        }

        החזר memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ساكن صحيح memfd_add_seals(صحيح fd, unsigned seals) {
        أكد(fd >= 0);

        החזר RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

ساكن صحيح memfd_get_seals(صحيح fd, unsigned *ret_seals) {
        صحيح r;

        أكد(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        אם (r < 0)
                החזר r;

        אם (ret_seals)
                *ret_seals = r;
        החזר 0;
}

صحيح memfd_set_sealed(صحيح fd) {
        החזר memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_get_sealed(صحيح fd) {
        unsigned seals;
        صحيح r;

        r = memfd_get_seals(fd, &seals);
        אם (r < 0)
                החזר r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        החזר FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_set_sealed_gracefully(صحيح fd) {
        صحيح r;

        r = memfd_set_sealed(fd);
        אם (r != -EPERM)
                החזר r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        אם (r < 0)
                החזר r;
        אם (r == 0)
                החזר -EPERM; /* Propagate the original error. */
        החזר 0;
}

صحيح memfd_get_size(صحيح fd, uint64_t *ret) {
        بنية stat stat;

        أكد(fd >= 0);
        أكد(ret);

        אם (fstat(fd, &stat) < 0)
                החזר -errno;

        *ret = stat.st_size;
        החזר 0;
}

صحيح memfd_set_size(صحيح fd, uint64_t sz) {
        أكد(fd >= 0);

        החזר RET_NERRNO(ftruncate(fd, sz));
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *data, size_t sz) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(data || sz == 0);

        אם (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        אם (fd < 0)
                החזר fd;

        אם (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                אם (r < 0)
                        החזר r;
        }

        r = memfd_set_sealed(fd);
        אם (r < 0)
                החזר r;

        החזר TAKE_FD(fd);
}
