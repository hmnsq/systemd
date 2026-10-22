/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "dlfcn-util.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "log.h"

ശൂന്യം* safe_dlclose(ശൂന്യം *dl) {
        എങ്കിൽ (!dl)
                തിരികെ ശൂന്യ;

        assert_se(dlclose(dl) == 0);
        തിരികെ ശൂന്യ;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dlsym_many_or_warnv(ശൂന്യം *dl, പൂർണ്ണസംഖ്യ log_level, va_list ap) {
        ശൂന്യം (**fn)(ശൂന്യം);

        /* Tries to resolve a bunch of function symbols, and logs an error about if it cannot resolve one of
         * them. Note that this function possibly modifies the supplied function pointers if the whole
         * operation fails. */

        ആകുമ്പോൾ ((fn = va_arg(ap, typeof(fn)))) {
                ശൂന്യം (*tfn)(ശൂന്യം);
                സ്ഥിരം അക്ഷരം *ചിഹ്നം;

                ചിഹ്നം = va_arg(ap, typeof(ചിഹ്നം));

                tfn = (typeof(tfn)) dlsym(dl, ചിഹ്നം);
                എങ്കിൽ (!tfn)
                        തിരികെ log_full_errno(log_level,
                                              SYNTHETIC_ERRNO(ELIBBAD),
                                              "Can't find symbol %s: %s", ചിഹ്നം, dlerror());
                *fn = tfn;
        }

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ dlsym_many_or_warn_sentinel(ശൂന്യം *dl, പൂർണ്ണസംഖ്യ log_level, ...) {
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ dlopen_verbose(ശൂന്യം **dlp, സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ log_level) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dlp);

        എങ്കിൽ (*dlp)
                തിരികെ 0; /* Already loaded */

        _cleanup_(dlclosep) ശൂന്യം *dl = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *dle = ശൂന്യ;
        r = dlopen_safe(filename, &dl, &dle);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(log_level, SYNTHETIC_ERRNO(EOPNOTSUPP),
                                      "Shared library '%s' is not available: %s", filename, dle ?: STRERROR(r));

        log_debug("Loaded shared library '%s' via dlopen().", filename);
        *dlp = TAKE_PTR(dl);
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ dlopen_many_sym_or_warn_sentinel(ശൂന്യം **dlp, സ്ഥിരം അക്ഷരം *filename, പൂർണ്ണസംഖ്യ log_level, ...) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(dlp);

        എങ്കിൽ (*dlp)
                തിരികെ 0; /* Already loaded */

        _cleanup_(dlclosep) ശൂന്യം *dl = ശൂന്യ;
        r = dlopen_verbose(&dl, filename, log_level);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        va_list ap;
        va_start(ap, log_level);
        r = dlsym_many_or_warnv(dl, log_level, ap);
        va_end(ap);

        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* Note that we never release the reference here, because there's no real reason to. After all this
         * was traditionally a regular shared library dependency which lives forever too. */
        *dlp = TAKE_PTR(dl);
        തിരികെ 1;
}

സ്ഥിതം bool dlopen_blocked = false;

ശൂന്യം block_dlopen(ശൂന്യം) {
        dlopen_blocked = true;
}

പൂർണ്ണസംഖ്യ dlopen_safe(സ്ഥിരം അക്ഷരം *filename, ശൂന്യം **ret, സ്ഥിരം അക്ഷരം **reterr_dlerror) {
#എങ്കിൽ BUILD_STATIC
        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EOPNOTSUPP),
                               "dlopen is not supported in static builds, cannot load %s.", filename);
#അല്ലെങ്കിൽ
        _cleanup_(dlclosep) ശൂന്യം *dl = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(filename);

        /* A wrapper around dlopen(), that takes dlopen_blocked into account, and tries to normalize the
         * error reporting a bit. */

        പൂർണ്ണസംഖ്യ flags = RTLD_NOW|RTLD_NODELETE; /* Always set RTLD_NOW + RTLD_NODELETE, for security reasons */

        /* If dlopen() is blocked we'll still try it, but set RTLD_NOLOAD, so that it will still work if
         * already loaded (for example because the binary linked to things regularly), but fail if not. */
        എങ്കിൽ (dlopen_blocked)
                flags |= RTLD_NOLOAD;

        errno = 0;
        dl = dlopen(filename, flags);
        എങ്കിൽ (!dl) {
                എങ്കിൽ (dlopen_blocked) {
                        (ശൂന്യം) dlerror(); /* consume error, so that no later call will return it */

                        എങ്കിൽ (reterr_dlerror)
                                *reterr_dlerror = ശൂന്യ;

                        തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EPERM), "Refusing loading of '%s', as loading further dlopen() modules has been blocked.", filename);
                }

                r = errno_or_else(ENOPKG);

                എങ്കിൽ (reterr_dlerror)
                        *reterr_dlerror = dlerror();
                അല്ലെങ്കിൽ
                        (ശൂന്യം) dlerror(); /* consume error, so that no later call will return it */

                തിരികെ r;
        }

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(dl);

        തിരികെ 0;
#endif
}
