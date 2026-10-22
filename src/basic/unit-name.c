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

        གལ་ཏེ (_unlikely_(flags == 0))
                ཕྱིར false;

        གལ་ཏེ (isempty(n))
                ཕྱིར false;

        གལ་ཏེ (strlen(n) >= UNIT_NAME_MAX)
                ཕྱིར false;

        e = strrchr(n, '.');
        གལ་ཏེ (!e || e == n)
                ཕྱིར false;

        གལ་ཏེ (unit_type_from_string(e + 1) < 0)
                ཕྱིར false;

        ཆེད་དུ (i = n, at = NULL; i < e; i++) {

                གལ་ཏེ (*i == '@' && !at)
                        at = i;

                གལ་ཏེ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ཕྱིར false;
        }

        གལ་ཏེ (at == n)
                ཕྱིར false;

        གལ་ཏེ (flags & UNIT_NAME_PLAIN)
                གལ་ཏེ (!at)
                        ཕྱིར true;

        གལ་ཏེ (flags & UNIT_NAME_INSTANCE)
                གལ་ཏེ (at && e > at + 1)
                        ཕྱིར true;

        གལ་ཏེ (flags & UNIT_NAME_TEMPLATE)
                གལ་ཏེ (at && e == at + 1)
                        ཕྱིར true;

        ཕྱིར false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        གལ་ཏེ (isempty(p))
                ཕྱིར false;

        ཕྱིར in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        གལ་ཏེ (isempty(i))
                ཕྱིར false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ཕྱིར in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        གལ་ཏེ (isempty(s))
                ཕྱིར false;

        གལ་ཏེ (s[0] != '.')
                ཕྱིར false;

        གལ་ཏེ (unit_type_from_string(s + 1) < 0)
                ཕྱིར false;

        ཕྱིར true;
}

ཧྲིལ unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        p = strchr(n, '@');
        གལ་ཏེ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        *ret = s;
        ཕྱིར 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        གལ་ཏེ (!p) {
                གལ་ཏེ (ret)
                        *ret = NULL;
                ཕྱིར UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        གལ་ཏེ (!d)
                ཕྱིར -EINVAL;

        གལ་ཏེ (ret) {
                var_na *i = strndup(p, d-p);
                གལ་ཏེ (!i)
                        ཕྱིར -ENOMEM;

                *ret = i;
        }
        ཕྱིར d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ཧྲིལ unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        d = strrchr(n, '.');
        གལ་ཏེ (!d)
                ཕྱིར -EINVAL;

        s = strndup(n, d - n);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        *ret = s;
        ཕྱིར 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ཕྱིར _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ཕྱིར unit_type_from_string(e + 1);
}

ཧྲིལ unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        གལ་ཏེ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        གལ་ཏེ (!unit_suffix_is_valid(suffix))
                ཕྱིར -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        གལ་ཏེ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        གལ་ཏེ (suffix[0] != '.')
                ཕྱིར -EINVAL;

        type = unit_type_from_string(suffix + 1);
        གལ་ཏེ (type < 0)
                ཕྱིར type;

        ཕྱིར unit_name_build_from_type(prefix, instance, type, ret);
}

ཧྲིལ unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        གལ་ཏེ (!unit_prefix_is_valid(prefix))
                ཕྱིར -EINVAL;

        ut = unit_type_to_string(type);

        གལ་ཏེ (instance) {
                གལ་ཏེ (!unit_instance_is_valid(instance))
                        ཕྱིར -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } དེ་མིན
                s = strjoin(prefix, ".", ut);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        གལ་ཏེ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ཕྱིར t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        གལ་ཏེ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ཆེད་དུ (; *f; f++) {
                གལ་ཏེ (*f == '/')
                        *(t++) = '-';
                དེ་མིན གལ་ཏེ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                དེ་མིན
                        *(t++) = *f;
        }

        ཕྱིར t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        གལ་ཏེ (!r)
                ཕྱིར NULL;

        t = do_escape(f, r);
        *t = 0;

        ཕྱིར r;
}

ཧྲིལ unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        གལ་ཏེ (!r)
                ཕྱིར -ENOMEM;

        ཆེད་དུ (t = r; *f; f++) {
                གལ་ཏེ (*f == '-')
                        *(t++) = '/';
                དེ་མིན གལ་ཏེ (*f == '\\') {
                        ཧྲིལ a, b;

                        གལ་ཏེ (f[1] != 'x')
                                ཕྱིར -EINVAL;

                        a = unhexchar(f[2]);
                        གལ་ཏེ (a < 0)
                                ཕྱིར -EINVAL;

                        b = unhexchar(f[3]);
                        གལ་ཏེ (b < 0)
                                ཕྱིར -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } དེ་མིན
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ཕྱིར 0;
}

ཧྲིལ unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        ཧྲིལ r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (empty_or_root(p))
                s = strdup("-");
        དེ་མིན {
                གལ་ཏེ (!path_is_normalized(p))
                        ཕྱིར -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        *ret = s;
        ཕྱིར 0;
}

ཧྲིལ unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ཧྲིལ r;

        assert(f);

        གལ་ཏེ (isempty(f))
                ཕྱིར -EINVAL;

        གལ་ཏེ (streq(f, "-")) {
                s = strdup("/");
                གལ་ཏེ (!s)
                        ཕྱིར -ENOMEM;
        } དེ་མིན {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                /* Don't accept trailing or leading slashes */
                གལ་ཏེ (startswith(w, "/") || endswith(w, "/"))
                        ཕྱིར -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                གལ་ཏེ (!s)
                        ཕྱིར -ENOMEM;

                གལ་ཏེ (!path_is_normalized(s))
                        ཕྱིར -EINVAL;
        }

        གལ་ཏེ (ret)
                *ret = TAKE_PTR(s);

        ཕྱིར 0;
}

