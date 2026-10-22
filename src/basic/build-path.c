/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <elf.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <link.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <sys/auxv.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "build-path.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "process-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "unistd.h"

స్థిర పూర్ణాంకం get_runpath_from_dynamic(స్థిరరాశి ElfW(Dyn) *d, ElfW(Addr) bias, స్థిరరాశి అక్షరం **ret) {
        size_t runpath_సూచీ = SIZE_MAX, rpath_సూచీ = SIZE_MAX;
        స్థిరరాశి అక్షరం *strtab = ఖాళీ;

        ప్రతిజ్ఞా(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        కొరకు (; d->d_tag != DT_NULL; d++) {

                ఎంపిక (d->d_tag) {

                స్థితి DT_RUNPATH:
                        runpath_సూచీ = (size_t) d->d_un.d_val;
                        ఆపు;

                స్థితి DT_RPATH:
                        rpath_సూచీ = (size_t) d->d_un.d_val;
                        ఆపు;

                స్థితి DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ఒకవేళ (d->d_un.d_val != 0)
                                strtab = (స్థిరరాశి అక్షరం *) ((uintptr_t) d->d_un.d_val
#ఒకవేళ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#అయితే_ముగింపు
                                );
                        ఆపు;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ఒకవేళ (strtab && runpath_సూచీ != SIZE_MAX)
                        ఆపు;
        }

        ఒకవేళ (!strtab)
                తిరిగి -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ఒకవేళ (runpath_సూచీ != SIZE_MAX) {
                ఒకవేళ (ret)
                        *ret = strtab + runpath_సూచీ;
                తిరిగి 1;
        }

        ఒకవేళ (rpath_సూచీ != SIZE_MAX) {
                ఒకవేళ (ret)
                        *ret = strtab + rpath_సూచీ;
                తిరిగి 1;
        }

        ఒకవేళ (ret)
                *ret = ఖాళీ;

        తిరిగి 0;
}

స్థిర పూర్ణాంకం get_runpath(స్థిరరాశి అక్షరం **ret) {
        గుర్తులేని దీర్ఘ phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ఒకవేళ (phdr == 0)
                తిరిగి -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ఒకవేళ (phnum == 0)
                తిరిగి -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ఒకవేళ (phent < పరిమాణం(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                తిరిగి -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool దొరికింది_bias = false, దొరికింది_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        కొరకు (గుర్తులేని దీర్ఘ i = 0; i < phnum; i++) {
                స్థిరరాశి ElfW(Phdr) *p = (స్థిరరాశి ElfW(Phdr)*) (phdr + (i * phent));

                ఎంపిక (p->p_type) {

                స్థితి PT_PHDR:
                        ఒకవేళ (p->p_vaddr > phdr) /* safety overflow check */
                                తిరిగి -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        దొరికింది_bias = true;
                        ఆపు;

                స్థితి PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        దొరికింది_dyn = true;
                        ఆపు;
                }

                ఒకవేళ (దొరికింది_bias && దొరికింది_dyn)
                        ఆపు;
        }

        ఒకవేళ (!దొరికింది_dyn)
                తిరిగి -ENOTRECOVERABLE;

        తిరిగి get_runpath_from_dynamic((స్థిరరాశి ElfW(Dyn)*) (bias + dyn), bias, ret);
}

పూర్ణాంకం get_build_exec_dir(అక్షరం **ret) {
        పూర్ణాంకం r;

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

        స్థిర పూర్ణాంకం runpath_cached = -ERRNO_MAX-1;
        ఒకవేళ (runpath_cached == -ERRNO_MAX-1) {
                స్థిరరాశి అక్షరం *runpath = ఖాళీ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ఒకవేళ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ఒకవేళ (runpath_cached < 0)
                తిరిగి runpath_cached;
        ఒకవేళ (runpath_cached == 0)
                తిరిగి -ENOEXEC;

        _cleanup_free_ అక్షరం *exe = ఖాళీ;
        r = get_process_exe(0, &exe);
        ఒకవేళ (r < 0)
                తిరిగి runpath_cached = r;

        తిరిగి మార్గం_వెలికితీయి_directory(exe, ret);
}

స్థిర పూర్ణాంకం కనుగొను_build_dir_ద్విమాన(స్థిరరాశి అక్షరం *fn, అక్షరం **ret) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fn);
        ప్రతిజ్ఞా(ret);

        _cleanup_free_ అక్షరం *build_dir = ఖాళీ;
        r = get_build_exec_dir(&build_dir);
        ఒకవేళ (r < 0)
                తిరిగి r;

        _cleanup_free_ అక్షరం *np = మార్గం_join(build_dir, fn);
        ఒకవేళ (!np)
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(np);
        తిరిగి 0;
}

స్థిర పూర్ణాంకం కనుగొను_environment_ద్విమాన(స్థిరరాశి అక్షరం *fn, స్థిరరాశి అక్షరం **ret) {

        ప్రతిజ్ఞా(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ అక్షరం *s = strdup(fn);
        ఒకవేళ (!s)
                తిరిగి -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ఒకవేళ (!strextend(&s, "_PATH"))
                తిరిగి -ENOMEM;

        స్థిరరాశి అక్షరం *e;
        e = secure_getenv(s);
        ఒకవేళ (!e)
                తిరిగి -ENXIO;

        *ret = e;
        తిరిగి 0;
}

పూర్ణాంకం invoke_callout_ద్విమాన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం *స్థిరరాశి argv[]) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ అక్షరం *fn = ఖాళీ;
        r = మార్గం_వెలికితీయి_filename(మార్గం, &fn);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == O_DIRECTORY) /* Uh? */
                తిరిగి -EISDIR;

        స్థిరరాశి అక్షరం *e;
        ఒకవేళ (కనుగొను_environment_ద్విమాన(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                తిరిగి -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ అక్షరం *np = ఖాళీ;
        ఒకవేళ (కనుగొను_build_dir_ద్విమాన(fn, &np) >= 0)
                execv(np, argv);

        execvp(మార్గం, argv);
        తిరిగి -errno;
}

పూర్ణాంకం pin_callout_ద్విమాన(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_మార్గం) {
        పూర్ణాంకం r, fd;

        ప్రతిజ్ఞా(మార్గం);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ అక్షరం *fn = ఖాళీ;
        r = మార్గం_వెలికితీయి_filename(మార్గం, &fn);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == O_DIRECTORY) /* Uh? */
                తిరిగి -EISDIR;

        స్థిరరాశి అక్షరం *e;
        ఒకవేళ (కనుగొను_environment_ద్విమాన(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ఖాళీ, ret_మార్గం, &fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                తిరిగి fd;
        }

        _cleanup_free_ అక్షరం *np = ఖాళీ;
        ఒకవేళ (కనుగొను_build_dir_ద్విమాన(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ఖాళీ, ret_మార్గం, &fd);
                ఒకవేళ (r >= 0)
                        తిరిగి fd;
        }

        r = కనుగొను_executable_నిండిన(మార్గం, /* root= */ ఖాళీ,
                                 /* exec_search_path= */ ఖాళీ, /* use_path_envvar= */ true,
                                 ret_మార్గం, &fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి fd;
}
