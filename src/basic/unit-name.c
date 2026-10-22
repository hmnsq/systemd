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

        ຖ້າ (_unlikely_(flags == 0))
                ສົ່ງຄືນ false;

        ຖ້າ (isempty(n))
                ສົ່ງຄືນ false;

        ຖ້າ (strlen(n) >= UNIT_NAME_MAX)
                ສົ່ງຄືນ false;

        e = strrchr(n, '.');
        ຖ້າ (!e || e == n)
                ສົ່ງຄືນ false;

        ຖ້າ (unit_type_from_string(e + 1) < 0)
                ສົ່ງຄືນ false;

        ສຳລັບ (i = n, at = NULL; i < e; i++) {

                ຖ້າ (*i == '@' && !at)
                        at = i;

                ຖ້າ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ສົ່ງຄືນ false;
        }

        ຖ້າ (at == n)
                ສົ່ງຄືນ false;

        ຖ້າ (flags & UNIT_NAME_PLAIN)
                ຖ້າ (!at)
                        ສົ່ງຄືນ true;

        ຖ້າ (flags & UNIT_NAME_INSTANCE)
                ຖ້າ (at && e > at + 1)
                        ສົ່ງຄືນ true;

        ຖ້າ (flags & UNIT_NAME_TEMPLATE)
                ຖ້າ (at && e == at + 1)
                        ສົ່ງຄືນ true;

        ສົ່ງຄືນ false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ຖ້າ (isempty(p))
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ຖ້າ (isempty(i))
                ສົ່ງຄືນ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ສົ່ງຄືນ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ຖ້າ (isempty(s))
                ສົ່ງຄືນ false;

        ຖ້າ (s[0] != '.')
                ສົ່ງຄືນ false;

        ຖ້າ (unit_type_from_string(s + 1) < 0)
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ຖ້າ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        p = strchr(n, '@');
        ຖ້າ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        *ret = s;
        ສົ່ງຄືນ 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ຖ້າ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ຖ້າ (!p) {
                ຖ້າ (ret)
                        *ret = NULL;
                ສົ່ງຄືນ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ຖ້າ (!d)
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (ret) {
                var_na *i = strndup(p, d-p);
                ຖ້າ (!i)
                        ສົ່ງຄືນ -ENOMEM;

                *ret = i;
        }
        ສົ່ງຄືນ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ຈຳນວນເຕັມ unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ຖ້າ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        d = strrchr(n, '.');
        ຖ້າ (!d)
                ສົ່ງຄືນ -EINVAL;

        s = strndup(n, d - n);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        *ret = s;
        ສົ່ງຄືນ 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ຖ້າ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ສົ່ງຄືນ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ສົ່ງຄືນ unit_type_from_string(e + 1);
}

ຈຳນວນເຕັມ unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ຖ້າ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (!unit_suffix_is_valid(suffix))
                ສົ່ງຄືນ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ຖ້າ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ຖ້າ (suffix[0] != '.')
                ສົ່ງຄືນ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ຖ້າ (type < 0)
                ສົ່ງຄືນ type;

        ສົ່ງຄືນ unit_name_build_from_type(prefix, instance, type, ret);
}

ຈຳນວນເຕັມ unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ຖ້າ (!unit_prefix_is_valid(prefix))
                ສົ່ງຄືນ -EINVAL;

        ut = unit_type_to_string(type);

        ຖ້າ (instance) {
                ຖ້າ (!unit_instance_is_valid(instance))
                        ສົ່ງຄືນ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ບໍ່ດັ່ງນັ້ນ
                s = strjoin(prefix, ".", ut);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ຖ້າ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ສົ່ງຄືນ t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ຖ້າ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ສຳລັບ (; *f; f++) {
                ຖ້າ (*f == '/')
                        *(t++) = '-';
                ບໍ່ດັ່ງນັ້ນ ຖ້າ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ບໍ່ດັ່ງນັ້ນ
                        *(t++) = *f;
        }

        ສົ່ງຄືນ t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ຖ້າ (!r)
                ສົ່ງຄືນ NULL;

        t = do_escape(f, r);
        *t = 0;

        ສົ່ງຄືນ r;
}

