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

        якщо (_unlikely_(flags == 0))
                повернути false;

        якщо (isempty(n))
                повернути false;

        якщо (strlen(n) >= UNIT_NAME_MAX)
                повернути false;

        e = strrchr(n, '.');
        якщо (!e || e == n)
                повернути false;

        якщо (unit_type_from_string(e + 1) < 0)
                повернути false;

        для (i = n, at = NULL; i < e; i++) {

                якщо (*i == '@' && !at)
                        at = i;

                якщо (!strchr(VALID_CHARS_WITH_AT, *i))
                        повернути false;
        }

        якщо (at == n)
                повернути false;

        якщо (flags & UNIT_NAME_PLAIN)
                якщо (!at)
                        повернути true;

        якщо (flags & UNIT_NAME_INSTANCE)
                якщо (at && e > at + 1)
                        повернути true;

        якщо (flags & UNIT_NAME_TEMPLATE)
                якщо (at && e == at + 1)
                        повернути true;

        повернути false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        якщо (isempty(p))
                повернути false;

        повернути in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        якщо (isempty(i))
                повернути false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        повернути in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        якщо (isempty(s))
                повернути false;

        якщо (s[0] != '.')
                повернути false;

        якщо (unit_type_from_string(s + 1) < 0)
                повернути false;

        повернути true;
}

ціле unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        якщо (!unit_name_is_valid(n, UNIT_NAME_ANY))
                повернути -EINVAL;

        p = strchr(n, '@');
        якщо (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        якщо (!s)
                повернути -ENOMEM;

        *ret = s;
        повернути 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        якщо (!unit_name_is_valid(n, UNIT_NAME_ANY))
                повернути -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        якщо (!p) {
                якщо (ret)
                        *ret = NULL;
                повернути UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        якщо (!d)
                повернути -EINVAL;

        якщо (ret) {
                char *i = strndup(p, d-p);
                якщо (!i)
                        повернути -ENOMEM;

                *ret = i;
        }
        повернути d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ціле unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        якщо (!unit_name_is_valid(n, UNIT_NAME_ANY))
                повернути -EINVAL;

        d = strrchr(n, '.');
        якщо (!d)
                повернути -EINVAL;

        s = strndup(n, d - n);
        якщо (!s)
                повернути -ENOMEM;

        *ret = s;
        повернути 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        якщо (!unit_name_is_valid(n, UNIT_NAME_ANY))
                повернути _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        повернути unit_type_from_string(e + 1);
}

ціле unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        якщо (!unit_name_is_valid(n, UNIT_NAME_ANY))
                повернути -EINVAL;

        якщо (!unit_suffix_is_valid(suffix))
                повернути -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        якщо (!s)
                повернути -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        якщо (!unit_name_is_valid(s, UNIT_NAME_ANY))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        якщо (suffix[0] != '.')
                повернути -EINVAL;

        type = unit_type_from_string(suffix + 1);
        якщо (type < 0)
                повернути type;

        повернути unit_name_build_from_type(prefix, instance, type, ret);
}

