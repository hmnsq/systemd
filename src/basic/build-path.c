/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <elf.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <link.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <sys/auxv.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "build-path.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "process-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "unistd.h"

நிலையான முழு get_runpath_from_dynamic(மாறிலி ElfW(Dyn) *d, ElfW(Addr) bias, மாறிலி எழுத்து **ret) {
        size_t runpath_index = SIZE_MAX, rpath_index = SIZE_MAX;
        மாறிலி எழுத்து *strtab = வெற்று;

        ப்ரதிஜ்ஞா(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        க்கு (; d->d_tag != DT_NULL; d++) {

                தேர்வு (d->d_tag) {

                நிலை DT_RUNPATH:
                        runpath_index = (size_t) d->d_un.d_val;
                        நிறுத்து;

                நிலை DT_RPATH:
                        rpath_index = (size_t) d->d_un.d_val;
                        நிறுத்து;

                நிலை DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        எனில் (d->d_un.d_val != 0)
                                strtab = (மாறிலி எழுத்து *) ((uintptr_t) d->d_un.d_val
#எனில் defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#endif
                                );
                        நிறுத்து;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                எனில் (strtab && runpath_index != SIZE_MAX)
                        நிறுத்து;
        }

        எனில் (!strtab)
                திரும்பு -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        எனில் (runpath_index != SIZE_MAX) {
                எனில் (ret)
                        *ret = strtab + runpath_index;
                திரும்பு 1;
        }

        எனில் (rpath_index != SIZE_MAX) {
                எனில் (ret)
                        *ret = strtab + rpath_index;
                திரும்பு 1;
        }

        எனில் (ret)
                *ret = வெற்று;

        திரும்பு 0;
}

நிலையான முழு get_runpath(மாறிலி எழுத்து **ret) {
        unsigned long phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        எனில் (phdr == 0)
                திரும்பு -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        எனில் (phnum == 0)
                திரும்பு -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        எனில் (phent < அளவு(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                திரும்பு -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool found_bias = false, found_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        க்கு (unsigned long i = 0; i < phnum; i++) {
                மாறிலி ElfW(Phdr) *p = (மாறிலி ElfW(Phdr)*) (phdr + (i * phent));

                தேர்வு (p->p_type) {

                நிலை PT_PHDR:
                        எனில் (p->p_vaddr > phdr) /* safety overflow check */
                                திரும்பு -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        found_bias = true;
                        நிறுத்து;

                நிலை PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        found_dyn = true;
                        நிறுத்து;
                }

                எனில் (found_bias && found_dyn)
                        நிறுத்து;
        }

        எனில் (!found_dyn)
                திரும்பு -ENOTRECOVERABLE;

        திரும்பு get_runpath_from_dynamic((மாறிலி ElfW(Dyn)*) (bias + dyn), bias, ret);
}

முழு get_build_exec_dir(எழுத்து **ret) {
        முழு r;

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

        நிலையான முழு runpath_cached = -ERRNO_MAX-1;
        எனில் (runpath_cached == -ERRNO_MAX-1) {
                மாறிலி எழுத்து *runpath = வெற்று;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                எனில் (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        எனில் (runpath_cached < 0)
                திரும்பு runpath_cached;
        எனில் (runpath_cached == 0)
                திரும்பு -ENOEXEC;

        _cleanup_free_ எழுத்து *exe = வெற்று;
        r = get_process_exe(0, &exe);
        எனில் (r < 0)
                திரும்பு runpath_cached = r;

        திரும்பு path_extract_directory(exe, ret);
}

நிலையான முழு find_build_dir_இருமம்(மாறிலி எழுத்து *fn, எழுத்து **ret) {
        முழு r;

        ப்ரதிஜ்ஞா(fn);
        ப்ரதிஜ்ஞா(ret);

        _cleanup_free_ எழுத்து *build_dir = வெற்று;
        r = get_build_exec_dir(&build_dir);
        எனில் (r < 0)
                திரும்பு r;

        _cleanup_free_ எழுத்து *np = path_join(build_dir, fn);
        எனில் (!np)
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(np);
        திரும்பு 0;
}

நிலையான முழு find_environment_இருமம்(மாறிலி எழுத்து *fn, மாறிலி எழுத்து **ret) {

        ப்ரதிஜ்ஞா(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ எழுத்து *s = strdup(fn);
        எனில் (!s)
                திரும்பு -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        எனில் (!strextend(&s, "_PATH"))
                திரும்பு -ENOMEM;

        மாறிலி எழுத்து *e;
        e = secure_getenv(s);
        எனில் (!e)
                திரும்பு -ENXIO;

        *ret = e;
        திரும்பு 0;
}

முழு invoke_callout_இருமம்(மாறிலி எழுத்து *path, எழுத்து *மாறிலி argv[]) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ எழுத்து *fn = வெற்று;
        r = path_extract_filename(path, &fn);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == O_DIRECTORY) /* Uh? */
                திரும்பு -EISDIR;

        மாறிலி எழுத்து *e;
        எனில் (find_environment_இருமம்(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                திரும்பு -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ எழுத்து *np = வெற்று;
        எனில் (find_build_dir_இருமம்(fn, &np) >= 0)
                execv(np, argv);

        execvp(path, argv);
        திரும்பு -errno;
}

முழு pin_callout_இருமம்(மாறிலி எழுத்து *path, எழுத்து **ret_path) {
        முழு r, fd;

        ப்ரதிஜ்ஞா(path);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ எழுத்து *fn = வெற்று;
        r = path_extract_filename(path, &fn);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == O_DIRECTORY) /* Uh? */
                திரும்பு -EISDIR;

        மாறிலி எழுத்து *e;
        எனில் (find_environment_இருமம்(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ வெற்று, ret_path, &fd);
                எனில் (r < 0)
                        திரும்பு r;

                திரும்பு fd;
        }

        _cleanup_free_ எழுத்து *np = வெற்று;
        எனில் (find_build_dir_இருமம்(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ வெற்று, ret_path, &fd);
                எனில் (r >= 0)
                        திரும்பு fd;
        }

        r = find_executable_full(path, /* root= */ வெற்று,
                                 /* exec_search_path= */ வெற்று, /* use_path_envvar= */ true,
                                 ret_path, &fd);
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு fd;
}
