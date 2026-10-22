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

ساکن صحیح get_runpath_from_dynamic(ثابت ElfW(Dyn) *d, ElfW(Addr) bias, ثابت حرف **ret) {
        size_t runpath_اشاریہ = SIZE_MAX, rpath_اشاریہ = SIZE_MAX;
        ثابت حرف *strtab = تہی;

        أكد(d);

        /* Iterates through the PT_DYNAMIC section to find the DT_RUNPATH/DT_RPATH entries */

        برائے (; d->d_tag != DT_NULL; d++) {

                انتخاب (d->d_tag) {

                حالت DT_RUNPATH:
                        runpath_اشاریہ = (size_t) d->d_un.d_val;
                        توڑو;

                حالت DT_RPATH:
                        rpath_اشاریہ = (size_t) d->d_un.d_val;
                        توڑو;

                حالت DT_STRTAB:
                        /* On MIPS, RISC-V, or with musl, DT_STRTAB records an offset, not a valid address,
                         * so it has to be adjusted using the bias calculated earlier. */
                        اگر (d->d_un.d_val != 0)
                                strtab = (ثابت حرف *) ((uintptr_t) d->d_un.d_val
#اگر defined(__mips__) || defined(__riscv) || !defined(__GLIBC__)
                                         + bias
#اگر_ختم
                                );
                        توڑو;
                }

                /* runpath wins, hence if we have the table and runpath we can exit the loop early */
                اگر (strtab && runpath_اشاریہ != SIZE_MAX)
                        توڑو;
        }

        اگر (!strtab)
                واپس -ENOTRECOVERABLE;

        /* According to ld.so runpath wins if both runpath and rpath are defined. */
        اگر (runpath_اشاریہ != SIZE_MAX) {
                اگر (ret)
                        *ret = strtab + runpath_اشاریہ;
                واپس 1;
        }

        اگر (rpath_اشاریہ != SIZE_MAX) {
                اگر (ret)
                        *ret = strtab + rpath_اشاریہ;
                واپس 1;
        }

        اگر (ret)
                *ret = تہی;

        واپس 0;
}

ساکن صحیح get_runpath(ثابت حرف **ret) {
        غیرعلامتی طویل phdr, phent, phnum;

        /* Finds the rpath/runpath in the program headers of the main executable we are running in */

        phdr = getauxval(AT_PHDR);      /* Start offset of phdr */
        اگر (phdr == 0)
                واپس -ENOTRECOVERABLE;

        phnum = getauxval(AT_PHNUM);    /* Number of entries in phdr */
        اگر (phnum == 0)
                واپس -ENOTRECOVERABLE;

        phent = getauxval(AT_PHENT);    /* Size of entries in phdr */
        اگر (phent < حجم(ElfW(Phdr))) /* Safety check, that our idea of the structure matches the file */
                واپس -ENOTRECOVERABLE;

        ElfW(Addr) bias = 0, dyn = 0;
        bool ملا_bias = false, ملا_dyn = false;

        /* Iterate through the Phdr structures to find the PT_PHDR and PT_DYNAMIC sections */
        برائے (غیرعلامتی طویل i = 0; i < phnum; i++) {
                ثابت ElfW(Phdr) *p = (ثابت ElfW(Phdr)*) (phdr + (i * phent));

                انتخاب (p->p_type) {

                حالت PT_PHDR:
                        اگر (p->p_vaddr > phdr) /* safety overflow check */
                                واپس -ENOTRECOVERABLE;

                        bias = (ElfW(Addr)) phdr - p->p_vaddr;
                        ملا_bias = true;
                        توڑو;

                حالت PT_DYNAMIC:
                        dyn = p->p_vaddr;
                        ملا_dyn = true;
                        توڑو;
                }

                اگر (ملا_bias && ملا_dyn)
                        توڑو;
        }

        اگر (!ملا_dyn)
                واپس -ENOTRECOVERABLE;

        واپس get_runpath_from_dynamic((ثابت ElfW(Dyn)*) (bias + dyn), bias, ret);
}

