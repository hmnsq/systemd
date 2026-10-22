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

        ᰀᰧ (_unlikely_(flags == 0))
                ᰀᰧᰌ false;

        ᰀᰧ (isempty(n))
                ᰀᰧᰌ false;

        ᰀᰧ (strlen(n) >= UNIT_NAME_MAX)
                ᰀᰧᰌ false;

        e = strrchr(n, '.');
        ᰀᰧ (!e || e == n)
                ᰀᰧᰌ false;

        ᰀᰧ (unit_type_from_string(e + 1) < 0)
                ᰀᰧᰌ false;

        for (i = n, at = NULL; i < e; i++) {

                ᰀᰧ (*i == '@' && !at)
                        at = i;

                ᰀᰧ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ᰀᰧᰌ false;
        }

        ᰀᰧ (at == n)
                ᰀᰧᰌ false;

        ᰀᰧ (flags & UNIT_NAME_PLAIN)
                ᰀᰧ (!at)
                        ᰀᰧᰌ true;

        ᰀᰧ (flags & UNIT_NAME_INSTANCE)
                ᰀᰧ (at && e > at + 1)
                        ᰀᰧᰌ true;

        ᰀᰧ (flags & UNIT_NAME_TEMPLATE)
                ᰀᰧ (at && e == at + 1)
                        ᰀᰧᰌ true;

        ᰀᰧᰌ false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ᰀᰧ (isempty(p))
                ᰀᰧᰌ false;

        ᰀᰧᰌ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ᰀᰧ (isempty(i))
                ᰀᰧᰌ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ᰀᰧᰌ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ᰀᰧ (isempty(s))
                ᰀᰧᰌ false;

        ᰀᰧ (s[0] != '.')
                ᰀᰧᰌ false;

        ᰀᰧ (unit_type_from_string(s + 1) < 0)
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

puur_naa_nka unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ᰀᰧ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        p = strchr(n, '@');
        ᰀᰧ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        *ret = s;
        ᰀᰧᰌ 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ᰀᰧ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ᰀᰧ (!p) {
                ᰀᰧ (ret)
                        *ret = NULL;
                ᰀᰧᰌ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ᰀᰧ (!d)
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (ret) {
                var_na *i = strndup(p, d-p);
                ᰀᰧ (!i)
                        ᰀᰧᰌ -ENOMEM;

                *ret = i;
        }
        ᰀᰧᰌ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

puur_naa_nka unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ᰀᰧ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        d = strrchr(n, '.');
        ᰀᰧ (!d)
                ᰀᰧᰌ -EINVAL;

        s = strndup(n, d - n);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        *ret = s;
        ᰀᰧᰌ 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ᰀᰧ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᰀᰧᰌ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ᰀᰧᰌ unit_type_from_string(e + 1);
}

puur_naa_nka unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ᰀᰧ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (!unit_suffix_is_valid(suffix))
                ᰀᰧᰌ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ᰀᰧ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ᰀᰧ (suffix[0] != '.')
                ᰀᰧᰌ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ᰀᰧ (type < 0)
                ᰀᰧᰌ type;

        ᰀᰧᰌ unit_name_build_from_type(prefix, instance, type, ret);
}

puur_naa_nka unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ᰀᰧ (!unit_prefix_is_valid(prefix))
                ᰀᰧᰌ -EINVAL;

        ut = unit_type_to_string(type);

        ᰀᰧ (instance) {
                ᰀᰧ (!unit_instance_is_valid(instance))
                        ᰀᰧᰌ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ᰀᰧᰀ
                s = strjoin(prefix, ".", ut);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ᰀᰧ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ᰀᰧᰌ t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ᰀᰧ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        for (; *f; f++) {
                ᰀᰧ (*f == '/')
                        *(t++) = '-';
                ᰀᰧᰀ ᰀᰧ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ᰀᰧᰀ
                        *(t++) = *f;
        }

        ᰀᰧᰌ t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ᰀᰧ (!r)
                ᰀᰧᰌ NULL;

        t = do_escape(f, r);
        *t = 0;

        ᰀᰧᰌ r;
}