ຈຳນວນເຕັມ unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ຖ້າ (!r)
                ສົ່ງຄືນ -ENOMEM;

        ສຳລັບ (t = r; *f; f++) {
                ຖ້າ (*f == '-')
                        *(t++) = '/';
                ບໍ່ດັ່ງນັ້ນ ຖ້າ (*f == '\\') {
                        ຈຳນວນເຕັມ a, b;

                        ຖ້າ (f[1] != 'x')
                                ສົ່ງຄືນ -EINVAL;

                        a = unhexchar(f[2]);
                        ຖ້າ (a < 0)
                                ສົ່ງຄືນ -EINVAL;

                        b = unhexchar(f[3]);
                        ຖ້າ (b < 0)
                                ສົ່ງຄືນ -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ບໍ່ດັ່ງນັ້ນ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        ຈຳນວນເຕັມ r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (empty_or_root(p))
                s = strdup("-");
        ບໍ່ດັ່ງນັ້ນ {
                ຖ້າ (!path_is_normalized(p))
                        ສົ່ງຄືນ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        *ret = s;
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(f);

        ຖ້າ (isempty(f))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (streq(f, "-")) {
                s = strdup("/");
                ຖ້າ (!s)
                        ສົ່ງຄືນ -ENOMEM;
        } ບໍ່ດັ່ງນັ້ນ {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                /* Don't accept trailing or leading slashes */
                ຖ້າ (startswith(w, "/") || endswith(w, "/"))
                        ສົ່ງຄືນ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ຖ້າ (!s)
                        ສົ່ງຄືນ -ENOMEM;

                ຖ້າ (!path_is_normalized(s))
                        ສົ່ງຄືນ -EINVAL;
        }

        ຖ້າ (ret)
                *ret = TAKE_PTR(s);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_replace_instance_full(
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

        ຖ້າ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ສົ່ງຄືນ -EINVAL;
        ຖ້າ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ສົ່ງຄືນ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

#ຖ້າ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ຖ້າ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ຖ້າ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ສົ່ງຄືນ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ສົ່ງຄືນ 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ຖ້າ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ສົ່ງຄືນ false;

        assert_se(s = strrchr(name, '.'));

        ຖ້າ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ສົ່ງຄືນ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ຖ້າ (s[-1] != '_')
                ສົ່ງຄືນ false;

        ສຳລັບ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ຖ້າ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

ຈຳນວນເຕັມ unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ຖ້າ (strlen(name) < UNIT_NAME_MAX)
                ສົ່ງຄືນ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ຖ້າ (!suffix)
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (unit_type_from_string(suffix+1) < 0)
                ສົ່ງຄືນ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ຖ້າ (!hash)
                ສົ່ງຄືນ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ຖ້າ (!n)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (!strextend(&n, "_", hash, suffix))
                ສົ່ງຄືນ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(path);
        assert(suffix);
        assert(ret);

        ຖ້າ (!unit_suffix_is_valid(suffix))
                ສົ່ງຄືນ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        s = strjoin(p, suffix);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ຖ້າ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ຈຳນວນເຕັມ r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ຖ້າ (!unit_prefix_is_valid(prefix))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (!unit_suffix_is_valid(suffix))
                ສົ່ງຄືນ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        s = strjoin(prefix, "@", p, suffix);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        ຖ້າ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ສົ່ງຄືນ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ຖ້າ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        ຈຳນວນເຕັມ r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ຖ້າ (r < 0)
                ສົ່ງຄືນ r;

        ຖ້າ (unit_name_is_hashed(name))
                ສົ່ງຄືນ -ENAMETOOLONG;

        ສົ່ງຄືນ unit_name_path_unescape(prefix, ret);
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

        ສຳລັບ (; *f; f++)
                ຖ້າ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ບໍ່ດັ່ງນັ້ນ ຖ້າ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ບໍ່ດັ່ງນັ້ນ
                        *(t++) = *f;
        *t = 0;

        ສົ່ງຄືນ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ຈຳນວນເຕັມ unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ຈຳນວນເຕັມ r;

        assert(name);
        assert(suffix);
        assert(ret);

        ຖ້າ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (!unit_suffix_is_valid(suffix))
                ສົ່ງຄືນ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ຖ້າ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ຖ້າ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ສົ່ງຄືນ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ຖ້າ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ຖ້າ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ຖ້າ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ຖ້າ (r < 0)
                        ສົ່ງຄືນ r;

                ຖ້າ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ຖ້າ (r >= 0) {
                                ຖ້າ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ສົ່ງຄືນ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ສົ່ງຄືນ 1;
                        }
                        ຖ້າ (r != -EINVAL)
                                ສົ່ງຄືນ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ຖ້າ (r >= 0) {
                        ຖ້າ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ສົ່ງຄືນ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ສົ່ງຄືນ 1;
                }
                ຖ້າ (r != -EINVAL)
                        ສົ່ງຄືນ r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ຖ້າ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ຖ້າ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ຖ້າ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ສົ່ງຄືນ -EINVAL;

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 1;

good:
        ສົ່ງຄືນ strdup_to(ret, name);
}

ຈຳນວນເຕັມ slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ຖ້າ (!slice_name_is_valid(slice))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ສົ່ງຄືນ 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ຖ້າ (!s)
                ສົ່ງຄືນ -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ຖ້າ (!dash)
                ສົ່ງຄືນ strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ສົ່ງຄືນ 1;
}

ຈຳນວນເຕັມ slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ຖ້າ (!slice_name_is_valid(slice))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (!unit_prefix_is_valid(name))
                ສົ່ງຄືນ -EINVAL;

        ຖ້າ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        ບໍ່ດັ່ງນັ້ນ {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ຖ້າ (!subslice)
                        ສົ່ງຄືນ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ສົ່ງຄືນ 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ຖ້າ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ສົ່ງຄືນ false;

        ຖ້າ (streq(name, SPECIAL_ROOT_SLICE))
                ສົ່ງຄືນ true;

        e = endswith(name, ".slice");
        ຖ້າ (!e)
                ສົ່ງຄືນ false;

        ສຳລັບ (p = name; p < e; p++) {

                ຖ້າ (*p == '-') {

                        /* Don't allow initial dash */
                        ຖ້າ (p == name)
                                ສົ່ງຄືນ false;

                        /* Don't allow multiple dashes */
                        ຖ້າ (dash)
                                ສົ່ງຄືນ false;

                        dash = true;
                } ບໍ່ດັ່ງນັ້ນ
                        dash = false;
        }

        /* Don't allow trailing hash */
        ຖ້າ (dash)
                ສົ່ງຄືນ false;

        ສົ່ງຄືນ true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ຖ້າ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ສົ່ງຄືນ false;

        p = strchr(a, '@');
        ຖ້າ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ຖ້າ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ສົ່ງຄືນ memcmp_nn(a, p - a, b, q - b) == 0;
}
