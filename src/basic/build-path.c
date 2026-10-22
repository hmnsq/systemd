/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <elf.h>
#include <fcntl.h>
#include <link.h>
#include <stdlib.h>
#include <sys/auxv.h>

#include "alloc-util.h"
#include "build-path.h"
#include "path-util.h"
#include "process-util.h"
#include "string-util.h"
#include "unistd.h"

static puur_naa_nka get_runpath_from_dynamic(const ElfW(Dyn) *d, ElfW(Addr) bias, const var_na **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        const var_na *strtab = NULL;

        assert(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        for (; d->d_tag != DT_NULL; d++) {

                switch (d->d_tag) {

                pak_sa DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        bha_nga;

                pak_sa DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        bha_nga;

                pak_sa DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        ᬬᬾᬦ᭄ (d->d_un.d_val != 0)
                                strtab = (const var_na *) ((uintptr_t) d->d_un.d_val
#ᬬᬾᬦ᭄ defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        bha_nga;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                ᬬᬾᬦ᭄ (strtab && runpath_index != SIZE_MAX)
                        bha_nga;
        }

        ᬬᬾᬦ᭄ (!strtab)
                ᬯᬮᬶ -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        ᬬᬾᬦ᭄ (runpath_index != SIZE_MAX) {
                ᬬᬾᬦ᭄ (ret)
                        *ret = strtab + runpath_index;
                ᬯᬮᬶ 1;
        }

        ᬬᬾᬦ᭄ (rpath_index != SIZE_MAX) {
                ᬬᬾᬦ᭄ (ret)
                        *ret = strtab + rpath_index;
                ᬯᬮᬶ 1;
        }

        ᬬᬾᬦ᭄ (ret)
                *ret = NULL;

        ᬯᬮᬶ 0;
}

static puur_naa_nka get_runpath(const var_na **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        ᬬᬾᬦ᭄ (phdr == 0)
                ᬯᬮᬶ -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        ᬬᬾᬦ᭄ (phnum == 0)
                ᬯᬮᬶ -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        ᬬᬾᬦ᭄ (phent < parimaa_na(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                ᬯᬮᬶ -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        for (unsigned long i = 0; i < phnum; i++) {
                const ElfW(Phdr) *p = (const ElfW(Phdr)*) (phdr + (i * phent));

                switch (p->p_type) {

                pak_sa PT_PHDR:
                        ᬬᬾᬦ᭄ (p->p_vaddr > phdr) /* safety overflow check */
                                ᬯᬮᬶ -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        bha_nga;

                pak_sa PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        bha_nga;
                }

                ᬬᬾᬦ᭄ (found_bias && found_dyn)
                        bha_nga;
        }

        ᬬᬾᬦ᭄ (!found_dyn)
                ᬯᬮᬶ -ENOTRECOVERABLE;

        ᬯᬮᬶ get_runpath_from_dynamic((const ElfW(Dyn)*) (bias + dyn), bias, ret);
}

puur_naa_nka get_build_exec_dir(var_na **ret) {
        puur_naa_nka r;

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

        static puur_naa_nka runpath_cached = -ERRNO_MAX-1;
        ᬬᬾᬦ᭄ (runpath_cached == -ERRNO_MAX-1) {
                const var_na *runpath = NULL;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                ᬬᬾᬦ᭄ (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        ᬬᬾᬦ᭄ (runpath_cached < 0)
                ᬯᬮᬶ runpath_cached;
        ᬬᬾᬦ᭄ (runpath_cached == 0)
                ᬯᬮᬶ -ENOEXEC;

        _cleanup_free_ var_na *exe = NULL;
        r = get_process_exe(0, &exe);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ runpath_cached = r;

        ᬯᬮᬶ path_extract_directory(exe, ret);
}

static puur_naa_nka find_build_dir_binary(const var_na *fn, var_na **ret) {
        puur_naa_nka r;

        assert(fn);
        assert(ret);

        _cleanup_free_ var_na *build_dir = NULL;
        r = get_build_exec_dir(&build_dir);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        _cleanup_free_ var_na *np = path_join(build_dir, fn);
        ᬬᬾᬦ᭄ (!np)
                ᬯᬮᬶ -ENOMEM;

        *ret = TAKE_PTR(np);
        ᬯᬮᬶ 0;
}

static puur_naa_nka find_environment_binary(const var_na *fn, const var_na **ret) {

        assert(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ var_na *s = strdup(fn);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        ᬬᬾᬦ᭄ (!strextend(&s, "_PATH"))
                ᬯᬮᬶ -ENOMEM;

        const var_na *e;
        e = secure_getenv(s);
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ -ENXIO;

        *ret = e;
        ᬯᬮᬶ 0;
}

puur_naa_nka invoke_callout_binary(const var_na *path, var_na *const argv[]) {
        puur_naa_nka r;

        assert(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ var_na *fn = NULL;
        r = path_extract_filename(path, &fn);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (r == O_DIRECTORY) /* Uh? */
                ᬯᬮᬶ -EISDIR;

        const var_na *e;
        ᬬᬾᬦ᭄ (find_environment_binary(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                ᬯᬮᬶ -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ var_na *np = NULL;
        ᬬᬾᬦ᭄ (find_build_dir_binary(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        ᬯᬮᬶ -errno;
}

puur_naa_nka pin_callout_binary(const var_na *path, var_na **ret_path) {
        puur_naa_nka r, fd;

        assert(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ var_na *fn = NULL;
        r = path_extract_filename(path, &fn);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;
        ᬬᬾᬦ᭄ (r == O_DIRECTORY) /* Uh? */
                ᬯᬮᬶ -EISDIR;

        const var_na *e;
        ᬬᬾᬦ᭄ (find_environment_binary(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ NULL, ret_path, &fd);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬯᬮᬶ fd;
        }

        _cleanup_free_ var_na *np = NULL;
        ᬬᬾᬦ᭄ (find_build_dir_binary(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ NULL, ret_path, &fd);
                ᬬᬾᬦ᭄ (r >= 0)
                        ᬯᬮᬶ fd;
        }

        r = find_executable_full(path, /* root= */ NULL,
                                 /* exec_search_path= */ NULL, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬯᬮᬶ fd;
}