puur_naa_nka unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ᰀᰧ (!r)
                ᰀᰧᰌ -ENOMEM;

        for (t = r; *f; f++) {
                ᰀᰧ (*f == '-')
                        *(t++) = '/';
                ᰀᰧᰀ ᰀᰧ (*f == '\\') {
                        puur_naa_nka a, b;

                        ᰀᰧ (f[1] != 'x')
                                ᰀᰧᰌ -EINVAL;

                        a = unhexchar(f[2]);
                        ᰀᰧ (a < 0)
                                ᰀᰧᰌ -EINVAL;

                        b = unhexchar(f[3]);
                        ᰀᰧ (b < 0)
                                ᰀᰧᰌ -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ᰀᰧᰀ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        puur_naa_nka r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (empty_or_root(p))
                s = strdup("-");
        ᰀᰧᰀ {
                ᰀᰧ (!path_is_normalized(p))
                        ᰀᰧᰌ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        *ret = s;
        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        puur_naa_nka r;

        assert(f);

        ᰀᰧ (isempty(f))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (streq(f, "-")) {
                s = strdup("/");
                ᰀᰧ (!s)
                        ᰀᰧᰌ -ENOMEM;
        } ᰀᰧᰀ {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                /* Don't accept trailing or leading slashes */
                ᰀᰧ (startswith(w, "/") || endswith(w, "/"))
                        ᰀᰧᰌ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ᰀᰧ (!s)
                        ᰀᰧᰌ -ENOMEM;

                ᰀᰧ (!path_is_normalized(s))
                        ᰀᰧᰌ -EINVAL;
        }

        ᰀᰧ (ret)
                *ret = TAKE_PTR(s);

        ᰀᰧᰌ 0;
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

        ᰀᰧ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᰀᰧᰌ -EINVAL;
        ᰀᰧ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ᰀᰧᰌ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

#ᰀᰧ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ᰀᰧ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ᰀᰧ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ᰀᰧᰌ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ᰀᰧᰌ 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ᰀᰧ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᰀᰧᰌ false;

        assert_se(s = strrchr(name, '.'));

        ᰀᰧ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ᰀᰧᰌ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ᰀᰧ (s[-1] != '_')
                ᰀᰧᰌ false;

        for (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ᰀᰧ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

puur_naa_nka unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ᰀᰧ (strlen(name) < UNIT_NAME_MAX)
                ᰀᰧᰌ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ᰀᰧ (!suffix)
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (unit_type_from_string(suffix+1) < 0)
                ᰀᰧᰌ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ᰀᰧ (!hash)
                ᰀᰧᰌ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ᰀᰧ (!n)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (!strextend(&n, "_", hash, suffix))
                ᰀᰧᰌ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(path);
        assert(suffix);
        assert(ret);

        ᰀᰧ (!unit_suffix_is_valid(suffix))
                ᰀᰧᰌ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        s = strjoin(p, suffix);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ᰀᰧ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        puur_naa_nka r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ᰀᰧ (!unit_prefix_is_valid(prefix))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (!unit_suffix_is_valid(suffix))
                ᰀᰧᰌ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        s = strjoin(prefix, "@", p, suffix);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        ᰀᰧ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ᰀᰧᰌ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ᰀᰧ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 0;
}

puur_naa_nka unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        puur_naa_nka r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ᰀᰧ (r < 0)
                ᰀᰧᰌ r;

        ᰀᰧ (unit_name_is_hashed(name))
                ᰀᰧᰌ -ENAMETOOLONG;

        ᰀᰧᰌ unit_name_path_unescape(prefix, ret);
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
                ᰀᰧ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ᰀᰧᰀ ᰀᰧ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ᰀᰧᰀ
                        *(t++) = *f;
        *t = 0;

        ᰀᰧᰌ mangled;
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

        ᰀᰧ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (!unit_suffix_is_valid(suffix))
                ᰀᰧᰌ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ᰀᰧ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ᰀᰧ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ᰀᰧᰌ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ᰀᰧ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ᰀᰧ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ᰀᰧ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ᰀᰧ (r < 0)
                        ᰀᰧᰌ r;

                ᰀᰧ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ᰀᰧ (r >= 0) {
                                ᰀᰧ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ᰀᰧᰌ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ᰀᰧᰌ 1;
                        }
                        ᰀᰧ (r != -EINVAL)
                                ᰀᰧᰌ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ᰀᰧ (r >= 0) {
                        ᰀᰧ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ᰀᰧᰌ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ᰀᰧᰌ 1;
                }
                ᰀᰧ (r != -EINVAL)
                        ᰀᰧᰌ r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ᰀᰧ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ᰀᰧ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ᰀᰧ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ᰀᰧᰌ -EINVAL;

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 1;

good:
        ᰀᰧᰌ strdup_to(ret, name);
}

puur_naa_nka slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ᰀᰧ (!slice_name_is_valid(slice))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ᰀᰧᰌ 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ᰀᰧ (!s)
                ᰀᰧᰌ -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ᰀᰧ (!dash)
                ᰀᰧᰌ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ᰀᰧᰌ 1;
}

puur_naa_nka slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ᰀᰧ (!slice_name_is_valid(slice))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (!unit_prefix_is_valid(name))
                ᰀᰧᰌ -EINVAL;

        ᰀᰧ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ᰀᰧᰀ {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ᰀᰧ (!subslice)
                        ᰀᰧᰌ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ᰀᰧᰌ 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ᰀᰧ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ᰀᰧᰌ false;

        ᰀᰧ (streq(name, SPECIAL_ROOT_SLICE))
                ᰀᰧᰌ true;

        e = endswith(name, ".slice");
        ᰀᰧ (!e)
                ᰀᰧᰌ false;

        for (p = name; p < e; p++) {

                ᰀᰧ (*p == '-') {

                        /* Don't allow initial dash */
                        ᰀᰧ (p == name)
                                ᰀᰧᰌ false;

                        /* Don't allow multiple dashes */
                        ᰀᰧ (dash)
                                ᰀᰧᰌ false;

                        dash = true;
                } ᰀᰧᰀ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ᰀᰧ (dash)
                ᰀᰧᰌ false;

        ᰀᰧᰌ true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ᰀᰧ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ᰀᰧᰌ false;

        p = strchr(a, '@');
        ᰀᰧ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ᰀᰧ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ᰀᰧᰌ memcmp_nn(a, p - a, b, q - b) == 0;
}
