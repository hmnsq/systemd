/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include "sd-id128.h"

#include "alloc-util.h"
#include "glob-util.h"
#include "hexdecoct.h"
#include "log.h"
#include "memory-util.h"
#include "path-util.h"
#include "sparse-endian.h"
#include "special.h"
#include "siphash24.h"
#include "string-util.h"
#include "unit-def.h"
#include "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_KEY SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(const var_na *n, UnitNameFlags flags) {
        const var_na *e, *i, *at;

        assert((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ᮚᮦᮔ᮪ (_unlikely_(flags == 0))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (isempty(n))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (strlen(n) >= UNIT_NAME_MAX)
                ᮝᮜᮤ false;

        e = strrchr(n, '.');
        ᮚᮦᮔ᮪ (!e || e == n)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (unit_type_from_string(e + 1) < 0)
                ᮝᮜᮤ false;

        for (i = n, at = NULL; i < e; i++) {

                ᮚᮦᮔ᮪ (*i == '@' && !at)
                        at = i;

                ᮚᮦᮔ᮪ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ᮝᮜᮤ false;
        }

        ᮚᮦᮔ᮪ (at == n)
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (flags & UNIT_NAME_PLAIN)
                ᮚᮦᮔ᮪ (!at)
                        ᮝᮜᮤ true;

        ᮚᮦᮔ᮪ (flags & UNIT_NAME_INSTANCE)
                ᮚᮦᮔ᮪ (at && e > at + 1)
                        ᮝᮜᮤ true;

        ᮚᮦᮔ᮪ (flags & UNIT_NAME_TEMPLATE)
                ᮚᮦᮔ᮪ (at && e == at + 1)
                        ᮝᮜᮤ true;

        ᮝᮜᮤ false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ᮚᮦᮔ᮪ (isempty(p))
                ᮝᮜᮤ false;

        ᮝᮜᮤ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ᮚᮦᮔ᮪ (isempty(i))
                ᮝᮜᮤ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ᮝᮜᮤ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ᮚᮦᮔ᮪ (isempty(s))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (s[0] != '.')
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (unit_type_from_string(s + 1) < 0)
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

puur_naa_nka unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        p = strchr(n, '@');
        ᮚᮦᮔ᮪ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        *ret = s;
        ᮝᮜᮤ 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ᮚᮦᮔ᮪ (!p) {
                ᮚᮦᮔ᮪ (ret)
                        *ret = NULL;
                ᮝᮜᮤ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (ret) {
                var_na *i = strndup(p, d-p);
                ᮚᮦᮔ᮪ (!i)
                        ᮝᮜᮤ -ENOMEM;

                *ret = i;
        }
        ᮝᮜᮤ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

puur_naa_nka unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        d = strrchr(n, '.');
        ᮚᮦᮔ᮪ (!d)
                ᮝᮜᮤ -EINVAL;

        s = strndup(n, d - n);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        *ret = s;
        ᮝᮜᮤ 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᮝᮜᮤ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ᮝᮜᮤ unit_type_from_string(e + 1);
}

puur_naa_nka unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!unit_suffix_is_valid(suffix))
                ᮝᮜᮤ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ᮚᮦᮔ᮪ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ᮚᮦᮔ᮪ (suffix[0] != '.')
                ᮝᮜᮤ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ᮚᮦᮔ᮪ (type < 0)
                ᮝᮜᮤ type;

        ᮝᮜᮤ unit_name_build_from_type(prefix, instance, type, ret);
}

puur_naa_nka unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_prefix_is_valid(prefix))
                ᮝᮜᮤ -EINVAL;

        ut = unit_type_to_string(type);

        ᮚᮦᮔ᮪ (instance) {
                ᮚᮦᮔ᮪ (!unit_instance_is_valid(instance))
                        ᮝᮜᮤ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ᮜᮤᮚ
                s = strjoin(prefix, ".", ut);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ᮚᮦᮔ᮪ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ᮝᮜᮤ t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ᮚᮦᮔ᮪ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        for (; *f; f++) {
                ᮚᮦᮔ᮪ (*f == '/')
                        *(t++) = '-';
                ᮜᮤᮚ ᮚᮦᮔ᮪ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ᮜᮤᮚ
                        *(t++) = *f;
        }

        ᮝᮜᮤ t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ᮚᮦᮔ᮪ (!r)
                ᮝᮜᮤ NULL;

        t = do_escape(f, r);
        *t = 0;

        ᮝᮜᮤ r;
}

