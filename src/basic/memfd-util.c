/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <stdio.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "fd-util.h"
#ଅନ୍ତର୍ଭାଵ "io-util.h"
#ଅନ୍ତର୍ଭାଵ "memfd-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_create_wrapper(ଅଚଳ ଅକ୍ଷର *name, unsigned mode) {
        unsigned mode_compat;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା mfd;

        ପ୍ରତିଜ୍ଞା(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        ଯଦି (mfd != -EINVAL)
                ଫେରନ୍ତୁ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        ଯଦି (mode == mode_compat)
                ଫେରନ୍ତୁ mfd;

        ଫେରନ୍ତୁ RET_NERRNO(memfd_create(name, mode_compat));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new_full(ଅଚଳ ଅକ୍ଷର *name, unsigned extra_flags) {
        _cleanup_free_ ଅକ୍ଷର *g = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ଯଦି (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ ଅକ୍ଷର *comm = ଖାଲି;
                r = pid_get_comm(/* pid= */ 0, &comm);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (isempty(comm))
                        name = "sd";
                ଅନ୍ୟଥା {
                        g = strjoin("sd-", comm);
                        ଯଦି (!g)
                                ଫେରନ୍ତୁ -ENOMEM;

                        name = g;
                }
        }

        ଫେରନ୍ତୁ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_add_seals(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned seals) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଫେରନ୍ତୁ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_get_seals(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, unsigned *ret_seals) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (ret_seals)
                *ret_seals = r;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_sealed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ଫେରନ୍ତୁ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_get_sealed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        unsigned seals;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = memfd_get_seals(fd, &seals);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        ଫେରନ୍ତୁ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_sealed_gracefully(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        r = memfd_set_sealed(fd);
        ଯଦି (r != -EPERM)
                ଫେରନ୍ତୁ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == 0)
                ଫେରନ୍ତୁ -EPERM; /* Propagate the original error. */
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_get_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t *ret) {
        ସଂରଚନା stat stat;

        ପ୍ରତିଜ୍ଞା(fd >= 0);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (fstat(fd, &stat) < 0)
                ଫେରନ୍ତୁ -errno;

        *ret = stat.st_size;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_set_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, uint64_t sz) {
        ପ୍ରତିଜ୍ଞା(fd >= 0);

        ଫେରନ୍ତୁ RET_NERRNO(ftruncate(fd, sz));
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା memfd_new_and_seal(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ ଶୂନ୍ୟ *data, size_t sz) {
        _cleanup_close_ ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = -EBADF;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(data || sz == 0);

        ଯଦି (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        ଯଦି (fd < 0)
                ଫେରନ୍ତୁ fd;

        ଯଦି (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;
        }

        r = memfd_set_sealed(fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ TAKE_FD(fd);
}
