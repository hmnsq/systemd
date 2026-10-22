/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdio.h>
#অন্তর্ভাব <sys/stat.h>
#অন্তর্ভাব <unistd.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "io-util.h"
#অন্তর্ভাব "memfd-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"

পূর্ণসংখ্যা memfd_create_wrapper(ধ্রুবক অক্ষর *name, unsigned mode) {
        unsigned mode_compat;
        পূর্ণসংখ্যা mfd;

        প্রতিজ্ঞা(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        যদি (mfd != -EINVAL)
                ফেরত mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        যদি (mode == mode_compat)
                ফেরত mfd;

        ফেরত RET_NERRNO(memfd_create(name, mode_compat));
}

পূর্ণসংখ্যা memfd_new_full(ধ্রুবক অক্ষর *name, unsigned extra_flags) {
        _cleanup_free_ অক্ষর *g = ফাঁকা;
        পূর্ণসংখ্যা r;

        যদি (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ অক্ষর *comm = ফাঁকা;
                r = pid_get_comm(/* pid= */ 0, &comm);
                যদি (r < 0)
                        ফেরত r;

                যদি (isempty(comm))
                        name = "sd";
                অন্যথা {
                        g = strjoin("sd-", comm);
                        যদি (!g)
                                ফেরত -ENOMEM;

                        name = g;
                }
        }

        ফেরত memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

স্থির পূর্ণসংখ্যা memfd_add_seals(পূর্ণসংখ্যা fd, unsigned seals) {
        প্রতিজ্ঞা(fd >= 0);

        ফেরত RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

স্থির পূর্ণসংখ্যা memfd_get_seals(পূর্ণসংখ্যা fd, unsigned *ret_seals) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        যদি (r < 0)
                ফেরত r;

        যদি (ret_seals)
                *ret_seals = r;
        ফেরত 0;
}

পূর্ণসংখ্যা memfd_set_sealed(পূর্ণসংখ্যা fd) {
        ফেরত memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

পূর্ণসংখ্যা memfd_get_sealed(পূর্ণসংখ্যা fd) {
        unsigned seals;
        পূর্ণসংখ্যা r;

        r = memfd_get_seals(fd, &seals);
        যদি (r < 0)
                ফেরত r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ফেরত FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

পূর্ণসংখ্যা memfd_set_sealed_gracefully(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        r = memfd_set_sealed(fd);
        যদি (r != -EPERM)
                ফেরত r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        যদি (r < 0)
                ফেরত r;
        যদি (r == 0)
                ফেরত -EPERM; /* Propagate the original error. */
        ফেরত 0;
}

পূর্ণসংখ্যা memfd_get_আকার(পূর্ণসংখ্যা fd, uint64_t *ret) {
        কাঠামো stat stat;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (fstat(fd, &stat) < 0)
                ফেরত -errno;

        *ret = stat.st_size;
        ফেরত 0;
}

পূর্ণসংখ্যা memfd_set_আকার(পূর্ণসংখ্যা fd, uint64_t sz) {
        প্রতিজ্ঞা(fd >= 0);

        ফেরত RET_NERRNO(ftruncate(fd, sz));
}

পূর্ণসংখ্যা memfd_new_and_seal(ধ্রুবক অক্ষর *name, ধ্রুবক শূন্য *তথ্য, size_t sz) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(তথ্য || sz == 0);

        যদি (sz == SIZE_MAX)
                sz = strlen(তথ্য);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        যদি (fd < 0)
                ফেরত fd;

        যদি (sz > 0) {
                r = pwrite_full(fd, তথ্য, sz, 0);
                যদি (r < 0)
                        ফেরত r;
        }

        r = memfd_set_sealed(fd);
        যদি (r < 0)
                ফেরত r;

        ফেরত TAKE_FD(fd);
}