puur_naa_nka unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ᮚᮦᮔ᮪ (!r)
                ᮝᮜᮤ -ENOMEM;

        for (t = r; *f; f++) {
                ᮚᮦᮔ᮪ (*f == '-')
                        *(t++) = '/';
                ᮜᮤᮚ ᮚᮦᮔ᮪ (*f == '\\') {
                        puur_naa_nka a, b;

                        ᮚᮦᮔ᮪ (f[1] != 'x')
                                ᮝᮜᮤ -EINVAL;

                        a = unhexchar(f[2]);
                        ᮚᮦᮔ᮪ (a < 0)
                                ᮝᮜᮤ -EINVAL;

                        b = unhexchar(f[3]);
                        ᮚᮦᮔ᮪ (b < 0)
                                ᮝᮜᮤ -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ᮜᮤᮚ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        puur_naa_nka r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (empty_or_root(p))
                s = strdup("-");
        ᮜᮤᮚ {
                ᮚᮦᮔ᮪ (!path_is_normalized(p))
                        ᮝᮜᮤ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        *ret = s;
        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(f);

        ᮚᮦᮔ᮪ (isempty(f))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (streq(f, "-")) {
                s = strdup("/");
                ᮚᮦᮔ᮪ (!s)
                        ᮝᮜᮤ -ENOMEM;
        } ᮜᮤᮚ {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                /* Don't accept trailing or leading slashes */
                ᮚᮦᮔ᮪ (startswith(w, "/") || endswith(w, "/"))
                        ᮝᮜᮤ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ᮚᮦᮔ᮪ (!s)
                        ᮝᮜᮤ -ENOMEM;

                ᮚᮦᮔ᮪ (!path_is_normalized(s))
                        ᮝᮜᮤ -EINVAL;
        }

        ᮚᮦᮔ᮪ (ret)
                *ret = TAKE_PTR(s);

        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_replace_instance_full(
                const var_na *original,
                const var_na *instance,
                bool accept_glob,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        const var_na *prefix, *suffix;
        size_t pl;

        assert(original);
        assert(instance);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᮝᮜᮤ -EINVAL;
        ᮚᮦᮔ᮪ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ᮝᮜᮤ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

#ᮚᮦᮔ᮪ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ᮚᮦᮔ᮪ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᮝᮜᮤ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ᮝᮜᮤ 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ᮚᮦᮔ᮪ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᮝᮜᮤ false;

        assert_se(s = strrchr(name, '.'));

        ᮚᮦᮔ᮪ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ᮝᮜᮤ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ᮚᮦᮔ᮪ (s[-1] != '_')
                ᮝᮜᮤ false;

        for (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ᮚᮦᮔ᮪ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

puur_naa_nka unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ᮚᮦᮔ᮪ (strlen(name) < UNIT_NAME_MAX)
                ᮝᮜᮤ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ᮚᮦᮔ᮪ (!suffix)
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (unit_type_from_string(suffix+1) < 0)
                ᮝᮜᮤ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ᮚᮦᮔ᮪ (!hash)
                ᮝᮜᮤ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ᮚᮦᮔ᮪ (!n)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (!strextend(&n, "_", hash, suffix))
                ᮝᮜᮤ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(path);
        assert(suffix);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_suffix_is_valid(suffix))
                ᮝᮜᮤ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        s = strjoin(p, suffix);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ᮚᮦᮔ᮪ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ᮚᮦᮔ᮪ (!unit_prefix_is_valid(prefix))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!unit_suffix_is_valid(suffix))
                ᮝᮜᮤ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        s = strjoin(prefix, "@", p, suffix);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        ᮚᮦᮔ᮪ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ᮝᮜᮤ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ᮚᮦᮔ᮪ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 0;
}

puur_naa_nka unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        puur_naa_nka r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ᮚᮦᮔ᮪ (r < 0)
                ᮝᮜᮤ r;

        ᮚᮦᮔ᮪ (unit_name_is_hashed(name))
                ᮝᮜᮤ -ENAMETOOLONG;

        ᮝᮜᮤ unit_name_path_unescape(prefix, ret);
}

static bool do_escape_mangle(const var_na *f, bool allow_globs, var_na *t) {
        const var_na *valid_chars;
        bool mangled = false;

        assert(f);
        assert(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        for (; *f; f++)
                ᮚᮦᮔ᮪ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ᮜᮤᮚ ᮚᮦᮔ᮪ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ᮜᮤᮚ
                        *(t++) = *f;
        *t = 0;

        ᮝᮜᮤ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
puur_naa_nka unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        puur_naa_nka r;

        assert(name);
        assert(suffix);
        assert(ret);

        ᮚᮦᮔ᮪ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!unit_suffix_is_valid(suffix))
                ᮝᮜᮤ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ᮚᮦᮔ᮪ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ᮚᮦᮔ᮪ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ᮝᮜᮤ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ᮚᮦᮔ᮪ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ᮚᮦᮔ᮪ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ᮚᮦᮔ᮪ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ᮚᮦᮔ᮪ (r < 0)
                        ᮝᮜᮤ r;

                ᮚᮦᮔ᮪ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ᮚᮦᮔ᮪ (r >= 0) {
                                ᮚᮦᮔ᮪ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ᮝᮜᮤ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ᮝᮜᮤ 1;
                        }
                        ᮚᮦᮔ᮪ (r != -EINVAL)
                                ᮝᮜᮤ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ᮚᮦᮔ᮪ (r >= 0) {
                        ᮚᮦᮔ᮪ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ᮝᮜᮤ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ᮝᮜᮤ 1;
                }
                ᮚᮦᮔ᮪ (r != -EINVAL)
                        ᮝᮜᮤ r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ᮚᮦᮔ᮪ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ᮚᮦᮔ᮪ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ᮚᮦᮔ᮪ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ᮝᮜᮤ -EINVAL;

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 1;

good:
        ᮝᮜᮤ strdup_to(ret, name);
}

puur_naa_nka slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ᮚᮦᮔ᮪ (!slice_name_is_valid(slice))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ᮝᮜᮤ 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ᮚᮦᮔ᮪ (!s)
                ᮝᮜᮤ -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ᮚᮦᮔ᮪ (!dash)
                ᮝᮜᮤ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ᮝᮜᮤ 1;
}

