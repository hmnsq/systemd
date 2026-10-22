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

        如果 (_unlikely_(flags == 0))
                返回 false;

        如果 (isempty(n))
                返回 false;

        如果 (strlen(n) >= UNIT_NAME_MAX)
                返回 false;

        e = strrchr(n, '.');
        如果 (!e || e == n)
                返回 false;

        如果 (unit_type_from_string(e + 1) < 0)
                返回 false;

        为 (i = n, at = NULL; i < e; i++) {

                如果 (*i == '@' && !at)
                        at = i;

                如果 (!strchr(VALID_CHARS_WITH_AT, *i))
                        返回 false;
        }

        如果 (at == n)
                返回 false;

        如果 (flags & UNIT_NAME_PLAIN)
                如果 (!at)
                        返回 true;

        如果 (flags & UNIT_NAME_INSTANCE)
                如果 (at && e > at + 1)
                        返回 true;

        如果 (flags & UNIT_NAME_TEMPLATE)
                如果 (at && e == at + 1)
                        返回 true;

        返回 false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        如果 (isempty(p))
                返回 false;

        返回 in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        如果 (isempty(i))
                返回 false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        返回 in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        如果 (isempty(s))
                返回 false;

        如果 (s[0] != '.')
                返回 false;

        如果 (unit_type_from_string(s + 1) < 0)
                返回 false;

        返回 true;
}

整数 unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        如果 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                返回 -EINVAL;

        p = strchr(n, '@');
        如果 (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        如果 (!s)
                返回 -ENOMEM;

        *ret = s;
        返回 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        如果 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                返回 -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        如果 (!p) {
                如果 (ret)
                        *ret = NULL;
                返回 UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        如果 (!d)
                返回 -EINVAL;

        如果 (ret) {
                char *i = strndup(p, d-p);
                如果 (!i)
                        返回 -ENOMEM;

                *ret = i;
        }
        返回 d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

整数 unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        如果 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                返回 -EINVAL;

        d = strrchr(n, '.');
        如果 (!d)
                返回 -EINVAL;

        s = strndup(n, d - n);
        如果 (!s)
                返回 -ENOMEM;

        *ret = s;
        返回 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        如果 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                返回 _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        返回 unit_type_from_string(e + 1);
}

整数 unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        如果 (!unit_name_is_valid(n, UNIT_NAME_ANY))
                返回 -EINVAL;

        如果 (!unit_suffix_is_valid(suffix))
                返回 -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        如果 (!s)
                返回 -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        如果 (!unit_name_is_valid(s, UNIT_NAME_ANY))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        如果 (suffix[0] != '.')
                返回 -EINVAL;

        type = unit_type_from_string(suffix + 1);
        如果 (type < 0)
                返回 type;

        返回 unit_name_build_from_type(prefix, instance, type, ret);
}