ཧྲིལ unit_name_replace_instance_full(
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

        གལ་ཏེ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ཕྱིར -EINVAL;
        གལ་ཏེ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ཕྱིར -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

#གལ་ཏེ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        གལ་ཏེ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        གལ་ཏེ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ཕྱིར -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ཕྱིར 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        གལ་ཏེ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ཕྱིར false;

        assert_se(s = strrchr(name, '.'));

        གལ་ཏེ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ཕྱིར false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        གལ་ཏེ (s[-1] != '_')
                ཕྱིར false;

        ཆེད་དུ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                གལ་ཏེ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ཕྱིར false;

        ཕྱིར true;
}

ཧྲིལ unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        གལ་ཏེ (strlen(name) < UNIT_NAME_MAX)
                ཕྱིར -EMSGSIZE;

        suffix = strrchr(name, '.');
        གལ་ཏེ (!suffix)
                ཕྱིར -EINVAL;

        གལ་ཏེ (unit_type_from_string(suffix+1) < 0)
                ཕྱིར -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        གལ་ཏེ (!hash)
                ཕྱིར -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        གལ་ཏེ (!n)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (!strextend(&n, "_", hash, suffix))
                ཕྱིར -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ཕྱིར 0;
}

ཧྲིལ unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ཧྲིལ r;

        assert(path);
        assert(suffix);
        assert(ret);

        གལ་ཏེ (!unit_suffix_is_valid(suffix))
                ཕྱིར -EINVAL;

        r = unit_name_path_escape(path, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        s = strjoin(p, suffix);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        གལ་ཏེ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ཧྲིལ r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        གལ་ཏེ (!unit_prefix_is_valid(prefix))
                ཕྱིར -EINVAL;

        གལ་ཏེ (!unit_suffix_is_valid(suffix))
                ཕྱིར -EINVAL;

        r = unit_name_path_escape(path, &p);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        s = strjoin(prefix, "@", p, suffix);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        གལ་ཏེ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ཕྱིར -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        གལ་ཏེ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 0;
}

ཧྲིལ unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        ཧྲིལ r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        གལ་ཏེ (r < 0)
                ཕྱིར r;

        གལ་ཏེ (unit_name_is_hashed(name))
                ཕྱིར -ENAMETOOLONG;

        ཕྱིར unit_name_path_unescape(prefix, ret);
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

        ཆེད་དུ (; *f; f++)
                གལ་ཏེ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } དེ་མིན གལ་ཏེ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } དེ་མིན
                        *(t++) = *f;
        *t = 0;

        ཕྱིར mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ཧྲིལ unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ཧྲིལ r;

        assert(name);
        assert(suffix);
        assert(ret);

        གལ་ཏེ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ཕྱིར -EINVAL;

        གལ་ཏེ (!unit_suffix_is_valid(suffix))
                ཕྱིར -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        གལ་ཏེ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                གལ་ཏེ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ཕྱིར log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        གལ་ཏེ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                གལ་ཏེ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        གལ་ཏེ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                གལ་ཏེ (r < 0)
                        ཕྱིར r;

                གལ་ཏེ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        གལ་ཏེ (r >= 0) {
                                གལ་ཏེ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ཕྱིར log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ཕྱིར 1;
                        }
                        གལ་ཏེ (r != -EINVAL)
                                ཕྱིར r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                གལ་ཏེ (r >= 0) {
                        གལ་ཏེ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ཕྱིར log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ཕྱིར 1;
                }
                གལ་ཏེ (r != -EINVAL)
                        ཕྱིར r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        གལ་ཏེ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        གལ་ཏེ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        གལ་ཏེ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ཕྱིར -EINVAL;

        *ret = TAKE_PTR(s);
        ཕྱིར 1;

good:
        ཕྱིར strdup_to(ret, name);
}

ཧྲིལ slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        གལ་ཏེ (!slice_name_is_valid(slice))
                ཕྱིར -EINVAL;

        གལ་ཏེ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ཕྱིར 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        གལ་ཏེ (!s)
                ཕྱིར -ENOMEM;

        var_na *dash = strrchr(s, '-');
        གལ་ཏེ (!dash)
                ཕྱིར strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ཕྱིར 1;
}

ཧྲིལ slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        གལ་ཏེ (!slice_name_is_valid(slice))
                ཕྱིར -EINVAL;

        གལ་ཏེ (!unit_prefix_is_valid(name))
                ཕྱིར -EINVAL;

        གལ་ཏེ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        དེ་མིན {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                གལ་ཏེ (!subslice)
                        ཕྱིར -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ཕྱིར 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        གལ་ཏེ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ཕྱིར false;

        གལ་ཏེ (streq(name, SPECIAL_ROOT_SLICE))
                ཕྱིར true;

        e = endswith(name, ".slice");
        གལ་ཏེ (!e)
                ཕྱིར false;

        ཆེད་དུ (p = name; p < e; p++) {

                གལ་ཏེ (*p == '-') {

                        /* Don't allow initial dash */
                        གལ་ཏེ (p == name)
                                ཕྱིར false;

                        /* Don't allow multiple dashes */
                        གལ་ཏེ (dash)
                                ཕྱིར false;

                        dash = true;
                } དེ་མིན
                        dash = false;
        }

        /* Don't allow trailing hash */
        གལ་ཏེ (dash)
                ཕྱིར false;

        ཕྱིར true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        གལ་ཏེ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ཕྱིར false;

        p = strchr(a, '@');
        གལ་ཏེ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        གལ་ཏེ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ཕྱིར memcmp_nn(a, p - a, b, q - b) == 0;
}
