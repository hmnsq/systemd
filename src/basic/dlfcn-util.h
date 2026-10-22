/* SPDX-License-Identifier: LGPL-2.1-or-later */
#సూచన once

#అన్తర్భావ <dlfcn.h>      /* IWYU pragma: export */

#అన్తర్భావ "forward.h"

శూన్య* సురక్షిత_dlclose(శూన్య *dl);

స్థిర అంతఃవరుస శూన్య dlclosep(శూన్య **dlp) {
        సురక్షిత_dlclose(*dlp);
}

పూర్ణాంకం dlopen_verbose(శూన్య **dlp, స్థిరరాశి అక్షరం *filename, పూర్ణాంకం log_level);
పూర్ణాంకం dlsym_many_or_warn_sentinel(శూన్య *dl, పూర్ణాంకం log_level, ...) _sentinel_;
పూర్ణాంకం dlopen_many_sym_or_warn_sentinel(శూన్య **dlp, స్థిరరాశి అక్షరం *filename, పూర్ణాంకం log_level, ...) _sentinel_;

#నిర్వచించు dlsym_many_or_warn(dl, log_level, ...) \
        dlsym_many_or_warn_sentinel(dl, log_level, __VA_ARGS__, ఖాళీ)
#నిర్వచించు dlopen_many_sym_or_warn(dlp, filename, log_level, ...) \
        dlopen_many_sym_or_warn_sentinel(dlp, filename, log_level, __VA_ARGS__, ఖాళీ)

#నిర్వచించు DLSYM_PROTOTYPE(చిహ్నం)                \
        typeof(చిహ్నం)* sym_##symbol

/* Macro useful for putting together variable/symbol name pairs when calling dlsym_many_or_warn(). Assumes
 * that each library symbol to resolve will be placed in a variable with the "sym_" prefix, i.e. a symbol
 * "foobar" is loaded into a variable "sym_foobar". */
#నిర్వచించు DLSYM_ARG(arg) \
        ({ assert_cc(__builtin_types_compatible_p(typeof(sym_##arg), typeof(&arg))); &sym_##arg; }), STRINGIFY(arg)

/* libbpf is a bit confused about type-safety and API compatibility. Provide a macro that can tape over that mess. Sad. */
#నిర్వచించు DLSYM_ARG_FORCE(arg) \
        &sym_##arg, STRINGIFY(arg)

/* Resolve a single optional symbol from an already-opened library handle. The pointer variable is expected
 * to be named sym_<name> (same convention as DLSYM_ARG). Only assigns on success, so the pointer keeps its
 * pre-existing value if the symbol is not present — useful for fallback initialization. dlerror() is
 * cleared first so callers can distinguish "symbol not found" from "symbol's value is NULL" by checking
 * dlerror() after; for function symbols (which can never be NULL on success) the _v check below is
 * sufficient. */
#నిర్వచించు DLSYM_OPTIONAL(dl, name)                                                                \
        ({                                                                                      \
                (శూన్య) dlerror();                                                               \
                typeof(sym_##name) _v = (typeof(sym_##name)) dlsym((dl), #name);                \
                ఒకవేళ (_v) sym_##name = _v;                                                        \
        })

/* If called dlopen_many_sym_or_warn() will fail with EPERM. This can be used to block lazy loading of shared
 * libs, if we transfer a process into a different namespace. Note that this does not work for all calls of
 * dlopen(), just those through our dlopen_safe() wrapper (which we use comprehensively in our
 * codebase). This hence has *no* effect on NSS. (Would be great if we could change that...) */
శూన్య ఖండం_dlopen(శూన్య);

పూర్ణాంకం dlopen_సురక్షిత(స్థిరరాశి అక్షరం *filename, శూన్య **ret, స్థిరరాశి అక్షరం **reterr_dlerror);
