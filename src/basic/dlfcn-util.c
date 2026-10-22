/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "dlfcn-util.h"
#include "errno-util.h"
#include "log.h"

void* safe_dlclose(void *dl) {
        အကယ်၍ (!dl)
                ပြန်ပေး NULL;

        assert_se(dlclose(dl) == 0);
        ပြန်ပေး NULL;
}

static ကိန်းပြည့် dlsym_many_or_warnv(void *dl, ကိန်းပြည့် log_level, va_list ap) {
        void (**fn)(void);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        နေစဉ် ((fn = va_arg(ap, typeof(fn)))) {
                void (*tfn)(void);
                const var_na *symbol;

                symbol = va_arg(ap, typeof(symbol));

                tfn = (typeof(tfn)) dlsym(dl, symbol);
                အကယ်၍ (!tfn)
                        ပြန်ပေး log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", symbol, dlerror());
                *fn = tfn;
        }

        ပြန်ပေး 0;
}

ကိန်းပြည့် dlsym_many_or_warn_sentinel(void *dl, ကိန်းပြည့် log_level, ...) {
        va_list ap;
        ကိန်းပြည့် r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        ပြန်ပေး r;
}

ကိန်းပြည့် dlopen_verbose(void **dlp, const var_na *filename, ကိန်းပြည့် log_level) {
        ကိန်းပြည့် r;

        assert(dlp);

        အကယ်၍ (*dlp)
                ပြန်ပေး 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        const var_na *dle = NULL;
        r = dlopen_safe(filename, &dl, &dle);
        အကယ်၍ (r < 0)
                ပြန်ပေး log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        ပြန်ပေး 1;
}

ကိန်းပြည့် dlopen_many_sym_or_warn_sentinel(void **dlp, const var_na *filename, ကိန်းပြည့် log_level, ...) {
        ကိန်းပြည့် r;

        assert(dlp);

        အကယ်၍ (*dlp)
                ပြန်ပေး 0; /* Already loaded */

        _cleanup_(dlclosep) void *dl = NULL;
        r = dlopen_verbose(&dl, filename, log_level);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        ပြန်ပေး 1;
}

static bool dlopen_blocked = false;

void block_dlopen(void) {
        dlopen_blocked = true;
}

ကိန်းပြည့် dlopen_safe(const var_na *filename, void **ret, const var_na **reterr_dlerror) {
#အကယ်၍ BUILD_STATIC
        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#မဟုတ်ပါက
        _cleanup_(dlclosep) void *dl = NULL;
        ကိန်းပြည့် r;

        assert(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        ကိန်းပြည့် flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        အကယ်၍ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        အကယ်၍ (!dl) {
                အကယ်၍ (dlopen_blocked) {
                        (void) dlerror(); /* consume error, so that no later call will return it */

                        အကယ်၍ (reterr_dlerror)
                                *reterr_dlerror = NULL;

                        ပြန်ပေး log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                အကယ်၍ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                မဟုတ်ပါက
                        (void) dlerror(); /* consume error, so that no later call will return it */

                ပြန်ပေး r;
        }

        အကယ်၍ (ret)
                *ret = TAKE_PTR(dl);

        ပြန်ပေး 0;
#endif
}
