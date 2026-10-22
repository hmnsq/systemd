/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "dlfcn-util.h"
#ଅନ୍ତର୍ଭାଵ "errno-util.h"
#ଅନ୍ତର୍ଭାଵ "log.h"

ଶୂନ୍ୟ* safe_dlclose(ଶୂନ୍ୟ *dl) {
        ଯଦି (!dl)
                ଫେରନ୍ତୁ ଖାଲି;

        assert_se(dlclose(dl) == 0);
        ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlsym_many_or_warnv(ଶୂନ୍ୟ *dl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level, va_list ap) {
        ଶୂନ୍ୟ (**fn)(ଶୂନ୍ୟ);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ଯେତେବେଳେ ((fn = va_arg(ap, typeof(fn)))) {
                ଶୂନ୍ୟ (*tfn)(ଶୂନ୍ୟ);
                ଅଚଳ ଅକ୍ଷର *ପ୍ରତୀକ;

                ପ୍ରତୀକ = va_arg(ap, typeof(ପ୍ରତୀକ));

                tfn = (typeof(tfn)) dlsym(dl, ପ୍ରତୀକ);
                ଯଦି (!tfn)
                        ଫେରନ୍ତୁ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", ପ୍ରତୀକ, dlerror());
                *fn = tfn;
        }

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlsym_many_or_warn_sentinel(ଶୂନ୍ୟ *dl, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level, ...) {
        va_list ap;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_verbose(ଶୂନ୍ୟ **dlp, ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dlp);

        ଯଦି (*dlp)
                ଫେରନ୍ତୁ 0; /* Already loaded */

        _cleanup_(dlclosep) ଶୂନ୍ୟ *dl = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *dle = ଖାଲି;
        r = dlopen_safe(filename, &dl, &dle);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_many_sym_or_warn_sentinel(ଶୂନ୍ୟ **dlp, ଅଚଳ ଅକ୍ଷର *filename, ପୂର୍ଣ୍ଣସଂଖ୍ୟା log_level, ...) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(dlp);

        ଯଦି (*dlp)
                ଫେରନ୍ତୁ 0; /* Already loaded */

        _cleanup_(dlclosep) ଶୂନ୍ୟ *dl = ଖାଲି;
        r = dlopen_verbose(&dl, filename, log_level);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ଫେରନ୍ତୁ 1;
}

ସ୍ଥିର bool dlopen_blocked = false;

ଶୂନ୍ୟ block_dlopen(ଶୂନ୍ୟ) {
        dlopen_blocked = true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା dlopen_safe(ଅଚଳ ଅକ୍ଷର *filename, ଶୂନ୍ୟ **ret, ଅଚଳ ଅକ୍ଷର **reterr_dlerror) {
#ଯଦି BUILD_STATIC
        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#ଅନ୍ୟଥା
        _cleanup_(dlclosep) ଶୂନ୍ୟ *dl = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ଯଦି (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ଯଦି (!dl) {
                ଯଦି (dlopen_blocked) {
                        (ଶୂନ୍ୟ) dlerror(); /* consume error, so that no later call will return it */

                        ଯଦି (reterr_dlerror)
                                *reterr_dlerror = ଖାଲି;

                        ଫେରନ୍ତୁ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ଯଦି (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                ଅନ୍ୟଥା
                        (ଶୂନ୍ୟ) dlerror(); /* consume error, so that no later call will return it */

                ଫେରନ୍ତୁ r;
        }

        ଯଦି (ret)
                *ret = TAKE_PTR(dl);

        ଫେରନ୍ତୁ 0;
#endif
}
