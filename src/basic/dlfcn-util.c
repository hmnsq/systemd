/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাৱ "dlfcn-util.h"
#অন্তর্ভাৱ "errno-util.h"
#অন্তর্ভাৱ "log.h"

শূন্য* safe_dlclose(শূন্য *dl) {
        যদি (!dl)
                ঘূৰাই রিক্ত;

        assert_se(dlclose(dl) == 0);
        ঘূৰাই রিক্ত;
}

স্থির পূর্ণাঙ্ক dlsym_many_or_warnv(শূন্য *dl, পূর্ণাঙ্ক log_level, va_list ap) {
        শূন্য (**fn)(শূন্য);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        যেতিয়ালৈ ((fn = va_arg(ap, typeof(fn)))) {
                শূন্য (*tfn)(শূন্য);
                ধ্রুৱ ৱর্ণ *প্ৰতীক;

                প্ৰতীক = va_arg(ap, typeof(প্ৰতীক));

                tfn = (typeof(tfn)) dlsym(dl, প্ৰতীক);
                যদি (!tfn)
                        ঘূৰাই log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", প্ৰতীক, dlerror());
                *fn = tfn;
        }

        ঘূৰাই 0;
}

পূর্ণাঙ্ক dlsym_many_or_warn_sentinel(শূন্য *dl, পূর্ণাঙ্ক log_level, ...) {
        va_list ap;
        পূর্ণাঙ্ক r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ঘূৰাই r;
}

পূর্ণাঙ্ক dlopen_verbose(শূন্য **dlp, ধ্রুৱ ৱর্ণ *filename, পূর্ণাঙ্ক log_level) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dlp);

        যদি (*dlp)
                ঘূৰাই 0; /* Already loaded */

        _cleanup_(dlclosep) শূন্য *dl = রিক্ত;
        ধ্রুৱ ৱর্ণ *dle = রিক্ত;
        r = dlopen_safe(filename, &dl, &dle);
        যদি (r < 0)
                ঘূৰাই log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ঘূৰাই 1;
}

পূর্ণাঙ্ক dlopen_many_sym_or_warn_sentinel(শূন্য **dlp, ধ্রুৱ ৱর্ণ *filename, পূর্ণাঙ্ক log_level, ...) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(dlp);

        যদি (*dlp)
                ঘূৰাই 0; /* Already loaded */

        _cleanup_(dlclosep) শূন্য *dl = রিক্ত;
        r = dlopen_verbose(&dl, filename, log_level);
        যদি (r < 0)
                ঘূৰাই r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        যদি (r < 0)
                ঘূৰাই r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ঘূৰাই 1;
}

স্থির bool dlopen_blocked = false;

শূন্য block_dlopen(শূন্য) {
        dlopen_blocked = true;
}

পূর্ণাঙ্ক dlopen_safe(ধ্রুৱ ৱর্ণ *filename, শূন্য **ret, ধ্রুৱ ৱর্ণ **reterr_dlerror) {
#যদি BUILD_STATIC
        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#অন্যথা
        _cleanup_(dlclosep) শূন্য *dl = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        পূর্ণাঙ্ক flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        যদি (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        যদি (!dl) {
                যদি (dlopen_blocked) {
                        (শূন্য) dlerror(); /* consume error, so that no later call will return it */

                        যদি (reterr_dlerror)
                                *reterr_dlerror = রিক্ত;

                        ঘূৰাই log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                যদি (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                অন্যথা
                        (শূন্য) dlerror(); /* consume error, so that no later call will return it */

                ঘূৰাই r;
        }

        যদি (ret)
                *ret = TAKE_PTR(dl);

        ঘূৰাই 0;
#endif
}
