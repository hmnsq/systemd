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

        ако (_unlikely_(flags == 0))
                врати false;

        ако (isempty(n))
                врати false;

        ако (strlen(n) >= UNIT_NAME_MAX)
                врати false;

        e = strrchr(n, '.');
        ако (!e || e == n)
                врати false;

        ако (unit_type_from_string(e + 1) < 0)
                врати false;

        за (i = n, at = NULL; i < e; i++) {

                ако (*i == '@' && !at)
                        at = i;

                ако (!strchr(VALID_CHARS_WITH_AT, *i))
                        врати false;
        }

        ако (at == n)
                врати false;

        ако (flags & UNIT_NAME_PLAIN)
                ако (!at)
                        врати true;

        ако (flags & UNIT_NAME_INSTANCE)
                ако (at && e > at + 1)
                        врати true;

        ако (flags & UNIT_NAME_TEMPLATE)
                ако (at && e == at + 1)
                        врати true;

        врати false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        ако (isempty(p))
                врати false;

        врати in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ако (isempty(i))
                врати false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        врати in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        ако (isempty(s))
                врати false;

        ако (s[0] != '.')
                врати false;

        ако (unit_type_from_string(s + 1) < 0)
                врати false;

        врати true;
}

цео unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        ако (!unit_name_is_valid(n, UNIT_NAME_ANY))
                врати -EINVAL;

        p = strchr(n, '@');
        ако (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ако (!s)
                врати -ENOMEM;

        *ret = s;
        врати 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        ако (!unit_name_is_valid(n, UNIT_NAME_ANY))
                врати -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ако (!p) {
                ако (ret)
                        *ret = NULL;
                врати UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ако (!d)
                врати -EINVAL;

        ако (ret) {
                char *i = strndup(p, d-p);
                ако (!i)
                        врати -ENOMEM;

                *ret = i;
        }
        врати d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

цео unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        ако (!unit_name_is_valid(n, UNIT_NAME_ANY))
                врати -EINVAL;

        d = strrchr(n, '.');
        ако (!d)
                врати -EINVAL;

        s = strndup(n, d - n);
        ако (!s)
                врати -ENOMEM;

        *ret = s;
        врати 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        ако (!unit_name_is_valid(n, UNIT_NAME_ANY))
                врати _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        врати unit_type_from_string(e + 1);
}

цео unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ако (!unit_name_is_valid(n, UNIT_NAME_ANY))
                врати -EINVAL;

        ако (!unit_suffix_is_valid(suffix))
                врати -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        ако (!s)
                врати -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ако (!unit_name_is_valid(s, UNIT_NAME_ANY))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 0;
}

цео unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ако (suffix[0] != '.')
                врати -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ако (type < 0)
                врати type;

        врати unit_name_build_from_type(prefix, instance, type, ret);
}

