/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <elf.h>
#شامل <fcntl.h>
#شامل <link.h>
#شامل <stdlib.h>
#شامل <sys/auxv.h>

#شامل "alloc-util.h"
#شامل "build-path.h"
#شامل "path-util.h"
#شامل "process-util.h"
#شامل "string-util.h"
#شامل "unistd.h"

ایستا صحیح get_runpath_from_dynamic(ثابت ElfW(Dyn) *d, ElfW(Addr) bias, ثابت نویسه **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ثابت نویسه *strtab = پوچ;

        أكد(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        برای (; d->d_tag != DT_NULL; d++) {

                انتخاب (d->d_tag) {

                حالت DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        بشکن;

                حالت DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        بشکن;

                حالت DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        اگر (d->d_un.d_val != 0)
                                strtab = (ثابت نویسه *) ((uintptr_t) d->d_un.d_val
#اگر defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        بشکن;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                اگر (strtab && runpath_index != SIZE_MAX)
                        بشکن;
        }

        اگر (!strtab)
                بازگشت -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        اگر (runpath_index != SIZE_MAX) {
                اگر (ret)
                        *ret = strtab + runpath_index;
                بازگشت 1;
        }

        اگر (rpath_index != SIZE_MAX) {
                اگر (ret)
                        *ret = strtab + rpath_index;
                بازگشت 1;
        }

        اگر (ret)
                *ret = پوچ;

        بازگشت 0;
}

ایستا صحیح get_runpath(ثابت نویسه **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        اگر (phdr == 0)
                بازگشت -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        اگر (phnum == 0)
                بازگشت -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        اگر (phent < اندازه(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                بازگشت -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        برای (unsigned long i = 0; i < phnum; i++) {
                ثابت ElfW(Phdr) *p = (ثابت ElfW(Phdr)*) (phdr + (i * phent));

                انتخاب (p->p_type) {

                حالت PT_PHDR:
                        اگر (p->p_vaddr > phdr) /* safety overflow check */
                                بازگشت -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        بشکن;

                حالت PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        بشکن;
                }

                اگر (found_bias && found_dyn)
                        بشکن;
        }

        اگر (!found_dyn)
                بازگشت -ENOTRECOVERABLE;

        بازگشت get_runpath_from_dynamic((ثابت ElfW(Dyn)*) (bias + dyn), bias, ret);
}

صحیح get_build_exec_dir(نویسه **ret) {
        صحیح r;

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

        ایستا صحیح runpath_cached = -ERRNO_MAX-1;
        اگر (runpath_cached == -ERRNO_MAX-1) {
                ثابت نویسه *runpath = پوچ;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                اگر (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        اگر (runpath_cached < 0)
                بازگشت runpath_cached;
        اگر (runpath_cached == 0)
                بازگشت -ENOEXEC;

        _cleanup_free_ نویسه *exe = پوچ;
        r = get_process_exe(0, &exe);
        اگر (r < 0)
                بازگشت runpath_cached = r;

        بازگشت path_extract_directory(exe, ret);
}

ایستا صحیح find_build_dir_دودویی(ثابت نویسه *fn, نویسه **ret) {
        صحیح r;

        أكد(fn);
        أكد(ret);

        _cleanup_free_ نویسه *build_dir = پوچ;
        r = get_build_exec_dir(&build_dir);
        اگر (r < 0)
                بازگشت r;

        _cleanup_free_ نویسه *np = path_join(build_dir, fn);
        اگر (!np)
                بازگشت -ENOMEM;

        *ret = TAKE_PTR(np);
        بازگشت 0;
}

ایستا صحیح find_environment_دودویی(ثابت نویسه *fn, ثابت نویسه **ret) {

        أكد(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ نویسه *s = strdup(fn);
        اگر (!s)
                بازگشت -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        اگر (!strextend(&s, "_PATH"))
                بازگشت -ENOMEM;

        ثابت نویسه *e;
        e = secure_getenv(s);
        اگر (!e)
                بازگشت -ENXIO;

        *ret = e;
        بازگشت 0;
}

صحیح invoke_callout_دودویی(ثابت نویسه *path, نویسه *ثابت argv[]) {
        صحیح r;

        أكد(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ نویسه *fn = پوچ;
        r = path_extract_filename(path, &fn);
        اگر (r < 0)
                بازگشت r;
        اگر (r == O_DIRECTORY) /* Uh? */
                بازگشت -EISDIR;

        ثابت نویسه *e;
        اگر (find_environment_دودویی(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                بازگشت -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ نویسه *np = پوچ;
        اگر (find_build_dir_دودویی(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        بازگشت -errno;
}

صحیح pin_callout_دودویی(ثابت نویسه *path, نویسه **ret_path) {
        صحیح r, fd;

        أكد(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ نویسه *fn = پوچ;
        r = path_extract_filename(path, &fn);
        اگر (r < 0)
                بازگشت r;
        اگر (r == O_DIRECTORY) /* Uh? */
                بازگشت -EISDIR;

        ثابت نویسه *e;
        اگر (find_environment_دودویی(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ پوچ, ret_path, &fd);
                اگر (r < 0)
                        بازگشت r;

                بازگشت fd;
        }

        _cleanup_free_ نویسه *np = پوچ;
        اگر (find_build_dir_دودویی(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ پوچ, ret_path, &fd);
                اگر (r >= 0)
                        بازگشت fd;
        }

        r = find_executable_full(path, /* root= */ پوچ,
                                 /* exec_search_path= */ پوچ, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت fd;
}
