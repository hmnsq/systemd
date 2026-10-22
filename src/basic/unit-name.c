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

bool unit_name_is_valid(const char *n, UnitNameFlags flags) {
        const char *e, *i, *at;

        assert((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        если (_unlikely_(flags == 0))
                вернуть false;

        если (isempty(n))
                вернуть false;

        если (strlen(n) >= UNIT_NAME_MAX)
                вернуть false;

        e = strrchr(n, '.');
        если (!e || e == n)
                вернуть false;

        если (unit_type_from_string(e + 1) < 0)
                вернуть false;

        для (i = n, at = NULL; i < e; i++) {

                если (*i == '@' && !at)
                        at = i;

                если (!strchr(VALID_CHARS_WITH_AT, *i))
                        вернуть false;
        }

        если (at == n)
                вернуть false;

        если (flags & UNIT_NAME_PLAIN)
                если (!at)
                        вернуть true;

        если (flags & UNIT_NAME_INSTANCE)
                если (at && e > at + 1)
                        вернуть true;

        если (flags & UNIT_NAME_TEMPLATE)
                если (at && e == at + 1)
                        вернуть true;

        вернуть false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        если (isempty(p))
                вернуть false;

        вернуть in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        если (isempty(i))
                вернуть false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        вернуть in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        если (isempty(s))
                вернуть false;

        если (s[0] != '.')
                вернуть false;

        если (unit_type_from_string(s + 1) < 0)
                вернуть false;

        вернуть true;
}

целое unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        если (!unit_name_is_valid(n, UNIT_NAME_ANY))
                вернуть -EINVAL;

        p = strchr(n, '@');
        если (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        если (!s)
                вернуть -ENOMEM;

        *ret = s;
        вернуть 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        если (!unit_name_is_valid(n, UNIT_NAME_ANY))
                вернуть -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        если (!p) {
                если (ret)
                        *ret = NULL;
                вернуть UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        если (!d)
                вернуть -EINVAL;

        если (ret) {
                char *i = strndup(p, d-p);
                если (!i)
                        вернуть -ENOMEM;

                *ret = i;
        }
        вернуть d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

целое unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        если (!unit_name_is_valid(n, UNIT_NAME_ANY))
                вернуть -EINVAL;

        d = strrchr(n, '.');
        если (!d)
                вернуть -EINVAL;

        s = strndup(n, d - n);
        если (!s)
                вернуть -ENOMEM;

        *ret = s;
        вернуть 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        если (!unit_name_is_valid(n, UNIT_NAME_ANY))
                вернуть _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        вернуть unit_type_from_string(e + 1);
}

целое unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        если (!unit_name_is_valid(n, UNIT_NAME_ANY))
                вернуть -EINVAL;

        если (!unit_suffix_is_valid(suffix))
                вернуть -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        если (!s)
                вернуть -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        если (!unit_name_is_valid(s, UNIT_NAME_ANY))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        если (suffix[0] != '.')
                вернуть -EINVAL;

        type = unit_type_from_string(suffix + 1);
        если (type < 0)
                вернуть type;

        вернуть unit_name_build_from_type(prefix, instance, type, ret);
}

целое unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        если (!unit_prefix_is_valid(prefix))
                вернуть -EINVAL;

        ut = unit_type_to_string(type);

        если (instance) {
                если (!unit_instance_is_valid(instance))
                        вернуть -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } иначе
                s = strjoin(prefix, ".", ut);
        если (!s)
                вернуть -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        если (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        вернуть t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        если (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        для (; *f; f++) {
                если (*f == '/')
                        *(t++) = '-';
                иначе если (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                иначе
                        *(t++) = *f;
        }

        вернуть t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        если (!r)
                вернуть NULL;

        t = do_escape(f, r);
        *t = 0;

        вернуть r;
}

целое unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        если (!r)
                вернуть -ENOMEM;

        для (t = r; *f; f++) {
                если (*f == '-')
                        *(t++) = '/';
                иначе если (*f == '\\') {
                        целое a, b;

                        если (f[1] != 'x')
                                вернуть -EINVAL;

                        a = unhexchar(f[2]);
                        если (a < 0)
                                вернуть -EINVAL;

                        b = unhexchar(f[3]);
                        если (b < 0)
                                вернуть -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } иначе
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        вернуть 0;
}

целое unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        целое r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        если (r < 0)
                вернуть r;

        если (empty_or_root(p))
                s = strdup("-");
        иначе {
                если (!path_is_normalized(p))
                        вернуть -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        если (!s)
                вернуть -ENOMEM;

        *ret = s;
        вернуть 0;
}

целое unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        целое r;

        assert(f);

        если (isempty(f))
                вернуть -EINVAL;

        если (streq(f, "-")) {
                s = strdup("/");
                если (!s)
                        вернуть -ENOMEM;
        } иначе {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                если (r < 0)
                        вернуть r;

                /* Don't accept trailing or leading slashes */
                если (startswith(w, "/") || endswith(w, "/"))
                        вернуть -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                если (!s)
                        вернуть -ENOMEM;

                если (!path_is_normalized(s))
                        вернуть -EINVAL;
        }

        если (ret)
                *ret = TAKE_PTR(s);

        вернуть 0;
}

целое unit_name_replace_instance_full(
                const char *original,
                const char *instance,
                bool accept_glob,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        const char *prefix, *suffix;
        size_t pl;

        assert(original);
        assert(instance);
        assert(ret);

        если (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                вернуть -EINVAL;
        если (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                вернуть -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        если (!s)
                вернуть -ENOMEM;

#если HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        если (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        если (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                вернуть -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        если (!s)
                вернуть -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        вернуть 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        если (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                вернуть false;

        assert_se(s = strrchr(name, '.'));

        если (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                вернуть false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        если (s[-1] != '_')
                вернуть false;

        для (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                если (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        вернуть false;

        вернуть true;
}

целое unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        если (strlen(name) < UNIT_NAME_MAX)
                вернуть -EMSGSIZE;

        suffix = strrchr(name, '.');
        если (!suffix)
                вернуть -EINVAL;

        если (unit_type_from_string(suffix+1) < 0)
                вернуть -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        если (!hash)
                вернуть -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        если (!n)
                вернуть -ENOMEM;

        если (!strextend(&n, "_", hash, suffix))
                вернуть -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        вернуть 0;
}

целое unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        целое r;

        assert(path);
        assert(suffix);
        assert(ret);

        если (!unit_suffix_is_valid(suffix))
                вернуть -EINVAL;

        r = unit_name_path_escape(path, &p);
        если (r < 0)
                вернуть r;

        s = strjoin(p, suffix);
        если (!s)
                вернуть -ENOMEM;

        если (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                если (r < 0)
                        вернуть r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        если (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        целое r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        если (!unit_prefix_is_valid(prefix))
                вернуть -EINVAL;

        если (!unit_suffix_is_valid(suffix))
                вернуть -EINVAL;

        r = unit_name_path_escape(path, &p);
        если (r < 0)
                вернуть r;

        s = strjoin(prefix, "@", p, suffix);
        если (!s)
                вернуть -ENOMEM;

        если (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                вернуть -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        если (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 0;
}

целое unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        целое r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        если (r < 0)
                вернуть r;

        если (unit_name_is_hashed(name))
                вернуть -ENAMETOOLONG;

        вернуть unit_name_path_unescape(prefix, ret);
}

static bool do_escape_mangle(const char *f, bool allow_globs, char *t) {
        const char *valid_chars;
        bool mangled = false;

        assert(f);
        assert(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        для (; *f; f++)
                если (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } иначе если (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } иначе
                        *(t++) = *f;
        *t = 0;

        вернуть mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
целое unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        целое r;

        assert(name);
        assert(suffix);
        assert(ret);

        если (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                вернуть -EINVAL;

        если (!unit_suffix_is_valid(suffix))
                вернуть -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        если (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                если (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        вернуть log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        если (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                если (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        если (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                если (r < 0)
                        вернуть r;

                если (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        если (r >= 0) {
                                если (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        вернуть log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                вернуть 1;
                        }
                        если (r != -EINVAL)
                                вернуть r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                если (r >= 0) {
                        если (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                вернуть log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        вернуть 1;
                }
                если (r != -EINVAL)
                        вернуть r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        если (!s)
                вернуть -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        если (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        если ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        если (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                вернуть -EINVAL;

        *ret = TAKE_PTR(s);
        вернуть 1;

good:
        вернуть strdup_to(ret, name);
}

целое slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        если (!slice_name_is_valid(slice))
                вернуть -EINVAL;

        если (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                вернуть 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        если (!s)
                вернуть -ENOMEM;

        char *dash = strrchr(s, '-');
        если (!dash)
                вернуть strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        вернуть 1;
}

целое slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        если (!slice_name_is_valid(slice))
                вернуть -EINVAL;

        если (!unit_prefix_is_valid(name))
                вернуть -EINVAL;

        если (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        иначе {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                если (!subslice)
                        вернуть -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        вернуть 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        если (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                вернуть false;

        если (streq(name, SPECIAL_ROOT_SLICE))
                вернуть true;

        e = endswith(name, ".slice");
        если (!e)
                вернуть false;

        для (p = name; p < e; p++) {

                если (*p == '-') {

                        /* Don't allow initial dash */
                        если (p == name)
                                вернуть false;

                        /* Don't allow multiple dashes */
                        если (dash)
                                вернуть false;

                        dash = true;
                } иначе
                        dash = false;
        }

        /* Don't allow trailing hash */
        если (dash)
                вернуть false;

        вернуть true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        если (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                вернуть false;

        p = strchr(a, '@');
        если (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        если (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        вернуть memcmp_nn(a, p - a, b, q - b) == 0;
}
