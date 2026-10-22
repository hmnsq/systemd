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

static ціле get_runpath_from_dynamic(const ElfW(Dyn) *d, ElfW(Addr) bias, const char **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        const char *strtab = NULL;

        assert(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        для (; d->d_tag != DT_NULL; d++) {

                switch (d->d_tag) {

                case DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        break;

                case DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        break;

                case DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        якщо (d->d_un.d_val != 0)
                                strtab = (const char *) ((uintptr_t) d->d_un.d_val
#якщо defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        break;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                якщо (strtab && runpath_index != SIZE_MAX)
                        break;
        }

        якщо (!strtab)
                повернути -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        якщо (runpath_index != SIZE_MAX) {
                якщо (ret)
                        *ret = strtab + runpath_index;
                повернути 1;
        }

        якщо (rpath_index != SIZE_MAX) {
                якщо (ret)
                        *ret = strtab + rpath_index;
                повернути 1;
        }

        якщо (ret)
                *ret = NULL;

        повернути 0;
}

static ціле get_runpath(const char **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        якщо (phdr == 0)
                повернути -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        якщо (phnum == 0)
                повернути -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        якщо (phent < sizeof(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                повернути -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        для (unsigned long i = 0; i < phnum; i++) {
                const ElfW(Phdr) *p = (const ElfW(Phdr)*) (phdr + (i * phent));

                switch (p->p_type) {

                case PT_PHDR:
                        якщо (p->p_vaddr > phdr) /* safety overflow check */
                                повернути -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        break;

                case PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        break;
                }

                якщо (found_bias && found_dyn)
                        break;
        }

        якщо (!found_dyn)
                повернути -ENOTRECOVERABLE;

        повернути get_runpath_from_dynamic((const ElfW(Dyn)*) (bias + dyn), bias, ret);
}

ціле get_build_exec_dir(char **ret) {
        ціле r;

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

        static ціле runpath_cached = -ERRNO_MAX-1;
        якщо (runpath_cached == -ERRNO_MAX-1) {
                const char *runpath = NULL;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                якщо (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        якщо (runpath_cached < 0)
                повернути runpath_cached;
        якщо (runpath_cached == 0)
                повернути -ENOEXEC;

        _cleanup_free_ char *exe = NULL;
        r = get_process_exe(0, &exe);
        якщо (r < 0)
                повернути runpath_cached = r;

        повернути path_extract_directory(exe, ret);
}

static ціле find_build_dir_binary(const char *fn, char **ret) {
        ціле r;

        assert(fn);
        assert(ret);

        _cleanup_free_ char *build_dir = NULL;
        r = get_build_exec_dir(&build_dir);
        якщо (r < 0)
                повернути r;

        _cleanup_free_ char *np = path_join(build_dir, fn);
        якщо (!np)
                повернути -ENOMEM;

        *ret = TAKE_PTR(np);
        повернути 0;
}

static ціле find_environment_binary(const char *fn, const char **ret) {

        assert(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ char *s = strdup(fn);
        якщо (!s)
                повернути -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        якщо (!strextend(&s, "_PATH"))
                повернути -ENOMEM;

        const char *e;
        e = secure_getenv(s);
        якщо (!e)
                повернути -ENXIO;

        *ret = e;
        повернути 0;
}

ціле invoke_callout_binary(const char *path, char *const argv[]) {
        ціле r;

        assert(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ char *fn = NULL;
        r = path_extract_filename(path, &fn);
        якщо (r < 0)
                повернути r;
        якщо (r == O_DIRECTORY) /* Uh? */
                повернути -EISDIR;

        const char *e;
        якщо (find_environment_binary(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                повернути -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ char *np = NULL;
        якщо (find_build_dir_binary(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        повернути -errno;
}

ціле pin_callout_binary(const char *path, char **ret_path) {
        ціле r, fd;

        assert(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ char *fn = NULL;
        r = path_extract_filename(path, &fn);
        якщо (r < 0)
                повернути r;
        якщо (r == O_DIRECTORY) /* Uh? */
                повернути -EISDIR;

        const char *e;
        якщо (find_environment_binary(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ NULL, ret_path, &fd);
                якщо (r < 0)
                        повернути r;

                повернути fd;
        }

        _cleanup_free_ char *np = NULL;
        якщо (find_build_dir_binary(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ NULL, ret_path, &fd);
                якщо (r >= 0)
                        повернути fd;
        }

        r = find_executable_full(path, /* root= */ NULL,
                                 /* exec_search_path= */ NULL, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        якщо (r < 0)
                повернути r;

        повернути fd;
}
