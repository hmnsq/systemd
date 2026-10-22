/* SPDX-License-Identifier: LGPL-2.1-or-later */

#અન્તર્ભાવ <elf.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <link.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <sys/auxv.h>

#અન્તર્ભાવ "alloc-util.h"
#અન્તર્ભાવ "build-path.h"
#અન્તર્ભાવ "path-util.h"
#અન્તર્ભાવ "process-util.h"
#અન્તર્ભાવ "string-util.h"
#અન્તર્ભાવ "unistd.h"

સ્થિર પૂર્ણાંક get_runpath_from_dynamic(અચલ ElfW(Dyn) *d, ElfW(Addr) bias, અચલ અક્ષર **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        અચલ અક્ષર *strtab = ખાલી;

        પ્રતિજ્ઞા(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        માટે (; d->d_tag != DT_NULL; d++) {

                પસંદગી (d->d_tag) {

                સ્થિતિ DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        તોડો;

                સ્થિતિ DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        તોડો;

                સ્થિતિ DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        જો (d->d_un.d_val != 0)
                                strtab = (અચલ અક્ષર *) ((uintptr_t) d->d_un.d_val
#જો defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        તોડો;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                જો (strtab && runpath_index != SIZE_MAX)
                        તોડો;
        }

        જો (!strtab)
                પાછા -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        જો (runpath_index != SIZE_MAX) {
                જો (ret)
                        *ret = strtab + runpath_index;
                પાછા 1;
        }

        જો (rpath_index != SIZE_MAX) {
                જો (ret)
                        *ret = strtab + rpath_index;
                પાછા 1;
        }

        જો (ret)
                *ret = ખાલી;

        પાછા 0;
}

સ્થિર પૂર્ણાંક get_runpath(અચલ અક્ષર **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        જો (phdr == 0)
                પાછા -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        જો (phnum == 0)
                પાછા -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        જો (phent < કદ(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                પાછા -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        માટે (unsigned long i = 0; i < phnum; i++) {
                અચલ ElfW(Phdr) *p = (અચલ ElfW(Phdr)*) (phdr + (i * phent));

                પસંદગી (p->p_type) {

                સ્થિતિ PT_PHDR:
                        જો (p->p_vaddr > phdr) /* safety overflow check */
                                પાછા -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        તોડો;

                સ્થિતિ PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        તોડો;
                }

                જો (found_bias && found_dyn)
                        તોડો;
        }

        જો (!found_dyn)
                પાછા -ENOTRECOVERABLE;

        પાછા get_runpath_from_dynamic((અચલ ElfW(Dyn)*) (bias + dyn), bias, ret);
}

પૂર્ણાંક get_build_exec_dir(અક્ષર **ret) {
        પૂર્ણાંક r;

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

        સ્થિર પૂર્ણાંક runpath_cached = -ERRNO_MAX-1;
        જો (runpath_cached == -ERRNO_MAX-1) {
                અચલ અક્ષર *runpath = ખાલી;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                જો (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        જો (runpath_cached < 0)
                પાછા runpath_cached;
        જો (runpath_cached == 0)
                પાછા -ENOEXEC;

        _cleanup_free_ અક્ષર *exe = ખાલી;
        r = get_process_exe(0, &exe);
        જો (r < 0)
                પાછા runpath_cached = r;

        પાછા path_extract_directory(exe, ret);
}

સ્થિર પૂર્ણાંક find_build_dir_દ્વિઅંકી(અચલ અક્ષર *fn, અક્ષર **ret) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(fn);
        પ્રતિજ્ઞા(ret);

        _cleanup_free_ અક્ષર *build_dir = ખાલી;
        r = get_build_exec_dir(&build_dir);
        જો (r < 0)
                પાછા r;

        _cleanup_free_ અક્ષર *np = path_join(build_dir, fn);
        જો (!np)
                પાછા -ENOMEM;

        *ret = TAKE_PTR(np);
        પાછા 0;
}

સ્થિર પૂર્ણાંક find_environment_દ્વિઅંકી(અચલ અક્ષર *fn, અચલ અક્ષર **ret) {

        પ્રતિજ્ઞા(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ અક્ષર *s = strdup(fn);
        જો (!s)
                પાછા -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        જો (!strextend(&s, "_PATH"))
                પાછા -ENOMEM;

        અચલ અક્ષર *e;
        e = secure_getenv(s);
        જો (!e)
                પાછા -ENXIO;

        *ret = e;
        પાછા 0;
}

પૂર્ણાંક invoke_callout_દ્વિઅંકી(અચલ અક્ષર *path, અક્ષર *અચલ argv[]) {
        પૂર્ણાંક r;

        પ્રતિજ્ઞા(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ અક્ષર *fn = ખાલી;
        r = path_extract_filename(path, &fn);
        જો (r < 0)
                પાછા r;
        જો (r == O_DIRECTORY) /* Uh? */
                પાછા -EISDIR;

        અચલ અક્ષર *e;
        જો (find_environment_દ્વિઅંકી(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                પાછા -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ અક્ષર *np = ખાલી;
        જો (find_build_dir_દ્વિઅંકી(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        પાછા -errno;
}

પૂર્ણાંક pin_callout_દ્વિઅંકી(અચલ અક્ષર *path, અક્ષર **ret_path) {
        પૂર્ણાંક r, fd;

        પ્રતિજ્ઞા(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ અક્ષર *fn = ખાલી;
        r = path_extract_filename(path, &fn);
        જો (r < 0)
                પાછા r;
        જો (r == O_DIRECTORY) /* Uh? */
                પાછા -EISDIR;

        અચલ અક્ષર *e;
        જો (find_environment_દ્વિઅંકી(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ ખાલી, ret_path, &fd);
                જો (r < 0)
                        પાછા r;

                પાછા fd;
        }

        _cleanup_free_ અક્ષર *np = ખાલી;
        જો (find_build_dir_દ્વિઅંકી(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ ખાલી, ret_path, &fd);
                જો (r >= 0)
                        પાછા fd;
        }

        r = find_executable_full(path, /* root= */ ખાલી,
                                 /* exec_search_path= */ ખાલી, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        જો (r < 0)
                પાછા r;

        પાછા fd;
}
