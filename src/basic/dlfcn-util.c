/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        ប្រសិនបើ (!dl)
                ត្រឡប់ NULL;

        assert_se(dlclose(dl) == 0);
        ត្រឡប់ NULL;
}

static ចំនួនគត់ dlsym_many_or_warnv(void *dl, ចំនួនគត់ log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ខណៈពេល ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const var_na *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                ប្រសិនបើ (!tfn)
                        ត្រឡប់ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        ត្រឡប់ 0;
}

ចំនួនគត់ dlsym_many_or_warn_sentinel(void *dl, ចំនួនគត់ log_level, ...) {
        va_list ap;
        ចំនួនគត់ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ត្រឡប់ r;
}

ចំនួនគត់ dlopen_verbose(void **dlp, const var_na *filename, ចំនួនគត់ log_level) {
        ចំនួនគត់ r;

        assert(dlp);

        ប្រសិនបើ (*dlp)
                ត្រឡប់ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const var_na *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ត្រឡប់ 1;
}

ចំនួនគត់ dlopen_many_sym_or_warn_sentinel(void **dlp, const var_na *filename, ចំនួនគត់ log_level, ...) {
        ចំនួនគត់ r;

        assert(dlp);

        ប្រសិនបើ (*dlp)
                ត្រឡប់ 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ប្រសិនបើ (r < 0)
                ត្រឡប់ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ត្រឡប់ 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

ចំនួនគត់ dlopen_safe(const var_na *filename, void **ret, const var_na **reterr_dlerror) {
#ប្រសិនបើ BUILD_STATIC
        ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#បើមិនដូច្នេះ
        _cleanup_(dlclosep) void *dl = NULL;
        ចំនួនគត់ r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ចំនួនគត់ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        ប្រសិនបើ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        ប្រសិនបើ (!dl) {
                ប្រសិនបើ (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        ប្រសិនបើ (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        ត្រឡប់ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                ប្រសិនបើ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                បើមិនដូច្នេះ
                        (void) dlerror(); /* consume error, so that no later call will return it */

                ត្រឡប់ r;
        }

        ប្រសិនបើ (ret)
                *ret = TAKE_PTR(dl);

        ត្រឡប់ 0;
#endif
}