整数 unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        如果 (!unit_prefix_is_valid(prefix))
                返回 -EINVAL;

        ut = unit_type_to_string(type);

        如果 (instance) {
                如果 (!unit_instance_is_valid(instance))
                        返回 -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } 否则
                s = strjoin(prefix, ".", ut);
        如果 (!s)
                返回 -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        如果 (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        返回 t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        如果 (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        为 (; *f; f++) {
                如果 (*f == '/')
                        *(t++) = '-';
                否则 如果 (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                否则
                        *(t++) = *f;
        }

        返回 t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        如果 (!r)
                返回 NULL;

        t = do_escape(f, r);
        *t = 0;

        返回 r;
}

整数 unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        如果 (!r)
                返回 -ENOMEM;

        为 (t = r; *f; f++) {
                如果 (*f == '-')
                        *(t++) = '/';
                否则 如果 (*f == '\\') {
                        整数 a, b;

                        如果 (f[1] != 'x')
                                返回 -EINVAL;

                        a = unhexchar(f[2]);
                        如果 (a < 0)
                                返回 -EINVAL;

                        b = unhexchar(f[3]);
                        如果 (b < 0)
                                返回 -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } 否则
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        返回 0;
}

整数 unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        整数 r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        如果 (r < 0)
                返回 r;

        如果 (empty_or_root(p))
                s = strdup("-");
        否则 {
                如果 (!path_is_normalized(p))
                        返回 -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        如果 (!s)
                返回 -ENOMEM;

        *ret = s;
        返回 0;
}

整数 unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(f);

        如果 (isempty(f))
                返回 -EINVAL;

        如果 (streq(f, "-")) {
                s = strdup("/");
                如果 (!s)
                        返回 -ENOMEM;
        } 否则 {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                如果 (r < 0)
                        返回 r;

                /* Don't accept trailing or leading slashes */
                如果 (startswith(w, "/") || endswith(w, "/"))
                        返回 -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                如果 (!s)
                        返回 -ENOMEM;

                如果 (!path_is_normalized(s))
                        返回 -EINVAL;
        }

        如果 (ret)
                *ret = TAKE_PTR(s);

        返回 0;
}

整数 unit_name_replace_instance_full(
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

        如果 (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                返回 -EINVAL;
        如果 (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                返回 -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        如果 (!s)
                返回 -ENOMEM;

#如果 HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        如果 (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        如果 (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                返回 -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        如果 (!s)
                返回 -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        返回 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        如果 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                返回 false;

        assert_se(s = strrchr(name, '.'));

        如果 (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                返回 false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        如果 (s[-1] != '_')
                返回 false;

        为 (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                如果 (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        返回 false;

        返回 true;
}

整数 unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        如果 (strlen(name) < UNIT_NAME_MAX)
                返回 -EMSGSIZE;

        suffix = strrchr(name, '.');
        如果 (!suffix)
                返回 -EINVAL;

        如果 (unit_type_from_string(suffix+1) < 0)
                返回 -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        如果 (!hash)
                返回 -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        如果 (!n)
                返回 -ENOMEM;

        如果 (!strextend(&n, "_", hash, suffix))
                返回 -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        返回 0;
}

整数 unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        整数 r;

        assert(path);
        assert(suffix);
        assert(ret);

        如果 (!unit_suffix_is_valid(suffix))
                返回 -EINVAL;

        r = unit_name_path_escape(path, &p);
        如果 (r < 0)
                返回 r;

        s = strjoin(p, suffix);
        如果 (!s)
                返回 -ENOMEM;

        如果 (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                如果 (r < 0)
                        返回 r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        如果 (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        整数 r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        如果 (!unit_prefix_is_valid(prefix))
                返回 -EINVAL;

        如果 (!unit_suffix_is_valid(suffix))
                返回 -EINVAL;

        r = unit_name_path_escape(path, &p);
        如果 (r < 0)
                返回 r;

        s = strjoin(prefix, "@", p, suffix);
        如果 (!s)
                返回 -ENOMEM;

        如果 (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                返回 -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        如果 (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 0;
}

整数 unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        整数 r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        如果 (r < 0)
                返回 r;

        如果 (unit_name_is_hashed(name))
                返回 -ENAMETOOLONG;

        返回 unit_name_path_unescape(prefix, ret);
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

        为 (; *f; f++)
                如果 (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } 否则 如果 (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } 否则
                        *(t++) = *f;
        *t = 0;

        返回 mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
整数 unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        整数 r;

        assert(name);
        assert(suffix);
        assert(ret);

        如果 (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                返回 -EINVAL;

        如果 (!unit_suffix_is_valid(suffix))
                返回 -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        如果 (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                如果 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        返回 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        如果 (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                如果 (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        如果 (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                如果 (r < 0)
                        返回 r;

                如果 (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        如果 (r >= 0) {
                                如果 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        返回 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                返回 1;
                        }
                        如果 (r != -EINVAL)
                                返回 r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                如果 (r >= 0) {
                        如果 (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                返回 log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        返回 1;
                }
                如果 (r != -EINVAL)
                        返回 r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        如果 (!s)
                返回 -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        如果 (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        如果 ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        如果 (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                返回 -EINVAL;

        *ret = TAKE_PTR(s);
        返回 1;

good:
        返回 strdup_to(ret, name);
}

整数 slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        如果 (!slice_name_is_valid(slice))
                返回 -EINVAL;

        如果 (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                返回 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        如果 (!s)
                返回 -ENOMEM;

        char *dash = strrchr(s, '-');
        如果 (!dash)
                返回 strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        返回 1;
}

整数 slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        如果 (!slice_name_is_valid(slice))
                返回 -EINVAL;

        如果 (!unit_prefix_is_valid(name))
                返回 -EINVAL;

        如果 (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        否则 {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                如果 (!subslice)
                        返回 -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        返回 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        如果 (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                返回 false;

        如果 (streq(name, SPECIAL_ROOT_SLICE))
                返回 true;

        e = endswith(name, ".slice");
        如果 (!e)
                返回 false;

        为 (p = name; p < e; p++) {

                如果 (*p == '-') {

                        /* Don't allow initial dash */
                        如果 (p == name)
                                返回 false;

                        /* Don't allow multiple dashes */
                        如果 (dash)
                                返回 false;

                        dash = true;
                } 否则
                        dash = false;
        }

        /* Don't allow trailing hash */
        如果 (dash)
                返回 false;

        返回 true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        如果 (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                返回 false;

        p = strchr(a, '@');
        如果 (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        如果 (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        返回 memcmp_nn(a, p - a, b, q - b) == 0;
}
