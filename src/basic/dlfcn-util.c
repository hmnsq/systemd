/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಶೂನ್ಯ* safe_dlclose(ಶೂನ್ಯ *dl) {
        ಯದಿ (!dl)
                ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

        assert_se(dlclose(dl) == 0);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dlsym_many_or_warnv(ಶೂನ್ಯ *dl, ಪೂರ್ಣಾಙ್ಕ log_level, va_list ap) {
        ಶೂನ್ಯ (**fn)(ಶೂನ್ಯ);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ಯಾವತ್ ((fn = va_arg(ap, typeof(fn)))) {
                ಶೂನ್ಯ (*tfn)(ಶೂನ್ಯ);
                ಧ್ರುವ ವರ್ಣ *ಚಿಹ್ನೆ;

                ಚಿಹ್ನೆ = va_arg(ap, typeof(ಚಿಹ್ನೆ));

                tfn = (typeof(tfn)) dlsym(dl, ಚಿಹ್ನೆ);
                ಯದಿ (!tfn)
                        ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", ಚಿಹ್ನೆ, dlerror());
                *fn = tfn;
        }

        ಪ್ರತ್ಯಾಗಮ 0;
}

ಪೂರ್ಣಾಙ್ಕ dlsym_many_or_warn_sentinel(ಶೂನ್ಯ *dl, ಪೂರ್ಣಾಙ್ಕ log_level, ...) {
        va_list ap;
        ಪೂರ್ಣಾಙ್ಕ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ಪ್ರತ್ಯಾಗಮ r;
}

ಪೂರ್ಣಾಙ್ಕ dlopen_verbose(ಶೂನ್ಯ **dlp, ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ log_level) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dlp);

        ಯದಿ (*dlp)
                ಪ್ರತ್ಯಾಗಮ 0; /* Already loaded */

        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ರಿಕ್ತ;
        ಧ್ರುವ ವರ್ಣ *dle = ರಿಕ್ತ;
        r = dlopen_safe(filename, &dl, &dle);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಪೂರ್ಣಾಙ್ಕ dlopen_many_sym_or_warn_sentinel(ಶೂನ್ಯ **dlp, ಧ್ರುವ ವರ್ಣ *filename, ಪೂರ್ಣಾಙ್ಕ log_level, ...) {
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(dlp);

        ಯದಿ (*dlp)
                ಪ್ರತ್ಯಾಗಮ 0; /* Already loaded */

        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ರಿಕ್ತ;
        r = dlopen_verbose(&dl, filename, log_level);
        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ಯದಿ (r < 0)
                ಪ್ರತ್ಯಾಗಮ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ಪ್ರತ್ಯಾಗಮ 1;
}

ಸ್ಥಿರ bool dlopen_blocked = false;

ಶೂನ್ಯ block_dlopen(ಶೂನ್ಯ) {
        dlopen_blocked = true;
}

ಪೂರ್ಣಾಙ್ಕ dlopen_safe(ಧ್ರುವ ವರ್ಣ *filename, ಶೂನ್ಯ **ret, ಧ್ರುವ ವರ್ಣ **reterr_dlerror) {
#ಯದಿ BUILD_STATIC
        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ಅನ್ಯಥಾ
        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ರಿಕ್ತ;
        ಪೂರ್ಣಾಙ್ಕ r;

        ಪ್ರತಿಜ್ಞಾ(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ಪೂರ್ಣಾಙ್ಕ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ಯದಿ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ಯದಿ (!dl) {
                ಯದಿ (dlopen_blocked) {
                        (ಶೂನ್ಯ) dlerror(); /* consume error, so that no later call will return it */

                        ಯದಿ (reterr_dlerror)
                                *reterr_dlerror = ರಿಕ್ತ;

                        ಪ್ರತ್ಯಾಗಮ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ಯದಿ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ಅನ್ಯಥಾ
                        (ಶೂನ್ಯ) dlerror(); /* consume error, so that no later call will return it */

                ಪ್ರತ್ಯಾಗಮ r;
        }

        ಯದಿ (ret)
                *ret = TAKE_PTR(dl);

        ಪ್ರತ್ಯಾಗಮ 0;
#endif
}
