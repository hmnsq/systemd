/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ "dlfcn-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"

ਖਾਲੀ* ਸੁਰੱਖਿਅਤ_dlclose(ਖਾਲੀ *dl) {
        ਜੇ (!dl)
                ਵਾਪਸ ਨਿਰਾ;

        assert_se(dlclose(dl) == 0);
        ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਪੂਰਨ dlsym_many_or_warnv(ਖਾਲੀ *dl, ਪੂਰਨ log_level, va_list ap) {
        ਖਾਲੀ (**fn)(ਖਾਲੀ);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ਜਦੋਂ ((fn = va_arg(ap, typeof(fn)))) {
                ਖਾਲੀ (*tfn)(ਖਾਲੀ);
                ਅਚਲ ਅੱਖਰ *ਚਿੰਨ੍ਹ;

                ਚਿੰਨ੍ਹ = va_arg(ap, typeof(ਚਿੰਨ੍ਹ));

                tfn = (typeof(tfn)) dlsym(dl, ਚਿੰਨ੍ਹ);
                ਜੇ (!tfn)
                        ਵਾਪਸ log_ਭਰਿਆ_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", ਚਿੰਨ੍ਹ, dlerror());
                *fn = tfn;
        }

        ਵਾਪਸ 0;
}

ਪੂਰਨ dlsym_many_or_warn_sentinel(ਖਾਲੀ *dl, ਪੂਰਨ log_level, ...) {
        va_list ap;
        ਪੂਰਨ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ਵਾਪਸ r;
}

ਪੂਰਨ dlopen_verbose(ਖਾਲੀ **dlp, ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ log_level) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dlp);

        ਜੇ (*dlp)
                ਵਾਪਸ 0; /* Already loaded */

        _cleanup_(dlclosep) ਖਾਲੀ *dl = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *dle = ਨਿਰਾ;
        r = dlopen_ਸੁਰੱਖਿਅਤ(filename, &dl, &dle);
        ਜੇ (r < 0)
                ਵਾਪਸ log_ਭਰਿਆ_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ਵਾਪਸ 1;
}

ਪੂਰਨ dlopen_many_sym_or_warn_sentinel(ਖਾਲੀ **dlp, ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ log_level, ...) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(dlp);

        ਜੇ (*dlp)
                ਵਾਪਸ 0; /* Already loaded */

        _cleanup_(dlclosep) ਖਾਲੀ *dl = ਨਿਰਾ;
        r = dlopen_verbose(&dl, filename, log_level);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ਵਾਪਸ 1;
}

ਸਥਿਰ bool dlopen_blocked = false;

ਖਾਲੀ ਖੰਡ_dlopen(ਖਾਲੀ) {
        dlopen_blocked = true;
}

ਪੂਰਨ dlopen_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *filename, ਖਾਲੀ **ret, ਅਚਲ ਅੱਖਰ **reterr_dlerror) {
#ਜੇ BUILD_STATIC
        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ਨਹੀਂਤਾਂ
        _cleanup_(dlclosep) ਖਾਲੀ *dl = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ਪੂਰਨ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ਜੇ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ਜੇ (!dl) {
                ਜੇ (dlopen_blocked) {
                        (ਖਾਲੀ) dlerror(); /* consume error, so that no later call will return it */

                        ਜੇ (reterr_dlerror)
                                *reterr_dlerror = ਨਿਰਾ;

                        ਵਾਪਸ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ਜੇ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ਨਹੀਂਤਾਂ
                        (ਖਾਲੀ) dlerror(); /* consume error, so that no later call will return it */

                ਵਾਪਸ r;
        }

        ਜੇ (ret)
                *ret = TAKE_PTR(dl);

        ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
}
