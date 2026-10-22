/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <elf.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <link.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <sys/auxv.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "build-path.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "process-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "unistd.h"

സ്ഥിതം പൂർണ്ണസംഖ്യ get_runpath_from_dynamic(സ്ഥിരം ElfW(Dyn) *d, ElfW(Addr) bias, സ്ഥിരം അക്ഷരം **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        സ്ഥിരം അക്ഷരം *strtab = ശൂന്യ;

        പ്രതിജ്ഞാ(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        വേണ്ടി (; d->d_tag != DT_NULL; d++) {

                തിരഞ്ഞെടുപ്പ് (d->d_tag) {

                അവസ്ഥ DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        നിർത്തുക;

                അവസ്ഥ DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        നിർത്തുക;

                അവസ്ഥ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        എങ്കിൽ (d->d_un.d_val != 0)
                                strtab = (സ്ഥിരം അക്ഷരം *) ((uintptr_t) d->d_un.d_val
#എങ്കിൽ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        നിർത്തുക;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                എങ്കിൽ (strtab && runpath_index != SIZE_MAX)
                        നിർത്തുക;
        }

        എങ്കിൽ (!strtab)
                തിരികെ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        എങ്കിൽ (runpath_index != SIZE_MAX) {
                എങ്കിൽ (ret)
                        *ret = strtab + runpath_index;
                തിരികെ 1;
        }

        എങ്കിൽ (rpath_index != SIZE_MAX) {
                എങ്കിൽ (ret)
                        *ret = strtab + rpath_index;
                തിരികെ 1;
        }

        എങ്കിൽ (ret)
                *ret = ശൂന്യ;

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ get_runpath(സ്ഥിരം അക്ഷരം **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        എങ്കിൽ (phdr == 0)
                തിരികെ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        എങ്കിൽ (phnum == 0)
                തിരികെ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        എങ്കിൽ (phent < വലുപ്പം(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                തിരികെ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        വേണ്ടി (unsigned long i = 0; i < phnum; i++) {
                സ്ഥിരം ElfW(Phdr) *p = (സ്ഥിരം ElfW(Phdr)*) (phdr + (i * phent));

                തിരഞ്ഞെടുപ്പ് (p->p_type) {

                അവസ്ഥ PT_PHDR:
                        എങ്കിൽ (p->p_vaddr > phdr) /* safety overflow check */
                                തിരികെ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        നിർത്തുക;

                അവസ്ഥ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        നിർത്തുക;
                }

                എങ്കിൽ (found_bias && found_dyn)
                        നിർത്തുക;
        }

        എങ്കിൽ (!found_dyn)
                തിരികെ -ENOTRECOVERABLE;

        തിരികെ get_runpath_from_dynamic((സ്ഥിരം ElfW(Dyn)*) (bias + dyn), bias, ret);
}

പൂർണ്ണസംഖ്യ get_build_exec_dir(അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

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

        സ്ഥിതം പൂർണ്ണസംഖ്യ runpath_cached = -ERRNO_MAX-1;
        എങ്കിൽ (runpath_cached == -ERRNO_MAX-1) {
                സ്ഥിരം അക്ഷരം *runpath = ശൂന്യ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                എങ്കിൽ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        എങ്കിൽ (runpath_cached < 0)
                തിരികെ runpath_cached;
        എങ്കിൽ (runpath_cached == 0)
                തിരികെ -ENOEXEC;

        _cleanup_free_ അക്ഷരം *exe = ശൂന്യ;
        r = get_process_exe(0, &exe);
        എങ്കിൽ (r < 0)
                തിരികെ runpath_cached = r;

        തിരികെ path_extract_directory(exe, ret);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ find_build_dir_ദ്വിമാന(സ്ഥിരം അക്ഷരം *fn, അക്ഷരം **ret) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fn);
        പ്രതിജ്ഞാ(ret);

        _cleanup_free_ അക്ഷരം *build_dir = ശൂന്യ;
        r = get_build_exec_dir(&build_dir);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        _cleanup_free_ അക്ഷരം *np = path_join(build_dir, fn);
        എങ്കിൽ (!np)
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(np);
        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ find_environment_ദ്വിമാന(സ്ഥിരം അക്ഷരം *fn, സ്ഥിരം അക്ഷരം **ret) {

        പ്രതിജ്ഞാ(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ അക്ഷരം *s = strdup(fn);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        എങ്കിൽ (!strextend(&s, "_PATH"))
                തിരികെ -ENOMEM;

        സ്ഥിരം അക്ഷരം *e;
        e = secure_getenv(s);
        എങ്കിൽ (!e)
                തിരികെ -ENXIO;

        *ret = e;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ invoke_callout_ദ്വിമാന(സ്ഥിരം അക്ഷരം *path, അക്ഷരം *സ്ഥിരം argv[]) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ അക്ഷരം *fn = ശൂന്യ;
        r = path_extract_filename(path, &fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == O_DIRECTORY) /* Uh? */
                തിരികെ -EISDIR;

        സ്ഥിരം അക്ഷരം *e;
        എങ്കിൽ (find_environment_ദ്വിമാന(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                തിരികെ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ അക്ഷരം *np = ശൂന്യ;
        എങ്കിൽ (find_build_dir_ദ്വിമാന(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        തിരികെ -errno;
}

പൂർണ്ണസംഖ്യ pin_callout_ദ്വിമാന(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_path) {
        പൂർണ്ണസംഖ്യ r, fd;

        പ്രതിജ്ഞാ(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ അക്ഷരം *fn = ശൂന്യ;
        r = path_extract_filename(path, &fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == O_DIRECTORY) /* Uh? */
                തിരികെ -EISDIR;

        സ്ഥിരം അക്ഷരം *e;
        എങ്കിൽ (find_environment_ദ്വിമാന(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ശൂന്യ, ret_path, &fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                തിരികെ fd;
        }

        _cleanup_free_ അക്ഷരം *np = ശൂന്യ;
        എങ്കിൽ (find_build_dir_ദ്വിമാന(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ശൂന്യ, ret_path, &fd);
                എങ്കിൽ (r >= 0)
                        തിരികെ fd;
        }

        r = find_executable_full(path, /* root= */ ശൂന്യ,
                                 /* exec_search_path= */ ശൂന്യ, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ fd;
}
