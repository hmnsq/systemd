/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "replace-var.h"
#അന്തര്ഭാവ "string-util.h"

/*
 * Generic infrastructure for replacing @FOO@ style variables in
 * strings. Will call a callback for each replacement.
 */

സ്ഥിതം പൂർണ്ണസംഖ്യ get_variable(സ്ഥിരം അക്ഷരം *b, അക്ഷരം **r) {
        size_t k;
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(b);
        പ്രതിജ്ഞാ(r);

        എങ്കിൽ (*b != '@')
                തിരികെ 0;

        k = strspn(b + 1, UPPERCASE_LETTERS "_");
        എങ്കിൽ (k <= 0 || b[k+1] != '@')
                തിരികെ 0;

        t = strndup(b + 1, k);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        *r = t;
        തിരികെ 1;
}

അക്ഷരം* replace_var(സ്ഥിരം അക്ഷരം *text, അക്ഷരം *(*lookup)(സ്ഥിരം അക്ഷരം *variable, ശൂന്യം *userdata), ശൂന്യം *userdata) {
        അക്ഷരം *r, *t;
        സ്ഥിരം അക്ഷരം *f;
        size_t l;

        പ്രതിജ്ഞാ(text);
        പ്രതിജ്ഞാ(lookup);

        l = strlen(text);
        r = new(അക്ഷരം, l+1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        f = text;
        t = r;
        ആകുമ്പോൾ (*f) {
                _cleanup_free_ അക്ഷരം *v = ശൂന്യ, *n = ശൂന്യ;
                അക്ഷരം *a;
                പൂർണ്ണസംഖ്യ k;
                size_t skip, d, nl;

                k = get_variable(f, &v);
                എങ്കിൽ (k < 0)
                        പോകുക oom;
                എങ്കിൽ (k == 0) {
                        *(t++) = *(f++);
                        തുടരുക;
                }

                n = lookup(v, userdata);
                എങ്കിൽ (!n)
                        പോകുക oom;

                skip = strlen(v) + 2;

                d = t - r;
                nl = l - skip + strlen(n);
                a = realloc(r, nl + 1);
                എങ്കിൽ (!a)
                        പോകുക oom;

                l = nl;
                r = a;
                t = r + d;

                t = stpcpy(t, n);
                f += skip;
        }

        *t = 0;
        തിരികെ r;

oom:
        തിരികെ mfree(r);
}
