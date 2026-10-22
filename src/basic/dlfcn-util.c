/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "dlfcn-util.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "log.h"

शून्य* safe_dlclose(शून्य *dl) {
        सचे (!dl)
                पटिनिवत्त रिक्त;

        assert_se(dlclose(dl) == 0);
        पटिनिवत्त रिक्त;
}

स्थिर पूर्णाङ्क dlsym_many_or_warnv(शून्य *dl, पूर्णाङ्क log_level, va_list ap) {
        शून्य (**fn)(शून्य);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        याव ((fn = va_arg(ap, typeof(fn)))) {
                शून्य (*tfn)(शून्य);
                ध्रुव वर्ण *चिह्नम्;

                चिह्नम् = va_arg(ap, typeof(चिह्नम्));

                tfn = (typeof(tfn)) dlsym(dl, चिह्नम्);
                सचे (!tfn)
                        पटिनिवत्त log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", चिह्नम्, dlerror());
                *fn = tfn;
        }

        पटिनिवत्त 0;
}

पूर्णाङ्क dlsym_many_or_warn_sentinel(शून्य *dl, पूर्णाङ्क log_level, ...) {
        va_list ap;
        पूर्णाङ्क r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        पटिनिवत्त r;
}

पूर्णाङ्क dlopen_verbose(शून्य **dlp, ध्रुव वर्ण *filename, पूर्णाङ्क log_level) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dlp);

        सचे (*dlp)
                पटिनिवत्त 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        ध्रुव वर्ण *dle = रिक्त;
        r = dlopen_safe(filename, &dl, &dle);
        सचे (r < 0)
                पटिनिवत्त log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        पटिनिवत्त 1;
}

पूर्णाङ्क dlopen_many_sym_or_warn_sentinel(शून्य **dlp, ध्रुव वर्ण *filename, पूर्णाङ्क log_level, ...) {
        पूर्णाङ्क r;

        प्रतिज्ञा(dlp);

        सचे (*dlp)
                पटिनिवत्त 0; /* Already loaded */

        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        r = dlopen_verbose(&dl, filename, log_level);
        सचे (r < 0)
                पटिनिवत्त r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        सचे (r < 0)
                पटिनिवत्त r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        पटिनिवत्त 1;
}

स्थिर bool dlopen_blocked = false;

शून्य block_dlopen(शून्य) {
        dlopen_blocked = true;
}

पूर्णाङ्क dlopen_safe(ध्रुव वर्ण *filename, शून्य **ret, ध्रुव वर्ण **reterr_dlerror) {
#सचे BUILD_STATIC
        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#अञ्ञथा
        _cleanup_(dlclosep) शून्य *dl = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        पूर्णाङ्क flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        सचे (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        सचे (!dl) {
                सचे (dlopen_blocked) {
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                        सचे (reterr_dlerror)
                                *reterr_dlerror = रिक्त;

                        पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                सचे (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                अञ्ञथा
                        (शून्य) dlerror(); /* consume error, so that no later call will return it */

                पटिनिवत्त r;
        }

        सचे (ret)
                *ret = TAKE_PTR(dl);

        पटिनिवत्त 0;
#endif
}
