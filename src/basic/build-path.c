/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ <elf.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <link.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <sys/auxv.h>

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "build-path.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "process-util.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "unistd.h"

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_runpath_from_dynamic(ଅଚଳ ElfW(Dyn) *d, ElfW(Addr) bias, ଅଚଳ ଅକ୍ଷର **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        ଅଚଳ ଅକ୍ଷର *strtab = ଖାଲି;

        ପ୍ରତିଜ୍ଞା(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        ପାଇଁ (; d->d_tag != DT_NULL; d++) {

                ବାଛ (d->d_tag) {

                ସ୍ଥିତି DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ଯଦି (d->d_un.d_val != 0)
                                strtab = (ଅଚଳ ଅକ୍ଷର *) ((uintptr_t) d->d_un.d_val
#ଯଦି defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        ଭାଙ୍ଗ;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ଯଦି (strtab && runpath_index != SIZE_MAX)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (!strtab)
                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ଯଦି (runpath_index != SIZE_MAX) {
                ଯଦି (ret)
                        *ret = strtab + runpath_index;
                ଫେରନ୍ତୁ 1;
        }

        ଯଦି (rpath_index != SIZE_MAX) {
                ଯଦି (ret)
                        *ret = strtab + rpath_index;
                ଫେରନ୍ତୁ 1;
        }

        ଯଦି (ret)
                *ret = ଖାଲି;

        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_runpath(ଅଚଳ ଅକ୍ଷର **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ଯଦି (phdr == 0)
                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ଯଦି (phnum == 0)
                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ଯଦି (phent < ଆକାର(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        ପାଇଁ (unsigned long i = 0; i < phnum; i++) {
                ଅଚଳ ElfW(Phdr) *p = (ଅଚଳ ElfW(Phdr)*) (phdr + (i * phent));

                ବାଛ (p->p_type) {

                ସ୍ଥିତି PT_PHDR:
                        ଯଦି (p->p_vaddr > phdr) /* safety overflow check */
                                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        ଭାଙ୍ଗ;

                ସ୍ଥିତି PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        ଭାଙ୍ଗ;
                }

                ଯଦି (found_bias && found_dyn)
                        ଭାଙ୍ଗ;
        }

        ଯଦି (!found_dyn)
                ଫେରନ୍ତୁ -ENOTRECOVERABLE;

        ଫେରନ୍ତୁ get_runpath_from_dynamic((ଅଚଳ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_build_exec_dir(ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

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

        ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା runpath_cached = -ERRNO_MAX-1;
        ଯଦି (runpath_cached == -ERRNO_MAX-1) {
                ଅଚଳ ଅକ୍ଷର *runpath = ଖାଲି;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ଯଦି (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ଯଦି (runpath_cached < 0)
                ଫେରନ୍ତୁ runpath_cached;
        ଯଦି (runpath_cached == 0)
                ଫେରନ୍ତୁ -ENOEXEC;

        _cleanup_free_ ଅକ୍ଷର *exe = ଖାଲି;
        r = get_process_exe(0, &exe);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ runpath_cached = r;

        ଫେରନ୍ତୁ path_extract_directory(exe, ret);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_build_dir_ଦ୍ୱିମିକ(ଅଚଳ ଅକ୍ଷର *fn, ଅକ୍ଷର **ret) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(fn);
        ପ୍ରତିଜ୍ଞା(ret);

        _cleanup_free_ ଅକ୍ଷର *build_dir = ଖାଲି;
        r = get_build_exec_dir(&build_dir);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        _cleanup_free_ ଅକ୍ଷର *np = path_join(build_dir, fn);
        ଯଦି (!np)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = TAKE_PTR(np);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା find_environment_ଦ୍ୱିମିକ(ଅଚଳ ଅକ୍ଷର *fn, ଅଚଳ ଅକ୍ଷର **ret) {

        ପ୍ରତିଜ୍ଞା(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ ଅକ୍ଷର *s = strdup(fn);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ଯଦି (!strextend(&s, "_PATH"))
                ଫେରନ୍ତୁ -ENOMEM;

        ଅଚଳ ଅକ୍ଷର *e;
        e = secure_getenv(s);
        ଯଦି (!e)
                ଫେରନ୍ତୁ -ENXIO;

        *ret = e;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା invoke_callout_ଦ୍ୱିମିକ(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର *ଅଚଳ argv[]) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି;
        r = path_extract_filename(path, &fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == O_DIRECTORY) /* Uh? */
                ଫେରନ୍ତୁ -EISDIR;

        ଅଚଳ ଅକ୍ଷର *e;
        ଯଦି (find_environment_ଦ୍ୱିମିକ(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ଫେରନ୍ତୁ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ ଅକ୍ଷର *np = ଖାଲି;
        ଯଦି (find_build_dir_ଦ୍ୱିମିକ(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        ଫେରନ୍ତୁ -errno;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା pin_callout_ଦ୍ୱିମିକ(ଅଚଳ ଅକ୍ଷର *path, ଅକ୍ଷର **ret_path) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, fd;

        ପ୍ରତିଜ୍ଞା(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ ଅକ୍ଷର *fn = ଖାଲି;
        r = path_extract_filename(path, &fn);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;
        ଯଦି (r == O_DIRECTORY) /* Uh? */
                ଫେରନ୍ତୁ -EISDIR;

        ଅଚଳ ଅକ୍ଷର *e;
        ଯଦି (find_environment_ଦ୍ୱିମିକ(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ଖାଲି, ret_path, &fd);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଫେରନ୍ତୁ fd;
        }

        _cleanup_free_ ଅକ୍ଷର *np = ଖାଲି;
        ଯଦି (find_build_dir_ଦ୍ୱିମିକ(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ଖାଲି, ret_path, &fd);
                ଯଦି (r >= 0)
                        ଫେରନ୍ତୁ fd;
        }

        r = find_executable_full(path, /* root= */ ଖାଲି,
                                 /* exec_search_path= */ ଖାଲି, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଫେରନ୍ତୁ fd;
}
