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

static số_nguyên get_runpath_from_dynamic(const ElfW(Dyn) *d, ElfW(Addr) bias, const char **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        const char *strtab = NULL;

        assert(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        cho (; d->d_tag != DT_NULL; d++) {

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
                        nếu (d->d_un.d_val != 0)
                                strtab = (const char *) ((uintptr_t) d->d_un.d_val
#nếu defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        break;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                nếu (strtab && runpath_index != SIZE_MAX)
                        break;
        }

        nếu (!strtab)
                trả_về -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        nếu (runpath_index != SIZE_MAX) {
                nếu (ret)
                        *ret = strtab + runpath_index;
                trả_về 1;
        }

        nếu (rpath_index != SIZE_MAX) {
                nếu (ret)
                        *ret = strtab + rpath_index;
                trả_về 1;
        }

        nếu (ret)
                *ret = NULL;

        trả_về 0;
}

static số_nguyên get_runpath(const char **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        nếu (phdr == 0)
                trả_về -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        nếu (phnum == 0)
                trả_về -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        nếu (phent < sizeof(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                trả_về -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        cho (unsigned long i = 0; i < phnum; i++) {
                const ElfW(Phdr) *p = (const ElfW(Phdr)*) (phdr + (i * phent));

                switch (p->p_type) {

                case PT_PHDR:
                        nếu (p->p_vaddr > phdr) /* safety overflow check */
                                trả_về -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        break;

                case PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        break;
                }

                nếu (found_bias && found_dyn)
                        break;
        }

        nếu (!found_dyn)
                trả_về -ENOTRECOVERABLE;

        trả_về get_runpath_from_dynamic((const ElfW(Dyn)*) (bias + dyn), bias, ret);
}

số_nguyên get_build_exec_dir(char **ret) {
        số_nguyên r;

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

        static số_nguyên runpath_cached = -ERRNO_MAX-1;
        nếu (runpath_cached == -ERRNO_MAX-1) {
                const char *runpath = NULL;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                nếu (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        nếu (runpath_cached < 0)
                trả_về runpath_cached;
        nếu (runpath_cached == 0)
                trả_về -ENOEXEC;

        _cleanup_free_ char *exe = NULL;
        r = get_process_exe(0, &exe);
        nếu (r < 0)
                trả_về runpath_cached = r;

        trả_về path_extract_directory(exe, ret);
}

static số_nguyên find_build_dir_binary(const char *fn, char **ret) {
        số_nguyên r;

        assert(fn);
        assert(ret);

        _cleanup_free_ char *build_dir = NULL;
        r = get_build_exec_dir(&build_dir);
        nếu (r < 0)
                trả_về r;

        _cleanup_free_ char *np = path_join(build_dir, fn);
        nếu (!np)
                trả_về -ENOMEM;

        *ret = TAKE_PTR(np);
        trả_về 0;
}

static số_nguyên find_environment_binary(const char *fn, const char **ret) {

        assert(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ char *s = strdup(fn);
        nếu (!s)
                trả_về -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        nếu (!strextend(&s, "_PATH"))
                trả_về -ENOMEM;

        const char *e;
        e = secure_getenv(s);
        nếu (!e)
                trả_về -ENXIO;

        *ret = e;
        trả_về 0;
}

số_nguyên invoke_callout_binary(const char *path, char *const argv[]) {
        số_nguyên r;

        assert(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ char *fn = NULL;
        r = path_extract_filename(path, &fn);
        nếu (r < 0)
                trả_về r;
        nếu (r == O_DIRECTORY) /* Uh? */
                trả_về -EISDIR;

        const char *e;
        nếu (find_environment_binary(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                trả_về -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ char *np = NULL;
        nếu (find_build_dir_binary(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        trả_về -errno;
}

số_nguyên pin_callout_binary(const char *path, char **ret_path) {
        số_nguyên r, fd;

        assert(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ char *fn = NULL;
        r = path_extract_filename(path, &fn);
        nếu (r < 0)
                trả_về r;
        nếu (r == O_DIRECTORY) /* Uh? */
                trả_về -EISDIR;

        const char *e;
        nếu (find_environment_binary(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ NULL, ret_path, &fd);
                nếu (r < 0)
                        trả_về r;

                trả_về fd;
        }

        _cleanup_free_ char *np = NULL;
        nếu (find_build_dir_binary(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ NULL, ret_path, &fd);
                nếu (r >= 0)
                        trả_về fd;
        }

        r = find_executable_full(path, /* root= */ NULL,
                                 /* exec_search_path= */ NULL, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        nếu (r < 0)
                trả_về r;

        trả_về fd;
}
