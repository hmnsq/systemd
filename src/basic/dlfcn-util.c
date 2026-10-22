/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

शून्य* safe_dlclose(शून्य *dl) {
        यदि (!dl)
                प्रत्यागम रिक्त;

        assert_se(dlclose(dl) == 0);
        प्रत्यागम रिक्त;
}

स्थिर पूर्णाङ्क dlsym_many_or_warnv(शून्य *dl, पूर्णाङ्क log_level, va_list ap) {
        शून्य (**fn)(शून्य);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        यावत् ((fn = va_arg(ap, typeof(fn)))) {
                शून्य (*tfn)(शून्य);
                ध्रुव वर्ण *प्रतीक;

                प्रतीक = va_arg(ap, typeof(प्रतीक));

                tfn = (typeof(tfn)) dlsym(dl, प्रतीक);
                यदि (!tfn)
                        प्रत्यागम log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", प्रतीक, dlerror());
                *fn = tfn;
        }

        प्रत्यागम 0;
}

पूर्णाङ्क dlsym_many_or_warn_sentinel(शून्य *dl, पूर्णाङ्क log_level, ...) {
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        प्रत्यागम r;
}

पूर्णाङ्क dlopen_verbose(शून्य **dlp, ध्रुव वर्ण *filename, पूर्णाङ्क log_level) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dlp);

        यदि (*dlp)
                प्रत्यागम 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        ध्रुव वर्ण *dle = रिक्त;
        r = dlopen_safe(filename, &dl, &dle);
        यदि (r < 0)
                प्रत्यागम log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        प्रत्यागम 1;
}

पूर्णाङ्क dlopen_many_sym_or_warn_sentinel(शून्य **dlp, ध्रुव वर्ण *filename, पूर्णाङ्क log_level, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dlp);

        यदि (*dlp)
                प्रत्यागम 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        r = dlopen_verbose(&dl, filename, log_level);
        यदि (r < 0)
                प्रत्यागम r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        यदि (r < 0)
                प्रत्यागम r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        प्रत्यागम 1;
}

स्थिर bool dlopen_blocked = false;

शून्य block_dlopen(शून्य) {
        dlopen_blocked = true;
}

पूर्णाङ्क dlopen_safe(ध्रुव वर्ण *filename, शून्य **ret, ध्रुव वर्ण **reterr_dlerror) {
#यदि BUILD_STATIC
        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#अन्यथा
        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        पूर्णाङ्क flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        यदि (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        यदि (!dl) {
                यदि (dlopen_blocked) {
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                        यदि (reterr_dlerror)
                                *reterr_dlerror = रिक्त;

                        प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                यदि (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                अन्यथा
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                प्रत्यागम r;
        }

        यदि (ret)
                *ret = TAKE_PTR(dl);

        प्रत्यागम 0;
#endif
}
