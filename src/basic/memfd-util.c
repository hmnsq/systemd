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

صحیح memfd_create_wrapper(ثابت حرف *name, غیرعلامتی منوال) {
        غیرعلامتی منوال_compat;
        صحیح mfd;

        أكد(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, منوال));
        اگر (mfd != -EINVAL)
                واپس mfd;

        منوال_compat = منوال & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        اگر (منوال == منوال_compat)
                واپس mfd;

        واپس RET_NERRNO(memfd_create(name, منوال_compat));
}

صحیح memfd_new_بھرا(ثابت حرف *name, غیرعلامتی extra_flags) {
        _cleanup_free_ حرف *g = تہی;
        صحیح r;

        اگر (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ حرف *comm = تہی;
                r = pid_get_comm(/* pid= */ 0, &comm);
                اگر (r < 0)
                        واپس r;

                اگر (isempty(comm))
                        name = "sd";
                ورنہ {
                        g = strjoin("sd-", comm);
                        اگر (!g)
                                واپس -ENOMEM;

                        name = g;
                }
        }

        واپس memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ساکن صحیح memfd_جوڑو_seals(صحیح fd, غیرعلامتی seals) {
        أكد(fd >= 0);

        واپس RET_NERRNO(fcntl(fd, F_جوڑو_SEALS, seals));
}

ساکن صحیح memfd_get_seals(صحیح fd, غیرعلامتی *ret_seals) {
        صحیح r;

        أكد(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        اگر (r < 0)
                واپس r;

        اگر (ret_seals)
                *ret_seals = r;
        واپس 0;
}

صحیح memfd_سیٹ_sealed(صحیح fd) {
        واپس memfd_جوڑو_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحیح memfd_get_sealed(صحیح fd) {
        غیرعلامتی seals;
        صحیح r;

        r = memfd_get_seals(fd, &seals);
        اگر (r < 0)
                واپس r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        واپس FLAGS_سیٹ(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحیح memfd_سیٹ_sealed_gracefully(صحیح fd) {
        صحیح r;

        r = memfd_سیٹ_sealed(fd);
        اگر (r != -EPERM)
                واپس r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        اگر (r < 0)
                واپس r;
        اگر (r == 0)
                واپس -EPERM; /* Propagate the original error. */
        واپس 0;
}

صحیح memfd_get_سائز(صحیح fd, uint64_t *ret) {
        ساخت stat stat;

        أكد(fd >= 0);
        أكد(ret);

        اگر (fstat(fd, &stat) < 0)
                واپس -errno;

        *ret = stat.st_size;
        واپس 0;
}

صحیح memfd_سیٹ_سائز(صحیح fd, uint64_t sz) {
        أكد(fd >= 0);

        واپس RET_NERRNO(ftruncate(fd, sz));
}

صحیح memfd_new_and_seal(ثابت حرف *name, ثابت خالی *ڈیٹا, size_t sz) {
        _cleanup_close_ صحیح fd = -EBADF;
        صحیح r;

        أكد(ڈیٹا || sz == 0);

        اگر (sz == SIZE_MAX)
                sz = لڑی_لمبائی(ڈیٹا);

        fd = memfd_new_بھرا(name, MFD_ALLOW_SEALING);
        اگر (fd < 0)
                واپس fd;

        اگر (sz > 0) {
                r = pwrite_بھرا(fd, ڈیٹا, sz, 0);
                اگر (r < 0)
                        واپس r;
        }

        r = memfd_سیٹ_sealed(fd);
        اگر (r < 0)
                واپس r;

        واپس TAKE_FD(fd);
}
