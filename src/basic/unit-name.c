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

        အကယ်၍ (_unlikely_(flags == 0))
                ပြန်ပေး false;

        အကယ်၍ (isempty(n))
                ပြန်ပေး false;

        အကယ်၍ (strlen(n) >= UNIT_NAME_MAX)
                ပြန်ပေး false;

        e = strrchr(n, '.');
        အကယ်၍ (!e || e == n)
                ပြန်ပေး false;

        အကယ်၍ (unit_type_from_string(e + 1) < 0)
                ပြန်ပေး false;

        အတွက် (i = n, at = NULL; i < e; i++) {

                အကယ်၍ (*i == '@' && !at)
                        at = i;

                အကယ်၍ (!strchr(VALID_CHARS_WITH_AT, *i))
                        ပြန်ပေး false;
        }

        အကယ်၍ (at == n)
                ပြန်ပေး false;

        အကယ်၍ (flags & UNIT_NAME_PLAIN)
                အကယ်၍ (!at)
                        ပြန်ပေး true;

        အကယ်၍ (flags & UNIT_NAME_INSTANCE)
                အကယ်၍ (at && e > at + 1)
                        ပြန်ပေး true;

        အကယ်၍ (flags & UNIT_NAME_TEMPLATE)
                အကယ်၍ (at && e == at + 1)
                        ပြန်ပေး true;

        ပြန်ပေး false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        အကယ်၍ (isempty(p))
                ပြန်ပေး false;

        ပြန်ပေး in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        အကယ်၍ (isempty(i))
                ပြန်ပေး false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ပြန်ပေး in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        အကယ်၍ (isempty(s))
                ပြန်ပေး false;

        အကယ်၍ (s[0] != '.')
                ပြန်ပေး false;

        အကယ်၍ (unit_type_from_string(s + 1) < 0)
                ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        အကယ်၍ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        p = strchr(n, '@');
        အကယ်၍ (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        *ret = s;
        ပြန်ပေး 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        အကယ်၍ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        အကယ်၍ (!p) {
                အကယ်၍ (ret)
                        *ret = NULL;
                ပြန်ပေး UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        အကယ်၍ (!d)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (ret) {
                var_na *i = strndup(p, d-p);
                အကယ်၍ (!i)
                        ပြန်ပေး -ENOMEM;

                *ret = i;
        }
        ပြန်ပေး d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ကိန်းပြည့် unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        အကယ်၍ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        d = strrchr(n, '.');
        အကယ်၍ (!d)
                ပြန်ပေး -EINVAL;

        s = strndup(n, d - n);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        *ret = s;
        ပြန်ပေး 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        အကယ်၍ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ပြန်ပေး _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ပြန်ပေး unit_type_from_string(e + 1);
}

ကိန်းပြည့် unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        အကယ်၍ (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!unit_suffix_is_valid(suffix))
                ပြန်ပေး -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        အကယ်၍ (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        အကယ်၍ (suffix[0] != '.')
                ပြန်ပေး -EINVAL;

        type = unit_type_from_string(suffix + 1);
        အကယ်၍ (type < 0)
                ပြန်ပေး type;

        ပြန်ပေး unit_name_build_from_type(prefix, instance, type, ret);
}

ကိန်းပြည့် unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        အကယ်၍ (!unit_prefix_is_valid(prefix))
                ပြန်ပေး -EINVAL;

        ut = unit_type_to_string(type);

        အကယ်၍ (instance) {
                အကယ်၍ (!unit_instance_is_valid(instance))
                        ပြန်ပေး -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } မဟုတ်ပါက
                s = strjoin(prefix, ".", ut);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        အကယ်၍ (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ပြန်ပေး t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        အကယ်၍ (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        အတွက် (; *f; f++) {
                အကယ်၍ (*f == '/')
                        *(t++) = '-';
                မဟုတ်ပါက အကယ်၍ (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                မဟုတ်ပါက
                        *(t++) = *f;
        }

        ပြန်ပေး t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        အကယ်၍ (!r)
                ပြန်ပေး NULL;

        t = do_escape(f, r);
        *t = 0;

        ပြန်ပေး r;
}

ကိန်းပြည့် unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        အကယ်၍ (!r)
                ပြန်ပေး -ENOMEM;

        အတွက် (t = r; *f; f++) {
                အကယ်၍ (*f == '-')
                        *(t++) = '/';
                မဟုတ်ပါက အကယ်၍ (*f == '\\') {
                        ကိန်းပြည့် a, b;

                        အကယ်၍ (f[1] != 'x')
                                ပြန်ပေး -EINVAL;

                        a = unhexchar(f[2]);
                        အကယ်၍ (a < 0)
                                ပြန်ပေး -EINVAL;

                        b = unhexchar(f[3]);
                        အကယ်၍ (b < 0)
                                ပြန်ပေး -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } မဟုတ်ပါက
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        ကိန်းပြည့် r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (empty_or_root(p))
                s = strdup("-");
        မဟုတ်ပါက {
                အကယ်၍ (!path_is_normalized(p))
                        ပြန်ပေး -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        *ret = s;
        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        ကိန်းပြည့် r;

        assert(f);

        အကယ်၍ (isempty(f))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (streq(f, "-")) {
                s = strdup("/");
                အကယ်၍ (!s)
                        ပြန်ပေး -ENOMEM;
        } မဟုတ်ပါက {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                /* Don't accept trailing or leading slashes */
                အကယ်၍ (startswith(w, "/") || endswith(w, "/"))
                        ပြန်ပေး -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                အကယ်၍ (!s)
                        ပြန်ပေး -ENOMEM;

                အကယ်၍ (!path_is_normalized(s))
                        ပြန်ပေး -EINVAL;
        }

        အကယ်၍ (ret)
                *ret = TAKE_PTR(s);

        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_replace_instance_full(
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

        အကယ်၍ (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ပြန်ပေး -EINVAL;
        အကယ်၍ (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ပြန်ပေး -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

#အကယ်၍ HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        အကယ်၍ (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        အကယ်၍ (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ပြန်ပေး -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ပြန်ပေး 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        အကယ်၍ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ပြန်ပေး false;

        assert_se(s = strrchr(name, '.'));

        အကယ်၍ (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ပြန်ပေး false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        အကယ်၍ (s[-1] != '_')
                ပြန်ပေး false;

        အတွက် (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                အကယ်၍ (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ပြန်ပေး false;

        ပြန်ပေး true;
}

ကိန်းပြည့် unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        အကယ်၍ (strlen(name) < UNIT_NAME_MAX)
                ပြန်ပေး -EMSGSIZE;

        suffix = strrchr(name, '.');
        အကယ်၍ (!suffix)
                ပြန်ပေး -EINVAL;

        အကယ်၍ (unit_type_from_string(suffix+1) < 0)
                ပြန်ပေး -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        အကယ်၍ (!hash)
                ပြန်ပေး -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        အကယ်၍ (!n)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (!strextend(&n, "_", hash, suffix))
                ပြန်ပေး -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ကိန်းပြည့် r;

        assert(path);
        assert(suffix);
        assert(ret);

        အကယ်၍ (!unit_suffix_is_valid(suffix))
                ပြန်ပေး -EINVAL;

        r = unit_name_path_escape(path, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        s = strjoin(p, suffix);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        အကယ်၍ (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        ကိန်းပြည့် r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        အကယ်၍ (!unit_prefix_is_valid(prefix))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!unit_suffix_is_valid(suffix))
                ပြန်ပေး -EINVAL;

        r = unit_name_path_escape(path, &p);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        s = strjoin(prefix, "@", p, suffix);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        အကယ်၍ (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ပြန်ပေး -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        အကယ်၍ (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 0;
}

ကိန်းပြည့် unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        ကိန်းပြည့် r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        အကယ်၍ (r < 0)
                ပြန်ပေး r;

        အကယ်၍ (unit_name_is_hashed(name))
                ပြန်ပေး -ENAMETOOLONG;

        ပြန်ပေး unit_name_path_unescape(prefix, ret);
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

        အတွက် (; *f; f++)
                အကယ်၍ (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } မဟုတ်ပါက အကယ်၍ (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } မဟုတ်ပါက
                        *(t++) = *f;
        *t = 0;

        ပြန်ပေး mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ကိန်းပြည့် unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ကိန်းပြည့် r;

        assert(name);
        assert(suffix);
        assert(ret);

        အကယ်၍ (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!unit_suffix_is_valid(suffix))
                ပြန်ပေး -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        အကယ်၍ (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                အကယ်၍ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        ပြန်ပေး log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        အကယ်၍ (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                အကယ်၍ (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        အကယ်၍ (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                အကယ်၍ (r < 0)
                        ပြန်ပေး r;

                အကယ်၍ (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        အကယ်၍ (r >= 0) {
                                အကယ်၍ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ပြန်ပေး log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ပြန်ပေး 1;
                        }
                        အကယ်၍ (r != -EINVAL)
                                ပြန်ပေး r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                အကယ်၍ (r >= 0) {
                        အကယ်၍ (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ပြန်ပေး log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ပြန်ပေး 1;
                }
                အကယ်၍ (r != -EINVAL)
                        ပြန်ပေး r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        အကယ်၍ (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        အကယ်၍ ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        အကယ်၍ (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ပြန်ပေး -EINVAL;

        *ret = TAKE_PTR(s);
        ပြန်ပေး 1;

good:
        ပြန်ပေး strdup_to(ret, name);
}

ကိန်းပြည့် slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        အကယ်၍ (!slice_name_is_valid(slice))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                ပြန်ပေး 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        အကယ်၍ (!s)
                ပြန်ပေး -ENOMEM;

        var_na *dash = strrchr(s, '-');
        အကယ်၍ (!dash)
                ပြန်ပေး strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ပြန်ပေး 1;
}

ကိန်းပြည့် slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        အကယ်၍ (!slice_name_is_valid(slice))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (!unit_prefix_is_valid(name))
                ပြန်ပေး -EINVAL;

        အကယ်၍ (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        မဟုတ်ပါက {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                အကယ်၍ (!subslice)
                        ပြန်ပေး -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ပြန်ပေး 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        အကယ်၍ (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ပြန်ပေး false;

        အကယ်၍ (streq(name, SPECIAL_ROOT_SLICE))
                ပြန်ပေး true;

        e = endswith(name, ".slice");
        အကယ်၍ (!e)
                ပြန်ပေး false;

        အတွက် (p = name; p < e; p++) {

                အကယ်၍ (*p == '-') {

                        /* Don't allow initial dash */
                        အကယ်၍ (p == name)
                                ပြန်ပေး false;

                        /* Don't allow multiple dashes */
                        အကယ်၍ (dash)
                                ပြန်ပေး false;

                        dash = true;
                } မဟုတ်ပါက
                        dash = false;
        }

        /* Don't allow trailing hash */
        အကယ်၍ (dash)
                ပြန်ပေး false;

        ပြန်ပေး true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        အကယ်၍ (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ပြန်ပေး false;

        p = strchr(a, '@');
        အကယ်၍ (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        အကယ်၍ (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        ပြန်ပေး memcmp_nn(a, p - a, b, q - b) == 0;
}
