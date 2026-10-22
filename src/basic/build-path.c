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

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_runpath_from_dynamic(ಧ್ರುವ ElfW(Dyn) *d, ElfW(Addr) bias, ಧ್ರುವ ವರ್ಣ **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ಧ್ರುವ ವರ್ಣ *strtab = ರಿಕ್ತ;

        ಪ್ರತಿಜ್ಞಾ(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        ಪ್ರತಿ (; d->d_tag != DT_NULL; d++) {

                ವಿಕಲ್ಪ (d->d_tag) {

                ಪಕ್ಷ DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        ಭಙ್ಗ;

                ಪಕ್ಷ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ಯದಿ (d->d_un.d_val != 0)
                                strtab = (ಧ್ರುವ ವರ್ಣ *) ((uintptr_t) d->d_un.d_val
#ಯದಿ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        ಭಙ್ಗ;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ಯದಿ (strtab && runpath_index != SIZE_MAX)
                        ಭಙ್ಗ;
        }

        ಯದಿ (!strtab)
                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ಯದಿ (runpath_index != SIZE_MAX) {
                ಯದಿ (ret)
                        *ret = strtab + runpath_index;
                ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಯದಿ (rpath_index != SIZE_MAX) {
                ಯದಿ (ret)
                        *ret = strtab + rpath_index;
                ಪ್ರತ್ಯಾಗಮ 1;
        }

        ಯದಿ (ret)
                *ret = ರಿಕ್ತ;

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_runpath(ಧ್ರುವ ವರ್ಣ **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ಯದಿ (phdr == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ಯದಿ (phnum == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ಯದಿ (phent < ಪರಿಮಾಣ(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        ಪ್ರತಿ (unsigned long i = 0; i < phnum; i++) {
                ಧ್ರುವ ElfW(Phdr) *p = (ಧ್ರುವ ElfW(Phdr)*) (phdr + (i * phent));

                ವಿಕಲ್ಪ (p->p_type) {

                ಪಕ್ಷ PT_PHDR:
                        ಯದಿ (p->p_vaddr > phdr) /* safety overflow check */
                                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        ಭಙ್ಗ;

                ಪಕ್ಷ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        ಭಙ್ಗ;
                }

                ಯದಿ (found_bias && found_dyn)
                        ಭಙ್ಗ;
        }

        ಯದಿ (!found_dyn)
                ಪ್ರತ್ಯಾಗಮ -ENOTRECOVERABLE;

        ಪ್ರತ್ಯಾಗಮ get_runpath_from_dynamic((ಧ್ರುವ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

ಪೂರ್ಣಾಙ್ಕ get_build_exec_dir(ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

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

        ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ runpath_cached = -ERRNO_MAX-1;
        ಯದಿ (runpath_cached == -ERRNO_MAX-1) {
                ಧ್ರುವ ವರ್ಣ *runpath = ರಿಕ್ತ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ಯದಿ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ಯದಿ (runpath_cached < 0)
                ಪ್ರತ್ಯಾಗಮ runpath_cached;
        ಯದಿ (runpath_cached == 0)
                ಪ್ರತ್ಯಾಗಮ -ENOEXEC;

        _cleanup_free_ ವರ್ಣ *exe = ರಿಕ್ತ;
        r = get_process_exe(0, &exe);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ runpath_cached = r;

        ಪ್ರತ್ಯಾಗಮ path_extract_directory(exe, ret);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_build_dir_ದ್ವಿಮಾನ(ಧ್ರುವ ವರ್ಣ *fn, ವರ್ಣ **ret) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(fn);
        ಪ್ರತಿಜ್ಞಾ(ret);

        _cleanup_free_ ವರ್ಣ *build_dir = ರಿಕ್ತ;
        r = get_build_exec_dir(&build_dir);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        _cleanup_free_ ವರ್ಣ *np = path_join(build_dir, fn);
        ಯದಿ (!np)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        *ret = TAKE_PTR(np);
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ find_environment_ದ್ವಿಮಾನ(ಧ್ರುವ ವರ್ಣ *fn, ಧ್ರುವ ವರ್ಣ **ret) {

        ಪ್ರತಿಜ್ಞಾ(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ ವರ್ಣ *s = strdup(fn);
        ಯದಿ (!s)
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ಯದಿ (!strextend(&s, "_PATH"))
                ಪ್ರತ್ಯಾಗಮ -ENOMEM;

        ಧ್ರುವ ವರ್ಣ *e;
        e = secure_getenv(s);
        ಯದಿ (!e)
                ಪ್ರತ್ಯಾಗಮ -ENXIO;

        *ret = e;
        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ invoke_callout_ದ್ವಿಮಾನ(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ *ಧ್ರುವ argv[]) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ;
        r = path_extract_filename(path, &fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == O_DIRECTORY) /* Uh? */
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಧ್ರುವ ವರ್ಣ *e;
        ಯದಿ (find_environment_ದ್ವಿಮಾನ(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ಪ್ರತ್ಯಾಗಮ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ ವರ್ಣ *np = ರಿಕ್ತ;
        ಯದಿ (find_build_dir_ದ್ವಿಮಾನ(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        ಪ್ರತ್ಯಾಗಮ -errno;
}

ಪೂರ್ಣಾಙ್ಕ pin_callout_ದ್ವಿಮಾನ(ಧ್ರುವ ವರ್ಣ *path, ವರ್ಣ **ret_path) {
        ಪೂರ್ಣಾಙ್ಕ r, fd;

        ಪ್ರತಿಜ್ಞಾ(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ ವರ್ಣ *fn = ರಿಕ್ತ;
        r = path_extract_filename(path, &fn);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;
        ಯದಿ (r == O_DIRECTORY) /* Uh? */
                ಪ್ರತ್ಯಾಗಮ -EISDIR;

        ಧ್ರುವ ವರ್ಣ *e;
        ಯದಿ (find_environment_ದ್ವಿಮಾನ(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ರಿಕ್ತ, ret_path, &fd);
                ಯದಿ (r < 0)
                        ಪ್ರತ್ಯಾಗಮ r;

                ಪ್ರತ್ಯಾಗಮ fd;
        }

        _cleanup_free_ ವರ್ಣ *np = ರಿಕ್ತ;
        ಯದಿ (find_build_dir_ದ್ವಿಮಾನ(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ರಿಕ್ತ, ret_path, &fd);
                ಯದಿ (r >= 0)
                        ಪ್ರತ್ಯಾಗಮ fd;
        }

        r = find_executable_full(path, /* root= */ ರಿಕ್ತ,
                                 /* exec_search_path= */ ರಿಕ್ತ, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        ಪ್ರತ್ಯಾಗಮ fd;
}
