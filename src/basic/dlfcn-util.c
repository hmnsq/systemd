/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "dlfcn-util.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "log.h"

வெறுமை* safe_dlclose(வெறுமை *dl) {
        எனில் (!dl)
                திரும்பு வெற்று;

        assert_se(dlclose(dl) == 0);
        திரும்பு வெற்று;
}

நிலையான முழு dlsym_many_or_warnv(வெறுமை *dl, முழு log_level, va_list ap) {
        வெறுமை (**fn)(வெறுமை);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        போது ((fn = va_arg(ap, typeof(fn)))) {
                வெறுமை (*tfn)(வெறுமை);
                மாறிலி எழுத்து *குறியீடு;

                குறியீடு = va_arg(ap, typeof(குறியீடு));

                tfn = (typeof(tfn)) dlsym(dl, குறியீடு);
                எனில் (!tfn)
                        திரும்பு log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", குறியீடு, dlerror());
                *fn = tfn;
        }

        திரும்பு 0;
}

முழு dlsym_many_or_warn_sentinel(வெறுமை *dl, முழு log_level, ...) {
        va_list ap;
        முழு r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        திரும்பு r;
}

முழு dlopen_verbose(வெறுமை **dlp, மாறிலி எழுத்து *filename, முழு log_level) {
        முழு r;

        ப்ரதிஜ்ஞா(dlp);

        எனில் (*dlp)
                திரும்பு 0; /* Already loaded */

        _cleanup_(dlclosep) வெறுமை *dl = வெற்று;
        மாறிலி எழுத்து *dle = வெற்று;
        r = dlopen_safe(filename, &dl, &dle);
        எனில் (r < 0)
                திரும்பு log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        திரும்பு 1;
}

முழு dlopen_many_sym_or_warn_sentinel(வெறுமை **dlp, மாறிலி எழுத்து *filename, முழு log_level, ...) {
        முழு r;

        ப்ரதிஜ்ஞா(dlp);

        எனில் (*dlp)
                திரும்பு 0; /* Already loaded */

        _cleanup_(dlclosep) வெறுமை *dl = வெற்று;
        r = dlopen_verbose(&dl, filename, log_level);
        எனில் (r < 0)
                திரும்பு r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        எனில் (r < 0)
                திரும்பு r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        திரும்பு 1;
}

நிலையான bool dlopen_blocked = false;

வெறுமை block_dlopen(வெறுமை) {
        dlopen_blocked = true;
}

முழு dlopen_safe(மாறிலி எழுத்து *filename, வெறுமை **ret, மாறிலி எழுத்து **reterr_dlerror) {
#எனில் BUILD_STATIC
        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#இல்லையெனில்
        _cleanup_(dlclosep) வெறுமை *dl = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        முழு flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        எனில் (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        எனில் (!dl) {
                எனில் (dlopen_blocked) {
                        (வெறுமை) dlerror(); /* consume error, so that no later call will return it */

                        எனில் (reterr_dlerror)
                                *reterr_dlerror = வெற்று;

                        திரும்பு log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                எனில் (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                இல்லையெனில்
                        (வெறுமை) dlerror(); /* consume error, so that no later call will return it */

                திரும்பு r;
        }

        எனில் (ret)
                *ret = TAKE_PTR(dl);

        திரும்பு 0;
#endif
}
