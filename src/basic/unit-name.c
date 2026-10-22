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

        ꦪꦼꦤ꧀ (_unlikely_(flags == 0))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (isempty(n))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (strlen(n) >= UNIT_NAME_MAX)
                ꦮꦭꦶ false;

        e = strrchr(n, '.');
        ꦪꦼꦤ꧀ (!e || e == n)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (unit_type_from_string(e + 1) < 0)
                ꦮꦭꦶ false;

        ꦏꦁꦒꦺꦴ (i = n, at = NULL; i < e; i++) {

                ꦪꦼꦤ꧀ (*i == '@' && !at)
                        at = i;

                ꦪꦼꦤ꧀ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ꦮꦭꦶ false;
        }

        ꦪꦼꦤ꧀ (at == n)
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (flags & UNIT_NAME_PLAIN)
                ꦪꦼꦤ꧀ (!at)
                        ꦮꦭꦶ true;

        ꦪꦼꦤ꧀ (flags & UNIT_NAME_INSTANCE)
                ꦪꦼꦤ꧀ (at && e > at + 1)
                        ꦮꦭꦶ true;

        ꦪꦼꦤ꧀ (flags & UNIT_NAME_TEMPLATE)
                ꦪꦼꦤ꧀ (at && e == at + 1)
                        ꦮꦭꦶ true;

        ꦮꦭꦶ false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ꦪꦼꦤ꧀ (isempty(p))
                ꦮꦭꦶ false;

        ꦮꦭꦶ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ꦪꦼꦤ꧀ (isempty(i))
                ꦮꦭꦶ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ꦮꦭꦶ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ꦪꦼꦤ꧀ (isempty(s))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (s[0] != '.')
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (unit_type_from_string(s + 1) < 0)
                ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}

puur_naa_nka unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        p = strchr(n, '@');
        ꦪꦼꦤ꧀ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        *ret = s;
        ꦮꦭꦶ 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ꦪꦼꦤ꧀ (!p) {
                ꦪꦼꦤ꧀ (ret)
                        *ret = NULL;
                ꦮꦭꦶ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ꦪꦼꦤ꧀ (!d)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (ret) {
                var_na *i = strndup(p, d-p);
                ꦪꦼꦤ꧀ (!i)
                        ꦮꦭꦶ -ENOMEM;

                *ret = i;
        }
        ꦮꦭꦶ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

puur_naa_nka unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        d = strrchr(n, '.');
        ꦪꦼꦤ꧀ (!d)
                ꦮꦭꦶ -EINVAL;

        s = strndup(n, d - n);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        *ret = s;
        ꦮꦭꦶ 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ꦮꦭꦶ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ꦮꦭꦶ unit_type_from_string(e + 1);
}

puur_naa_nka unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (!unit_suffix_is_valid(suffix))
                ꦮꦭꦶ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ꦪꦼꦤ꧀ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ꦪꦼꦤ꧀ (suffix[0] != '.')
                ꦮꦭꦶ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ꦪꦼꦤ꧀ (type < 0)
                ꦮꦭꦶ type;

        ꦮꦭꦶ unit_name_build_from_type(prefix, instance, type, ret);
}

puur_naa_nka unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_prefix_is_valid(prefix))
                ꦮꦭꦶ -EINVAL;

        ut = unit_type_to_string(type);

        ꦪꦼꦤ꧀ (instance) {
                ꦪꦼꦤ꧀ (!unit_instance_is_valid(instance))
                        ꦮꦭꦶ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ꦭꦶꦪ
                s = strjoin(prefix, ".", ut);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ꦪꦼꦤ꧀ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ꦮꦭꦶ t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ꦪꦼꦤ꧀ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ꦏꦁꦒꦺꦴ (; *f; f++) {
                ꦪꦼꦤ꧀ (*f == '/')
                        *(t++) = '-';
                ꦭꦶꦪ ꦪꦼꦤ꧀ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ꦭꦶꦪ
                        *(t++) = *f;
        }

        ꦮꦭꦶ t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ꦪꦼꦤ꧀ (!r)
                ꦮꦭꦶ NULL;

        t = do_escape(f, r);
        *t = 0;

        ꦮꦭꦶ r;
}