صحیح get_build_exec_dir(حرف **ret) {
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

        ساکن صحیح runpath_cached = -ERRNO_MAX-1;
        اگر (runpath_cached == -ERRNO_MAX-1) {
                ثابت حرف *runpath = تہی;

                runpath_cached = get_runpath(&runpath);

                /* We only care if the runpath starts with $ORIGIN/ */
                اگر (runpath_cached > 0 && !startswith(runpath, "$ORIGIN/"))
                        runpath_cached = 0;
        }
        اگر (runpath_cached < 0)
                واپس runpath_cached;
        اگر (runpath_cached == 0)
                واپس -ENOEXEC;

        _cleanup_free_ حرف *exe = تہی;
        r = get_process_exe(0, &exe);
        اگر (r < 0)
                واپس runpath_cached = r;

        واپس راستہ_نکالو_directory(exe, ret);
}

ساکن صحیح ڈھونڈو_build_dir_ثنائی(ثابت حرف *fn, حرف **ret) {
        صحیح r;

        أكد(fn);
        أكد(ret);

        _cleanup_free_ حرف *build_dir = تہی;
        r = get_build_exec_dir(&build_dir);
        اگر (r < 0)
                واپس r;

        _cleanup_free_ حرف *np = راستہ_join(build_dir, fn);
        اگر (!np)
                واپس -ENOMEM;

        *ret = TAKE_PTR(np);
        واپس 0;
}

ساکن صحیح ڈھونڈو_environment_ثنائی(ثابت حرف *fn, ثابت حرف **ret) {

        أكد(ret);

        /* If a path such as /usr/lib/systemd/systemd-foobar is specified, then this will check for an
         * environment variable SYSTEMD_FOOBAR_PATH and return it if set. */

        _cleanup_free_ حرف *s = strdup(fn);
        اگر (!s)
                واپس -ENOMEM;

        ascii_strupper(s);
        string_replace_char(s, '-', '_');

        اگر (!strextend(&s, "_PATH"))
                واپس -ENOMEM;

        ثابت حرف *e;
        e = secure_getenv(s);
        اگر (!e)
                واپس -ENXIO;

        *ret = e;
        واپس 0;
}

صحیح invoke_callout_ثنائی(ثابت حرف *راستہ, حرف *ثابت argv[]) {
        صحیح r;

        أكد(راستہ);

        /* Just like execv(), but tries to execute the specified binary in the build dir instead, if known */

        _cleanup_free_ حرف *fn = تہی;
        r = راستہ_نکالو_filename(راستہ, &fn);
        اگر (r < 0)
                واپس r;
        اگر (r == O_DIRECTORY) /* Uh? */
                واپس -EISDIR;

        ثابت حرف *e;
        اگر (ڈھونڈو_environment_ثنائی(fn, &e) >= 0) {
                /* If there's an explicit environment variable set for this binary, prefer it */
                execv(e, argv);
                واپس -errno; /* The environment variable counts, let's fail otherwise */
        }

        _cleanup_free_ حرف *np = تہی;
        اگر (ڈھونڈو_build_dir_ثنائی(fn, &np) >= 0)
                execv(np, argv);

        execvp(راستہ, argv);
        واپس -errno;
}

صحیح pin_callout_ثنائی(ثابت حرف *راستہ, حرف **ret_راستہ) {
        صحیح r, fd;

        أكد(راستہ);

        /* Similar to invoke_callout_binary(), but pins (i.e. O_PATH opens) the binary instead of executing
         * it, also optionally provides the path to the binary. */

        _cleanup_free_ حرف *fn = تہی;
        r = راستہ_نکالو_filename(راستہ, &fn);
        اگر (r < 0)
                واپس r;
        اگر (r == O_DIRECTORY) /* Uh? */
                واپس -EISDIR;

        ثابت حرف *e;
        اگر (ڈھونڈو_environment_ثنائی(fn, &e) >= 0) {
                /* The environment variable counts. We'd fail if the executable is not available/invalid. */
                r = open_and_check_executable(e, /* root= */ تہی, ret_راستہ, &fd);
                اگر (r < 0)
                        واپس r;

                واپس fd;
        }

        _cleanup_free_ حرف *np = تہی;
        اگر (ڈھونڈو_build_dir_ثنائی(fn, &np) >= 0) {
                r = open_and_check_executable(np, /* root= */ تہی, ret_راستہ, &fd);
                اگر (r >= 0)
                        واپس fd;
        }

        r = ڈھونڈو_executable_بھرا(راستہ, /* root= */ تہی,
                                 /* exec_search_path= */ تہی, /* use_path_envvar= */ true,
                                 ret_راستہ, &fd);
        اگر (r < 0)
                واپس r;

        واپس fd;
}
