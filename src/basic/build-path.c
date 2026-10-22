/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <elf.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <link.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <sys/auxv.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "build-path.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "process-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unistd.h"

स्थिर पूर्णाङ्क get_runpath_from_dynamic(ध्रुव ElfW(Dyn) *d, ElfW(Addr) bias, ध्रुव वर्ण **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ध्रुव वर्ण *strtab = रिक्त;

        प्रतिज्ञा(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        अत्थाय (; d->d_tag != DT_NULL; d++) {

                विकल्प (d->d_tag) {

                पक्ष DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        भङ्ग;

                पक्ष DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        भङ्ग;

                पक्ष DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        सचे (d->d_un.d_val != 0)
                                strtab = (ध्रुव वर्ण *) ((uintptr_t) d->d_un.d_val
#सचे defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        भङ्ग;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                सचे (strtab && runpath_index != SIZE_MAX)
                        भङ्ग;
        }

        सचे (!strtab)
                पटिनिवत्त -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        सचे (runpath_index != SIZE_MAX) {
                सचे (ret)
                        *ret = strtab + runpath_index;
                पटिनिवत्त 1;
        }

        सचे (rpath_index != SIZE_MAX) {
                सचे (ret)
                        *ret = strtab + rpath_index;
                पटिनिवत्त 1;
        }

        सचे (ret)
                *ret = रिक्त;

        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क get_runpath(ध्रुव वर्ण **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        सचे (phdr == 0)
                पटिनिवत्त -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        सचे (phnum == 0)
                पटिनिवत्त -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        सचे (phent < परिमाण(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                पटिनिवत्त -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        अत्थाय (unsigned long i = 0; i < phnum; i++) {
                ध्रुव ElfW(Phdr) *p = (ध्रुव ElfW(Phdr)*) (phdr + (i * phent));

                विकल्प (p->p_type) {

                पक्ष PT_PHDR:
                        सचे (p->p_vaddr > phdr) /* safety overflow check */
                                पटिनिवत्त -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        भङ्ग;

                पक्ष PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        भङ्ग;
                }

                सचे (found_bias && found_dyn)
                        भङ्ग;
        }

        सचे (!found_dyn)
                पटिनिवत्त -ENOTRECOVERABLE;

        पटिनिवत्त get_runpath_from_dynamic((ध्रुव ElfW(Dyn)*) (bias + dyn), bias, ret);
}

पूर्णाङ्क get_build_exec_dir(वर्ण **ret) {
        पूर्णाङ्क r;

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

        स्थिर पूर्णाङ्क runpath_cached = -ERRNO_MAX-1;
        सचे (runpath_cached == -ERRNO_MAX-1) {
                ध्रुव वर्ण *runpath = रिक्त;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                सचे (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        सचे (runpath_cached < 0)
                पटिनिवत्त runpath_cached;
        सचे (runpath_cached == 0)
                पटिनिवत्त -ENOEXEC;

        _cleanup_free_ वर्ण *exe = रिक्त;
        r = get_process_exe(0, &exe);
        सचे (r < 0)
                पटिनिवत्त runpath_cached = r;

        पटिनिवत्त path_extract_directory(exe, ret);
}

स्थिर पूर्णाङ्क find_build_dir_द्विमान(ध्रुव वर्ण *fn, वर्ण **ret) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fn);
        प्रतिज्ञा(ret);

        _cleanup_free_ वर्ण *build_dir = रिक्त;
        r = get_build_exec_dir(&build_dir);
        सचे (r < 0)
                पटिनिवत्त r;

        _cleanup_free_ वर्ण *np = path_join(build_dir, fn);
        सचे (!np)
                पटिनिवत्त -ENOMEM;

        *ret = TAKE_PTR(np);
        पटिनिवत्त 0;
}

स्थिर पूर्णाङ्क find_environment_द्विमान(ध्रुव वर्ण *fn, ध्रुव वर्ण **ret) {

        प्रतिज्ञा(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ वर्ण *s = strdup(fn);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        सचे (!strextend(&s, "_PATH"))
                पटिनिवत्त -ENOMEM;

        ध्रुव वर्ण *e;
        e = secure_getenv(s);
        सचे (!e)
                पटिनिवत्त -ENXIO;

        *ret = e;
        पटिनिवत्त 0;
}

पूर्णाङ्क invoke_callout_द्विमान(ध्रुव वर्ण *path, वर्ण *ध्रुव argv[]) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ वर्ण *fn = रिक्त;
        r = path_extract_filename(path, &fn);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == O_DIRECTORY) /* Uh? */
                पटिनिवत्त -EISDIR;

        ध्रुव वर्ण *e;
        सचे (find_environment_द्विमान(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                पटिनिवत्त -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ वर्ण *np = रिक्त;
        सचे (find_build_dir_द्विमान(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        पटिनिवत्त -errno;
}

पूर्णाङ्क pin_callout_द्विमान(ध्रुव वर्ण *path, वर्ण **ret_path) {
        पूर्णाङ्क r, fd;

        प्रतिज्ञा(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ वर्ण *fn = रिक्त;
        r = path_extract_filename(path, &fn);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r == O_DIRECTORY) /* Uh? */
                पटिनिवत्त -EISDIR;

        ध्रुव वर्ण *e;
        सचे (find_environment_द्विमान(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ रिक्त, ret_path, &fd);
                सचे (r < 0)
                        पटिनिवत्त r;

                पटिनिवत्त fd;
        }

        _cleanup_free_ वर्ण *np = रिक्त;
        सचे (find_build_dir_द्विमान(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ रिक्त, ret_path, &fd);
                सचे (r >= 0)
                        पटिनिवत्त fd;
        }

        r = find_executable_full(path, /* root= */ रिक्त,
                                 /* exec_search_path= */ रिक्त, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त fd;
}