puur_naa_nka unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ꦪꦼꦤ꧀ (!r)
                ꦮꦭꦶ -ENOMEM;

        ꦏꦁꦒꦺꦴ (t = r; *f; f++) {
                ꦪꦼꦤ꧀ (*f == '-')
                        *(t++) = '/';
                ꦭꦶꦪ ꦪꦼꦤ꧀ (*f == '\\') {
                        puur_naa_nka a, b;

                        ꦪꦼꦤ꧀ (f[1] != 'x')
                                ꦮꦭꦶ -EINVAL;

                        a = unhexchar(f[2]);
                        ꦪꦼꦤ꧀ (a < 0)
                                ꦮꦭꦶ -EINVAL;

                        b = unhexchar(f[3]);
                        ꦪꦼꦤ꧀ (b < 0)
                                ꦮꦭꦶ -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ꦭꦶꦪ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        puur_naa_nka r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (empty_or_root(p))
                s = strdup("-");
        ꦭꦶꦪ {
                ꦪꦼꦤ꧀ (!path_is_normalized(p))
                        ꦮꦭꦶ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        *ret = s;
        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(f);

        ꦪꦼꦤ꧀ (isempty(f))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (streq(f, "-")) {
                s = strdup("/");
                ꦪꦼꦤ꧀ (!s)
                        ꦮꦭꦶ -ENOMEM;
        } ꦭꦶꦪ {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                /* Don't accept trailing or leading slashes */
                ꦪꦼꦤ꧀ (startswith(w, "/") || endswith(w, "/"))
                        ꦮꦭꦶ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ꦪꦼꦤ꧀ (!s)
                        ꦮꦭꦶ -ENOMEM;

                ꦪꦼꦤ꧀ (!path_is_normalized(s))
                        ꦮꦭꦶ -EINVAL;
        }

        ꦪꦼꦤ꧀ (ret)
                *ret = TAKE_PTR(s);

        ꦮꦭꦶ 0;
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

        ꦪꦼꦤ꧀ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ꦮꦭꦶ -EINVAL;
        ꦪꦼꦤ꧀ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ꦮꦭꦶ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

#ꦪꦼꦤ꧀ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ꦪꦼꦤ꧀ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ꦮꦭꦶ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ꦮꦭꦶ 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ꦪꦼꦤ꧀ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ꦮꦭꦶ false;

        assert_se(s = strrchr(name, '.'));

        ꦪꦼꦤ꧀ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ꦮꦭꦶ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ꦪꦼꦤ꧀ (s[-1] != '_')
                ꦮꦭꦶ false;

        ꦏꦁꦒꦺꦴ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ꦪꦼꦤ꧀ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}

puur_naa_nka unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ꦪꦼꦤ꧀ (strlen(name) < UNIT_NAME_MAX)
                ꦮꦭꦶ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ꦪꦼꦤ꧀ (!suffix)
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (unit_type_from_string(suffix+1) < 0)
                ꦮꦭꦶ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ꦪꦼꦤ꧀ (!hash)
                ꦮꦭꦶ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ꦪꦼꦤ꧀ (!n)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (!strextend(&n, "_", hash, suffix))
                ꦮꦭꦶ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(path);
        assert(suffix);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_suffix_is_valid(suffix))
                ꦮꦭꦶ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        s = strjoin(p, suffix);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ꦪꦼꦤ꧀ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ꦪꦼꦤ꧀ (!unit_prefix_is_valid(prefix))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (!unit_suffix_is_valid(suffix))
                ꦮꦭꦶ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        s = strjoin(prefix, "@", p, suffix);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        ꦪꦼꦤ꧀ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ꦮꦭꦶ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ꦪꦼꦤ꧀ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 0;
}

