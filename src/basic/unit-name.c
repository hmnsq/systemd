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

        만약 (_unlikely_(flags == 0))
                반환 false;

        만약 (isempty(n))
                반환 false;

        만약 (strlen(n) >= UNIT_NAME_MAX)
                반환 false;

        e = strrchr(n, '.');
        만약 (!e || e == n)
                반환 false;

        만약 (unit_type_from_string(e + 1) < 0)
                반환 false;

        위해 (i = n, at = NULL; i < e; i++) {

                만약 (*i == '@' && !at)
                        at = i;

                만약 (!strchr(VALID_CHARS_WITH_AT, *i))
                        반환 false;
        }

        만약 (at == n)
                반환 false;

        만약 (flags & UNIT_NAME_PLAIN)
                만약 (!at)
                        반환 true;

        만약 (flags & UNIT_NAME_INSTANCE)
                만약 (at && e > at + 1)
                        반환 true;

        만약 (flags & UNIT_NAME_TEMPLATE)
                만약 (at && e == at + 1)
                        반환 true;

        반환 false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        만약 (isempty(p))
                반환 false;

        반환 in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        만약 (isempty(i))
                반환 false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        반환 in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        만약 (isempty(s))
                반환 false;

        만약 (s[0] != '.')
                반환 false;

        만약 (unit_type_from_string(s + 1) < 0)
                반환 false;

        반환 true;
}

정수 unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        만약 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                반환 -EINVAL;

        p = strchr(n, '@');
        만약 (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        만약 (!s)
                반환 -ENOMEM;

        *ret = s;
        반환 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        만약 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                반환 -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        만약 (!p) {
                만약 (ret)
                        *ret = NULL;
                반환 UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        만약 (!d)
                반환 -EINVAL;

        만약 (ret) {
                char *i = strndup(p, d-p);
                만약 (!i)
                        반환 -ENOMEM;

                *ret = i;
        }
        반환 d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

정수 unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        만약 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                반환 -EINVAL;

        d = strrchr(n, '.');
        만약 (!d)
                반환 -EINVAL;

        s = strndup(n, d - n);
        만약 (!s)
                반환 -ENOMEM;

        *ret = s;
        반환 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        만약 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                반환 _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        반환 unit_type_from_string(e + 1);
}

정수 unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        만약 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                반환 -EINVAL;

        만약 (!unit_suffix_is_valid(suffix))
                반환 -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        만약 (!s)
                반환 -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        만약 (!unit_name_is_valid(s, UNIT_NAME_ANY))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 0;
}

정수 unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        만약 (suffix[0] != '.')
                반환 -EINVAL;

        type = unit_type_from_string(suffix + 1);
        만약 (type < 0)
                반환 type;

        반환 unit_name_build_from_type(prefix, instance, type, ret);
}

