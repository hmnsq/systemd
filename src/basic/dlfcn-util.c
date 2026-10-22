/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "dlfcn-util.h"
#ضمن "errno-util.h"
#ضمن "log.h"

فراغ* safe_dlclose(فراغ *dl) {
        إذا (!dl)
                أرجع عدم;

        assert_se(dlclose(dl) == 0);
        أرجع عدم;
}

ساكن صحيح dlsym_many_or_warnv(فراغ *dl, صحيح log_level, va_list ap) {
        فراغ (**fn)(فراغ);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        طالما ((fn = va_arg(ap, typeof(fn)))) {
                فراغ (*tfn)(فراغ);
                ثابت حرف *رمز;

                رمز = va_arg(ap, typeof(رمز));

                tfn = (typeof(tfn)) dlsym(dl, رمز);
                إذا (!tfn)
                        أرجع log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", رمز, dlerror());
                *fn = tfn;
        }

        أرجع 0;
}

صحيح dlsym_many_or_warn_sentinel(فراغ *dl, صحيح log_level, ...) {
        va_list ap;
        صحيح r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        أرجع r;
}

صحيح dlopen_verbose(فراغ **dlp, ثابت حرف *filename, صحيح log_level) {
        صحيح r;

        أكد(dlp);

        إذا (*dlp)
                أرجع 0; /* Already loaded */

        _cleanup_(dlclosep) فراغ *dl = عدم;
        ثابت حرف *dle = عدم;
        r = dlopen_safe(filename, &dl, &dle);
        إذا (r < 0)
                أرجع log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        أرجع 1;
}

صحيح dlopen_many_sym_or_warn_sentinel(فراغ **dlp, ثابت حرف *filename, صحيح log_level, ...) {
        صحيح r;

        أكد(dlp);

        إذا (*dlp)
                أرجع 0; /* Already loaded */

        _cleanup_(dlclosep) فراغ *dl = عدم;
        r = dlopen_verbose(&dl, filename, log_level);
        إذا (r < 0)
                أرجع r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        إذا (r < 0)
                أرجع r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        أرجع 1;
}

ساكن bool dlopen_blocked = false;

فراغ block_dlopen(فراغ) {
        dlopen_blocked = true;
}

صحيح dlopen_safe(ثابت حرف *filename, فراغ **ret, ثابت حرف **reterr_dlerror) {
#إذا BUILD_STATIC
        أرجع log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#وإلا
        _cleanup_(dlclosep) فراغ *dl = عدم;
        صحيح r;

        أكد(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        صحيح flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        إذا (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        إذا (!dl) {
                إذا (dlopen_blocked) {
                        (فراغ) dlerror(); /* consume error, so that no later call will return it */

                        إذا (reterr_dlerror)
                                *reterr_dlerror = عدم;

                        أرجع log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                إذا (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                وإلا
                        (فراغ) dlerror(); /* consume error, so that no later call will return it */

                أرجع r;
        }

        إذا (ret)
                *ret = TAKE_PTR(dl);

        أرجع 0;
#endif
}
