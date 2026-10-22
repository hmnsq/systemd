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

        もし (_unlikely_(flags == 0))
                戻る false;

        もし (isempty(n))
                戻る false;

        もし (strlen(n) >= UNIT_NAME_MAX)
                戻る false;

        e = strrchr(n, '.');
        もし (!e || e == n)
                戻る false;

        もし (unit_type_from_string(e + 1) < 0)
                戻る false;

        ために (i = n, at = NULL; i < e; i++) {

                もし (*i == '@' && !at)
                        at = i;

                もし (!strchr(VALID_CHARS_WITH_AT, *i))
                        戻る false;
        }

        もし (at == n)
                戻る false;

        もし (flags & UNIT_NAME_PLAIN)
                もし (!at)
                        戻る true;

        もし (flags & UNIT_NAME_INSTANCE)
                もし (at && e > at + 1)
                        戻る true;

        もし (flags & UNIT_NAME_TEMPLATE)
                もし (at && e == at + 1)
                        戻る true;

        戻る false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        もし (isempty(p))
                戻る false;

        戻る in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        もし (isempty(i))
                戻る false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        戻る in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        もし (isempty(s))
                戻る false;

        もし (s[0] != '.')
                戻る false;

        もし (unit_type_from_string(s + 1) < 0)
                戻る false;

        戻る true;
}

整数 unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        もし (!unit_name_is_valid(n, UNIT_NAME_ANY))
                戻る -EINVAL;

        p = strchr(n, '@');
        もし (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        もし (!s)
                戻る -ENOMEM;

        *ret = s;
        戻る 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        もし (!unit_name_is_valid(n, UNIT_NAME_ANY))
                戻る -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        もし (!p) {
                もし (ret)
                        *ret = NULL;
                戻る UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        もし (!d)
                戻る -EINVAL;

        もし (ret) {
                char *i = strndup(p, d-p);
                もし (!i)
                        戻る -ENOMEM;

                *ret = i;
        }
        戻る d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

整数 unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        もし (!unit_name_is_valid(n, UNIT_NAME_ANY))
                戻る -EINVAL;

        d = strrchr(n, '.');
        もし (!d)
                戻る -EINVAL;

        s = strndup(n, d - n);
        もし (!s)
                戻る -ENOMEM;

        *ret = s;
        戻る 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        もし (!unit_name_is_valid(n, UNIT_NAME_ANY))
                戻る _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        戻る unit_type_from_string(e + 1);
}

整数 unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        もし (!unit_name_is_valid(n, UNIT_NAME_ANY))
                戻る -EINVAL;

        もし (!unit_suffix_is_valid(suffix))
                戻る -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        もし (!s)
                戻る -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        もし (!unit_name_is_valid(s, UNIT_NAME_ANY))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 0;
}

整数 unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        もし (suffix[0] != '.')
                戻る -EINVAL;

        type = unit_type_from_string(suffix + 1);
        もし (type < 0)
                戻る type;

        戻る unit_name_build_from_type(prefix, instance, type, ret);
}