puur_naa_nka slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ᮚᮦᮔ᮪ (!slice_name_is_valid(slice))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (!unit_prefix_is_valid(name))
                ᮝᮜᮤ -EINVAL;

        ᮚᮦᮔ᮪ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ᮜᮤᮚ {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ᮚᮦᮔ᮪ (!subslice)
                        ᮝᮜᮤ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ᮝᮜᮤ 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ᮚᮦᮔ᮪ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᮝᮜᮤ false;

        ᮚᮦᮔ᮪ (streq(name, SPECIAL_ROOT_SLICE))
                ᮝᮜᮤ true;

        e = endswith(name, ".slice");
        ᮚᮦᮔ᮪ (!e)
                ᮝᮜᮤ false;

        for (p = name; p < e; p++) {

                ᮚᮦᮔ᮪ (*p == '-') {

                        /* Don't allow initial dash */
                        ᮚᮦᮔ᮪ (p == name)
                                ᮝᮜᮤ false;

                        /* Don't allow multiple dashes */
                        ᮚᮦᮔ᮪ (dash)
                                ᮝᮜᮤ false;

                        dash = true;
                } ᮜᮤᮚ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ᮚᮦᮔ᮪ (dash)
                ᮝᮜᮤ false;

        ᮝᮜᮤ true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ᮚᮦᮔ᮪ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ᮝᮜᮤ false;

        p = strchr(a, '@');
        ᮚᮦᮔ᮪ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ᮚᮦᮔ᮪ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ᮝᮜᮤ memcmp_nn(a, p - a, b, q - b) == 0;
}
