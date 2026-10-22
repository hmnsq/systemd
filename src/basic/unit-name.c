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

        ᬬᬾᬦ᭄ (_unlikely_(flags == 0))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (isempty(n))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (strlen(n) >= UNIT_NAME_MAX)
                ᬯᬮᬶ false;

        e = strrchr(n, '.');
        ᬬᬾᬦ᭄ (!e || e == n)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (unit_type_from_string(e + 1) < 0)
                ᬯᬮᬶ false;

        for (i = n, at = NULL; i < e; i++) {

                ᬬᬾᬦ᭄ (*i == '@' && !at)
                        at = i;

                ᬬᬾᬦ᭄ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ᬯᬮᬶ false;
        }

        ᬬᬾᬦ᭄ (at == n)
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (flags & UNIT_NAME_PLAIN)
                ᬬᬾᬦ᭄ (!at)
                        ᬯᬮᬶ true;

        ᬬᬾᬦ᭄ (flags & UNIT_NAME_INSTANCE)
                ᬬᬾᬦ᭄ (at && e > at + 1)
                        ᬯᬮᬶ true;

        ᬬᬾᬦ᭄ (flags & UNIT_NAME_TEMPLATE)
                ᬬᬾᬦ᭄ (at && e == at + 1)
                        ᬯᬮᬶ true;

        ᬯᬮᬶ false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ᬬᬾᬦ᭄ (isempty(p))
                ᬯᬮᬶ false;

        ᬯᬮᬶ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ᬬᬾᬦ᭄ (isempty(i))
                ᬯᬮᬶ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ᬯᬮᬶ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ᬬᬾᬦ᭄ (isempty(s))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (s[0] != '.')
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (unit_type_from_string(s + 1) < 0)
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

puur_naa_nka unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        p = strchr(n, '@');
        ᬬᬾᬦ᭄ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        *ret = s;
        ᬯᬮᬶ 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ᬬᬾᬦ᭄ (!p) {
                ᬬᬾᬦ᭄ (ret)
                        *ret = NULL;
                ᬯᬮᬶ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ᬬᬾᬦ᭄ (!d)
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (ret) {
                var_na *i = strndup(p, d-p);
                ᬬᬾᬦ᭄ (!i)
                        ᬯᬮᬶ -ENOMEM;

                *ret = i;
        }
        ᬯᬮᬶ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

puur_naa_nka unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        d = strrchr(n, '.');
        ᬬᬾᬦ᭄ (!d)
                ᬯᬮᬶ -EINVAL;

        s = strndup(n, d - n);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        *ret = s;
        ᬯᬮᬶ 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᬯᬮᬶ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ᬯᬮᬶ unit_type_from_string(e + 1);
}

puur_naa_nka unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (!unit_suffix_is_valid(suffix))
                ᬯᬮᬶ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ᬬᬾᬦ᭄ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ᬬᬾᬦ᭄ (suffix[0] != '.')
                ᬯᬮᬶ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ᬬᬾᬦ᭄ (type < 0)
                ᬯᬮᬶ type;

        ᬯᬮᬶ unit_name_build_from_type(prefix, instance, type, ret);
}

puur_naa_nka unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_prefix_is_valid(prefix))
                ᬯᬮᬶ -EINVAL;

        ut = unit_type_to_string(type);

        ᬬᬾᬦ᭄ (instance) {
                ᬬᬾᬦ᭄ (!unit_instance_is_valid(instance))
                        ᬯᬮᬶ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ᬮᬶᬬ
                s = strjoin(prefix, ".", ut);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ᬬᬾᬦ᭄ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ᬯᬮᬶ t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ᬬᬾᬦ᭄ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        for (; *f; f++) {
                ᬬᬾᬦ᭄ (*f == '/')
                        *(t++) = '-';
                ᬮᬶᬬ ᬬᬾᬦ᭄ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ᬮᬶᬬ
                        *(t++) = *f;
        }

        ᬯᬮᬶ t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ᬬᬾᬦ᭄ (!r)
                ᬯᬮᬶ NULL;

        t = do_escape(f, r);
        *t = 0;

        ᬯᬮᬶ r;
}

