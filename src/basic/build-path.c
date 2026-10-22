/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <elf.h>
#ضمن <fcntl.h>
#ضمن <link.h>
#ضمن <stdlib.h>
#ضمن <sys/auxv.h>

#ضمن "alloc-util.h"
#ضمن "build-path.h"
#ضمن "path-util.h"
#ضمن "process-util.h"
#ضمن "string-util.h"
#ضمن "unistd.h"

ساكن صحيح get_runpath_from_dynamic(ثابت ElfW(Dyn) *d, ElfW(Addr) bias, ثابت حرف **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ثابت حرف *strtab = عدم;

        أكد(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        עבור (; d->d_tag != DT_NULL; d++) {

                بدل (d->d_tag) {

                حالة DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        اكسر;

                حالة DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        اكسر;

                حالة DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        אם (d->d_un.d_val != 0)
                                strtab = (ثابت حرف *) ((uintptr_t) d->d_un.d_val
#אם defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        اكسر;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                אם (strtab && runpath_index != SIZE_MAX)
                        اكسر;
        }

        אם (!strtab)
                החזר -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        אם (runpath_index != SIZE_MAX) {
                אם (ret)
                        *ret = strtab + runpath_index;
                החזר 1;
        }

        אם (rpath_index != SIZE_MAX) {
                אם (ret)
                        *ret = strtab + rpath_index;
                החזר 1;
        }

        אם (ret)
                *ret = عدم;

        החזר 0;
}

ساكن صحيح get_runpath(ثابت حرف **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        אם (phdr == 0)
                החזר -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        אם (phnum == 0)
                החזר -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        אם (phent < حجم(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                החזר -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        עבור (unsigned long i = 0; i < phnum; i++) {
                ثابت ElfW(Phdr) *p = (ثابت ElfW(Phdr)*) (phdr + (i * phent));

                بدل (p->p_type) {

                حالة PT_PHDR:
                        אם (p->p_vaddr > phdr) /* safety overflow check */
                                החזר -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        اكسر;

                حالة PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        اكسر;
                }

                אם (found_bias && found_dyn)
                        اكسر;
        }

        אם (!found_dyn)
                החזר -ENOTRECOVERABLE;

        החזר get_runpath_from_dynamic((ثابت ElfW(Dyn)*) (bias + dyn), bias, ret);
}

صحيح get_build_exec_dir(حرف **ret) {
        صحيح r;

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

        ساكن صحيح runpath_cached = -ERRNO_MAX-1;
        אם (runpath_cached == -ERRNO_MAX-1) {
                ثابت حرف *runpath = عدم;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                אם (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        אם (runpath_cached < 0)
                החזר runpath_cached;
        אם (runpath_cached == 0)
                החזר -ENOEXEC;

        _cleanup_free_ حرف *exe = عدم;
        r = get_process_exe(0, &exe);
        אם (r < 0)
                החזר runpath_cached = r;

        החזר path_extract_directory(exe, ret);
}

ساكن صحيح find_build_dir_binary(ثابت حرف *fn, حرف **ret) {
        صحيح r;

        أكد(fn);
        أكد(ret);

        _cleanup_free_ حرف *build_dir = عدم;
        r = get_build_exec_dir(&build_dir);
        אם (r < 0)
                החזר r;

        _cleanup_free_ حرف *np = path_join(build_dir, fn);
        אם (!np)
                החזר -ENOMEM;

        *ret = TAKE_PTR(np);
        החזר 0;
}

ساكن صحيح find_environment_binary(ثابت حرف *fn, ثابت حرف **ret) {

        أكد(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ حرف *s = strdup(fn);
        אם (!s)
                החזר -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        אם (!strextend(&s, "_PATH"))
                החזר -ENOMEM;

        ثابت حرف *e;
        e = secure_getenv(s);
        אם (!e)
                החזר -ENXIO;

        *ret = e;
        החזר 0;
}

صحيح invoke_callout_binary(ثابت حرف *path, حرف *ثابت argv[]) {
        صحيح r;

        أكد(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ حرف *fn = عدم;
        r = path_extract_filename(path, &fn);
        אם (r < 0)
                החזר r;
        אם (r == O_DIRECTORY) /* Uh? */
                החזר -EISDIR;

        ثابت حرف *e;
        אם (find_environment_binary(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                החזר -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ حرف *np = عدم;
        אם (find_build_dir_binary(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        החזר -errno;
}

صحيح pin_callout_binary(ثابت حرف *path, حرف **ret_path) {
        صحيح r, fd;

        أكد(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ حرف *fn = عدم;
        r = path_extract_filename(path, &fn);
        אם (r < 0)
                החזר r;
        אם (r == O_DIRECTORY) /* Uh? */
                החזר -EISDIR;

        ثابت حرف *e;
        אם (find_environment_binary(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ عدم, ret_path, &fd);
                אם (r < 0)
                        החזר r;

                החזר fd;
        }

        _cleanup_free_ حرف *np = عدم;
        אם (find_build_dir_binary(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ عدم, ret_path, &fd);
                אם (r >= 0)
                        החזר fd;
        }

        r = find_executable_full(path, /* root= */ عدم,
                                 /* exec_search_path= */ عدم, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        אם (r < 0)
                החזר r;

        החזר fd;
}
