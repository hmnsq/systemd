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

        nếu (_unlikely_(flags == 0))
                trả_về false;

        nếu (isempty(n))
                trả_về false;

        nếu (strlen(n) >= UNIT_NAME_MAX)
                trả_về false;

        e = strrchr(n, '.');
        nếu (!e || e == n)
                trả_về false;

        nếu (unit_type_from_string(e + 1) < 0)
                trả_về false;

        cho (i = n, at = NULL; i < e; i++) {

                nếu (*i == '@' && !at)
                        at = i;

                nếu (!strchr(VALID_CHARS_WITH_AT, *i))
                        trả_về false;
        }

        nếu (at == n)
                trả_về false;

        nếu (flags & UNIT_NAME_PLAIN)
                nếu (!at)
                        trả_về true;

        nếu (flags & UNIT_NAME_INSTANCE)
                nếu (at && e > at + 1)
                        trả_về true;

        nếu (flags & UNIT_NAME_TEMPLATE)
                nếu (at && e == at + 1)
                        trả_về true;

        trả_về false;
}

bool unit_prefix_is_valid(const char *p) {

        /* We don't allow additional @ in the prefix string */

        nếu (isempty(p))
                trả_về false;

        trả_về in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const char *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        nếu (isempty(i))
                trả_về false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        trả_về in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const char *s) {
        nếu (isempty(s))
                trả_về false;

        nếu (s[0] != '.')
                trả_về false;

        nếu (unit_type_from_string(s + 1) < 0)
                trả_về false;

        trả_về true;
}

