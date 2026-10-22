/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "dlfcn-util.h"
#شامل "errno-util.h"
#شامل "log.h"

تهی* safe_dlclose(تهی *dl) {
        اگر (!dl)
                بازگشت پوچ;

        assert_se(dlclose(dl) == 0);
        بازگشت پوچ;
}

ایستا صحیح dlsym_many_or_warnv(تهی *dl, صحیح log_level, va_list ap) {
        تهی (**fn)(تهی);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        وقتی ((fn = va_arg(ap, typeof(fn)))) {
                تهی (*tfn)(تهی);
                ثابت نویسه *نماد;

                نماد = va_arg(ap, typeof(نماد));

                tfn = (typeof(tfn)) dlsym(dl, نماد);
                اگر (!tfn)
                        بازگشت log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", نماد, dlerror());
                *fn = tfn;
        }

        بازگشت 0;
}

صحیح dlsym_many_or_warn_sentinel(تهی *dl, صحیح log_level, ...) {
        va_list ap;
        صحیح r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        بازگشت r;
}

صحیح dlopen_verbose(تهی **dlp, ثابت نویسه *filename, صحیح log_level) {
        صحیح r;

        أكد(dlp);

        اگر (*dlp)
                بازگشت 0; /* Already loaded */

        _cleanup_(dlclosep) تهی *dl = پوچ;
        ثابت نویسه *dle = پوچ;
        r = dlopen_safe(filename, &dl, &dle);
        اگر (r < 0)
                بازگشت log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        بازگشت 1;
}

صحیح dlopen_many_sym_or_warn_sentinel(تهی **dlp, ثابت نویسه *filename, صحیح log_level, ...) {
        صحیح r;

        أكد(dlp);

        اگر (*dlp)
                بازگشت 0; /* Already loaded */

        _cleanup_(dlclosep) تهی *dl = پوچ;
        r = dlopen_verbose(&dl, filename, log_level);
        اگر (r < 0)
                بازگشت r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        اگر (r < 0)
                بازگشت r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        بازگشت 1;
}

ایستا bool dlopen_blocked = false;

تهی block_dlopen(تهی) {
        dlopen_blocked = true;
}

صحیح dlopen_safe(ثابت نویسه *filename, تهی **ret, ثابت نویسه **reterr_dlerror) {
#اگر BUILD_STATIC
        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#وگرنه
        _cleanup_(dlclosep) تهی *dl = پوچ;
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
                        (تهی) dlerror(); /* consume error, so that no later call will return it */

                        اگر (reterr_dlerror)
                                *reterr_dlerror = پوچ;

                        بازگشت log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                اگر (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                وگرنه
                        (تهی) dlerror(); /* consume error, so that no later call will return it */

                بازگشت r;
        }

        اگر (ret)
                *ret = TAKE_PTR(dl);

        بازگشت 0;
#endif
}