정수 unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        만약 (!unit_prefix_is_valid(prefix))
                반환 -EINVAL;

        ut = unit_type_to_string(type);

        만약 (instance) {
                만약 (!unit_instance_is_valid(instance))
                        반환 -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } 아니면
                s = strjoin(prefix, ".", ut);
        만약 (!s)
                반환 -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        만약 (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        반환 t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        만약 (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        위해 (; *f; f++) {
                만약 (*f == '/')
                        *(t++) = '-';
                아니면 만약 (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                아니면
                        *(t++) = *f;
        }

        반환 t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        만약 (!r)
                반환 NULL;

        t = do_escape(f, r);
        *t = 0;

        반환 r;
}

정수 unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        만약 (!r)
                반환 -ENOMEM;

        위해 (t = r; *f; f++) {
                만약 (*f == '-')
                        *(t++) = '/';
                아니면 만약 (*f == '\\') {
                        정수 a, b;

                        만약 (f[1] != 'x')
                                반환 -EINVAL;

                        a = unhexchar(f[2]);
                        만약 (a < 0)
                                반환 -EINVAL;

                        b = unhexchar(f[3]);
                        만약 (b < 0)
                                반환 -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } 아니면
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        반환 0;
}

정수 unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        정수 r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        만약 (r < 0)
                반환 r;

        만약 (empty_or_root(p))
                s = strdup("-");
        아니면 {
                만약 (!path_is_normalized(p))
                        반환 -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        만약 (!s)
                반환 -ENOMEM;

        *ret = s;
        반환 0;
}

정수 unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        정수 r;

        assert(f);

        만약 (isempty(f))
                반환 -EINVAL;

        만약 (streq(f, "-")) {
                s = strdup("/");
                만약 (!s)
                        반환 -ENOMEM;
        } 아니면 {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                만약 (r < 0)
                        반환 r;

                /* Don't accept trailing or leading slashes */
                만약 (startswith(w, "/") || endswith(w, "/"))
                        반환 -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                만약 (!s)
                        반환 -ENOMEM;

                만약 (!path_is_normalized(s))
                        반환 -EINVAL;
        }

        만약 (ret)
                *ret = TAKE_PTR(s);

        반환 0;
}

정수 unit_name_replace_instance_full(
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

        만약 (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                반환 -EINVAL;
        만약 (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                반환 -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        만약 (!s)
                반환 -ENOMEM;

#만약 HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        만약 (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 0;
}

정수 unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        만약 (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                반환 -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        만약 (!s)
                반환 -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        반환 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        만약 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                반환 false;

        assert_se(s = strrchr(name, '.'));

        만약 (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                반환 false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        만약 (s[-1] != '_')
                반환 false;

        위해 (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                만약 (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        반환 false;

        반환 true;
}

정수 unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        만약 (strlen(name) < UNIT_NAME_MAX)
                반환 -EMSGSIZE;

        suffix = strrchr(name, '.');
        만약 (!suffix)
                반환 -EINVAL;

        만약 (unit_type_from_string(suffix+1) < 0)
                반환 -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        만약 (!hash)
                반환 -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        만약 (!n)
                반환 -ENOMEM;

        만약 (!strextend(&n, "_", hash, suffix))
                반환 -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        반환 0;
}

정수 unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        정수 r;

        assert(path);
        assert(suffix);
        assert(ret);

        만약 (!unit_suffix_is_valid(suffix))
                반환 -EINVAL;

        r = unit_name_path_escape(path, &p);
        만약 (r < 0)
                반환 r;

        s = strjoin(p, suffix);
        만약 (!s)
                반환 -ENOMEM;

        만약 (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                만약 (r < 0)
                        반환 r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        만약 (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 0;
}

정수 unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        정수 r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        만약 (!unit_prefix_is_valid(prefix))
                반환 -EINVAL;

        만약 (!unit_suffix_is_valid(suffix))
                반환 -EINVAL;

        r = unit_name_path_escape(path, &p);
        만약 (r < 0)
                반환 r;

        s = strjoin(prefix, "@", p, suffix);
        만약 (!s)
                반환 -ENOMEM;

        만약 (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                반환 -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        만약 (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 0;
}

정수 unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        정수 r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        만약 (r < 0)
                반환 r;

        만약 (unit_name_is_hashed(name))
                반환 -ENAMETOOLONG;

        반환 unit_name_path_unescape(prefix, ret);
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

        위해 (; *f; f++)
                만약 (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } 아니면 만약 (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } 아니면
                        *(t++) = *f;
        *t = 0;

        반환 mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
정수 unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        정수 r;

        assert(name);
        assert(suffix);
        assert(ret);

        만약 (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                반환 -EINVAL;

        만약 (!unit_suffix_is_valid(suffix))
                반환 -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        만약 (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                만약 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        반환 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        만약 (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                만약 (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        만약 (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                만약 (r < 0)
                        반환 r;

                만약 (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        만약 (r >= 0) {
                                만약 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        반환 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                반환 1;
                        }
                        만약 (r != -EINVAL)
                                반환 r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                만약 (r >= 0) {
                        만약 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                반환 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        반환 1;
                }
                만약 (r != -EINVAL)
                        반환 r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        만약 (!s)
                반환 -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        만약 (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        만약 ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        만약 (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                반환 -EINVAL;

        *ret = TAKE_PTR(s);
        반환 1;

good:
        반환 strdup_to(ret, name);
}

정수 slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        만약 (!slice_name_is_valid(slice))
                반환 -EINVAL;

        만약 (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                반환 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        만약 (!s)
                반환 -ENOMEM;

        char *dash = strrchr(s, '-');
        만약 (!dash)
                반환 strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        반환 1;
}

정수 slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        만약 (!slice_name_is_valid(slice))
                반환 -EINVAL;

        만약 (!unit_prefix_is_valid(name))
                반환 -EINVAL;

        만약 (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        아니면 {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                만약 (!subslice)
                        반환 -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        반환 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        만약 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                반환 false;

        만약 (streq(name, SPECIAL_ROOT_SLICE))
                반환 true;

        e = endswith(name, ".slice");
        만약 (!e)
                반환 false;

        위해 (p = name; p < e; p++) {

                만약 (*p == '-') {

                        /* Don't allow initial dash */
                        만약 (p == name)
                                반환 false;

                        /* Don't allow multiple dashes */
                        만약 (dash)
                                반환 false;

                        dash = true;
                } 아니면
                        dash = false;
        }

        /* Don't allow trailing hash */
        만약 (dash)
                반환 false;

        반환 true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        만약 (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                반환 false;

        p = strchr(a, '@');
        만약 (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        만약 (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        반환 memcmp_nn(a, p - a, b, q - b) == 0;
}
