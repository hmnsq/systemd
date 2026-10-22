/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <elf.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <link.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <sys/auxv.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "build-path.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "process-util.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "unistd.h"

স্থির পূর্ণসংখ্যা get_runpath_from_dynamic(ধ্রুবক ElfW(Dyn) *d, ElfW(Addr) bias, ধ্রুবক অক্ষর **ret) {
        size_t runpath_সূচক = SIZE_MAX, rpath_সূচক = SIZE_MAX;
        ধ্রুবক অক্ষর *strtab = ফাঁকা;

        প্রতিজ্ঞা(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        জন্য (; d->d_tag != DT_NULL; d++) {

                নির্বাচন (d->d_tag) {

                ক্ষেত্র DT_RUNPATH:
                        runpath_সূচক = (size_t) d->d_un.d_val;
                        বিরতি;

                ক্ষেত্র DT_RPATH:
                        rpath_সূচক = (size_t) d->d_un.d_val;
                        বিরতি;

                ক্ষেত্র DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        যদি (d->d_un.d_val != 0)
                                strtab = (ধ্রুবক অক্ষর *) ((uintptr_t) d->d_un.d_val
#যদি defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        বিরতি;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                যদি (strtab && runpath_সূচক != SIZE_MAX)
                        বিরতি;
        }

        যদি (!strtab)
                ফেরত -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        যদি (runpath_সূচক != SIZE_MAX) {
                যদি (ret)
                        *ret = strtab + runpath_সূচক;
                ফেরত 1;
        }

        যদি (rpath_সূচক != SIZE_MAX) {
                যদি (ret)
                        *ret = strtab + rpath_সূচক;
                ফেরত 1;
        }

        যদি (ret)
                *ret = ফাঁকা;

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা get_runpath(ধ্রুবক অক্ষর **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        যদি (phdr == 0)
                ফেরত -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        যদি (phnum == 0)
                ফেরত -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        যদি (phent < আকার(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ফেরত -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        জন্য (unsigned long i = 0; i < phnum; i++) {
                ধ্রুবক ElfW(Phdr) *p = (ধ্রুবক ElfW(Phdr)*) (phdr + (i * phent));

                নির্বাচন (p->p_type) {

                ক্ষেত্র PT_PHDR:
                        যদি (p->p_vaddr > phdr) /* safety overflow check */
                                ফেরত -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        বিরতি;

                ক্ষেত্র PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        বিরতি;
                }

                যদি (found_bias && found_dyn)
                        বিরতি;
        }

        যদি (!found_dyn)
                ফেরত -ENOTRECOVERABLE;

        ফেরত get_runpath_from_dynamic((ধ্রুবক ElfW(Dyn)*) (bias + dyn), bias, ret);
}

পূর্ণসংখ্যা get_build_exec_dir(অক্ষর **ret) {
        পূর্ণসংখ্যা r;

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

        স্থির পূর্ণসংখ্যা runpath_cached = -ERRNO_MAX-1;
        যদি (runpath_cached == -ERRNO_MAX-1) {
                ধ্রুবক অক্ষর *runpath = ফাঁকা;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                যদি (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        যদি (runpath_cached < 0)
                ফেরত runpath_cached;
        যদি (runpath_cached == 0)
                ফেরত -ENOEXEC;

        _cleanup_free_ অক্ষর *exe = ফাঁকা;
        r = get_process_exe(0, &exe);
        যদি (r < 0)
                ফেরত runpath_cached = r;

        ফেরত path_extract_directory(exe, ret);
}

স্থির পূর্ণসংখ্যা find_build_dir_দ্বিমিক(ধ্রুবক অক্ষর *fn, অক্ষর **ret) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fn);
        প্রতিজ্ঞা(ret);

        _cleanup_free_ অক্ষর *build_dir = ফাঁকা;
        r = get_build_exec_dir(&build_dir);
        যদি (r < 0)
                ফেরত r;

        _cleanup_free_ অক্ষর *np = path_join(build_dir, fn);
        যদি (!np)
                ফেরত -ENOMEM;

        *ret = TAKE_PTR(np);
        ফেরত 0;
}

স্থির পূর্ণসংখ্যা find_environment_দ্বিমিক(ধ্রুবক অক্ষর *fn, ধ্রুবক অক্ষর **ret) {

        প্রতিজ্ঞা(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ অক্ষর *s = strdup(fn);
        যদি (!s)
                ফেরত -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        যদি (!strextend(&s, "_PATH"))
                ফেরত -ENOMEM;

        ধ্রুবক অক্ষর *e;
        e = secure_getenv(s);
        যদি (!e)
                ফেরত -ENXIO;

        *ret = e;
        ফেরত 0;
}

পূর্ণসংখ্যা invoke_callout_দ্বিমিক(ধ্রুবক অক্ষর *path, অক্ষর *ধ্রুবক argv[]) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ অক্ষর *fn = ফাঁকা;
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ফেরত r;
        যদি (r == O_DIRECTORY) /* Uh? */
                ফেরত -EISDIR;

        ধ্রুবক অক্ষর *e;
        যদি (find_environment_দ্বিমিক(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ফেরত -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ অক্ষর *np = ফাঁকা;
        যদি (find_build_dir_দ্বিমিক(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        ফেরত -errno;
}

পূর্ণসংখ্যা pin_callout_দ্বিমিক(ধ্রুবক অক্ষর *path, অক্ষর **ret_path) {
        পূর্ণসংখ্যা r, fd;

        প্রতিজ্ঞা(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ অক্ষর *fn = ফাঁকা;
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ফেরত r;
        যদি (r == O_DIRECTORY) /* Uh? */
                ফেরত -EISDIR;

        ধ্রুবক অক্ষর *e;
        যদি (find_environment_দ্বিমিক(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ফাঁকা, ret_path, &fd);
                যদি (r < 0)
                        ফেরত r;

                ফেরত fd;
        }

        _cleanup_free_ অক্ষর *np = ফাঁকা;
        যদি (find_build_dir_দ্বিমিক(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ফাঁকা, ret_path, &fd);
                যদি (r >= 0)
                        ফেরত fd;
        }

        r = find_executable_full(path, /* root= */ ফাঁকা,
                                 /* exec_search_path= */ ফাঁকা, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        যদি (r < 0)
                ফেরত r;

        ফেরত fd;
}
