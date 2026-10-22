/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <sys/stat.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "io-util.h"
#അന്തര്ഭാവ "memfd-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"

പൂർണ്ണസംഖ്യ memfd_create_wrapper(സ്ഥിരം അക്ഷരം *name, unsigned mode) {
        unsigned mode_compat;
        പൂർണ്ണസംഖ്യ mfd;

        പ്രതിജ്ഞാ(name);

        /* Wrapper around memfd_create() which adds compat with older kernels where memfd_create() didn't
         * support MFD_EXEC/MFD_NOEXEC_SEAL. (kernel 6.3+) */

        mfd = RET_NERRNO(memfd_create(name, mode));
        എങ്കിൽ (mfd != -EINVAL)
                തിരികെ mfd;

        mode_compat = mode & ~(MFD_EXEC | MFD_NOEXEC_SEAL);

        എങ്കിൽ (mode == mode_compat)
                തിരികെ mfd;

        തിരികെ RET_NERRNO(memfd_create(name, mode_compat));
}

പൂർണ്ണസംഖ്യ memfd_new_full(സ്ഥിരം അക്ഷരം *name, unsigned extra_flags) {
        _cleanup_free_ അക്ഷരം *g = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!name) {

                /* If no name is specified we generate one. We include
                 * a hint indicating our library implementation, and
                 * add the thread name to it */

                _cleanup_free_ അക്ഷരം *comm = ശൂന്യ;
                r = pid_get_comm(/* pid= */ 0, &comm);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (isempty(comm))
                        name = "sd";
                അല്ലെങ്കിൽ {
                        g = strjoin("sd-", comm);
                        എങ്കിൽ (!g)
                                തിരികെ -ENOMEM;

                        name = g;
                }
        }

        തിരികെ memfd_create_wrapper(
                        name,
                        MFD_CLOEXEC | MFD_NOEXEC_SEAL | extra_flags);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ memfd_add_seals(പൂർണ്ണസംഖ്യ fd, unsigned seals) {
        പ്രതിജ്ഞാ(fd >= 0);

        തിരികെ RET_NERRNO(fcntl(fd, F_ADD_SEALS, seals));
}

സ്ഥിതം പൂർണ്ണസംഖ്യ memfd_get_seals(പൂർണ്ണസംഖ്യ fd, unsigned *ret_seals) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = RET_NERRNO(fcntl(fd, F_GET_SEALS));
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_seals)
                *ret_seals = r;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ memfd_set_sealed(പൂർണ്ണസംഖ്യ fd) {
        തിരികെ memfd_add_seals(fd, F_SEAL_SEAL | F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

പൂർണ്ണസംഖ്യ memfd_get_sealed(പൂർണ്ണസംഖ്യ fd) {
        unsigned seals;
        പൂർണ്ണസംഖ്യ r;

        r = memfd_get_seals(fd, &seals);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* We ignore F_SEAL_EXEC here to support older kernels.
         * It is supported since kernel v6.3 (6fd7353829cafc4067aad9eea0dc95da67e7df16). */
        തിരികെ FLAGS_SET(seals, F_SEAL_SHRINK | F_SEAL_GROW | F_SEAL_WRITE);
}

പൂർണ്ണസംഖ്യ memfd_set_sealed_gracefully(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        r = memfd_set_sealed(fd);
        എങ്കിൽ (r != -EPERM)
                തിരികെ r;

        /* Already sealed? Check if required seals are set. */
        r = memfd_get_sealed(fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0)
                തിരികെ -EPERM; /* Propagate the original error. */
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ memfd_get_size(പൂർണ്ണസംഖ്യ fd, uint64_t *ret) {
        ഘടന stat stat;

        പ്രതിജ്ഞാ(fd >= 0);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (fstat(fd, &stat) < 0)
                തിരികെ -errno;

        *ret = stat.st_size;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ memfd_set_size(പൂർണ്ണസംഖ്യ fd, uint64_t sz) {
        പ്രതിജ്ഞാ(fd >= 0);

        തിരികെ RET_NERRNO(ftruncate(fd, sz));
}

പൂർണ്ണസംഖ്യ memfd_new_and_seal(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം ശൂന്യം *data, size_t sz) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(data || sz == 0);

        എങ്കിൽ (sz == SIZE_MAX)
                sz = strlen(data);

        fd = memfd_new_full(name, MFD_ALLOW_SEALING);
        എങ്കിൽ (fd < 0)
                തിരികെ fd;

        എങ്കിൽ (sz > 0) {
                r = pwrite_full(fd, data, sz, 0);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        r = memfd_set_sealed(fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ TAKE_FD(fd);
}