puur_naa_nka unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        puur_naa_nka r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ꦪꦼꦤ꧀ (r < 0)
                ꦮꦭꦶ r;

        ꦪꦼꦤ꧀ (unit_name_is_hashed(name))
                ꦮꦭꦶ -ENAMETOOLONG;

        ꦮꦭꦶ unit_name_path_unescape(prefix, ret);
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

        ꦏꦁꦒꦺꦴ (; *f; f++)
                ꦪꦼꦤ꧀ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ꦭꦶꦪ ꦪꦼꦤ꧀ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ꦭꦶꦪ
                        *(t++) = *f;
        *t = 0;

        ꦮꦭꦶ mangled;
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

        ꦪꦼꦤ꧀ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (!unit_suffix_is_valid(suffix))
                ꦮꦭꦶ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ꦪꦼꦤ꧀ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ꦪꦼꦤ꧀ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ꦮꦭꦶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ꦪꦼꦤ꧀ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ꦪꦼꦤ꧀ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ꦪꦼꦤ꧀ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ꦪꦼꦤ꧀ (r < 0)
                        ꦮꦭꦶ r;

                ꦪꦼꦤ꧀ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ꦪꦼꦤ꧀ (r >= 0) {
                                ꦪꦼꦤ꧀ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ꦮꦭꦶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ꦮꦭꦶ 1;
                        }
                        ꦪꦼꦤ꧀ (r != -EINVAL)
                                ꦮꦭꦶ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ꦪꦼꦤ꧀ (r >= 0) {
                        ꦪꦼꦤ꧀ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ꦮꦭꦶ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ꦮꦭꦶ 1;
                }
                ꦪꦼꦤ꧀ (r != -EINVAL)
                        ꦮꦭꦶ r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ꦪꦼꦤ꧀ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ꦪꦼꦤ꧀ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ꦪꦼꦤ꧀ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ꦮꦭꦶ -EINVAL;

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 1;

good:
        ꦮꦭꦶ strdup_to(ret, name);
}

puur_naa_nka slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ꦪꦼꦤ꧀ (!slice_name_is_valid(slice))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ꦮꦭꦶ 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ꦪꦼꦤ꧀ (!s)
                ꦮꦭꦶ -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ꦪꦼꦤ꧀ (!dash)
                ꦮꦭꦶ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ꦮꦭꦶ 1;
}

puur_naa_nka slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ꦪꦼꦤ꧀ (!slice_name_is_valid(slice))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (!unit_prefix_is_valid(name))
                ꦮꦭꦶ -EINVAL;

        ꦪꦼꦤ꧀ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ꦭꦶꦪ {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ꦪꦼꦤ꧀ (!subslice)
                        ꦮꦭꦶ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ꦮꦭꦶ 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ꦪꦼꦤ꧀ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ꦮꦭꦶ false;

        ꦪꦼꦤ꧀ (streq(name, SPECIAL_ROOT_SLICE))
                ꦮꦭꦶ true;

        e = endswith(name, ".slice");
        ꦪꦼꦤ꧀ (!e)
                ꦮꦭꦶ false;

        ꦏꦁꦒꦺꦴ (p = name; p < e; p++) {

                ꦪꦼꦤ꧀ (*p == '-') {

                        /* Don't allow initial dash */
                        ꦪꦼꦤ꧀ (p == name)
                                ꦮꦭꦶ false;

                        /* Don't allow multiple dashes */
                        ꦪꦼꦤ꧀ (dash)
                                ꦮꦭꦶ false;

                        dash = true;
                } ꦭꦶꦪ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ꦪꦼꦤ꧀ (dash)
                ꦮꦭꦶ false;

        ꦮꦭꦶ true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ꦪꦼꦤ꧀ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ꦮꦭꦶ false;

        p = strchr(a, '@');
        ꦪꦼꦤ꧀ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ꦪꦼꦤ꧀ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ꦮꦭꦶ memcmp_nn(a, p - a, b, q - b) == 0;
}