puur_naa_nka unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ᬬᬾᬦ᭄ (!r)
                ᬯᬮᬶ -ENOMEM;

        for (t = r; *f; f++) {
                ᬬᬾᬦ᭄ (*f == '-')
                        *(t++) = '/';
                ᬮᬶᬬ ᬬᬾᬦ᭄ (*f == '\\') {
                        puur_naa_nka a, b;

                        ᬬᬾᬦ᭄ (f[1] != 'x')
                                ᬯᬮᬶ -EINVAL;

                        a = unhexchar(f[2]);
                        ᬬᬾᬦ᭄ (a < 0)
                                ᬯᬮᬶ -EINVAL;

                        b = unhexchar(f[3]);
                        ᬬᬾᬦ᭄ (b < 0)
                                ᬯᬮᬶ -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ᬮᬶᬬ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        puur_naa_nka r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (empty_or_root(p))
                s = strdup("-");
        ᬮᬶᬬ {
                ᬬᬾᬦ᭄ (!path_is_normalized(p))
                        ᬯᬮᬶ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        *ret = s;
        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(f);

        ᬬᬾᬦ᭄ (isempty(f))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (streq(f, "-")) {
                s = strdup("/");
                ᬬᬾᬦ᭄ (!s)
                        ᬯᬮᬶ -ENOMEM;
        } ᬮᬶᬬ {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                /* Don't accept trailing or leading slashes */
                ᬬᬾᬦ᭄ (startswith(w, "/") || endswith(w, "/"))
                        ᬯᬮᬶ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ᬬᬾᬦ᭄ (!s)
                        ᬯᬮᬶ -ENOMEM;

                ᬬᬾᬦ᭄ (!path_is_normalized(s))
                        ᬯᬮᬶ -EINVAL;
        }

        ᬬᬾᬦ᭄ (ret)
                *ret = TAKE_PTR(s);

        ᬯᬮᬶ 0;
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

        ᬬᬾᬦ᭄ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᬯᬮᬶ -EINVAL;
        ᬬᬾᬦ᭄ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ᬯᬮᬶ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

#ᬬᬾᬦ᭄ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ᬬᬾᬦ᭄ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᬯᬮᬶ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ᬯᬮᬶ 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ᬬᬾᬦ᭄ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᬯᬮᬶ false;

        assert_se(s = strrchr(name, '.'));

        ᬬᬾᬦ᭄ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ᬯᬮᬶ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ᬬᬾᬦ᭄ (s[-1] != '_')
                ᬯᬮᬶ false;

        for (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ᬬᬾᬦ᭄ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

puur_naa_nka unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ᬬᬾᬦ᭄ (strlen(name) < UNIT_NAME_MAX)
                ᬯᬮᬶ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ᬬᬾᬦ᭄ (!suffix)
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (unit_type_from_string(suffix+1) < 0)
                ᬯᬮᬶ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ᬬᬾᬦ᭄ (!hash)
                ᬯᬮᬶ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ᬬᬾᬦ᭄ (!n)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (!strextend(&n, "_", hash, suffix))
                ᬯᬮᬶ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(path);
        assert(suffix);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_suffix_is_valid(suffix))
                ᬯᬮᬶ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        s = strjoin(p, suffix);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ᬬᬾᬦ᭄ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ᬬᬾᬦ᭄ (!unit_prefix_is_valid(prefix))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (!unit_suffix_is_valid(suffix))
                ᬯᬮᬶ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        s = strjoin(prefix, "@", p, suffix);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        ᬬᬾᬦ᭄ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ᬯᬮᬶ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ᬬᬾᬦ᭄ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 0;
}

puur_naa_nka unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        puur_naa_nka r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ᬬᬾᬦ᭄ (r < 0)
                ᬯᬮᬶ r;

        ᬬᬾᬦ᭄ (unit_name_is_hashed(name))
                ᬯᬮᬶ -ENAMETOOLONG;

        ᬯᬮᬶ unit_name_path_unescape(prefix, ret);
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
                ᬬᬾᬦ᭄ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ᬮᬶᬬ ᬬᬾᬦ᭄ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ᬮᬶᬬ
                        *(t++) = *f;
        *t = 0;

        ᬯᬮᬶ mangled;
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

        ᬬᬾᬦ᭄ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (!unit_suffix_is_valid(suffix))
                ᬯᬮᬶ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ᬬᬾᬦ᭄ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ᬬᬾᬦ᭄ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ᬯᬮᬶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ᬬᬾᬦ᭄ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ᬬᬾᬦ᭄ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ᬬᬾᬦ᭄ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ᬬᬾᬦ᭄ (r < 0)
                        ᬯᬮᬶ r;

                ᬬᬾᬦ᭄ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ᬬᬾᬦ᭄ (r >= 0) {
                                ᬬᬾᬦ᭄ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ᬯᬮᬶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ᬯᬮᬶ 1;
                        }
                        ᬬᬾᬦ᭄ (r != -EINVAL)
                                ᬯᬮᬶ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ᬬᬾᬦ᭄ (r >= 0) {
                        ᬬᬾᬦ᭄ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ᬯᬮᬶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ᬯᬮᬶ 1;
                }
                ᬬᬾᬦ᭄ (r != -EINVAL)
                        ᬯᬮᬶ r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ᬬᬾᬦ᭄ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ᬬᬾᬦ᭄ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ᬬᬾᬦ᭄ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ᬯᬮᬶ -EINVAL;

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 1;

good:
        ᬯᬮᬶ strdup_to(ret, name);
}

puur_naa_nka slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ᬬᬾᬦ᭄ (!slice_name_is_valid(slice))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ᬯᬮᬶ 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ᬬᬾᬦ᭄ (!s)
                ᬯᬮᬶ -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ᬬᬾᬦ᭄ (!dash)
                ᬯᬮᬶ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ᬯᬮᬶ 1;
}

