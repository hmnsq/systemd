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

صحيح memfd_create_wrapper(ثابت حرف *name, بنانشان گهڻائي) {
        بنانشان گهڻائي_compat;
        صحيح mfd;

        أكد(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, گهڻائي));
        إذا (mfd != -EINVAL)
                أرجع mfd;

        گهڻائي_compat = گهڻائي & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        إذا (گهڻائي == گهڻائي_compat)
                أرجع mfd;

        أرجع RET_NERRNO(memfd_create(name, گهڻائي_compat));
}

صحيح memfd_new_ڀريل(ثابت حرف *name, بنانشان extra_flags) {
        _cleanup_free_ حرف *g = عدم;
        صحيح r;

        إذا (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ حرف *comm = عدم;
                r = pid_get_comm(/* pid= */ 0, &comm);
                إذا (r < 0)
                        أرجع r;

                إذا (isempty(comm))
                        name = "sd";
                وإلا {
                        g = strjoin("sd-", comm);
                        إذا (!g)
                                أرجع -ENOMEM;

                        name = g;
                }
        }

        أرجع memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ساكن صحيح memfd_جوڙيو_seals(صحيح fd, بنانشان seals) {
        أكد(fd >= 0);

        أرجع RET_NERRNO(fcntl(fd, F_جوڙيو_SEALS, seals));
}

ساكن صحيح memfd_get_seals(صحيح fd, بنانشان *ret_seals) {
        صحيح r;

        أكد(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        إذا (r < 0)
                أرجع r;

        إذا (ret_seals)
                *ret_seals = r;
        أرجع 0;
}

صحيح memfd_سيٽ_sealed(صحيح fd) {
        أرجع memfd_جوڙيو_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_get_sealed(صحيح fd) {
        بنانشان seals;
        صحيح r;

        r = memfd_get_seals(fd, &seals);
        إذا (r < 0)
                أرجع r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        أرجع FLAGS_سيٽ(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

صحيح memfd_سيٽ_sealed_gracefully(صحيح fd) {
        صحيح r;

        r = memfd_سيٽ_sealed(fd);
        إذا (r != -EPERM)
                أرجع r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        إذا (r < 0)
                أرجع r;
        إذا (r == 0)
                أرجع -EPERM; /* Propagate the original error. */
        أرجع 0;
}

صحيح memfd_get_سائيز(صحيح fd, uint64_t *ret) {
        بنية stat stat;

        أكد(fd >= 0);
        أكد(ret);

        إذا (fstat(fd, &stat) < 0)
                أرجع -errno;

        *ret = stat.st_size;
        أرجع 0;
}

صحيح memfd_سيٽ_سائيز(صحيح fd, uint64_t sz) {
        أكد(fd >= 0);

        أرجع RET_NERRNO(ftruncate(fd, sz));
}

صحيح memfd_new_and_seal(ثابت حرف *name, ثابت فراغ *معلومات, size_t sz) {
        _cleanup_close_ صحيح fd = -EBADF;
        صحيح r;

        أكد(معلومات || sz == 0);

        إذا (sz == SIZE_MAX)
                sz = لڙي_ڊيگهه(معلومات);

        fd = memfd_new_ڀريل(name, MFD_ALLOW_SEALING);
        إذا (fd < 0)
                أرجع fd;

        إذا (sz > 0) {
                r = pwrite_ڀريل(fd, معلومات, sz, 0);
                إذا (r < 0)
                        أرجع r;
        }

        r = memfd_سيٽ_sealed(fd);
        إذا (r < 0)
                أرجع r;

        أرجع TAKE_FD(fd);
}
