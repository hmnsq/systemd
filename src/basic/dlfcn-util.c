/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ಅನ್ತರ್ಭಾವ "dlfcn-util.h"
#ಅನ್ತರ್ಭಾವ "errno-util.h"
#ಅನ್ತರ್ಭಾವ "log.h"

ಶೂನ್ಯ* ಸುರಕ್ಷಿತ_dlclose(ಶೂನ್ಯ *dl) {
        ಆದರೆ (!dl)
                ಹಿಂತಿರುಗಿ ಖಾಲಿ;

        assert_se(dlclose(dl) == 0);
        ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dlsym_many_or_warnv(ಶೂನ್ಯ *dl, ಪೂರ್ಣಾಂಕ log_level, va_list ap) {
        ಶೂನ್ಯ (**fn)(ಶೂನ್ಯ);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ಆಗ ((fn = va_arg(ap, typeof(fn)))) {
                ಶೂನ್ಯ (*tfn)(ಶೂನ್ಯ);
                ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಚಿಹ್ನೆ;

                ಚಿಹ್ನೆ = va_arg(ap, typeof(ಚಿಹ್ನೆ));

                tfn = (typeof(tfn)) dlsym(dl, ಚಿಹ್ನೆ);
                ಆದರೆ (!tfn)
                        ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", ಚಿಹ್ನೆ, dlerror());
                *fn = tfn;
        }

        ಹಿಂತಿರುಗಿ 0;
}

ಪೂರ್ಣಾಂಕ dlsym_many_or_warn_sentinel(ಶೂನ್ಯ *dl, ಪೂರ್ಣಾಂಕ log_level, ...) {
        va_list ap;
        ಪೂರ್ಣಾಂಕ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ಹಿಂತಿರುಗಿ r;
}

ಪೂರ್ಣಾಂಕ dlopen_verbose(ಶೂನ್ಯ **dlp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ log_level) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dlp);

        ಆದರೆ (*dlp)
                ಹಿಂತಿರುಗಿ 0; /* Already loaded */

        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ಖಾಲಿ;
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dle = ಖಾಲಿ;
        r = dlopen_ಸುರಕ್ಷಿತ(filename, &dl, &dle);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ log_ತುಂಬಿದ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ಹಿಂತಿರುಗಿ 1;
}

ಪೂರ್ಣಾಂಕ dlopen_many_sym_or_warn_sentinel(ಶೂನ್ಯ **dlp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಪೂರ್ಣಾಂಕ log_level, ...) {
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(dlp);

        ಆದರೆ (*dlp)
                ಹಿಂತಿರುಗಿ 0; /* Already loaded */

        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ಖಾಲಿ;
        r = dlopen_verbose(&dl, filename, log_level);
        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ಆದರೆ (r < 0)
                ಹಿಂತಿರುಗಿ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ಹಿಂತಿರುಗಿ 1;
}

ಸ್ಥಿರ bool dlopen_blocked = false;

ಶೂನ್ಯ ಖಂಡ_dlopen(ಶೂನ್ಯ) {
        dlopen_blocked = true;
}

ಪೂರ್ಣಾಂಕ dlopen_ಸುರಕ್ಷಿತ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *filename, ಶೂನ್ಯ **ret, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **reterr_dlerror) {
#ಆದರೆ BUILD_STATIC
        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ಇಲ್ಲದಿದ್ದರೆ
        _cleanup_(dlclosep) ಶೂನ್ಯ *dl = ಖಾಲಿ;
        ಪೂರ್ಣಾಂಕ r;

        ಪ್ರತಿಜ್ಞಾ(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ಪೂರ್ಣಾಂಕ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ಆದರೆ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ಆದರೆ (!dl) {
                ಆದರೆ (dlopen_blocked) {
                        (ಶೂನ್ಯ) dlerror(); /* consume error, so that no later call will return it */

                        ಆದರೆ (reterr_dlerror)
                                *reterr_dlerror = ಖಾಲಿ;

                        ಹಿಂತಿರುಗಿ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ಆದರೆ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ಇಲ್ಲದಿದ್ದರೆ
                        (ಶೂನ್ಯ) dlerror(); /* consume error, so that no later call will return it */

                ಹಿಂತಿರುಗಿ r;
        }

        ಆದರೆ (ret)
                *ret = TAKE_PTR(dl);

        ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
}