puur_naa_nka slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ᬬᬾᬦ᭄ (!slice_name_is_valid(slice))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (!unit_prefix_is_valid(name))
                ᬯᬮᬶ -EINVAL;

        ᬬᬾᬦ᭄ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ᬮᬶᬬ {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ᬬᬾᬦ᭄ (!subslice)
                        ᬯᬮᬶ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ᬯᬮᬶ 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ᬬᬾᬦ᭄ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᬯᬮᬶ false;

        ᬬᬾᬦ᭄ (streq(name, SPECIAL_ROOT_SLICE))
                ᬯᬮᬶ true;

        e = endswith(name, ".slice");
        ᬬᬾᬦ᭄ (!e)
                ᬯᬮᬶ false;

        for (p = name; p < e; p++) {

                ᬬᬾᬦ᭄ (*p == '-') {

                        /* Don't allow initial dash */
                        ᬬᬾᬦ᭄ (p == name)
                                ᬯᬮᬶ false;

                        /* Don't allow multiple dashes */
                        ᬬᬾᬦ᭄ (dash)
                                ᬯᬮᬶ false;

                        dash = true;
                } ᬮᬶᬬ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ᬬᬾᬦ᭄ (dash)
                ᬯᬮᬶ false;

        ᬯᬮᬶ true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ᬬᬾᬦ᭄ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ᬯᬮᬶ false;

        p = strchr(a, '@');
        ᬬᬾᬦ᭄ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ᬬᬾᬦ᭄ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ᬯᬮᬶ memcmp_nn(a, p - a, b, q - b) == 0;
}
