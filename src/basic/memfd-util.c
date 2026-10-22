/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <stdio.h>
#অন্তর্ভাৱ <sys/stat.h>
#অন্তর্ভাৱ <unistd.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "fd-util.h"
#অন্তর্ভাৱ "io-util.h"
#অন্তর্ভাৱ "memfd-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "string-util.h"

পূর্ণাঙ্ক memfd_create_wrapper(ধ্রুৱ ৱর্ণ *name, unsigned mode) {
        unsigned mode_compat;
        পূর্ণাঙ্ক mfd;

        প্রতিজ্ঞা(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        যদি (mfd != -EINVAL)
                ঘূৰাই mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        যদি (mode == mode_compat)
                ঘূৰাই mfd;

        ঘূৰাই RET_NERRNO(memfd_create(name, mode_compat));
}

পূর্ণাঙ্ক memfd_new_full(ধ্রুৱ ৱর্ণ *name, unsigned extra_flags) {
        _cleanup_free_ ৱর্ণ *g = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ ৱর্ণ *comm = রিক্ত;
                r = pid_get_comm(/* pid= */ 0, &comm);
                যদি (r < 0)
                        ঘূৰাই r;

                যদি (isempty(comm))
                        name = "sd";
                অন্যথা {
                        g = strjoin("sd-", comm);
                        যদি (!g)
                                ঘূৰাই -ENOMEM;

                        name = g;
                }
        }

        ঘূৰাই memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

স্থির পূর্ণাঙ্ক memfd_add_seals(পূর্ণাঙ্ক fd, unsigned seals) {
        প্রতিজ্ঞা(fd >= 0);

        ঘূৰাই RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

স্থির পূর্ণাঙ্ক memfd_get_seals(পূর্ণাঙ্ক fd, unsigned *ret_seals) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        যদি (r < 0)
                ঘূৰাই r;

        যদি (ret_seals)
                *ret_seals = r;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক memfd_set_sealed(পূর্ণাঙ্ক fd) {
        ঘূৰাই memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

পূর্ণাঙ্ক memfd_get_sealed(পূর্ণাঙ্ক fd) {
        unsigned seals;
        পূর্ণাঙ্ক r;

        r = memfd_get_seals(fd, &seals);
        যদি (r < 0)
                ঘূৰাই r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ঘূৰাই FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

পূর্ণাঙ্ক memfd_set_sealed_gracefully(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        r = memfd_set_sealed(fd);
        যদি (r != -EPERM)
                ঘূৰাই r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == 0)
                ঘূৰাই -EPERM; /* Propagate the original error. */
        ঘূৰাই 0;
}

পূর্ণাঙ্ক memfd_get_size(পূর্ণাঙ্ক fd, uint64_t *ret) {
        সংরচনা stat stat;

        প্রতিজ্ঞা(fd >= 0);
        প্রতিজ্ঞা(ret);

        যদি (fstat(fd, &stat) < 0)
                ঘূৰাই -errno;

        *ret = stat.st_size;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক memfd_set_size(পূর্ণাঙ্ক fd, uint64_t sz) {
        প্রতিজ্ঞা(fd >= 0);

        ঘূৰাই RET_NERRNO(ftruncate(fd, sz));
}

পূর্ণাঙ্ক memfd_new_and_seal(ধ্রুৱ ৱর্ণ *name, ধ্রুৱ শূন্য *data, size_t sz) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(data || sz == 0);

        যদি (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        যদি (fd < 0)
                ঘূৰাই fd;

        যদি (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                যদি (r < 0)
                        ঘূৰাই r;
        }

        r = memfd_set_sealed(fd);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই TAKE_FD(fd);
}
