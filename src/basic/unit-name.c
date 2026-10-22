/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل "sd-id128.h"

#شامل "alloc-util.h"
#شامل "glob-util.h"
#شامل "hexdecoct.h"
#شامل "log.h"
#شامل "memory-util.h"
#شامل "path-util.h"
#شامل "sparse-endian.h"
#شامل "special.h"
#شامل "siphash24.h"
#شامل "string-util.h"
#شامل "unit-def.h"
#شامل "unit-name.h"

/* Characters valid in a unit name. */
#تعریف_کرو VALID_CHARS                             \
        ALPHANUMERICAL                          \
        ":-_.\\"

/* The same, but also permits the single @ character that may appear */
#تعریف_کرو VALID_CHARS_WITH_AT                     \
        "@"                                     \
        VALID_CHARS

/* All chars valid in a unit name glob */
#تعریف_کرو VALID_CHARS_GLOB                        \
        VALID_CHARS_WITH_AT                     \
        "[]!-*?"

#تعریف_کرو LONG_UNIT_NAME_HASH_کلید SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#تعریف_کرو UNIT_NAME_HASH_لمبائی_CHARS 16

bool unit_name_is_valid(ثابت حرف *n, UnitNameFlags flags) {
        ثابت حرف *e, *i, *at;

        أكد((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        اگر (_unlikely_(flags == 0))
                واپس false;

        اگر (isempty(n))
                واپس false;

        اگر (لڑی_لمبائی(n) >= UNIT_NAME_MAX)
                واپس false;

        e = strrchr(n, '.');
        اگر (!e || e == n)
                واپس false;

        اگر (unit_type_from_string(e + 1) < 0)
                واپس false;

        برائے (i = n, at = تہی; i < e; i++) {

                اگر (*i == '@' && !at)
                        at = i;

                اگر (!strchr(VALID_CHARS_WITH_AT, *i))
                        واپس false;
        }

        اگر (at == n)
                واپس false;

        اگر (flags & UNIT_NAME_PLAIN)
                اگر (!at)
                        واپس true;

        اگر (flags & UNIT_NAME_INSTANCE)
                اگر (at && e > at + 1)
                        واپس true;

        اگر (flags & UNIT_NAME_TEMPLATE)
                اگر (at && e == at + 1)
                        واپس true;

        واپس false;
}

bool unit_سابقہ_is_valid(ثابت حرف *p) {

        /* We don't allow additional @ in the prefix string */

        اگر (isempty(p))
                واپس false;

        واپس اندر_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ثابت حرف *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        اگر (isempty(i))
                واپس false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        واپس اندر_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ثابت حرف *s) {
        اگر (isempty(s))
                واپس false;

        اگر (s[0] != '.')
                واپس false;

        اگر (unit_type_from_string(s + 1) < 0)
                واپس false;

        واپس true;
}

صحیح unit_name_مقصد_سابقہ(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p;
        حرف *s;

        أكد(n);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                واپس -EINVAL;

        p = strchr(n, '@');
        اگر (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        اگر (!s)
                واپس -ENOMEM;

        *ret = s;
        واپس 0;
}

UnitNameFlags unit_name_مقصد_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *p, *d;

        أكد(n);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                واپس -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        اگر (!p) {
                اگر (ret)
                        *ret = تہی;
                واپس UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        اگر (!d)
                واپس -EINVAL;

        اگر (ret) {
                حرف *i = strndup(p, d-p);
                اگر (!i)
                        واپس -ENOMEM;

                *ret = i;
        }
        واپس d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

صحیح unit_name_مقصد_سابقہ_and_instance(ثابت حرف *n, حرف **ret) {
        ثابت حرف *d;
        حرف *s;

        أكد(n);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                واپس -EINVAL;

        d = strrchr(n, '.');
        اگر (!d)
                واپس -EINVAL;

        s = strndup(n, d - n);
        اگر (!s)
                واپس -ENOMEM;

        *ret = s;
        واپس 0;
}

UnitType unit_name_مقصد_type(ثابت حرف *n) {
        ثابت حرف *e;

        أكد(n);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                واپس _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        واپس unit_type_from_string(e + 1);
}

صحیح unit_name_change_suffix(ثابت حرف *n, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        size_t a, b;
        ثابت حرف *e;

        أكد(n);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_name_is_valid(n, UNIT_NAME_ANY))
                واپس -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                واپس -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = لڑی_لمبائی(suffix);

        s = new(حرف, a + b + 1);
        اگر (!s)
                واپس -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        اگر (!unit_name_is_valid(s, UNIT_NAME_ANY))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح unit_name_build(ثابت حرف *سابقہ, ثابت حرف *instance, ثابت حرف *suffix, حرف **ret) {
        UnitType type;

        أكد(سابقہ);
        أكد(suffix);
        أكد(ret);

        اگر (suffix[0] != '.')
                واپس -EINVAL;

        type = unit_type_from_string(suffix + 1);
        اگر (type < 0)
                واپس type;

        واپس unit_name_build_from_type(سابقہ, instance, type, ret);
}

صحیح unit_name_build_from_type(ثابت حرف *سابقہ, ثابت حرف *instance, UnitType type, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        ثابت حرف *ut;

        أكد(سابقہ);
        أكد(type >= 0);
        أكد(type < _UNIT_TYPE_MAX);
        أكد(ret);

        اگر (!unit_سابقہ_is_valid(سابقہ))
                واپس -EINVAL;

        ut = unit_type_مقصد_string(type);

        اگر (instance) {
                اگر (!unit_instance_is_valid(instance))
                        واپس -EINVAL;

                s = strjoin(سابقہ, "@", instance, ".", ut);
        } ورنہ
                s = strjoin(سابقہ, ".", ut);
        اگر (!s)
                واپس -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        اگر (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 0;
}

ساکن حرف *do_escape_char(حرف c, حرف *t) {
        أكد(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        واپس t;
}

ساکن حرف *do_escape(ثابت حرف *f, حرف *t) {
        أكد(f);
        أكد(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        اگر (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        برائے (; *f; f++) {
                اگر (*f == '/')
                        *(t++) = '-';
                ورنہ اگر (اندر_سیٹ(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ورنہ
                        *(t++) = *f;
        }

        واپس t;
}

حرف* unit_name_escape(ثابت حرف *f) {
        حرف *r, *t;

        أكد(f);

        r = new(حرف, لڑی_لمبائی(f)*4+1);
        اگر (!r)
                واپس تہی;

        t = do_escape(f, r);
        *t = 0;

        واپس r;
}

صحیح unit_name_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *r = تہی;
        حرف *t;

        أكد(f);
        أكد(ret);

        r = strdup(f);
        اگر (!r)
                واپس -ENOMEM;

        برائے (t = r; *f; f++) {
                اگر (*f == '-')
                        *(t++) = '/';
                ورنہ اگر (*f == '\\') {
                        صحیح a, b;

                        اگر (f[1] != 'x')
                                واپس -EINVAL;

                        a = unhexchar(f[2]);
                        اگر (a < 0)
                                واپس -EINVAL;

                        b = unhexchar(f[3]);
                        اگر (b < 0)
                                واپس -EINVAL;

                        *(t++) = (حرف) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ورنہ
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        واپس 0;
}

صحیح unit_name_راستہ_escape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *p = تہی;
        حرف *s;
        صحیح r;

        أكد(f);
        أكد(ret);

        r = راستہ_simplify_alloc(f, &p);
        اگر (r < 0)
                واپس r;

        اگر (خالی_خانے_or_جڑ(p))
                s = strdup("-");
        ورنہ {
                اگر (!راستہ_is_normalized(p))
                        واپس -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        اگر (!s)
                واپس -ENOMEM;

        *ret = s;
        واپس 0;
}

صحیح unit_name_راستہ_unescape(ثابت حرف *f, حرف **ret) {
        _cleanup_free_ حرف *s = تہی;
        صحیح r;

        أكد(f);

        اگر (isempty(f))
                واپس -EINVAL;

        اگر (streq(f, "-")) {
                s = strdup("/");
                اگر (!s)
                        واپس -ENOMEM;
        } ورنہ {
                _cleanup_free_ حرف *w = تہی;

                r = unit_name_unescape(f, &w);
                اگر (r < 0)
                        واپس r;

                /* Don't accept trailing or leading slashes */
                اگر (startswith(w, "/") || endswith(w, "/"))
                        واپس -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                اگر (!s)
                        واپس -ENOMEM;

                اگر (!راستہ_is_normalized(s))
                        واپس -EINVAL;
        }

        اگر (ret)
                *ret = TAKE_PTR(s);

        واپس 0;
}

صحیح unit_name_replace_instance_بھرا(
                ثابت حرف *original,
                ثابت حرف *instance,
                bool accept_glob,
                حرف **ret) {

        _cleanup_free_ حرف *s = تہی;
        ثابت حرف *سابقہ, *suffix;
        size_t pl;

        أكد(original);
        أكد(instance);
        أكد(ret);

        اگر (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                واپس -EINVAL;
        اگر (!unit_instance_is_valid(instance) && !(accept_glob && اندر_charset(instance, VALID_CHARS_GLOB)))
                واپس -EINVAL;

        سابقہ = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        أكد(سابقہ < suffix);

        pl = سابقہ - original + 1; /* include '@' */

        s = new(حرف, pl + لڑی_لمبائی(instance) + لڑی_لمبائی(suffix) + 1);
        اگر (!s)
                واپس -ENOMEM;

#اگر HAS_FEATURE_حافظہ_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + لڑی_لمبائی(instance) + لڑی_لمبائی(suffix) + 1);
#اگر_ختم

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        اگر (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح unit_name_template(ثابت حرف *f, حرف **ret) {
        ثابت حرف *p, *e;
        حرف *s;
        size_t a;

        أكد(f);
        أكد(ret);

        اگر (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                واپس -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(حرف, a + 1 + لڑی_لمبائی(e) + 1);
        اگر (!s)
                واپس -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        واپس 0;
}

bool unit_name_is_hashed(ثابت حرف *name) {
        ثابت حرف *s;

        اگر (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                واپس false;

        assert_se(s = strrchr(name, '.'));

        اگر (s - name < UNIT_NAME_HASH_لمبائی_CHARS + 1)
                واپس false;

        s -= UNIT_NAME_HASH_لمبائی_CHARS;
        اگر (s[-1] != '_')
                واپس false;

        برائے (size_t i = 0; i < UNIT_NAME_HASH_لمبائی_CHARS; i++)
                اگر (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        واپس false;

        واپس true;
}

صحیح unit_name_hash_long(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *n = تہی, *hash = تہی;
        ثابت حرف *suffix;
        le64_t h;
        size_t len;

        أكد(ret);

        اگر (لڑی_لمبائی(name) < UNIT_NAME_MAX)
                واپس -EMSGSIZE;

        suffix = strrchr(name, '.');
        اگر (!suffix)
                واپس -EINVAL;

        اگر (unit_type_from_string(suffix+1) < 0)
                واپس -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_کلید.بائٹیں));

        hash = hexmem(&h, حجم(h));
        اگر (!hash)
                واپس -ENOMEM;

        assert_se(لڑی_لمبائی(hash) == UNIT_NAME_HASH_لمبائی_CHARS);

        len = UNIT_NAME_MAX - 1 - لڑی_لمبائی(suffix+1) - UNIT_NAME_HASH_لمبائی_CHARS - 2;
        أكد(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        اگر (!n)
                واپس -ENOMEM;

        اگر (!strextend(&n, "_", hash, suffix))
                واپس -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        واپس 0;
}

صحیح unit_name_from_راستہ(ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = تہی, *s = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_suffix_is_valid(suffix))
                واپس -EINVAL;

        r = unit_name_راستہ_escape(راستہ, &p);
        اگر (r < 0)
                واپس r;

        s = strjoin(p, suffix);
        اگر (!s)
                واپس -ENOMEM;

        اگر (لڑی_لمبائی(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ حرف *n = تہی;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                اگر (r < 0)
                        واپس r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        اگر (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح unit_name_from_راستہ_instance(ثابت حرف *سابقہ, ثابت حرف *راستہ, ثابت حرف *suffix, حرف **ret) {
        _cleanup_free_ حرف *p = تہی, *s = تہی;
        صحیح r;

        أكد(سابقہ);
        أكد(راستہ);
        أكد(suffix);
        أكد(ret);

        اگر (!unit_سابقہ_is_valid(سابقہ))
                واپس -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                واپس -EINVAL;

        r = unit_name_راستہ_escape(راستہ, &p);
        اگر (r < 0)
                واپس r;

        s = strjoin(سابقہ, "@", p, suffix);
        اگر (!s)
                واپس -ENOMEM;

        اگر (لڑی_لمبائی(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                واپس -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        اگر (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 0;
}

صحیح unit_name_مقصد_راستہ(ثابت حرف *name, حرف **ret) {
        _cleanup_free_ حرف *سابقہ = تہی;
        صحیح r;

        أكد(name);

        r = unit_name_مقصد_سابقہ(name, &سابقہ);
        اگر (r < 0)
                واپس r;

        اگر (unit_name_is_hashed(name))
                واپس -ENAMETOOLONG;

        واپس unit_name_راستہ_unescape(سابقہ, ret);
}

ساکن bool do_escape_mangle(ثابت حرف *f, bool allow_globs, حرف *t) {
        ثابت حرف *valid_chars;
        bool mangled = false;

        أكد(f);
        أكد(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        برائے (; *f; f++)
                اگر (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ورنہ اگر (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ورنہ
                        *(t++) = *f;
        *t = 0;

        واپس mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
صحیح unit_name_mangle_with_suffix(
                ثابت حرف *name,
                ثابت حرف *operation,
                UnitNameMangle flags,
                ثابت حرف *suffix,
                حرف **ret) {

        _cleanup_free_ حرف *s = تہی;
        bool mangled, suggest_escape = true, warn = FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_WARN);
        صحیح r;

        أكد(name);
        أكد(suffix);
        أكد(ret);

        اگر (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                واپس -EINVAL;

        اگر (!unit_suffix_is_valid(suffix))
                واپس -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        اگر (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                اگر (FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ثابت حرف *e = ASSERT_PTR(strrchr(name, '.'));

                        واپس log_بھرا_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                جاؤ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        اگر (string_is_glob(name) && اندر_charset(name, VALID_CHARS_GLOB)) {
                اگر (FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_GLOB))
                        جاؤ good;
                log_بھرا(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        اگر (راستہ_is_absolute(name)) {
                _cleanup_free_ حرف *n = تہی, *u = تہی;

                r = راستہ_simplify_alloc(name, &n);
                اگر (r < 0)
                        واپس r;

                اگر (is_device_راستہ(n)) {
                        r = unit_name_from_راستہ(n, ".device", &u);
                        اگر (r >= 0) {
                                اگر (FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        واپس log_بھرا_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                واپس 1;
                        }
                        اگر (r != -EINVAL)
                                واپس r;
                }

                r = unit_name_from_راستہ(n, ".mount", &u);
                اگر (r >= 0) {
                        اگر (FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                واپس log_بھرا_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        واپس 1;
                }
                اگر (r != -EINVAL)
                        واپس r;
        }

        s = new(حرف, لڑی_لمبائی(name) * 4 + لڑی_لمبائی(suffix) + 1);
        اگر (!s)
                واپس -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_GLOB), s);
        اگر (mangled)
                log_بھرا(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        اگر ((!FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_مقصد_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        اگر (!FLAGS_سیٹ(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                واپس -EINVAL;

        *ret = TAKE_PTR(s);
        واپس 1;

good:
        واپس strdup_مقصد(ret, name);
}

صحیح قاش_build_والد_قاش(ثابت حرف *قاش, حرف **ret) {
        أكد(قاش);
        أكد(ret);

        اگر (!قاش_name_is_valid(قاش))
                واپس -EINVAL;

        اگر (streq(قاش, SPECIAL_جڑ_قاش)) {
                *ret = تہی;
                واپس 0;
        }

        _cleanup_free_ حرف *s = strdup(قاش);
        اگر (!s)
                واپس -ENOMEM;

        حرف *dash = strrchr(s, '-');
        اگر (!dash)
                واپس strdup_مقصد_بھرا(ret, SPECIAL_جڑ_قاش);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        واپس 1;
}

صحیح قاش_build_subslice(ثابت حرف *قاش, ثابت حرف *name, حرف **ret) {
        حرف *subslice;

        أكد(قاش);
        أكد(name);
        أكد(ret);

        اگر (!قاش_name_is_valid(قاش))
                واپس -EINVAL;

        اگر (!unit_سابقہ_is_valid(name))
                واپس -EINVAL;

        اگر (streq(قاش, SPECIAL_جڑ_قاش))
                subslice = strjoin(name, ".slice");
        ورنہ {
                ثابت حرف *e;

                assert_se(e = endswith(قاش, ".slice"));

                subslice = new(حرف, (e - قاش) + 1 + لڑی_لمبائی(name) + 6 + 1);
                اگر (!subslice)
                        واپس -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, قاش, e - قاش), "-"), name), ".slice");
        }

        *ret = subslice;
        واپس 0;
}

bool قاش_name_is_valid(ثابت حرف *name) {
        ثابت حرف *p, *e;
        bool dash = false;

        اگر (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                واپس false;

        اگر (streq(name, SPECIAL_جڑ_قاش))
                واپس true;

        e = endswith(name, ".slice");
        اگر (!e)
                واپس false;

        برائے (p = name; p < e; p++) {

                اگر (*p == '-') {

                        /* Don't allow initial dash */
                        اگر (p == name)
                                واپس false;

                        /* Don't allow multiple dashes */
                        اگر (dash)
                                واپس false;

                        dash = true;
                } ورنہ
                        dash = false;
        }

        /* Don't allow trailing hash */
        اگر (dash)
                واپس false;

        واپس true;
}

bool unit_name_سابقہ_equal(ثابت حرف *a, ثابت حرف *b) {
        ثابت حرف *p, *q;

        أكد(a);
        أكد(b);

        اگر (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                واپس false;

        p = strchr(a, '@');
        اگر (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        اگر (!q)
                q = strrchr(b, '.');

        أكد(p);
        أكد(q);

        واپس memcmp_nn(a, p - a, b, q - b) == 0;
}
