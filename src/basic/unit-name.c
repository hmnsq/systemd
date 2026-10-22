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

        ถ้า (_unlikely_(flags == 0))
                คืน false;

        ถ้า (isempty(n))
                คืน false;

        ถ้า (strlen(n) >= UNIT_NAME_MAX)
                คืน false;

        e = strrchr(n, '.');
        ถ้า (!e || e == n)
                คืน false;

        ถ้า (unit_type_from_string(e + 1) < 0)
                คืน false;

        สำหรับ (i = n, at = NULL; i < e; i++) {

                ถ้า (*i == '@' && !at)
                        at = i;

                ถ้า (!strchr(VALID_CHARS_WITH_AT, *i))
                        คืน false;
        }

        ถ้า (at == n)
                คืน false;

        ถ้า (flags & UNIT_NAME_PLAIN)
                ถ้า (!at)
                        คืน true;

        ถ้า (flags & UNIT_NAME_INSTANCE)
                ถ้า (at && e > at + 1)
                        คืน true;

        ถ้า (flags & UNIT_NAME_TEMPLATE)
                ถ้า (at && e == at + 1)
                        คืน true;

        คืน false;
}

bool unit_prefix_is_valid(const var_na *p) {

        /* We don't allow additional @ in the prefix string */

        ถ้า (isempty(p))
                คืน false;

        คืน in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(const var_na *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ถ้า (isempty(i))
                คืน false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        คืน in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(const var_na *s) {
        ถ้า (isempty(s))
                คืน false;

        ถ้า (s[0] != '.')
                คืน false;

        ถ้า (unit_type_from_string(s + 1) < 0)
                คืน false;

        คืน true;
}

จำนวนเต็ม unit_name_to_prefix(const var_na *n, var_na **ret) {
        const var_na *p;
        var_na *s;

        assert(n);
        assert(ret);

        ถ้า (!unit_name_is_valid(n, UNIT_NAME_ANY))
                คืน -EINVAL;

        p = strchr(n, '@');
        ถ้า (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ถ้า (!s)
                คืน -ENOMEM;

        *ret = s;
        คืน 0;
}

UnitNameFlags unit_name_to_instance(const var_na *n, var_na **ret) {
        const var_na *p, *d;

        assert(n);

        ถ้า (!unit_name_is_valid(n, UNIT_NAME_ANY))
                คืน -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ถ้า (!p) {
                ถ้า (ret)
                        *ret = NULL;
                คืน UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ถ้า (!d)
                คืน -EINVAL;

        ถ้า (ret) {
                var_na *i = strndup(p, d-p);
                ถ้า (!i)
                        คืน -ENOMEM;

                *ret = i;
        }
        คืน d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

จำนวนเต็ม unit_name_to_prefix_and_instance(const var_na *n, var_na **ret) {
        const var_na *d;
        var_na *s;

        assert(n);
        assert(ret);

        ถ้า (!unit_name_is_valid(n, UNIT_NAME_ANY))
                คืน -EINVAL;

        d = strrchr(n, '.');
        ถ้า (!d)
                คืน -EINVAL;

        s = strndup(n, d - n);
        ถ้า (!s)
                คืน -ENOMEM;

        *ret = s;
        คืน 0;
}

UnitType unit_name_to_type(const var_na *n) {
        const var_na *e;

        assert(n);

        ถ้า (!unit_name_is_valid(n, UNIT_NAME_ANY))
                คืน _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        คืน unit_type_from_string(e + 1);
}

จำนวนเต็ม unit_name_change_suffix(const var_na *n, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        size_t a, b;
        const var_na *e;

        assert(n);
        assert(suffix);
        assert(ret);

        ถ้า (!unit_name_is_valid(n, UNIT_NAME_ANY))
                คืน -EINVAL;

        ถ้า (!unit_suffix_is_valid(suffix))
                คืน -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(var_na, a + b + 1);
        ถ้า (!s)
                คืน -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ถ้า (!unit_name_is_valid(s, UNIT_NAME_ANY))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม unit_name_build(const var_na *prefix, const var_na *instance, const var_na *suffix, var_na **ret) {
        UnitType type;

        assert(prefix);
        assert(suffix);
        assert(ret);

        ถ้า (suffix[0] != '.')
                คืน -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ถ้า (type < 0)
                คืน type;

        คืน unit_name_build_from_type(prefix, instance, type, ret);
}

จำนวนเต็ม unit_name_build_from_type(const var_na *prefix, const var_na *instance, UnitType type, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        const var_na *ut;

        assert(prefix);
        assert(type >= 0);
        assert(type < _UNIT_TYPE_MAX);
        assert(ret);

        ถ้า (!unit_prefix_is_valid(prefix))
                คืน -EINVAL;

        ut = unit_type_to_string(type);

        ถ้า (instance) {
                ถ้า (!unit_instance_is_valid(instance))
                        คืน -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } มิฉะนั้น
                s = strjoin(prefix, ".", ut);
        ถ้า (!s)
                คืน -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ถ้า (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 0;
}

static var_na *do_escape_char(var_na c, var_na *t) {
        assert(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        คืน t;
}

static var_na *do_escape(const var_na *f, var_na *t) {
        assert(f);
        assert(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ถ้า (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        สำหรับ (; *f; f++) {
                ถ้า (*f == '/')
                        *(t++) = '-';
                มิฉะนั้น ถ้า (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                มิฉะนั้น
                        *(t++) = *f;
        }

        คืน t;
}

var_na* unit_name_escape(const var_na *f) {
        var_na *r, *t;

        assert(f);

        r = new(var_na, strlen(f)*4+1);
        ถ้า (!r)
                คืน NULL;

        t = do_escape(f, r);
        *t = 0;

        คืน r;
}

จำนวนเต็ม unit_name_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *r = NULL;
        var_na *t;

        assert(f);
        assert(ret);

        r = strdup(f);
        ถ้า (!r)
                คืน -ENOMEM;

        สำหรับ (t = r; *f; f++) {
                ถ้า (*f == '-')
                        *(t++) = '/';
                มิฉะนั้น ถ้า (*f == '\\') {
                        จำนวนเต็ม a, b;

                        ถ้า (f[1] != 'x')
                                คืน -EINVAL;

                        a = unhexchar(f[2]);
                        ถ้า (a < 0)
                                คืน -EINVAL;

                        b = unhexchar(f[3]);
                        ถ้า (b < 0)
                                คืน -EINVAL;

                        *(t++) = (var_na) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } มิฉะนั้น
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        คืน 0;
}

จำนวนเต็ม unit_name_path_escape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *p = NULL;
        var_na *s;
        จำนวนเต็ม r;

        assert(f);
        assert(ret);

        r = path_simplify_alloc(f, &p);
        ถ้า (r < 0)
                คืน r;

        ถ้า (empty_or_root(p))
                s = strdup("-");
        มิฉะนั้น {
                ถ้า (!path_is_normalized(p))
                        คืน -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ถ้า (!s)
                คืน -ENOMEM;

        *ret = s;
        คืน 0;
}

จำนวนเต็ม unit_name_path_unescape(const var_na *f, var_na **ret) {
        _cleanup_free_ var_na *s = NULL;
        จำนวนเต็ม r;

        assert(f);

        ถ้า (isempty(f))
                คืน -EINVAL;

        ถ้า (streq(f, "-")) {
                s = strdup("/");
                ถ้า (!s)
                        คืน -ENOMEM;
        } มิฉะนั้น {
                _cleanup_free_ var_na *w = NULL;

                r = unit_name_unescape(f, &w);
                ถ้า (r < 0)
                        คืน r;

                /* Don't accept trailing or leading slashes */
                ถ้า (startswith(w, "/") || endswith(w, "/"))
                        คืน -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ถ้า (!s)
                        คืน -ENOMEM;

                ถ้า (!path_is_normalized(s))
                        คืน -EINVAL;
        }

        ถ้า (ret)
                *ret = TAKE_PTR(s);

        คืน 0;
}

จำนวนเต็ม unit_name_replace_instance_full(
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

        ถ้า (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                คืน -EINVAL;
        ถ้า (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                คืน -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        assert(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(var_na, pl + strlen(instance) + strlen(suffix) + 1);
        ถ้า (!s)
                คืน -ENOMEM;

#ถ้า HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ถ้า (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม unit_name_template(const var_na *f, var_na **ret) {
        const var_na *p, *e;
        var_na *s;
        size_t a;

        assert(f);
        assert(ret);

        ถ้า (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                คืน -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(var_na, a + 1 + strlen(e) + 1);
        ถ้า (!s)
                คืน -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        คืน 0;
}

bool unit_name_is_hashed(const var_na *name) {
        const var_na *s;

        ถ้า (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                คืน false;

        assert_se(s = strrchr(name, '.'));

        ถ้า (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                คืน false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ถ้า (s[-1] != '_')
                คืน false;

        สำหรับ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ถ้า (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        คืน false;

        คืน true;
}

จำนวนเต็ม unit_name_hash_long(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *n = NULL, *hash = NULL;
        const var_na *suffix;
        le64_t h;
        size_t len;

        assert(ret);

        ถ้า (strlen(name) < UNIT_NAME_MAX)
                คืน -EMSGSIZE;

        suffix = strrchr(name, '.');
        ถ้า (!suffix)
                คืน -EINVAL;

        ถ้า (unit_type_from_string(suffix+1) < 0)
                คืน -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_KEY.bytes));

        hash = hexmem(&h, parimaa_na(h));
        ถ้า (!hash)
                คืน -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        assert(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ถ้า (!n)
                คืน -ENOMEM;

        ถ้า (!strextend(&n, "_", hash, suffix))
                คืน -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        คืน 0;
}

จำนวนเต็ม unit_name_from_path(const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        จำนวนเต็ม r;

        assert(path);
        assert(suffix);
        assert(ret);

        ถ้า (!unit_suffix_is_valid(suffix))
                คืน -EINVAL;

        r = unit_name_path_escape(path, &p);
        ถ้า (r < 0)
                คืน r;

        s = strjoin(p, suffix);
        ถ้า (!s)
                คืน -ENOMEM;

        ถ้า (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ var_na *n = NULL;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ถ้า (r < 0)
                        คืน r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ถ้า (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม unit_name_from_path_instance(const var_na *prefix, const var_na *path, const var_na *suffix, var_na **ret) {
        _cleanup_free_ var_na *p = NULL, *s = NULL;
        จำนวนเต็ม r;

        assert(prefix);
        assert(path);
        assert(suffix);
        assert(ret);

        ถ้า (!unit_prefix_is_valid(prefix))
                คืน -EINVAL;

        ถ้า (!unit_suffix_is_valid(suffix))
                คืน -EINVAL;

        r = unit_name_path_escape(path, &p);
        ถ้า (r < 0)
                คืน r;

        s = strjoin(prefix, "@", p, suffix);
        ถ้า (!s)
                คืน -ENOMEM;

        ถ้า (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                คืน -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ถ้า (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 0;
}

จำนวนเต็ม unit_name_to_path(const var_na *name, var_na **ret) {
        _cleanup_free_ var_na *prefix = NULL;
        จำนวนเต็ม r;

        assert(name);

        r = unit_name_to_prefix(name, &prefix);
        ถ้า (r < 0)
                คืน r;

        ถ้า (unit_name_is_hashed(name))
                คืน -ENAMETOOLONG;

        คืน unit_name_path_unescape(prefix, ret);
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

        สำหรับ (; *f; f++)
                ถ้า (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } มิฉะนั้น ถ้า (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } มิฉะนั้น
                        *(t++) = *f;
        *t = 0;

        คืน mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
จำนวนเต็ม unit_name_mangle_with_suffix(
                const var_na *name,
                const var_na *operation,
                UnitNameMangle flags,
                const var_na *suffix,
                var_na **ret) {

        _cleanup_free_ var_na *s = NULL;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        จำนวนเต็ม r;

        assert(name);
        assert(suffix);
        assert(ret);

        ถ้า (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                คืน -EINVAL;

        ถ้า (!unit_suffix_is_valid(suffix))
                คืน -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ถ้า (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ถ้า (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        const var_na *e = ASSERT_PTR(strrchr(name, '.'));

                        คืน log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                goto good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ถ้า (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ถ้า (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        goto good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ถ้า (path_is_absolute(name)) {
                _cleanup_free_ var_na *n = NULL, *u = NULL;

                r = path_simplify_alloc(name, &n);
                ถ้า (r < 0)
                        คืน r;

                ถ้า (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ถ้า (r >= 0) {
                                ถ้า (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        คืน log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                คืน 1;
                        }
                        ถ้า (r != -EINVAL)
                                คืน r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ถ้า (r >= 0) {
                        ถ้า (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                คืน log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        คืน 1;
                }
                ถ้า (r != -EINVAL)
                        คืน r;
        }

        s = new(var_na, strlen(name) * 4 + strlen(suffix) + 1);
        ถ้า (!s)
                คืน -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ถ้า (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ถ้า ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ถ้า (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                คืน -EINVAL;

        *ret = TAKE_PTR(s);
        คืน 1;

good:
        คืน strdup_to(ret, name);
}

จำนวนเต็ม slice_build_parent_slice(const var_na *slice, var_na **ret) {
        assert(slice);
        assert(ret);

        ถ้า (!slice_name_is_valid(slice))
                คืน -EINVAL;

        ถ้า (streq(slice, SPECIAL_ROOT_SLICE)) {
                *ret = NULL;
                คืน 0;
        }

        _cleanup_free_ var_na *s = strdup(slice);
        ถ้า (!s)
                คืน -ENOMEM;

        var_na *dash = strrchr(s, '-');
        ถ้า (!dash)
                คืน strdup_to_full(ret, SPECIAL_ROOT_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        คืน 1;
}

จำนวนเต็ม slice_build_subslice(const var_na *slice, const var_na *name, var_na **ret) {
        var_na *subslice;

        assert(slice);
        assert(name);
        assert(ret);

        ถ้า (!slice_name_is_valid(slice))
                คืน -EINVAL;

        ถ้า (!unit_prefix_is_valid(name))
                คืน -EINVAL;

        ถ้า (streq(slice, SPECIAL_ROOT_SLICE))
                subslice = strjoin(name, ".slice");
        มิฉะนั้น {
                const var_na *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(var_na, (e - slice) + 1 + strlen(name) + 6 + 1);
                ถ้า (!subslice)
                        คืน -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        คืน 0;
}

bool slice_name_is_valid(const var_na *name) {
        const var_na *p, *e;
        bool dash = false;

        ถ้า (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                คืน false;

        ถ้า (streq(name, SPECIAL_ROOT_SLICE))
                คืน true;

        e = endswith(name, ".slice");
        ถ้า (!e)
                คืน false;

        สำหรับ (p = name; p < e; p++) {

                ถ้า (*p == '-') {

                        /* Don't allow initial dash */
                        ถ้า (p == name)
                                คืน false;

                        /* Don't allow multiple dashes */
                        ถ้า (dash)
                                คืน false;

                        dash = true;
                } มิฉะนั้น
                        dash = false;
        }

        /* Don't allow trailing hash */
        ถ้า (dash)
                คืน false;

        คืน true;
}

bool unit_name_prefix_equal(const var_na *a, const var_na *b) {
        const var_na *p, *q;

        assert(a);
        assert(b);

        ถ้า (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                คืน false;

        p = strchr(a, '@');
        ถ้า (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ถ้า (!q)
                q = strrchr(b, '.');

        assert(p);
        assert(q);

        คืน memcmp_nn(a, p - a, b, q - b) == 0;
}
