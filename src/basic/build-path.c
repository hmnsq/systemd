/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ <elf.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <link.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <sys/auxv.h>

#ಅನ್ತರ್ಭಾವ "alloc-util.h"
#ಅನ್ತರ್ಭಾವ "build-path.h"
#ಅನ್ತರ್ಭಾವ "path-util.h"
#ಅನ್ತರ್ಭಾವ "process-util.h"
#ಅನ್ತರ್ಭಾವ "string-util.h"
#ಅನ್ತರ್ಭಾವ "unistd.h"

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_runpath_from_dynamic(ಸ್ಥಿರಾಂಕ ElfW(Dyn) *d, ElfW(Addr) bias, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        size_t runpath_ಸೂಚ್ಯಂಕ = SIZE_MAX, rpath_ಸೂಚ್ಯಂಕ = SIZE_MAX;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *strtab = ಖಾಲಿ;

        ಪ್ರತಿಜ್ಞಾ(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        ಗಾಗಿ (; d->d_tag != DT_NULL; d++) {

                ಆಯ್ಕೆ (d->d_tag) {

                ಸ್ಥಿತಿ DT_RUNPATH:
                        runpath_ಸೂಚ್ಯಂಕ = (size_t) d->d_un.d_val;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DT_RPATH:
                        rpath_ಸೂಚ್ಯಂಕ = (size_t) d->d_un.d_val;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ಆದರೆ (d->d_un.d_val != 0)
                                strtab = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *) ((uintptr_t) d->d_un.d_val
#ಆದರೆ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#ಆದರೆ_ಅಂತ್ಯ
                                );
                        ನಿಲ್ಲಿಸು;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ಆದರೆ (strtab && runpath_ಸೂಚ್ಯಂಕ != SIZE_MAX)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (!strtab)
                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ಆದರೆ (runpath_ಸೂಚ್ಯಂಕ != SIZE_MAX) {
                ಆದರೆ (ret)
                        *ret = strtab + runpath_ಸೂಚ್ಯಂಕ;
                ಹಿಂತಿರುಗಿ 1;
        }

        ಆದರೆ (rpath_ಸೂಚ್ಯಂಕ != SIZE_MAX) {
                ಆದರೆ (ret)
                        *ret = strtab + rpath_ಸೂಚ್ಯಂಕ;
                ಹಿಂತಿರುಗಿ 1;
        }

        ಆದರೆ (ret)
                *ret = ಖಾಲಿ;

        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_runpath(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {
        ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ಆದರೆ (phdr == 0)
                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ಆದರೆ (phnum == 0)
                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ಆದರೆ (phent < ಗಾತ್ರ(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool ಸಿಕ್ಕಿತು_bias = false, ಸಿಕ್ಕಿತು_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ i = 0; i < phnum; i++) {
                ಸ್ಥಿರಾಂಕ ElfW(Phdr) *p = (ಸ್ಥಿರಾಂಕ ElfW(Phdr)*) (phdr + (i * phent));

                ಆಯ್ಕೆ (p->p_type) {

                ಸ್ಥಿತಿ PT_PHDR:
                        ಆದರೆ (p->p_vaddr > phdr) /* safety overflow check */
                                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        ಸಿಕ್ಕಿತು_bias = true;
                        ನಿಲ್ಲಿಸು;

                ಸ್ಥಿತಿ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        ಸಿಕ್ಕಿತು_dyn = true;
                        ನಿಲ್ಲಿಸು;
                }

                ಆದರೆ (ಸಿಕ್ಕಿತು_bias && ಸಿಕ್ಕಿತು_dyn)
                        ನಿಲ್ಲಿಸು;
        }

        ಆದರೆ (!ಸಿಕ್ಕಿತು_dyn)
                ಹಿಂತಿರುಗಿ -ENOTRECOVERABLE;

        ಹಿಂತಿರುಗಿ get_runpath_from_dynamic((ಸ್ಥಿರಾಂಕ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

ಪೂರ್ಣಾಂಕ get_build_exec_dir(ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        /* Returns the build execution directory if we are invoked in a build environment. Specifically, this
         * checks if the main program binary has an rpath/runpath set (i.e. an explicit directory where to
         * look for shared libraries) to $ORIGIN. If so we know that this is not a regular installed binary,
         * but one which shall acquire its libraries from below a directory it is located in, i.e. a build
         * directory or similar. In that case it typically makes sense to also search for our auxiliary
         * executables we fork() off in a directory close to our main program binary, rather than in the
         * system.
         *
         * This function is supposed to be used when looking for "callout" binaries that are closely related
         * to the main program (i.e. speak a specific protocol between each other). And where it's generally
         * a good idea to use the binary from the build tree (if there is one) instead of the system.
         *
         * Note that this does *not* actually return the rpath/runpath but the instead the directory the main
         * executable was found in. This follows the logic that the result is supposed to be used for
         * executable binaries (i.e. stuff in bindir), not for shared libraries (i.e. stuff in libdir), and
         * hence the literal shared library path would just be wrong.
         *
         * TLDR: if we look for callouts in this dir first, running binaries from the meson build tree
         * automatically uses the right callout.
         *
         * Returns:
         *     -ENOEXEC         → We are not running in an rpath/runpath $ORIGIN environment
         *     -ENOENT          → We don't know our own binary path
         *     -NOTRECOVERABLE  → Dynamic binary information missing?
         */

        ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ runpath_cached = -ERRNO_MAX-1;
        ಆದರೆ (runpath_cached == -ERRNO_MAX-1) {
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *runpath = ಖಾಲಿ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ಆದರೆ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ಆದರೆ (runpath_cached < 0)
                ಹಿಂತಿರುಗಿ runpath_cached;
        ಆದರೆ (runpath_cached == 0)
                ಹಿಂತಿರುಗಿ -ENOEXEC;

        _cleanup_free_ ಅಕ್ಷರ *exe = ಖಾಲಿ;
        r = get_process_exe(0, &exe);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ runpath_cached = r;

        ಹಿಂತಿರುಗಿ ಮಾರ್ಗ_ಹೊರತೆಗೆ_directory(exe, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_build_dir_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಅಕ್ಷರ **ret) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ಅಕ್ಷರ *build_dir = ಖಾಲಿ;
        r = get_build_exec_dir(&build_dir);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        _cleanup_free_ ಅಕ್ಷರ *np = ಮಾರ್ಗ_join(build_dir, fn);
        ಆದರೆ (!np)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        *ret = TAKE_PTR(np);
        ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕಂಡುಹಿಡಿ_environment_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **ret) {

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ ಅಕ್ಷರ *s = strdup(fn);
        ಆದರೆ (!s)
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ಆದರೆ (!strextend(&s, "_PATH"))
                ಹಿಂತಿರುಗಿ -ENOMEM;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        e = secure_getenv(s);
        ಆದರೆ (!e)
                ಹಿಂತಿರುಗಿ -ENXIO;

        *ret = e;
        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ invoke_callout_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ *ಸ್ಥಿರಾಂಕ argv[]) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ;
        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == O_DIRECTORY) /* Uh? */
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಆದರೆ (ಕಂಡುಹಿಡಿ_environment_ದ್ವಿಮಾನ(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ಹಿಂತಿರುಗಿ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ ಅಕ್ಷರ *np = ಖಾಲಿ;
        ಆದರೆ (ಕಂಡುಹಿಡಿ_build_dir_ದ್ವಿಮಾನ(fn, &np) >= 0)
                execv(np, argv);

        execvp(ಮಾರ್ಗ, argv);
        ಹಿಂತಿರುಗಿ -errno;
}

ಪೂರ್ಣಾಂಕ pin_callout_ದ್ವಿಮಾನ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಅಕ್ಷರ **ret_ಮಾರ್ಗ) {
        ಪೂರ್ಣಾಂಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(ಮಾರ್ಗ);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ ಅಕ್ಷರ *fn = ಖಾಲಿ;
        r = ಮಾರ್ಗ_ಹೊರತೆಗೆ_filename(ಮಾರ್ಗ, &fn);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;
        ಆದರೆ (r == O_DIRECTORY) /* Uh? */
                ಹಿಂತಿರುಗಿ -EISDIR;

        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *e;
        ಆದರೆ (ಕಂಡುಹಿಡಿ_environment_ದ್ವಿಮಾನ(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ಖಾಲಿ, ret_ಮಾರ್ಗ, &fd);
                ಆದರೆ (r < 0)
                        ಹಿಂತಿರುಗಿ r;

                ಹಿಂತಿರುಗಿ fd;
        }

        _cleanup_free_ ಅಕ್ಷರ *np = ಖಾಲಿ;
        ಆದರೆ (ಕಂಡುಹಿಡಿ_build_dir_ದ್ವಿಮಾನ(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ಖಾಲಿ, ret_ಮಾರ್ಗ, &fd);
                ಆದರೆ (r >= 0)
                        ಹಿಂತಿರುಗಿ fd;
        }

        r = ಕಂಡುಹಿಡಿ_executable_ತುಂಬಿದ(ಮಾರ್ಗ, /* root= */ ಖಾಲಿ,
                                 /* exec_search_path= */ ಖಾಲಿ, /* use_path_envvar= */ true,
                                 ret_ಮಾರ್ಗ, &fd);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        ಹಿಂತಿರುಗಿ fd;
}