số_nguyên unit_name_to_prefix(const char *n, char **ret) {
        const char *p;
        char *s;

        assert(n);
        assert(ret);

        nếu (!unit_name_is_valid(n, UNIT_NAME_ANY))
                trả_về -EINVAL;

        p = strchr(n, '@');
        nếu (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        nếu (!s)
                trả_về -ENOMEM;

        *ret = s;
        trả_về 0;
}

UnitNameFlags unit_name_to_instance(const char *n, char **ret) {
        const char *p, *d;

        assert(n);

        nếu (!unit_name_is_valid(n, UNIT_NAME_ANY))
                trả_về -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        nếu (!p) {
                nếu (ret)
                        *ret = NULL;
                trả_về UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        nếu (!d)
                trả_về -EINVAL;

        nếu (ret) {
                char *i = strndup(p, d-p);
                nếu (!i)
                        trả_về -ENOMEM;

                *ret = i;
        }
        trả_về d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

số_nguyên unit_name_to_prefix_and_instance(const char *n, char **ret) {
        const char *d;
        char *s;

        assert(n);
        assert(ret);

        nếu (!unit_name_is_valid(n, UNIT_NAME_ANY))
                trả_về -EINVAL;

        d = strrchr(n, '.');
        nếu (!d)
                trả_về -EINVAL;

        s = strndup(n, d - n);
        nếu (!s)
                trả_về -ENOMEM;

        *ret = s;
        trả_về 0;
}

UnitType unit_name_to_type(const char *n) {
        const char *e;

        assert(n);

        nếu (!unit_name_is_valid(n, UNIT_NAME_ANY))
                trả_về _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        trả_về unit_type_from_string(e + 1);
}

số_nguyên unit_name_change_suffix(const char *n, const char *suffix, char **ret) {
        _cleanup_free_ char *s = NULL;
        size_t a, b;
        const char *e;

        assert(n);
        assert(suffix);
        assert(ret);

        nếu (!unit_name_is_valid(n, UNIT_NAME_ANY))
                trả_về -EINVAL;

        nếu (!unit_suffix_is_valid(suffix))
                trả_về -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(char, a + b + 1);
        nếu (!s)
                trả_về -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        nếu (!unit_name_is_valid(s, UNIT_NAME_ANY))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên unit_name_build(const char *prefix, const char *instance, const char *suffix, char **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        nếu (suffix[0] != '.')
                trả_về -EINVAL;

        type = unit_type_from_string(suffix + 1);
        nếu (type < 0)
                trả_về type;

        trả_về unit_name_build_from_type(prefix, instance, type, ret);
}

số_nguyên unit_name_build_from_type(const char *prefix, const char *instance, UnitType type, char **ret) {
        _cleanup_free_ char *s = NULL;
        const char *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        nếu (!unit_prefix_is_valid(prefix))
                trả_về -EINVAL;

        ut = unit_type_to_string(type);

        nếu (instance) {
                nếu (!unit_instance_is_valid(instance))
                        trả_về -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } không_thì
                s = strjoin(prefix, ".", ut);
        nếu (!s)
                trả_về -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        nếu (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

static char *do_escape_char(char c, char *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        trả_về t;
}

static char *do_escape(const char *f, char *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        nếu (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        cho (; *f; f++) {
                nếu (*f == '/')
                        *(t++) = '-';
                không_thì nếu (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                không_thì
                        *(t++) = *f;
        }

        trả_về t;
}

char* unit_name_escape(const char *f) {
        char *r, *t;

        assert(f);

        r = new(char, strlen(f)*4+1);
        nếu (!r)
                trả_về NULL;

        t = do_escape(f, r);
        *t = 0;

        trả_về r;
}

số_nguyên unit_name_unescape(const char *f, char **ret) {
        _cleanup_free_ char *r = NULL;
        char *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        nếu (!r)
                trả_về -ENOMEM;

        cho (t = r; *f; f++) {
                nếu (*f == '-')
                        *(t++) = '/';
                không_thì nếu (*f == '\\') {
                        số_nguyên a, b;

                        nếu (f[1] != 'x')
                                trả_về -EINVAL;

                        a = unhexchar(f[2]);
                        nếu (a < 0)
                                trả_về -EINVAL;

                        b = unhexchar(f[3]);
                        nếu (b < 0)
                                trả_về -EINVAL;

                        *(t++) = (char) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } không_thì
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        trả_về 0;
}

số_nguyên unit_name_path_escape(const char *f, char **ret) {
        _cleanup_free_ char *p = NULL;
        char *s;
        số_nguyên r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        nếu (r < 0)
                trả_về r;

        nếu (empty_or_root(p))
                s = strdup("-");
        không_thì {
                nếu (!path_is_normalized(p))
                        trả_về -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        nếu (!s)
                trả_về -ENOMEM;

        *ret = s;
        trả_về 0;
}

số_nguyên unit_name_path_unescape(const char *f, char **ret) {
        _cleanup_free_ char *s = NULL;
        số_nguyên r;

        assert(f);

        nếu (isempty(f))
                trả_về -EINVAL;

        nếu (streq(f, "-")) {
                s = strdup("/");
                nếu (!s)
                        trả_về -ENOMEM;
        } không_thì {
                _cleanup_free_ char *w = NULL;

                r = unit_name_unescape(f, &w);
                nếu (r < 0)
                        trả_về r;

                /* Don't accept trailing or leading slashes */
                nếu (startswith(w, "/") || endswith(w, "/"))
                        trả_về -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                nếu (!s)
                        trả_về -ENOMEM;

                nếu (!path_is_normalized(s))
                        trả_về -EINVAL;
        }

        nếu (ret)
                *ret = TAKE_PTR(s);

        trả_về 0;
}

số_nguyên unit_name_replace_instance_full(
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

        nếu (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                trả_về -EINVAL;
        nếu (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                trả_về -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(char, pl + strlen(instance) + strlen(suffix) + 1);
        nếu (!s)
                trả_về -ENOMEM;

#nếu HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        nếu (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên unit_name_template(const char *f, char **ret) {
        const char *p, *e;
        char *s;
        size_t a;

        assert(f);
        assert(ret);

        nếu (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                trả_về -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(char, a + 1 + strlen(e) + 1);
        nếu (!s)
                trả_về -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        trả_về 0;
}

bool unit_name_is_hashed(const char *name) {
        const char *s;

        nếu (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                trả_về false;

        assert_se(s = strrchr(name, '.'));

        nếu (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                trả_về false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        nếu (s[-1] != '_')
                trả_về false;

        cho (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                nếu (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        trả_về false;

        trả_về true;
}

số_nguyên unit_name_hash_long(const char *name, char **ret) {
        _cleanup_free_ char *n = NULL, *hash = NULL;
        const char *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        nếu (strlen(name) < UNIT_NAME_MAX)
                trả_về -EMSGSIZE;

        suffix = strrchr(name, '.');
        nếu (!suffix)
                trả_về -EINVAL;

        nếu (unit_type_from_string(suffix+1) < 0)
                trả_về -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, sizeof(h));
        nếu (!hash)
                trả_về -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        nếu (!n)
                trả_về -ENOMEM;

        nếu (!strextend(&n, "_", hash, suffix))
                trả_về -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        trả_về 0;
}

số_nguyên unit_name_from_path(const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        số_nguyên r;

        assert(path);
        assert(suffix);
        assert(ret);

        nếu (!unit_suffix_is_valid(suffix))
                trả_về -EINVAL;

        r = unit_name_path_escape(path, &p);
        nếu (r < 0)
                trả_về r;

        s = strjoin(p, suffix);
        nếu (!s)
                trả_về -ENOMEM;

        nếu (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ char *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                nếu (r < 0)
                        trả_về r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        nếu (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên unit_name_from_path_instance(const char *prefix, const char *path, const char *suffix, char **ret) {
        _cleanup_free_ char *p = NULL, *s = NULL;
        số_nguyên r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        nếu (!unit_prefix_is_valid(prefix))
                trả_về -EINVAL;

        nếu (!unit_suffix_is_valid(suffix))
                trả_về -EINVAL;

        r = unit_name_path_escape(path, &p);
        nếu (r < 0)
                trả_về r;

        s = strjoin(prefix, "@", p, suffix);
        nếu (!s)
                trả_về -ENOMEM;

        nếu (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                trả_về -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        nếu (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 0;
}

số_nguyên unit_name_to_path(const char *name, char **ret) {
        _cleanup_free_ char *prefix = NULL;
        số_nguyên r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        nếu (r < 0)
                trả_về r;

        nếu (unit_name_is_hashed(name))
                trả_về -ENAMETOOLONG;

        trả_về unit_name_path_unescape(prefix, ret);
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

        cho (; *f; f++)
                nếu (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } không_thì nếu (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } không_thì
                        *(t++) = *f;
        *t = 0;

        trả_về mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
số_nguyên unit_name_mangle_with_suffix(
                const char *name,
                const char *operation,
                UnitNameMangle flags,
                const char *suffix,
                char **ret) {

        _cleanup_free_ char *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        số_nguyên r;

        assert(name);
        assert(suffix);
        assert(ret);

        nếu (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                trả_về -EINVAL;

        nếu (!unit_suffix_is_valid(suffix))
                trả_về -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        nếu (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                nếu (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const char *e = ASSERT_PTR(strrchr(name, '.'));

                        trả_về log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        nếu (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                nếu (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        nếu (path_is_absolute(name)) {
                _cleanup_free_ char *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                nếu (r < 0)
                        trả_về r;

                nếu (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        nếu (r >= 0) {
                                nếu (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        trả_về log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                trả_về 1;
                        }
                        nếu (r != -EINVAL)
                                trả_về r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                nếu (r >= 0) {
                        nếu (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                trả_về log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        trả_về 1;
                }
                nếu (r != -EINVAL)
                        trả_về r;
        }

        s = new(char, strlen(name) * 4 + strlen(suffix) + 1);
        nếu (!s)
                trả_về -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        nếu (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        nếu ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        nếu (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                trả_về -EINVAL;

        *ret = TAKE_PTR(s);
        trả_về 1;

good:
        trả_về strdup_to(ret, name);
}

số_nguyên slice_build_parent_slice(const char *slice, char **ret) {
        assert(slice);
        assert(ret);

        nếu (!slice_name_is_valid(slice))
                trả_về -EINVAL;

        nếu (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                trả_về 0;
        }

        _cleanup_free_ char *s = strdup(slice);
        nếu (!s)
                trả_về -ENOMEM;

        char *dash = strrchr(s, '-');
        nếu (!dash)
                trả_về strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        trả_về 1;
}

số_nguyên slice_build_subslice(const char *slice, const char *name, char **ret) {
        char *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        nếu (!slice_name_is_valid(slice))
                trả_về -EINVAL;

        nếu (!unit_prefix_is_valid(name))
                trả_về -EINVAL;

        nếu (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        không_thì {
                const char *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(char, (e - slice) + 1 + strlen(name) + 6 + 1);
                nếu (!subslice)
                        trả_về -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        trả_về 0;
}

bool slice_name_is_valid(const char *name) {
        const char *p, *e;
        bool dash = false;

        nếu (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                trả_về false;

        nếu (streq(name, SPECIAL_ROOT_SLICE))
                trả_về true;

        e = endswith(name, ".slice");
        nếu (!e)
                trả_về false;

        cho (p = name; p < e; p++) {

                nếu (*p == '-') {

                        /* Don't allow initial dash */
                        nếu (p == name)
                                trả_về false;

                        /* Don't allow multiple dashes */
                        nếu (dash)
                                trả_về false;

                        dash = true;
                } không_thì
                        dash = false;
        }

        /* Don't allow trailing hash */
        nếu (dash)
                trả_về false;

        trả_về true;
}

bool unit_name_prefix_equal(const char *a, const char *b) {
        const char *p, *q;

        assert(a);
        assert(b);

        nếu (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                trả_về false;

        p = strchr(a, '@');
        nếu (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        nếu (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        trả_về memcmp_nn(a, p - a, b, q - b) == 0;
}
