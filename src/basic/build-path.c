/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <elf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <link.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/auxv.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "build-path.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "process-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "unistd.h"

ਸਥਿਰ ਪੂਰਨ get_runpath_from_dynamic(ਅਚਲ ElfW(Dyn) *d, ElfW(Addr) bias, ਅਚਲ ਅੱਖਰ **ret) {
        size_t runpath_ਸੂਚਕਾਂਕ = SIZE_MAX, rpath_ਸੂਚਕਾਂਕ = SIZE_MAX;
        ਅਚਲ ਅੱਖਰ *strtab = ਨਿਰਾ;

        ਪ੍ਰਤਿਜ੍ਞਾ(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        ਲਈ (; d->d_tag != DT_NULL; d++) {

                ਚੋਣ (d->d_tag) {

                ਹਾਲਤ DT_RUNPATH:
                        runpath_ਸੂਚਕਾਂਕ = (size_t) d->d_un.d_val;
                        ਰੋਕੋ;

                ਹਾਲਤ DT_RPATH:
                        rpath_ਸੂਚਕਾਂਕ = (size_t) d->d_un.d_val;
                        ਰੋਕੋ;

                ਹਾਲਤ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ਜੇ (d->d_un.d_val != 0)
                                strtab = (ਅਚਲ ਅੱਖਰ *) ((uintptr_t) d->d_un.d_val
#ਜੇ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#ਜੇ_ਅੰਤ
                                );
                        ਰੋਕੋ;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ਜੇ (strtab && runpath_ਸੂਚਕਾਂਕ != SIZE_MAX)
                        ਰੋਕੋ;
        }

        ਜੇ (!strtab)
                ਵਾਪਸ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ਜੇ (runpath_ਸੂਚਕਾਂਕ != SIZE_MAX) {
                ਜੇ (ret)
                        *ret = strtab + runpath_ਸੂਚਕਾਂਕ;
                ਵਾਪਸ 1;
        }

        ਜੇ (rpath_ਸੂਚਕਾਂਕ != SIZE_MAX) {
                ਜੇ (ret)
                        *ret = strtab + rpath_ਸੂਚਕਾਂਕ;
                ਵਾਪਸ 1;
        }

        ਜੇ (ret)
                *ret = ਨਿਰਾ;

        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ get_runpath(ਅਚਲ ਅੱਖਰ **ret) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ਜੇ (phdr == 0)
                ਵਾਪਸ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ਜੇ (phnum == 0)
                ਵਾਪਸ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ਜੇ (phent < ਆਕਾਰ(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ਵਾਪਸ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool ਮਿਲਿਆ_bias = false, ਮਿਲਿਆ_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ i = 0; i < phnum; i++) {
                ਅਚਲ ElfW(Phdr) *p = (ਅਚਲ ElfW(Phdr)*) (phdr + (i * phent));

                ਚੋਣ (p->p_type) {

                ਹਾਲਤ PT_PHDR:
                        ਜੇ (p->p_vaddr > phdr) /* safety overflow check */
                                ਵਾਪਸ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        ਮਿਲਿਆ_bias = true;
                        ਰੋਕੋ;

                ਹਾਲਤ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        ਮਿਲਿਆ_dyn = true;
                        ਰੋਕੋ;
                }

                ਜੇ (ਮਿਲਿਆ_bias && ਮਿਲਿਆ_dyn)
                        ਰੋਕੋ;
        }

        ਜੇ (!ਮਿਲਿਆ_dyn)
                ਵਾਪਸ -ENOTRECOVERABLE;

        ਵਾਪਸ get_runpath_from_dynamic((ਅਚਲ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

ਪੂਰਨ get_build_exec_dir(ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

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

        ਸਥਿਰ ਪੂਰਨ runpath_cached = -ERRNO_MAX-1;
        ਜੇ (runpath_cached == -ERRNO_MAX-1) {
                ਅਚਲ ਅੱਖਰ *runpath = ਨਿਰਾ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ਜੇ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ਜੇ (runpath_cached < 0)
                ਵਾਪਸ runpath_cached;
        ਜੇ (runpath_cached == 0)
                ਵਾਪਸ -ENOEXEC;

        _cleanup_free_ ਅੱਖਰ *exe = ਨਿਰਾ;
        r = get_process_exe(0, &exe);
        ਜੇ (r < 0)
                ਵਾਪਸ runpath_cached = r;

        ਵਾਪਸ ਰਸਤਾ_ਕੱਢੋ_directory(exe, ret);
}

ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_build_dir_ਦੁਆਧਾਰੀ(ਅਚਲ ਅੱਖਰ *fn, ਅੱਖਰ **ret) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fn);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        _cleanup_free_ ਅੱਖਰ *build_dir = ਨਿਰਾ;
        r = get_build_exec_dir(&build_dir);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        _cleanup_free_ ਅੱਖਰ *np = ਰਸਤਾ_join(build_dir, fn);
        ਜੇ (!np)
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(np);
        ਵਾਪਸ 0;
}

ਸਥਿਰ ਪੂਰਨ ਲੱਭੋ_environment_ਦੁਆਧਾਰੀ(ਅਚਲ ਅੱਖਰ *fn, ਅਚਲ ਅੱਖਰ **ret) {

        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ ਅੱਖਰ *s = strdup(fn);
        ਜੇ (!s)
                ਵਾਪਸ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ਜੇ (!strextend(&s, "_PATH"))
                ਵਾਪਸ -ENOMEM;

        ਅਚਲ ਅੱਖਰ *e;
        e = secure_getenv(s);
        ਜੇ (!e)
                ਵਾਪਸ -ENXIO;

        *ret = e;
        ਵਾਪਸ 0;
}

ਪੂਰਨ invoke_callout_ਦੁਆਧਾਰੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ *ਅਚਲ argv[]) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ;
        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == O_DIRECTORY) /* Uh? */
                ਵਾਪਸ -EISDIR;

        ਅਚਲ ਅੱਖਰ *e;
        ਜੇ (ਲੱਭੋ_environment_ਦੁਆਧਾਰੀ(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ਵਾਪਸ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ ਅੱਖਰ *np = ਨਿਰਾ;
        ਜੇ (ਲੱਭੋ_build_dir_ਦੁਆਧਾਰੀ(fn, &np) >= 0)
                execv(np, argv);

        execvp(ਰਸਤਾ, argv);
        ਵਾਪਸ -errno;
}

ਪੂਰਨ pin_callout_ਦੁਆਧਾਰੀ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_ਰਸਤਾ) {
        ਪੂਰਨ r, fd;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ ਅੱਖਰ *fn = ਨਿਰਾ;
        r = ਰਸਤਾ_ਕੱਢੋ_filename(ਰਸਤਾ, &fn);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == O_DIRECTORY) /* Uh? */
                ਵਾਪਸ -EISDIR;

        ਅਚਲ ਅੱਖਰ *e;
        ਜੇ (ਲੱਭੋ_environment_ਦੁਆਧਾਰੀ(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ਨਿਰਾ, ret_ਰਸਤਾ, &fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਵਾਪਸ fd;
        }

        _cleanup_free_ ਅੱਖਰ *np = ਨਿਰਾ;
        ਜੇ (ਲੱਭੋ_build_dir_ਦੁਆਧਾਰੀ(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ਨਿਰਾ, ret_ਰਸਤਾ, &fd);
                ਜੇ (r >= 0)
                        ਵਾਪਸ fd;
        }

        r = ਲੱਭੋ_executable_ਭਰਿਆ(ਰਸਤਾ, /* root= */ ਨਿਰਾ,
                                 /* exec_search_path= */ ਨਿਰਾ, /* use_path_envvar= */ true,
                                 ret_ਰਸਤਾ, &fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ fd;
}
