/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "dlfcn-util.h"
#شامل "errno-util.h"
#شامل "log.h"

خالی* محفوظ_dlclose(خالی *dl) {
        اگر (!dl)
                واپس تہی;

        assert_se(dlclose(dl) == 0);
        واپس تہی;
}

ساکن صحیح dlsym_many_or_warnv(خالی *dl, صحیح log_level, va_list ap) {
        خالی (**fn)(خالی);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        جب ((fn = va_arg(ap, typeof(fn)))) {
                خالی (*tfn)(خالی);
                ثابت حرف *علامت;

                علامت = va_arg(ap, typeof(علامت));

                tfn = (typeof(tfn)) dlsym(dl, علامت);
                اگر (!tfn)
                        واپس log_بھرا_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", علامت, dlerror());
                *fn = tfn;
        }

        واپس 0;
}

صحیح dlsym_many_or_warn_sentinel(خالی *dl, صحیح log_level, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        واپس r;
}

صحیح dlopen_verbose(خالی **dlp, ثابت حرف *filename, صحیح log_level) {
        صحیح r;

        أكد(dlp);

        اگر (*dlp)
                واپس 0; /* Already loaded */

        _cleanup_(dlclosep) خالی *dl = تہی;
        ثابت حرف *dle = تہی;
        r = dlopen_محفوظ(filename, &dl, &dle);
        اگر (r < 0)
                واپس log_بھرا_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        واپس 1;
}

صحیح dlopen_many_sym_or_warn_sentinel(خالی **dlp, ثابت حرف *filename, صحیح log_level, ...) {
        صحیح r;

        أكد(dlp);

        اگر (*dlp)
                واپس 0; /* Already loaded */

        _cleanup_(dlclosep) خالی *dl = تہی;
        r = dlopen_verbose(&dl, filename, log_level);
        اگر (r < 0)
                واپس r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        اگر (r < 0)
                واپس r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        واپس 1;
}

ساکن bool dlopen_blocked = false;

خالی بلاک_dlopen(خالی) {
        dlopen_blocked = true;
}

صحیح dlopen_محفوظ(ثابت حرف *filename, خالی **ret, ثابت حرف **reterr_dlerror) {
#اگر BUILD_STATIC
        واپس log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ورنہ
        _cleanup_(dlclosep) خالی *dl = تہی;
        صحیح r;

        أكد(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        صحیح flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        اگر (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        اگر (!dl) {
                اگر (dlopen_blocked) {
                        (خالی) dlerror(); /* consume error, so that no later call will return it */

                        اگر (reterr_dlerror)
                                *reterr_dlerror = تہی;

                        واپس log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                اگر (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ورنہ
                        (خالی) dlerror(); /* consume error, so that no later call will return it */

                واپس r;
        }

        اگر (ret)
                *ret = TAKE_PTR(dl);

        واپس 0;
#اگر_ختم
}
