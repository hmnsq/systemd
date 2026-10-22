/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdio.h>
#شامل <sys/stat.h>
#شامل <unistd.h>

#شامل "alloc-util.h"
#شامل "errno-util.h"
#شامل "fd-util.h"
#شامل "io-util.h"
#شامل "memfd-util.h"
#شامل "process-util.h"
#شامل "string-util.h"

صحیح memfd_create_wrapper(ثابت نویسه *name, unsigned mode) {
        unsigned mode_compat;
        صحیح mfd;

        أكد(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        اگر (mfd != -EINVAL)
                بازگشت mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        اگر (mode == mode_compat)
                بازگشت mfd;

        بازگشت RET_NERRNO(memfd_create(name, mode_compat));
}

صحیح memfd_new_full(ثابت نویسه *name, unsigned extra_flags) {
        _cleanup_free_ نویسه *g = پوچ;
        صحیح r;

        اگر (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ نویسه *comm = پوچ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                اگر (r < 0)
                        بازگشت r;

                اگر (isempty(comm))
                        name = "sd";
                وگرنه {
                        g = strjoin("sd-", comm);
                        اگر (!g)
                                بازگشت -ENOMEM;

                        name = g;
                }
        }

        بازگشت memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ایستا صحیح memfd_add_seals(صحیح fd, unsigned seals) {
        أكد(fd >= 0);

        بازگشت RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

ایستا صحیح memfd_get_seals(صحیح fd, unsigned *ret_seals) {
        صحیح r;

        أكد(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        اگر (r < 0)
                بازگشت r;

        اگر (ret_seals)
                *ret_seals = r;
        بازگشت 0;
}

صحیح memfd_set_sealed(صحیح fd) {
        بازگشت memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحیح memfd_get_sealed(صحیح fd) {
        unsigned seals;
        صحیح r;

        r = memfd_get_seals(fd, &seals);
        اگر (r < 0)
                بازگشت r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        بازگشت FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحیح memfd_set_sealed_gracefully(صحیح fd) {
        صحیح r;

        r = memfd_set_sealed(fd);
        اگر (r != -EPERM)
                بازگشت r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        اگر (r < 0)
                بازگشت r;
        اگر (r == 0)
                بازگشت -EPERM; /* Propagate the original error. */
        بازگشت 0;
}

صحیح memfd_get_size(صحیح fd, uint64_t *ret) {
        ساختار stat stat;

        أكد(fd >= 0);
        أكد(ret);

        اگر (fstat(fd, &stat) < 0)
                بازگشت -errno;

        *ret = stat.st_size;
        بازگشت 0;
}

صحیح memfd_set_size(صحیح fd, uint64_t sz) {
        أكد(fd >= 0);

        بازگشت RET_NERRNO(ftruncate(fd, sz));
}

صحیح memfd_new_and_seal(ثابت نویسه *name, ثابت تهی *data, size_t sz) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(data || sz == 0);

        اگر (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        اگر (fd < 0)
                بازگشت fd;

        اگر (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                اگر (r < 0)
                        بازگشت r;
        }

        r = memfd_set_sealed(fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت TAKE_FD(fd);
}