цео unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ако (!unit_prefix_is_valid(prefix))
                врати -EINVAL;

        ut = unit_type_to_string(type);

        ако (instance) {
                ако (!unit_instance_is_valid(instance))
                        врати -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } инако
                s = strjoin(prefix, ".", ut);
        ако (!s)
                врати -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ако (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        врати t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ако (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        за (; *f; f++) {
                ако (*f == '/')
                        *(t++) = '-';
                инако ако (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                инако
                        *(t++) = *f;
        }

        врати t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        ако (!r)
                врати NULL;

        t = do_escape(f, r);
        *t = 0;

        врати r;
}

цео unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ако (!r)
                врати -ENOMEM;

        за (t = r; *f; f++) {
                ако (*f == '-')
                        *(t++) = '/';
                инако ако (*f == '\\') {
                        цео a, b;

                        ако (f[1] != 'x')
                                врати -EINVAL;

                        a = unhexchar(f[2]);
                        ако (a < 0)
                                врати -EINVAL;

                        b = unhexchar(f[3]);
                        ако (b < 0)
                                врати -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } инако
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        врати 0;
}

цео unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        цео r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ако (r < 0)
                врати r;

        ако (empty_or_root(p))
                s = strdup("-");
        инако {
                ако (!path_is_normalized(p))
                        врати -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ако (!s)
                врати -ENOMEM;

        *ret = s;
        врати 0;
}

цео unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        цео r;

        assert(f);

        ако (isempty(f))
                врати -EINVAL;

        ако (streq(f, "-")) {
                s = strdup("/");
                ако (!s)
                        врати -ENOMEM;
        } инако {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                ако (r < 0)
                        врати r;

                /* Don't accept trailing or leading slashes */
                ако (startswith(w, "/") || endswith(w, "/"))
                        врати -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ако (!s)
                        врати -ENOMEM;

                ако (!path_is_normalized(s))
                        врати -EINVAL;
        }

        ако (ret)
                *ret = TAKE_PTR(s);

        врати 0;
}

цео unit_name_replace_instance_full(
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

        ако (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                врати -EINVAL;
        ако (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                врати -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        ако (!s)
                врати -ENOMEM;

#ако HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ако (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 0;
}

цео unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        ако (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                врати -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        ако (!s)
                врати -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        врати 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        ако (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                врати false;

        assert_se(s = strrchr(name, '.'));

        ако (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                врати false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ако (s[-1] != '_')
                врати false;

        за (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ако (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        врати false;

        врати true;
}

цео unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ако (strlen(name) < UNIT_NAME_MAX)
                врати -EMSGSIZE;

        suffix = strrchr(name, '.');
        ако (!suffix)
                врати -EINVAL;

        ако (unit_type_from_string(suffix+1) < 0)
                врати -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        ако (!hash)
                врати -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ако (!n)
                врати -ENOMEM;

        ако (!strextend(&n, "_", hash, suffix))
                врати -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        врати 0;
}

цео unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        цео r;

        assert(path);
        assert(suffix);
        assert(ret);

        ако (!unit_suffix_is_valid(suffix))
                врати -EINVAL;

        r = unit_name_path_escape(path, &p);
        ако (r < 0)
                врати r;

        s = strjoin(p, suffix);
        ако (!s)
                врати -ENOMEM;

        ако (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ако (r < 0)
                        врати r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ако (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 0;
}

цео unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        цео r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ако (!unit_prefix_is_valid(prefix))
                врати -EINVAL;

        ако (!unit_suffix_is_valid(suffix))
                врати -EINVAL;

        r = unit_name_path_escape(path, &p);
        ако (r < 0)
                врати r;

        s = strjoin(prefix, "@", p, suffix);
        ако (!s)
                врати -ENOMEM;

        ако (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                врати -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ако (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 0;
}

цео unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        цео r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ако (r < 0)
                врати r;

        ако (unit_name_is_hashed(name))
                врати -ENAMETOOLONG;

        врати unit_name_path_unescape(prefix, ret);
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

        за (; *f; f++)
                ако (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } инако ако (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } инако
                        *(t++) = *f;
        *t = 0;

        врати mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
цео unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        цео r;

        assert(name);
        assert(suffix);
        assert(ret);

        ако (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                врати -EINVAL;

        ако (!unit_suffix_is_valid(suffix))
                врати -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ако (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ако (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        врати log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ако (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ако (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ако (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ако (r < 0)
                        врати r;

                ако (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ако (r >= 0) {
                                ако (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        врати log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                врати 1;
                        }
                        ако (r != -EINVAL)
                                врати r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ако (r >= 0) {
                        ако (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                врати log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        врати 1;
                }
                ако (r != -EINVAL)
                        врати r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        ако (!s)
                врати -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ако (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ако ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ако (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                врати -EINVAL;

        *ret = TAKE_PTR(s);
        врати 1;

good:
        врати strdup_to(ret, name);
}

цео slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        ако (!slice_name_is_valid(slice))
                врати -EINVAL;

        ако (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                врати 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        ако (!s)
                врати -ENOMEM;

        char *dash = strrchr(s, '-');
        ако (!dash)
                врати strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        врати 1;
}

цео slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ако (!slice_name_is_valid(slice))
                врати -EINVAL;

        ако (!unit_prefix_is_valid(name))
                врати -EINVAL;

        ако (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        инако {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                ако (!subslice)
                        врати -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        врати 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        ако (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                врати false;

        ако (streq(name, SPECIAL_ROOT_SLICE))
                врати true;

        e = endswith(name, ".slice");
        ако (!e)
                врати false;

        за (p = name; p < e; p++) {

                ако (*p == '-') {

                        /* Don't allow initial dash */
                        ако (p == name)
                                врати false;

                        /* Don't allow multiple dashes */
                        ако (dash)
                                врати false;

                        dash = true;
                } инако
                        dash = false;
        }

        /* Don't allow trailing hash */
        ако (dash)
                врати false;

        врати true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        ако (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                врати false;

        p = strchr(a, '@');
        ако (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ако (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        врати memcmp_nn(a, p - a, b, q - b) == 0;
}
