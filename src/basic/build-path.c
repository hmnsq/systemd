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

स्थिर पूर्णांक get_runpath_from_dynamic(स्थिरांक ElfW(Dyn) *d, ElfW(Addr) bias, स्थिरांक अक्षर **ret) {
        माप_प्रकार runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        स्थिरांक अक्षर *strtab = रिक्त;

        प्रतिज्ञा(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        साठी (; d->d_tag != DT_NULL; d++) {

                निवड (d->d_tag) {

                स्थिती DT_RUNPATH:
                        runpath_index = (माप_प्रकार) d->d_un.d_val;
                        थांबा;

                स्थिती DT_RPATH:
                        rpath_index = (माप_प्रकार) d->d_un.d_val;
                        थांबा;

                स्थिती DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        जर (d->d_un.d_val != 0)
                                strtab = (स्थिरांक अक्षर *) ((uintptr_t) d->d_un.d_val
#जर defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        थांबा;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                जर (strtab && runpath_index != SIZE_MAX)
                        थांबा;
        }

        जर (!strtab)
                परत -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        जर (runpath_index != SIZE_MAX) {
                जर (ret)
                        *ret = strtab + runpath_index;
                परत 1;
        }

        जर (rpath_index != SIZE_MAX) {
                जर (ret)
                        *ret = strtab + rpath_index;
                परत 1;
        }

        जर (ret)
                *ret = रिक्त;

        परत 0;
}

स्थिर पूर्णांक get_runpath(स्थिरांक अक्षर **ret) {
        अचिन्हित दीर्घ phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        जर (phdr == 0)
                परत -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        जर (phnum == 0)
                परत -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        जर (phent < आकार(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                परत -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        साठी (अचिन्हित दीर्घ i = 0; i < phnum; i++) {
                स्थिरांक ElfW(Phdr) *p = (स्थिरांक ElfW(Phdr)*) (phdr + (i * phent));

                निवड (p->p_type) {

                स्थिती PT_PHDR:
                        जर (p->p_vaddr > phdr) /* safety overflow check */
                                परत -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        थांबा;

                स्थिती PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        थांबा;
                }

                जर (found_bias && found_dyn)
                        थांबा;
        }

        जर (!found_dyn)
                परत -ENOTRECOVERABLE;

        परत get_runpath_from_dynamic((स्थिरांक ElfW(Dyn)*) (bias + dyn), bias, ret);
}

पूर्णांक get_build_exec_dir(अक्षर **ret) {
        पूर्णांक r;

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

        स्थिर पूर्णांक runpath_cached = -ERRNO_MAX-1;
        जर (runpath_cached == -ERRNO_MAX-1) {
                स्थिरांक अक्षर *runpath = रिक्त;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                जर (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        जर (runpath_cached < 0)
                परत runpath_cached;
        जर (runpath_cached == 0)
                परत -ENOEXEC;

        _cleanup_free_ अक्षर *exe = रिक्त;
        r = get_process_exe(0, &exe);
        जर (r < 0)
                परत runpath_cached = r;

        परत path_extract_directory(exe, ret);
}

स्थिर पूर्णांक find_build_dir_द्विमान(स्थिरांक अक्षर *fn, अक्षर **ret) {
        पूर्णांक r;

        प्रतिज्ञा(fn);
        प्रतिज्ञा(ret);

        _cleanup_free_ अक्षर *build_dir = रिक्त;
        r = get_build_exec_dir(&build_dir);
        जर (r < 0)
                परत r;

        _cleanup_free_ अक्षर *np = path_join(build_dir, fn);
        जर (!np)
                परत -ENOMEM;

        *ret = TAKE_PTR(np);
        परत 0;
}

स्थिर पूर्णांक find_environment_द्विमान(स्थिरांक अक्षर *fn, स्थिरांक अक्षर **ret) {

        प्रतिज्ञा(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ अक्षर *s = strdup(fn);
        जर (!s)
                परत -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        जर (!strextend(&s, "_PATH"))
                परत -ENOMEM;

        स्थिरांक अक्षर *e;
        e = secure_getenv(s);
        जर (!e)
                परत -ENXIO;

        *ret = e;
        परत 0;
}

पूर्णांक invoke_callout_द्विमान(स्थिरांक अक्षर *path, अक्षर *स्थिरांक argv[]) {
        पूर्णांक r;

        प्रतिज्ञा(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ अक्षर *fn = रिक्त;
        r = path_extract_filename(path, &fn);
        जर (r < 0)
                परत r;
        जर (r == O_DIRECTORY) /* Uh? */
                परत -EISDIR;

        स्थिरांक अक्षर *e;
        जर (find_environment_द्विमान(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                परत -त्रुटि_सं; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ अक्षर *np = रिक्त;
        जर (find_build_dir_द्विमान(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        परत -त्रुटि_सं;
}

पूर्णांक pin_callout_द्विमान(स्थिरांक अक्षर *path, अक्षर **ret_path) {
        पूर्णांक r, fd;

        प्रतिज्ञा(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ अक्षर *fn = रिक्त;
        r = path_extract_filename(path, &fn);
        जर (r < 0)
                परत r;
        जर (r == O_DIRECTORY) /* Uh? */
                परत -EISDIR;

        स्थिरांक अक्षर *e;
        जर (find_environment_द्विमान(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ रिक्त, ret_path, &fd);
                जर (r < 0)
                        परत r;

                परत fd;
        }

        _cleanup_free_ अक्षर *np = रिक्त;
        जर (find_build_dir_द्विमान(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ रिक्त, ret_path, &fd);
                जर (r >= 0)
                        परत fd;
        }

        r = find_executable_full(path, /* root= */ रिक्त,
                                 /* exec_search_path= */ रिक्त, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        जर (r < 0)
                परत r;

        परत fd;
}