ціле unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        якщо (!unit_prefix_is_valid(prefix))
                повернути -EINVAL;

        ut = unit_type_to_string(type);

        якщо (instance) {
                якщо (!unit_instance_is_valid(instance))
                        повернути -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } інакше
                s = strjoin(prefix, ".", ut);
        якщо (!s)
                повернути -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        якщо (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        повернути t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        якщо (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        для (; *f; f++) {
                якщо (*f == '/')
                        *(t++) = '-';
                інакше якщо (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                інакше
                        *(t++) = *f;
        }

        повернути t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        якщо (!r)
                повернути NULL;

        t = do_escape(f, r);
        *t = 0;

        повернути r;
}

ціле unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        якщо (!r)
                повернути -ENOMEM;

        для (t = r; *f; f++) {
                якщо (*f == '-')
                        *(t++) = '/';
                інакше якщо (*f == '\\') {
                        ціле a, b;

                        якщо (f[1] != 'x')
                                повернути -EINVAL;

                        a = unhexchar(f[2]);
                        якщо (a < 0)
                                повернути -EINVAL;

                        b = unhexchar(f[3]);
                        якщо (b < 0)
                                повернути -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } інакше
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        повернути 0;
}

ціле unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        ціле r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        якщо (r < 0)
                повернути r;

        якщо (empty_or_root(p))
                s = strdup("-");
        інакше {
                якщо (!path_is_normalized(p))
                        повернути -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        якщо (!s)
                повернути -ENOMEM;

        *ret = s;
        повернути 0;
}

ціле unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        ціле r;

        assert(f);

        якщо (isempty(f))
                повернути -EINVAL;

        якщо (streq(f, "-")) {
                s = strdup("/");
                якщо (!s)
                        повернути -ENOMEM;
        } інакше {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                якщо (r < 0)
                        повернути r;

                /* Don't accept trailing or leading slashes */
                якщо (startswith(w, "/") || endswith(w, "/"))
                        повернути -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                якщо (!s)
                        повернути -ENOMEM;

                якщо (!path_is_normalized(s))
                        повернути -EINVAL;
        }

        якщо (ret)
                *ret = TAKE_PTR(s);

        повернути 0;
}

ціле unit_name_replace_instance_full(
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

        якщо (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                повернути -EINVAL;
        якщо (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                повернути -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        якщо (!s)
                повернути -ENOMEM;

#якщо HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        якщо (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        якщо (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                повернути -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        якщо (!s)
                повернути -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        повернути 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        якщо (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                повернути false;

        assert_se(s = strrchr(name, '.'));

        якщо (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                повернути false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        якщо (s[-1] != '_')
                повернути false;

        для (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                якщо (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        повернути false;

        повернути true;
}

ціле unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        якщо (strlen(name) < UNIT_NAME_MAX)
                повернути -EMSGSIZE;

        suffix = strrchr(name, '.');
        якщо (!suffix)
                повернути -EINVAL;

        якщо (unit_type_from_string(suffix+1) < 0)
                повернути -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        якщо (!hash)
                повернути -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        якщо (!n)
                повернути -ENOMEM;

        якщо (!strextend(&n, "_", hash, suffix))
                повернути -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        повернути 0;
}

ціле unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        ціле r;

        assert(path);
        assert(suffix);
        assert(ret);

        якщо (!unit_suffix_is_valid(suffix))
                повернути -EINVAL;

        r = unit_name_path_escape(path, &p);
        якщо (r < 0)
                повернути r;

        s = strjoin(p, suffix);
        якщо (!s)
                повернути -ENOMEM;

        якщо (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                якщо (r < 0)
                        повернути r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        якщо (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        ціле r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        якщо (!unit_prefix_is_valid(prefix))
                повернути -EINVAL;

        якщо (!unit_suffix_is_valid(suffix))
                повернути -EINVAL;

        r = unit_name_path_escape(path, &p);
        якщо (r < 0)
                повернути r;

        s = strjoin(prefix, "@", p, suffix);
        якщо (!s)
                повернути -ENOMEM;

        якщо (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                повернути -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        якщо (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 0;
}

ціле unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        ціле r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        якщо (r < 0)
                повернути r;

        якщо (unit_name_is_hashed(name))
                повернути -ENAMETOOLONG;

        повернути unit_name_path_unescape(prefix, ret);
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
                якщо (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } інакше якщо (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } інакше
                        *(t++) = *f;
        *t = 0;

        повернути mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ціле unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ціле r;

        assert(name);
        assert(suffix);
        assert(ret);

        якщо (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                повернути -EINVAL;

        якщо (!unit_suffix_is_valid(suffix))
                повернути -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        якщо (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                якщо (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        повернути log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        якщо (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                якщо (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        якщо (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                якщо (r < 0)
                        повернути r;

                якщо (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        якщо (r >= 0) {
                                якщо (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        повернути log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                повернути 1;
                        }
                        якщо (r != -EINVAL)
                                повернути r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                якщо (r >= 0) {
                        якщо (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                повернути log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        повернути 1;
                }
                якщо (r != -EINVAL)
                        повернути r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        якщо (!s)
                повернути -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        якщо (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        якщо ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        якщо (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                повернути -EINVAL;

        *ret = TAKE_PTR(s);
        повернути 1;

good:
        повернути strdup_to(ret, name);
}

ціле slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        якщо (!slice_name_is_valid(slice))
                повернути -EINVAL;

        якщо (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                повернути 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        якщо (!s)
                повернути -ENOMEM;

        char *dash = strrchr(s, '-');
        якщо (!dash)
                повернути strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        повернути 1;
}

ціле slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        якщо (!slice_name_is_valid(slice))
                повернути -EINVAL;

        якщо (!unit_prefix_is_valid(name))
                повернути -EINVAL;

        якщо (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        інакше {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                якщо (!subslice)
                        повернути -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        повернути 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        якщо (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                повернути false;

        якщо (streq(name, SPECIAL_ROOT_SLICE))
                повернути true;

        e = endswith(name, ".slice");
        якщо (!e)
                повернути false;

        для (p = name; p < e; p++) {

                якщо (*p == '-') {

                        /* Don't allow initial dash */
                        якщо (p == name)
                                повернути false;

                        /* Don't allow multiple dashes */
                        якщо (dash)
                                повернути false;

                        dash = true;
                } інакше
                        dash = false;
        }

        /* Don't allow trailing hash */
        якщо (dash)
                повернути false;

        повернути true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        якщо (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                повернути false;

        p = strchr(a, '@');
        якщо (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        якщо (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        повернути memcmp_nn(a, p - a, b, q - b) == 0;
}