整数 unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        もし (!unit_prefix_is_valid(prefix))
                戻る -EINVAL;

        ut = unit_type_to_string(type);

        もし (instance) {
                もし (!unit_instance_is_valid(instance))
                        戻る -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } 否则
                s = strjoin(prefix, ".", ut);
        もし (!s)
                戻る -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        もし (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        戻る t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        もし (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ために (; *f; f++) {
                もし (*f == '/')
                        *(t++) = '-';
                否则 もし (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                否则
                        *(t++) = *f;
        }

        戻る t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        もし (!r)
                戻る NULL;

        t = do_escape(f, r);
        *t = 0;

        戻る r;
}

整数 unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        もし (!r)
                戻る -ENOMEM;

        ために (t = r; *f; f++) {
                もし (*f == '-')
                        *(t++) = '/';
                否则 もし (*f == '\\') {
                        整数 a, b;

                        もし (f[1] != 'x')
                                戻る -EINVAL;

                        a = unhexchar(f[2]);
                        もし (a < 0)
                                戻る -EINVAL;

                        b = unhexchar(f[3]);
                        もし (b < 0)
                                戻る -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } 否则
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        戻る 0;
}

整数 unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        整数 r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        もし (r < 0)
                戻る r;

        もし (empty_or_root(p))
                s = strdup("-");
        否则 {
                もし (!path_is_normalized(p))
                        戻る -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        もし (!s)
                戻る -ENOMEM;

        *ret = s;
        戻る 0;
}

整数 unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        整数 r;

        assert(f);

        もし (isempty(f))
                戻る -EINVAL;

        もし (streq(f, "-")) {
                s = strdup("/");
                もし (!s)
                        戻る -ENOMEM;
        } 否则 {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                もし (r < 0)
                        戻る r;

                /* Don't accept trailing or leading slashes */
                もし (startswith(w, "/") || endswith(w, "/"))
                        戻る -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                もし (!s)
                        戻る -ENOMEM;

                もし (!path_is_normalized(s))
                        戻る -EINVAL;
        }

        もし (ret)
                *ret = TAKE_PTR(s);

        戻る 0;
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

        もし (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                戻る -EINVAL;
        もし (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                戻る -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        もし (!s)
                戻る -ENOMEM;

#もし HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        もし (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 0;
}

整数 unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        もし (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                戻る -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        もし (!s)
                戻る -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        戻る 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        もし (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                戻る false;

        assert_se(s = strrchr(name, '.'));

        もし (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                戻る false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        もし (s[-1] != '_')
                戻る false;

        ために (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                もし (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        戻る false;

        戻る true;
}

整数 unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        もし (strlen(name) < UNIT_NAME_MAX)
                戻る -EMSGSIZE;

        suffix = strrchr(name, '.');
        もし (!suffix)
                戻る -EINVAL;

        もし (unit_type_from_string(suffix+1) < 0)
                戻る -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        もし (!hash)
                戻る -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        もし (!n)
                戻る -ENOMEM;

        もし (!strextend(&n, "_", hash, suffix))
                戻る -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        戻る 0;
}

整数 unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        整数 r;

        assert(path);
        assert(suffix);
        assert(ret);

        もし (!unit_suffix_is_valid(suffix))
                戻る -EINVAL;

        r = unit_name_path_escape(path, &p);
        もし (r < 0)
                戻る r;

        s = strjoin(p, suffix);
        もし (!s)
                戻る -ENOMEM;

        もし (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                もし (r < 0)
                        戻る r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        もし (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 0;
}

整数 unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        整数 r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        もし (!unit_prefix_is_valid(prefix))
                戻る -EINVAL;

        もし (!unit_suffix_is_valid(suffix))
                戻る -EINVAL;

        r = unit_name_path_escape(path, &p);
        もし (r < 0)
                戻る r;

        s = strjoin(prefix, "@", p, suffix);
        もし (!s)
                戻る -ENOMEM;

        もし (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                戻る -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        もし (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 0;
}

整数 unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        整数 r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        もし (r < 0)
                戻る r;

        もし (unit_name_is_hashed(name))
                戻る -ENAMETOOLONG;

        戻る unit_name_path_unescape(prefix, ret);
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

        ために (; *f; f++)
                もし (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } 否则 もし (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } 否则
                        *(t++) = *f;
        *t = 0;

        戻る mangled;
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

        もし (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                戻る -EINVAL;

        もし (!unit_suffix_is_valid(suffix))
                戻る -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        もし (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                もし (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        戻る log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        もし (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                もし (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        もし (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                もし (r < 0)
                        戻る r;

                もし (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        もし (r >= 0) {
                                もし (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        戻る log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                戻る 1;
                        }
                        もし (r != -EINVAL)
                                戻る r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                もし (r >= 0) {
                        もし (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                戻る log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        戻る 1;
                }
                もし (r != -EINVAL)
                        戻る r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        もし (!s)
                戻る -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        もし (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        もし ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        もし (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                戻る -EINVAL;

        *ret = TAKE_PTR(s);
        戻る 1;

good:
        戻る strdup_to(ret, name);
}

整数 slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        もし (!slice_name_is_valid(slice))
                戻る -EINVAL;

        もし (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                戻る 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        もし (!s)
                戻る -ENOMEM;

        char *dash = strrchr(s, '-');
        もし (!dash)
                戻る strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        戻る 1;
}

整数 slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        もし (!slice_name_is_valid(slice))
                戻る -EINVAL;

        もし (!unit_prefix_is_valid(name))
                戻る -EINVAL;

        もし (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        否则 {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                もし (!subslice)
                        戻る -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        戻る 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        もし (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                戻る false;

        もし (streq(name, SPECIAL_ROOT_SLICE))
                戻る true;

        e = endswith(name, ".slice");
        もし (!e)
                戻る false;

        ために (p = name; p < e; p++) {

                もし (*p == '-') {

                        /* Don't allow initial dash */
                        もし (p == name)
                                戻る false;

                        /* Don't allow multiple dashes */
                        もし (dash)
                                戻る false;

                        dash = true;
                } 否则
                        dash = false;
        }

        /* Don't allow trailing hash */
        もし (dash)
                戻る false;

        戻る true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        もし (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                戻る false;

        p = strchr(a, '@');
        もし (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        もし (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        戻る memcmp_nn(a, p - a, b, q - b) == 0;
}
