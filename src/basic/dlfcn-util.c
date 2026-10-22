/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        གལ་སྲིད (!dl)
                ལོག NULL;

        assert_se(dlclose(dl) == 0);
        ལོག NULL;
}

static ཧྲིལ dlsym_many_or_warnv(void *dl, ཧྲིལ log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        སྐབས ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const var_na *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                གལ་སྲིད (!tfn)
                        ལོག log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        ལོག 0;
}

ཧྲིལ dlsym_many_or_warn_sentinel(void *dl, ཧྲིལ log_level, ...) {
        va_list ap;
        ཧྲིལ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ལོག r;
}

ཧྲིལ dlopen_verbose(void **dlp, const var_na *filename, ཧྲིལ log_level) {
        ཧྲིལ r;

        assert(dlp);

        གལ་སྲིད (*dlp)
                ལོག 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const var_na *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        གལ་སྲིད (r < 0)
                ལོག log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ལོག 1;
}

ཧྲིལ dlopen_many_sym_or_warn_sentinel(void **dlp, const var_na *filename, ཧྲིལ log_level, ...) {
        ཧྲིལ r;

        assert(dlp);

        གལ་སྲིད (*dlp)
                ལོག 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        གལ་སྲིད (r < 0)
                ལོག r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        གལ་སྲིད (r < 0)
                ལོག r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ལོག 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

ཧྲིལ dlopen_safe(const var_na *filename, void **ret, const var_na **reterr_dlerror) {
#གལ་སྲིད BUILD_STATIC
        ལོག log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#དེ་མིན
        _cleanup_(dlclosep) void *dl = NULL;
        ཧྲིལ r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ཧྲིལ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        གལ་སྲིད (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        གལ་སྲིད (!dl) {
                གལ་སྲིད (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        གལ་སྲིད (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        ལོག log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                གལ་སྲིད (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                དེ་མིན
                        (void) dlerror(); /* consume error, so that no later call will return it */

                ལོག r;
        }

        གལ་སྲིད (ret)
                *ret = TAKE_PTR(dl);

        ལོག 0;
#endif
}
