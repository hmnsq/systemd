/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن "sd-id128.h"

#ضمن "alloc-util.h"
#ضمن "glob-util.h"
#ضمن "hexdecoct.h"
#ضمن "log.h"
#ضمن "memory-util.h"
#ضمن "path-util.h"
#ضمن "sparse-endian.h"
#ضمن "special.h"
#ضمن "siphash24.h"
#ضمن "string-util.h"
#ضمن "unit-def.h"
#ضمن "unit-name.h"

/* Characters valid in a unit name. */
#تعریف_کړه VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#تعریف_کړه VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#تعریف_کړه VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#تعریف_کړه LONG_UNIT_NAME_HASH_کلی SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#تعریف_کړه UNIT_NAME_HASH_اوږدتیا_CHARS 16

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) {
        ثابت حرف *e, *i, *at;

        أكد((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        إذا (_unlikely_(flags == 0))
                أرجع false;

        إذا (isempty(n))
                أرجع false;

        إذا (مزي_اوږدوالی(n) >= UNIT_NAME_MAX)
                أرجع false;

        e = strrchr(n, '.');
        إذا (!e || e == n)
                أرجع false;

        إذا (unit_type_from_string(e + 1) < 0)
                أرجع false;

        لكل (i = n, at = عدم; i < e; i++) {

                إذا (*i == '@' && !at)
                        at = i;

                إذا (!strchr(VALID_CHARS_WITH_AT, *i))
                        أرجع false;
        }

        إذا (at == n)
                أرجع false;

        إذا (flags & UNIT_NAME_PLAIN)
                إذا (!at)
                        أرجع true;

        إذا (flags & UNIT_NAME_INSTANCE)
                إذا (at && e > at + 1)
                        أرجع true;

        إذا (flags & UNIT_NAME_TEMPLATE)
                إذا (at && e == at + 1)
                        أرجع true;

        أرجع false;
}

bool unit_مختاړی_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        إذا (isempty(p))
                أرجع false;

        أرجع دننه_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        إذا (isempty(i))
                أرجع false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        أرجع دننه_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        إذا (isempty(s))
                أرجع false;

        إذا (s[0] != '.')
                أرجع false;

        إذا (unit_type_from_string(s + 1) < 0)
                أرجع false;

        أرجع true;
}

صحيح unit_name_منزل_مختاړی(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        p = strchr(n, '@');
        إذا (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        إذا (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

UnitNameFlags unit_name_منزل_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        إذا (!p) {
                إذا (ret)
                        *ret = عدم;
                أرجع UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        إذا (!d)
                أرجع -EINVAL;

        إذا (ret) {
                حرف *i = strndup(p, d-p);
                إذا (!i)
                        أرجع -ENOMEM;

                *ret = i;
        }
        أرجع d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحيح unit_name_منزل_مختاړی_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        d = strrchr(n, '.');
        إذا (!d)
                أرجع -EINVAL;

        s = strndup(n, d - n);
        إذا (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

UnitType unit_name_منزل_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        أرجع unit_type_from_string(e + 1);
}

صحيح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_name_is_valid(n, UNIT_NAME_ANY))
                أرجع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = مزي_اوږدوالی(suffix);

        s = new(حرف, a + b + 1);
        إذا (!s)
                أرجع -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        إذا (!unit_name_is_valid(s, UNIT_NAME_ANY))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_build(ثابت حرف *مختاړی, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(مختاړی);
        أكد(suffix);
        أكد(ret);

        إذا (suffix[0] != '.')
                أرجع -EINVAL;

        type = unit_type_from_string(suffix + 1);
        إذا (type < 0)
                أرجع type;

        أرجع unit_name_build_from_type(مختاړی, instance, type, ret);
}

صحيح unit_name_build_from_type(ثابت حرف *مختاړی, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *ut;

        أكد(مختاړی);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        إذا (!unit_مختاړی_is_valid(مختاړی))
                أرجع -EINVAL;

        ut = unit_type_منزل_string(type);

        إذا (instance) {
                إذا (!unit_instance_is_valid(instance))
                        أرجع -EINVAL;

                s = strjoin(مختاړی, "@", instance, ".", ut);
        } وإلا
                s = strjoin(مختاړی, ".", ut);
        إذا (!s)
                أرجع -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        إذا (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

ساكن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        أرجع t;
}

ساكن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        إذا (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        لكل (; *f; f++) {
                إذا (*f == '/')
                        *(t++) = '-';
                وإلا إذا (دننه_سېټ(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                وإلا
                        *(t++) = *f;
        }

        أرجع t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, مزي_اوږدوالی(f)*4+1);
        إذا (!r)
                أرجع عدم;

        t = do_escape(f, r);
        *t = 0;

        أرجع r;
}

صحيح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = عدم;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        إذا (!r)
                أرجع -ENOMEM;

        لكل (t = r; *f; f++) {
                إذا (*f == '-')
                        *(t++) = '/';
                وإلا إذا (*f == '\\') {
                        صحيح a, b;

                        إذا (f[1] != 'x')
                                أرجع -EINVAL;

                        a = unhexchar(f[2]);
                        إذا (a < 0)
                                أرجع -EINVAL;

                        b = unhexchar(f[3]);
                        إذا (b < 0)
                                أرجع -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } وإلا
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        أرجع 0;
}

صحيح unit_name_لار_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = عدم;
        حرف *s;
        صحيح r;

        أكد(f);
        أكد(ret);

        r = لار_simplify_alloc(f, &p);
        إذا (r < 0)
                أرجع r;

        إذا (تشه_or_ریښه(p))
                s = strdup("-");
        وإلا {
                إذا (!لار_is_normalized(p))
                        أرجع -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        إذا (!s)
                أرجع -ENOMEM;

        *ret = s;
        أرجع 0;
}

صحيح unit_name_لار_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = عدم;
        صحيح r;

        أكد(f);

        إذا (isempty(f))
                أرجع -EINVAL;

        إذا (streq(f, "-")) {
                s = strdup("/");
                إذا (!s)
                        أرجع -ENOMEM;
        } وإلا {
                _cleanup_free_ حرف *w = عدم;

                r = unit_name_unescape(f, &w);
                إذا (r < 0)
                        أرجع r;

                /* Don't accept trailing or leading slashes */
                إذا (startswith(w, "/") || endswith(w, "/"))
                        أرجع -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                إذا (!s)
                        أرجع -ENOMEM;

                إذا (!لار_is_normalized(s))
                        أرجع -EINVAL;
        }

        إذا (ret)
                *ret = TAKE_PTR(s);

        أرجع 0;
}

صحيح unit_name_replace_instance_ډک(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret) {

        _cleanup_free_ حرف *s = عدم;
        ثابت حرف *مختاړی, *suffix;
        size_t pl;

        أكد(original);
        أكد(instance);
        أكد(ret);

        إذا (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                أرجع -EINVAL;
        إذا (!unit_instance_is_valid(instance) && !(accept_glob && دننه_charset(instance, VALID_CHARS_GLOB)))
                أرجع -EINVAL;

        مختاړی = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(مختاړی < suffix);

        pl = مختاړی - original + 1; /* include '@' */

        s = new(حرف, pl + مزي_اوږدوالی(instance) + مزي_اوږدوالی(suffix) + 1);
        إذا (!s)
                أرجع -ENOMEM;

#إذا HAS_FEATURE_حافظه_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + مزي_اوږدوالی(instance) + مزي_اوږدوالی(suffix) + 1);
#د_که_پای

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        إذا (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        إذا (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                أرجع -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + مزي_اوږدوالی(e) + 1);
        إذا (!s)
                أرجع -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        أرجع 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        إذا (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                أرجع false;

        assert_se(s = strrchr(name, '.'));

        إذا (s - name < UNIT_NAME_HASH_اوږدتیا_CHARS + 1)
                أرجع false;

        s -= UNIT_NAME_HASH_اوږدتیا_CHARS;
        إذا (s[-1] != '_')
                أرجع false;

        لكل (size_t i = 0; i < UNIT_NAME_HASH_اوږدتیا_CHARS; i++)
                إذا (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        أرجع false;

        أرجع true;
}

صحيح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = عدم, *hash = عدم;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        إذا (مزي_اوږدوالی(name) < UNIT_NAME_MAX)
                أرجع -EMSGSIZE;

        suffix = strrchr(name, '.');
        إذا (!suffix)
                أرجع -EINVAL;

        إذا (unit_type_from_string(suffix+1) < 0)
                أرجع -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_کلی.بایټونه));

        hash = hexmem(&h, حجم(h));
        إذا (!hash)
                أرجع -ENOMEM;

        assert_se(مزي_اوږدوالی(hash) == UNIT_NAME_HASH_اوږدتیا_CHARS);

        len = UNIT_NAME_MAX - 1 - مزي_اوږدوالی(suffix+1) - UNIT_NAME_HASH_اوږدتیا_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        إذا (!n)
                أرجع -ENOMEM;

        إذا (!strextend(&n, "_", hash, suffix))
                أرجع -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        أرجع 0;
}

صحيح unit_name_from_لار(ثابت حرف *لار, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(لار);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        r = unit_name_لار_escape(لار, &p);
        إذا (r < 0)
                أرجع r;

        s = strjoin(p, suffix);
        إذا (!s)
                أرجع -ENOMEM;

        إذا (مزي_اوږدوالی(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = عدم;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                إذا (r < 0)
                        أرجع r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        إذا (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_from_لار_instance(ثابت حرف *مختاړی, ثابت حرف *لار, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = عدم, *s = عدم;
        صحيح r;

        أكد(مختاړی);
        أكد(لار);
        أكد(suffix);
        أكد(ret);

        إذا (!unit_مختاړی_is_valid(مختاړی))
                أرجع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        r = unit_name_لار_escape(لار, &p);
        إذا (r < 0)
                أرجع r;

        s = strjoin(مختاړی, "@", p, suffix);
        إذا (!s)
                أرجع -ENOMEM;

        إذا (مزي_اوږدوالی(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                أرجع -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        إذا (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 0;
}

صحيح unit_name_منزل_لار(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *مختاړی = عدم;
        صحيح r;

        أكد(name);

        r = unit_name_منزل_مختاړی(name, &مختاړی);
        إذا (r < 0)
                أرجع r;

        إذا (unit_name_is_hashed(name))
                أرجع -ENAMETOOLONG;

        أرجع unit_name_لار_unescape(مختاړی, ret);
}

ساكن bool do_escape_mangle(ثابت حرف *f, bool allow_globs, حرف *t) {
        ثابت حرف *valid_chars;
        bool mangled = false;

        أكد(f);
        أكد(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        لكل (; *f; f++)
                إذا (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } وإلا إذا (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } وإلا
                        *(t++) = *f;
        *t = 0;

        أرجع mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
صحيح unit_name_mangle_with_suffix(
                ثابت حرف *name,
                ثابت حرف *operation,
                UnitNameMangle flags,
                ثابت حرف *suffix,
                حرف **ret) {

        _cleanup_free_ حرف *s = عدم;
        bool mangled, suggest_escape = true, warn = FLAGS_سېټ(flags, UNIT_NAME_MANGLE_WARN);
        صحيح r;

        أكد(name);
        أكد(suffix);
        أكد(ret);

        إذا (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                أرجع -EINVAL;

        إذا (!unit_suffix_is_valid(suffix))
                أرجع -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        إذا (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                إذا (FLAGS_سېټ(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        أرجع log_ډک_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                اذهب good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        إذا (string_is_glob(name) && دننه_charset(name, VALID_CHARS_GLOB)) {
                إذا (FLAGS_سېټ(flags, UNIT_NAME_MANGLE_GLOB))
                        اذهب good;
                log_ډک(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        إذا (لار_is_absolute(name)) {
                _cleanup_free_ حرف *n = عدم, *u = عدم;

                r = لار_simplify_alloc(name, &n);
                إذا (r < 0)
                        أرجع r;

                إذا (is_device_لار(n)) {
                        r = unit_name_from_لار(n, ".device", &u);
                        إذا (r >= 0) {
                                إذا (FLAGS_سېټ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        أرجع log_ډک_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                أرجع 1;
                        }
                        إذا (r != -EINVAL)
                                أرجع r;
                }

                r = unit_name_from_لار(n, ".mount", &u);
                إذا (r >= 0) {
                        إذا (FLAGS_سېټ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                أرجع log_ډک_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        أرجع 1;
                }
                إذا (r != -EINVAL)
                        أرجع r;
        }

        s = new(حرف, مزي_اوږدوالی(name) * 4 + مزي_اوږدوالی(suffix) + 1);
        إذا (!s)
                أرجع -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_سېټ(flags, UNIT_NAME_MANGLE_GLOB), s);
        إذا (mangled)
                log_ډک(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        إذا ((!FLAGS_سېټ(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_منزل_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        إذا (!FLAGS_سېټ(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                أرجع -EINVAL;

        *ret = TAKE_PTR(s);
        أرجع 1;

good:
        أرجع strdup_منزل(ret, name);
}

صحيح ټوټه_build_پلار_ټوټه(ثابت حرف *ټوټه, حرف **ret) {
        أكد(ټوټه);
        أكد(ret);

        إذا (!ټوټه_name_is_valid(ټوټه))
                أرجع -EINVAL;

        إذا (streq(ټوټه, SPECIAL_ریښه_ټوټه)) {
                *ret = عدم;
                أرجع 0;
        }

        _cleanup_free_ حرف *s = strdup(ټوټه);
        إذا (!s)
                أرجع -ENOMEM;

        حرف *dash = strrchr(s, '-');
        إذا (!dash)
                أرجع strdup_منزل_ډک(ret, SPECIAL_ریښه_ټوټه);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        أرجع 1;
}

صحيح ټوټه_build_subslice(ثابت حرف *ټوټه, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(ټوټه);
        أكد(name);
        أكد(ret);

        إذا (!ټوټه_name_is_valid(ټوټه))
                أرجع -EINVAL;

        إذا (!unit_مختاړی_is_valid(name))
                أرجع -EINVAL;

        إذا (streq(ټوټه, SPECIAL_ریښه_ټوټه))
                subslice = strjoin(name, ".slice");
        وإلا {
                ثابت حرف *e;

                assert_se(e = endswith(ټوټه, ".slice"));

                subslice = new(حرف, (e - ټوټه) + 1 + مزي_اوږدوالی(name) + 6 + 1);
                إذا (!subslice)
                        أرجع -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, ټوټه, e - ټوټه), "-"), name), ".slice");
        }

        *ret = subslice;
        أرجع 0;
}

bool ټوټه_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        إذا (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                أرجع false;

        إذا (streq(name, SPECIAL_ریښه_ټوټه))
                أرجع true;

        e = endswith(name, ".slice");
        إذا (!e)
                أرجع false;

        لكل (p = name; p < e; p++) {

                إذا (*p == '-') {

                        /* Don't allow initial dash */
                        إذا (p == name)
                                أرجع false;

                        /* Don't allow multiple dashes */
                        إذا (dash)
                                أرجع false;

                        dash = true;
                } وإلا
                        dash = false;
        }

        /* Don't allow trailing hash */
        إذا (dash)
                أرجع false;

        أرجع true;
}

bool unit_name_مختاړی_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        إذا (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                أرجع false;

        p = strchr(a, '@');
        إذا (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        إذا (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        أرجع memcmp_nn(a, p - a, b, q - b) == 0;
}
