/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ <elf.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <link.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <sys/auxv.h>

#অন্তর্ভাৱ "alloc-util.h"
#অন্তর্ভাৱ "build-path.h"
#অন্তর্ভাৱ "path-util.h"
#অন্তর্ভাৱ "process-util.h"
#অন্তর্ভাৱ "string-util.h"
#অন্তর্ভাৱ "unistd.h"

স্থির পূর্ণাঙ্ক get_runpath_from_dynamic(ধ্রুৱ ElfW(Dyn) *d, ElfW(Addr) bias, ধ্রুৱ ৱর্ণ **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ধ্রুৱ ৱর্ণ *strtab = রিক্ত;

        প্রতিজ্ঞা(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        বাবে (; d->d_tag != DT_NULL; d++) {

                ৱিকল্প (d->d_tag) {

                পক্ষ DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        ভঙ্গ;

                পক্ষ DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        ভঙ্গ;

                পক্ষ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        যদি (d->d_un.d_val != 0)
                                strtab = (ধ্রুৱ ৱর্ণ *) ((uintptr_t) d->d_un.d_val
#যদি defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        ভঙ্গ;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                যদি (strtab && runpath_index != SIZE_MAX)
                        ভঙ্গ;
        }

        যদি (!strtab)
                ঘূৰাই -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        যদি (runpath_index != SIZE_MAX) {
                যদি (ret)
                        *ret = strtab + runpath_index;
                ঘূৰাই 1;
        }

        যদি (rpath_index != SIZE_MAX) {
                যদি (ret)
                        *ret = strtab + rpath_index;
                ঘূৰাই 1;
        }

        যদি (ret)
                *ret = রিক্ত;

        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক get_runpath(ধ্রুৱ ৱর্ণ **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        যদি (phdr == 0)
                ঘূৰাই -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        যদি (phnum == 0)
                ঘূৰাই -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        যদি (phent < পরিমাণ(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ঘূৰাই -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        বাবে (unsigned long i = 0; i < phnum; i++) {
                ধ্রুৱ ElfW(Phdr) *p = (ধ্রুৱ ElfW(Phdr)*) (phdr + (i * phent));

                ৱিকল্প (p->p_type) {

                পক্ষ PT_PHDR:
                        যদি (p->p_vaddr > phdr) /* safety overflow check */
                                ঘূৰাই -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        ভঙ্গ;

                পক্ষ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        ভঙ্গ;
                }

                যদি (found_bias && found_dyn)
                        ভঙ্গ;
        }

        যদি (!found_dyn)
                ঘূৰাই -ENOTRECOVERABLE;

        ঘূৰাই get_runpath_from_dynamic((ধ্রুৱ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

পূর্ণাঙ্ক get_build_exec_dir(ৱর্ণ **ret) {
        পূর্ণাঙ্ক r;

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

        স্থির পূর্ণাঙ্ক runpath_cached = -ERRNO_MAX-1;
        যদি (runpath_cached == -ERRNO_MAX-1) {
                ধ্রুৱ ৱর্ণ *runpath = রিক্ত;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                যদি (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        যদি (runpath_cached < 0)
                ঘূৰাই runpath_cached;
        যদি (runpath_cached == 0)
                ঘূৰাই -ENOEXEC;

        _cleanup_free_ ৱর্ণ *exe = রিক্ত;
        r = get_process_exe(0, &exe);
        যদি (r < 0)
                ঘূৰাই runpath_cached = r;

        ঘূৰাই path_extract_directory(exe, ret);
}

স্থির পূর্ণাঙ্ক find_build_dir_দ্বিমিক(ধ্রুৱ ৱর্ণ *fn, ৱর্ণ **ret) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fn);
        প্রতিজ্ঞা(ret);

        _cleanup_free_ ৱর্ণ *build_dir = রিক্ত;
        r = get_build_exec_dir(&build_dir);
        যদি (r < 0)
                ঘূৰাই r;

        _cleanup_free_ ৱর্ণ *np = path_join(build_dir, fn);
        যদি (!np)
                ঘূৰাই -ENOMEM;

        *ret = TAKE_PTR(np);
        ঘূৰাই 0;
}

স্থির পূর্ণাঙ্ক find_environment_দ্বিমিক(ধ্রুৱ ৱর্ণ *fn, ধ্রুৱ ৱর্ণ **ret) {

        প্রতিজ্ঞা(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ ৱর্ণ *s = strdup(fn);
        যদি (!s)
                ঘূৰাই -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        যদি (!strextend(&s, "_PATH"))
                ঘূৰাই -ENOMEM;

        ধ্রুৱ ৱর্ণ *e;
        e = secure_getenv(s);
        যদি (!e)
                ঘূৰাই -ENXIO;

        *ret = e;
        ঘূৰাই 0;
}

পূর্ণাঙ্ক invoke_callout_দ্বিমিক(ধ্রুৱ ৱর্ণ *path, ৱর্ণ *ধ্রুৱ argv[]) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ ৱর্ণ *fn = রিক্ত;
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == O_DIRECTORY) /* Uh? */
                ঘূৰাই -EISDIR;

        ধ্রুৱ ৱর্ণ *e;
        যদি (find_environment_দ্বিমিক(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ঘূৰাই -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ ৱর্ণ *np = রিক্ত;
        যদি (find_build_dir_দ্বিমিক(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        ঘূৰাই -errno;
}

পূর্ণাঙ্ক pin_callout_দ্বিমিক(ধ্রুৱ ৱর্ণ *path, ৱর্ণ **ret_path) {
        পূর্ণাঙ্ক r, fd;

        প্রতিজ্ঞা(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ ৱর্ণ *fn = রিক্ত;
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ঘূৰাই r;
        যদি (r == O_DIRECTORY) /* Uh? */
                ঘূৰাই -EISDIR;

        ধ্রুৱ ৱর্ণ *e;
        যদি (find_environment_দ্বিমিক(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ রিক্ত, ret_path, &fd);
                যদি (r < 0)
                        ঘূৰাই r;

                ঘূৰাই fd;
        }

        _cleanup_free_ ৱর্ণ *np = রিক্ত;
        যদি (find_build_dir_দ্বিমিক(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ রিক্ত, ret_path, &fd);
                যদি (r >= 0)
                        ঘূৰাই fd;
        }

        r = find_executable_full(path, /* root= */ রিক্ত,
                                 /* exec_search_path= */ রিক্ত, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        যদি (r < 0)
                ঘূৰাই r;

        ঘূৰাই fd;
}
