/* SPDX-License-Identifier: LGPL-2.1-or-later */

#समावेश <elf.h>
#समावेश <fcntl.h>
#समावेश <link.h>
#समावेश <stdlib.h>
#समावेश <sys/auxv.h>

#समावेश "alloc-util.h"
#समावेश "build-path.h"
#समावेश "path-util.h"
#समावेश "process-util.h"
#समावेश "string-util.h"
#समावेश "unistd.h"

अटल पूर्णांक get_runpath_from_dynamic(स्थिर ElfW(Dyn) *d, ElfW(Addr) bias, स्थिर अक्षर **ret) {
        माप_प्रकार runpath_सूचकांक = SIZE_MAX, rpath_सूचकांक = SIZE_MAX;
        स्थिर अक्षर *strtab = शून्य;

        निश्चित(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        क्रम (; d->d_tag != DT_NULL; d++) {

                चयन (d->d_tag) {

                हाल DT_RUNPATH:
                        runpath_सूचकांक = (माप_प्रकार) d->d_un.d_val;
                        अवरोध;

                हाल DT_RPATH:
                        rpath_सूचकांक = (माप_प्रकार) d->d_un.d_val;
                        अवरोध;

                हाल DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        अगर (d->d_un.d_val != 0)
                                strtab = (स्थिर अक्षर *) ((uintptr_t) d->d_un.d_val
#अगर defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#पूर्ण_अगर
                                );
                        अवरोध;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                अगर (strtab && runpath_सूचकांक != SIZE_MAX)
                        अवरोध;
        }

        अगर (!strtab)
                वापस -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        अगर (runpath_सूचकांक != SIZE_MAX) {
                अगर (ret)
                        *ret = strtab + runpath_सूचकांक;
                वापस 1;
        }

        अगर (rpath_सूचकांक != SIZE_MAX) {
                अगर (ret)
                        *ret = strtab + rpath_सूचकांक;
                वापस 1;
        }

        अगर (ret)
                *ret = शून्य;

        वापस 0;
}

अटल पूर्णांक get_runpath(स्थिर अक्षर **ret) {
        अचिन्हित दीर्घ phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        अगर (phdr == 0)
                वापस -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        अगर (phnum == 0)
                वापस -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        अगर (phent < माप(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                वापस -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        क्रम (अचिन्हित दीर्घ i = 0; i < phnum; i++) {
                स्थिर ElfW(Phdr) *p = (स्थिर ElfW(Phdr)*) (phdr + (i * phent));

                चयन (p->p_type) {

                हाल PT_PHDR:
                        अगर (p->p_vaddr > phdr) /* safety overflow check */
                                वापस -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        अवरोध;

                हाल PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        अवरोध;
                }

                अगर (found_bias && found_dyn)
                        अवरोध;
        }

        अगर (!found_dyn)
                वापस -ENOTRECOVERABLE;

        वापस get_runpath_from_dynamic((स्थिर ElfW(Dyn)*) (bias + dyn), bias, ret);
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

        अटल पूर्णांक runpath_cached = -ERRNO_MAX-1;
        अगर (runpath_cached == -ERRNO_MAX-1) {
                स्थिर अक्षर *runpath = शून्य;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                अगर (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        अगर (runpath_cached < 0)
                वापस runpath_cached;
        अगर (runpath_cached == 0)
                वापस -ENOEXEC;

        _cleanup_free_ अक्षर *exe = शून्य;
        r = get_process_exe(0, &exe);
        अगर (r < 0)
                वापस runpath_cached = r;

        वापस path_extract_directory(exe, ret);
}

अटल पूर्णांक find_build_dir_द्विआधारी(स्थिर अक्षर *fn, अक्षर **ret) {
        पूर्णांक r;

        निश्चित(fn);
        निश्चित(ret);

        _cleanup_free_ अक्षर *build_dir = शून्य;
        r = get_build_exec_dir(&build_dir);
        अगर (r < 0)
                वापस r;

        _cleanup_free_ अक्षर *np = path_join(build_dir, fn);
        अगर (!np)
                वापस -ENOMEM;

        *ret = TAKE_PTR(np);
        वापस 0;
}

अटल पूर्णांक find_environment_द्विआधारी(स्थिर अक्षर *fn, स्थिर अक्षर **ret) {

        निश्चित(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ अक्षर *s = strdup(fn);
        अगर (!s)
                वापस -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        अगर (!strextend(&s, "_PATH"))
                वापस -ENOMEM;

        स्थिर अक्षर *e;
        e = secure_getenv(s);
        अगर (!e)
                वापस -ENXIO;

        *ret = e;
        वापस 0;
}

पूर्णांक invoke_callout_द्विआधारी(स्थिर अक्षर *path, अक्षर *स्थिर argv[]) {
        पूर्णांक r;

        निश्चित(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ अक्षर *fn = शून्य;
        r = path_extract_filename(path, &fn);
        अगर (r < 0)
                वापस r;
        अगर (r == O_DIRECTORY) /* Uh? */
                वापस -EISDIR;

        स्थिर अक्षर *e;
        अगर (find_environment_द्विआधारी(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                वापस -त्रुटि_सं; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ अक्षर *np = शून्य;
        अगर (find_build_dir_द्विआधारी(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        वापस -त्रुटि_सं;
}

पूर्णांक pin_callout_द्विआधारी(स्थिर अक्षर *path, अक्षर **ret_path) {
        पूर्णांक r, fd;

        निश्चित(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ अक्षर *fn = शून्य;
        r = path_extract_filename(path, &fn);
        अगर (r < 0)
                वापस r;
        अगर (r == O_DIRECTORY) /* Uh? */
                वापस -EISDIR;

        स्थिर अक्षर *e;
        अगर (find_environment_द्विआधारी(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ शून्य, ret_path, &fd);
                अगर (r < 0)
                        वापस r;

                वापस fd;
        }

        _cleanup_free_ अक्षर *np = शून्य;
        अगर (find_build_dir_द्विआधारी(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ शून्य, ret_path, &fd);
                अगर (r >= 0)
                        वापस fd;
        }

        r = find_executable_full(path, /* root= */ शून्य,
                                 /* exec_search_path= */ शून्य, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        अगर (r < 0)
                वापस r;

        वापस fd;
}
