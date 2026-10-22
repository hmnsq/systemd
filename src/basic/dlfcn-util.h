/* SPDX-License-Identifier: LGPL-2.1-or-later */
#ਹਦਾਇਤ once

#ਅਨ੍ਤਰ੍ਭਾਵ <dlfcn.h>      /* IWYU pragma: export */

#ਅਨ੍ਤਰ੍ਭਾਵ "forward.h"

ਖਾਲੀ* ਸੁਰੱਖਿਅਤ_dlclose(ਖਾਲੀ *dl);

ਸਥਿਰ ਸਤਰਅੰਦਰ ਖਾਲੀ dlclosep(ਖਾਲੀ **dlp) {
        ਸੁਰੱਖਿਅਤ_dlclose(*dlp);
}

ਪੂਰਨ dlopen_verbose(ਖਾਲੀ **dlp, ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ log_level);
ਪੂਰਨ dlsym_many_or_warn_sentinel(ਖਾਲੀ *dl, ਪੂਰਨ log_level, ...) _sentinel_;
ਪੂਰਨ dlopen_many_sym_or_warn_sentinel(ਖਾਲੀ **dlp, ਅਚਲ ਅੱਖਰ *filename, ਪੂਰਨ log_level, ...) _sentinel_;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dlsym_many_or_warn(dl, log_level, ...) \
        dlsym_many_or_warn_sentinel(dl, log_level, __VA_ARGS__, ਨਿਰਾ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ dlopen_many_sym_or_warn(dlp, filename, log_level, ...) \
        dlopen_many_sym_or_warn_sentinel(dlp, filename, log_level, __VA_ARGS__, ਨਿਰਾ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLSYM_PROTOTYPE(ਚਿੰਨ੍ਹ)                \
        typeof(ਚਿੰਨ੍ਹ)* sym_##symbol

/* Macro useful for putting together variable/symbol name pairs when calling dlsym_many_or_warn(). Assumes
 * that each library symbol to resolve will be placed in a variable with the "sym_" prefix, i.e. a symbol
 * "foobar" is loaded into a variable "sym_foobar". */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLSYM_ARG(arg) \
        ({ assert_cc(__builtin_types_compatible_p(typeof(sym_##arg), typeof(&arg))); &sym_##arg; }), STRINGIFY(arg)

/* libbpf is a bit confused about type-safety and API compatibility. Provide a macro that can tape over that mess. Sad. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLSYM_ARG_FORCE(arg) \
        &sym_##arg, STRINGIFY(arg)

/* Resolve a single optional symbol from an already-opened library handle. The pointer variable is expected
 * to be named sym_<name> (same convention as DLSYM_ARG). Only assigns on success, so the pointer keeps its
 * pre-existing value if the symbol is not present — useful for fallback initialization. dlerror() is
 * cleared first so callers can distinguish "symbol not found" from "symbol's value is NULL" by checking
 * dlerror() after; for function symbols (which can never be NULL on success) the _v check below is
 * sufficient. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DLSYM_OPTIONAL(dl, name)                                                                \
        ({                                                                                      \
                (ਖਾਲੀ) dlerror();                                                               \
                typeof(sym_##name) _v = (typeof(sym_##name)) dlsym((dl), #name);                \
                ਜੇ (_v) sym_##name = _v;                                                        \
        })

/* If called dlopen_many_sym_or_warn() will fail with EPERM. This can be used to block lazy loading of shared
 * libs, if we transfer a process into a different namespace. Note that this does not work for all calls of
 * dlopen(), just those through our dlopen_safe() wrapper (which we use comprehensively in our
 * codebase). This hence has *no* effect on NSS. (Would be great if we could change that...) */
ਖਾਲੀ ਖੰਡ_dlopen(ਖਾਲੀ);

ਪੂਰਨ dlopen_ਸੁਰੱਖਿਅਤ(ਅਚਲ ਅੱਖਰ *filename, ਖਾਲੀ **ret, ਅਚਲ ਅੱਖਰ **reterr_dlerror);
