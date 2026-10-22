/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ "sd-id128.h"

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "glob-util.h"
#അന്തര്ഭാവ "hexdecoct.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "memory-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "sparse-endian.h"
#അന്തര്ഭാവ "special.h"
#അന്തര്ഭാവ "siphash24.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "unit-def.h"
#അന്തര്ഭാവ "unit-name.h"

/* Characters valid in a unit name. */
#define VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#define VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#define VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#define LONG_UNIT_NAME_HASH_താക്കോൽ SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(സ്ഥിരം അക്ഷരം *n, UnitNameFlags flags) {
        സ്ഥിരം അക്ഷരം *e, *i, *at;

        പ്രതിജ്ഞാ((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        എങ്കിൽ (_unlikely_(flags == 0))
                തിരികെ false;

        എങ്കിൽ (isempty(n))
                തിരികെ false;

        എങ്കിൽ (strlen(n) >= UNIT_NAME_MAX)
                തിരികെ false;

        e = strrchr(n, '.');
        എങ്കിൽ (!e || e == n)
                തിരികെ false;

        എങ്കിൽ (unit_type_from_string(e + 1) < 0)
                തിരികെ false;

        വേണ്ടി (i = n, at = ശൂന്യ; i < e; i++) {

                എങ്കിൽ (*i == '@' && !at)
                        at = i;

                എങ്കിൽ (!strchr(VALID_CHARS_WITH_AT, *i))
                        തിരികെ false;
        }

        എങ്കിൽ (at == n)
                തിരികെ false;

        എങ്കിൽ (flags & UNIT_NAME_PLAIN)
                എങ്കിൽ (!at)
                        തിരികെ true;

        എങ്കിൽ (flags & UNIT_NAME_INSTANCE)
                എങ്കിൽ (at && e > at + 1)
                        തിരികെ true;

        എങ്കിൽ (flags & UNIT_NAME_TEMPLATE)
                എങ്കിൽ (at && e == at + 1)
                        തിരികെ true;

        തിരികെ false;
}

bool unit_prefix_is_valid(സ്ഥിരം അക്ഷരം *p) {

        /* We don't allow additional @ in the prefix string */

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        തിരികെ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(സ്ഥിരം അക്ഷരം *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        എങ്കിൽ (isempty(i))
                തിരികെ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        തിരികെ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(സ്ഥിരം അക്ഷരം *s) {
        എങ്കിൽ (isempty(s))
                തിരികെ false;

        എങ്കിൽ (s[0] != '.')
                തിരികെ false;

        എങ്കിൽ (unit_type_from_string(s + 1) < 0)
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ unit_name_to_prefix(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p;
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(n);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        p = strchr(n, '@');
        എങ്കിൽ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        *ret = s;
        തിരികെ 0;
}

UnitNameFlags unit_name_to_instance(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p, *d;

        പ്രതിജ്ഞാ(n);

        എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        എങ്കിൽ (!p) {
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        എങ്കിൽ (!d)
                തിരികെ -EINVAL;

        എങ്കിൽ (ret) {
                അക്ഷരം *i = strndup(p, d-p);
                എങ്കിൽ (!i)
                        തിരികെ -ENOMEM;

                *ret = i;
        }
        തിരികെ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

പൂർണ്ണസംഖ്യ unit_name_to_prefix_and_instance(സ്ഥിരം അക്ഷരം *n, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *d;
        അക്ഷരം *s;

        പ്രതിജ്ഞാ(n);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        d = strrchr(n, '.');
        എങ്കിൽ (!d)
                തിരികെ -EINVAL;

        s = strndup(n, d - n);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        *ret = s;
        തിരികെ 0;
}

UnitType unit_name_to_type(സ്ഥിരം അക്ഷരം *n) {
        സ്ഥിരം അക്ഷരം *e;

        പ്രതിജ്ഞാ(n);

        എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                തിരികെ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        തിരികെ unit_type_from_string(e + 1);
}

പൂർണ്ണസംഖ്യ unit_name_change_suffix(സ്ഥിരം അക്ഷരം *n, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        size_t a, b;
        സ്ഥിരം അക്ഷരം *e;

        പ്രതിജ്ഞാ(n);
        പ്രതിജ്ഞാ(suffix);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        എങ്കിൽ (!unit_suffix_is_valid(suffix))
                തിരികെ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(അക്ഷരം, a + b + 1);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        എങ്കിൽ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_build(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *instance, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret) {
        UnitType type;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(suffix);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (suffix[0] != '.')
                തിരികെ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        എങ്കിൽ (type < 0)
                തിരികെ type;

        തിരികെ unit_name_build_from_type(prefix, instance, type, ret);
}

പൂർണ്ണസംഖ്യ unit_name_build_from_type(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *instance, UnitType type, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *ut;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(type >= 0);
        പ്രതിജ്ഞാ(type < _UNIT_TYPE_MAX);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_prefix_is_valid(prefix))
                തിരികെ -EINVAL;

        ut = unit_type_to_string(type);

        എങ്കിൽ (instance) {
                എങ്കിൽ (!unit_instance_is_valid(instance))
                        തിരികെ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } അല്ലെങ്കിൽ
                s = strjoin(prefix, ".", ut);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        എങ്കിൽ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

സ്ഥിതം അക്ഷരം *do_escape_char(അക്ഷരം c, അക്ഷരം *t) {
        പ്രതിജ്ഞാ(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        തിരികെ t;
}

സ്ഥിതം അക്ഷരം *do_escape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം *t) {
        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        എങ്കിൽ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        വേണ്ടി (; *f; f++) {
                എങ്കിൽ (*f == '/')
                        *(t++) = '-';
                അല്ലെങ്കിൽ എങ്കിൽ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                അല്ലെങ്കിൽ
                        *(t++) = *f;
        }

        തിരികെ t;
}

അക്ഷരം* unit_name_escape(സ്ഥിരം അക്ഷരം *f) {
        അക്ഷരം *r, *t;

        പ്രതിജ്ഞാ(f);

        r = new(അക്ഷരം, strlen(f)*4+1);
        എങ്കിൽ (!r)
                തിരികെ ശൂന്യ;

        t = do_escape(f, r);
        *t = 0;

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ unit_name_unescape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *r = ശൂന്യ;
        അക്ഷരം *t;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret);

        r = strdup(f);
        എങ്കിൽ (!r)
                തിരികെ -ENOMEM;

        വേണ്ടി (t = r; *f; f++) {
                എങ്കിൽ (*f == '-')
                        *(t++) = '/';
                അല്ലെങ്കിൽ എങ്കിൽ (*f == '\\') {
                        പൂർണ്ണസംഖ്യ a, b;

                        എങ്കിൽ (f[1] != 'x')
                                തിരികെ -EINVAL;

                        a = unhexchar(f[2]);
                        എങ്കിൽ (a < 0)
                                തിരികെ -EINVAL;

                        b = unhexchar(f[3]);
                        എങ്കിൽ (b < 0)
                                തിരികെ -EINVAL;

                        *(t++) = (അക്ഷരം) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } അല്ലെങ്കിൽ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_path_escape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        അക്ഷരം *s;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret);

        r = path_simplify_alloc(f, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (empty_or_വേര്(p))
                s = strdup("-");
        അല്ലെങ്കിൽ {
                എങ്കിൽ (!path_is_normalized(p))
                        തിരികെ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        *ret = s;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_path_unescape(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(f);

        എങ്കിൽ (isempty(f))
                തിരികെ -EINVAL;

        എങ്കിൽ (streq(f, "-")) {
                s = strdup("/");
                എങ്കിൽ (!s)
                        തിരികെ -ENOMEM;
        } അല്ലെങ്കിൽ {
                _cleanup_free_ അക്ഷരം *w = ശൂന്യ;

                r = unit_name_unescape(f, &w);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                /* Don't accept trailing or leading slashes */
                എങ്കിൽ (startswith(w, "/") || endswith(w, "/"))
                        തിരികെ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                എങ്കിൽ (!s)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (!path_is_normalized(s))
                        തിരികെ -EINVAL;
        }

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(s);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_replace_instance_full(
                സ്ഥിരം അക്ഷരം *original,
                സ്ഥിരം അക്ഷരം *instance,
                bool accept_glob,
                അക്ഷരം **ret) {

        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *prefix, *suffix;
        size_t pl;

        പ്രതിജ്ഞാ(original);
        പ്രതിജ്ഞാ(instance);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                തിരികെ -EINVAL;
        എങ്കിൽ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                തിരികെ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        പ്രതിജ്ഞാ(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(അക്ഷരം, pl + strlen(instance) + strlen(suffix) + 1);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

#എങ്കിൽ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        എങ്കിൽ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_template(സ്ഥിരം അക്ഷരം *f, അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *p, *e;
        അക്ഷരം *s;
        size_t a;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                തിരികെ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(അക്ഷരം, a + 1 + strlen(e) + 1);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        തിരികെ 0;
}

bool unit_name_is_hashed(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം അക്ഷരം *s;

        എങ്കിൽ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                തിരികെ false;

        assert_se(s = strrchr(name, '.'));

        എങ്കിൽ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                തിരികെ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        എങ്കിൽ (s[-1] != '_')
                തിരികെ false;

        വേണ്ടി (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                എങ്കിൽ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ unit_name_hash_long(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *n = ശൂന്യ, *hash = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *suffix;
        le64_t h;
        size_t len;

        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (strlen(name) < UNIT_NAME_MAX)
                തിരികെ -EMSGSIZE;

        suffix = strrchr(name, '.');
        എങ്കിൽ (!suffix)
                തിരികെ -EINVAL;

        എങ്കിൽ (unit_type_from_string(suffix+1) < 0)
                തിരികെ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_താക്കോൽ.bytes));

        hash = hexmem(&h, വലുപ്പം(h));
        എങ്കിൽ (!hash)
                തിരികെ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        പ്രതിജ്ഞാ(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        എങ്കിൽ (!n)
                തിരികെ -ENOMEM;

        എങ്കിൽ (!strextend(&n, "_", hash, suffix))
                തിരികെ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_from_path(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(suffix);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_suffix_is_valid(suffix))
                തിരികെ -EINVAL;

        r = unit_name_path_escape(path, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        s = strjoin(p, suffix);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        എങ്കിൽ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ അക്ഷരം *n = ശൂന്യ;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        എങ്കിൽ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_from_path_instance(സ്ഥിരം അക്ഷരം *prefix, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *suffix, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *s = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(prefix);
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(suffix);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!unit_prefix_is_valid(prefix))
                തിരികെ -EINVAL;

        എങ്കിൽ (!unit_suffix_is_valid(suffix))
                തിരികെ -EINVAL;

        r = unit_name_path_escape(path, &p);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        s = strjoin(prefix, "@", p, suffix);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        എങ്കിൽ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                തിരികെ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        എങ്കിൽ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ unit_name_to_path(സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *prefix = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);

        r = unit_name_to_prefix(name, &prefix);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (unit_name_is_hashed(name))
                തിരികെ -ENAMETOOLONG;

        തിരികെ unit_name_path_unescape(prefix, ret);
}

സ്ഥിതം bool do_escape_mangle(സ്ഥിരം അക്ഷരം *f, bool allow_globs, അക്ഷരം *t) {
        സ്ഥിരം അക്ഷരം *valid_chars;
        bool mangled = false;

        പ്രതിജ്ഞാ(f);
        പ്രതിജ്ഞാ(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        വേണ്ടി (; *f; f++)
                എങ്കിൽ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } അല്ലെങ്കിൽ എങ്കിൽ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } അല്ലെങ്കിൽ
                        *(t++) = *f;
        *t = 0;

        തിരികെ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
പൂർണ്ണസംഖ്യ unit_name_mangle_with_suffix(
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *operation,
                UnitNameMangle flags,
                സ്ഥിരം അക്ഷരം *suffix,
                അക്ഷരം **ret) {

        _cleanup_free_ അക്ഷരം *s = ശൂന്യ;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(suffix);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                തിരികെ -EINVAL;

        എങ്കിൽ (!unit_suffix_is_valid(suffix))
                തിരികെ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        എങ്കിൽ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                എങ്കിൽ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        സ്ഥിരം അക്ഷരം *e = ASSERT_PTR(strrchr(name, '.'));

                        തിരികെ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                പോകുക good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        എങ്കിൽ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                എങ്കിൽ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        പോകുക good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        എങ്കിൽ (path_is_absolute(name)) {
                _cleanup_free_ അക്ഷരം *n = ശൂന്യ, *u = ശൂന്യ;

                r = path_simplify_alloc(name, &n);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                എങ്കിൽ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        എങ്കിൽ (r >= 0) {
                                എങ്കിൽ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        തിരികെ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                തിരികെ 1;
                        }
                        എങ്കിൽ (r != -EINVAL)
                                തിരികെ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                എങ്കിൽ (r >= 0) {
                        എങ്കിൽ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                തിരികെ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        തിരികെ 1;
                }
                എങ്കിൽ (r != -EINVAL)
                        തിരികെ r;
        }

        s = new(അക്ഷരം, strlen(name) * 4 + strlen(suffix) + 1);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        എങ്കിൽ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        എങ്കിൽ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        എങ്കിൽ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(s);
        തിരികെ 1;

good:
        തിരികെ strdup_to(ret, name);
}

പൂർണ്ണസംഖ്യ slice_build_parent_slice(സ്ഥിരം അക്ഷരം *slice, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(slice);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!slice_name_is_valid(slice))
                തിരികെ -EINVAL;

        എങ്കിൽ (streq(slice, SPECIAL_വേര്_SLICE)) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        _cleanup_free_ അക്ഷരം *s = strdup(slice);
        എങ്കിൽ (!s)
                തിരികെ -ENOMEM;

        അക്ഷരം *dash = strrchr(s, '-');
        എങ്കിൽ (!dash)
                തിരികെ strdup_to_full(ret, SPECIAL_വേര്_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ slice_build_subslice(സ്ഥിരം അക്ഷരം *slice, സ്ഥിരം അക്ഷരം *name, അക്ഷരം **ret) {
        അക്ഷരം *subslice;

        പ്രതിജ്ഞാ(slice);
        പ്രതിജ്ഞാ(name);
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!slice_name_is_valid(slice))
                തിരികെ -EINVAL;

        എങ്കിൽ (!unit_prefix_is_valid(name))
                തിരികെ -EINVAL;

        എങ്കിൽ (streq(slice, SPECIAL_വേര്_SLICE))
                subslice = strjoin(name, ".slice");
        അല്ലെങ്കിൽ {
                സ്ഥിരം അക്ഷരം *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(അക്ഷരം, (e - slice) + 1 + strlen(name) + 6 + 1);
                എങ്കിൽ (!subslice)
                        തിരികെ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        തിരികെ 0;
}

bool slice_name_is_valid(സ്ഥിരം അക്ഷരം *name) {
        സ്ഥിരം അക്ഷരം *p, *e;
        bool dash = false;

        എങ്കിൽ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                തിരികെ false;

        എങ്കിൽ (streq(name, SPECIAL_വേര്_SLICE))
                തിരികെ true;

        e = endswith(name, ".slice");
        എങ്കിൽ (!e)
                തിരികെ false;

        വേണ്ടി (p = name; p < e; p++) {

                എങ്കിൽ (*p == '-') {

                        /* Don't allow initial dash */
                        എങ്കിൽ (p == name)
                                തിരികെ false;

                        /* Don't allow multiple dashes */
                        എങ്കിൽ (dash)
                                തിരികെ false;

                        dash = true;
                } അല്ലെങ്കിൽ
                        dash = false;
        }

        /* Don't allow trailing hash */
        എങ്കിൽ (dash)
                തിരികെ false;

        തിരികെ true;
}

bool unit_name_prefix_equal(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        സ്ഥിരം അക്ഷരം *p, *q;

        പ്രതിജ്ഞാ(a);
        പ്രതിജ്ഞാ(b);

        എങ്കിൽ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                തിരികെ false;

        p = strchr(a, '@');
        എങ്കിൽ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        എങ്കിൽ (!q)
                q = strrchr(b, '.');

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(q);

        തിരികെ memcmp_nn(a, p - a, b, q - b) == 0;
}
