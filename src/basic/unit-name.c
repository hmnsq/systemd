/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ "sd-id128.h"

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "glob-util.h"
#அந்தர்பாவ "hexdecoct.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "memory-util.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "sparse-endian.h"
#அந்தர்பாவ "special.h"
#அந்தர்பாவ "siphash24.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "unit-def.h"
#அந்தர்பாவ "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_சாவி SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(மாறிலி எழுத்து *n, UnitNameFlags flags) {
        மாறிலி எழுத்து *e, *i, *at;

        ப்ரதிஜ்ஞா((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        எனில் (_unlikely_(flags == 0))
                திரும்பு false;

        எனில் (isempty(n))
                திரும்பு false;

        எனில் (strlen(n) >= UNIT_NAME_MAX)
                திரும்பு false;

        e = strrchr(n, '.');
        எனில் (!e || e == n)
                திரும்பு false;

        எனில் (unit_type_from_string(e + 1) < 0)
                திரும்பு false;

        க்கு (i = n, at = வெற்று; i < e; i++) {

                எனில் (*i == '@' && !at)
                        at = i;

                எனில் (!strchr(VALID_CHARS_WITH_AT, *i))
                        திரும்பு false;
        }

        எனில் (at == n)
                திரும்பு false;

        எனில் (flags & UNIT_NAME_PLAIN)
                எனில் (!at)
                        திரும்பு true;

        எனில் (flags & UNIT_NAME_INSTANCE)
                எனில் (at && e > at + 1)
                        திரும்பு true;

        எனில் (flags & UNIT_NAME_TEMPLATE)
                எனில் (at && e == at + 1)
                        திரும்பு true;

        திரும்பு false;
}

bool unit_prefix_is_valid(மாறிலி எழுத்து *p) {

        /* We don't allow additional @ in the prefix string */

        எனில் (isempty(p))
                திரும்பு false;

        திரும்பு in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(மாறிலி எழுத்து *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        எனில் (isempty(i))
                திரும்பு false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        திரும்பு in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(மாறிலி எழுத்து *s) {
        எனில் (isempty(s))
                திரும்பு false;

        எனில் (s[0] != '.')
                திரும்பு false;

        எனில் (unit_type_from_string(s + 1) < 0)
                திரும்பு false;

        திரும்பு true;
}

முழு unit_name_to_prefix(மாறிலி எழுத்து *n, எழுத்து **ret) {
        மாறிலி எழுத்து *p;
        எழுத்து *s;

        ப்ரதிஜ்ஞா(n);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_name_is_valid(n, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        p = strchr(n, '@');
        எனில் (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        எனில் (!s)
                திரும்பு -ENOMEM;

        *ret = s;
        திரும்பு 0;
}

UnitNameFlags unit_name_to_instance(மாறிலி எழுத்து *n, எழுத்து **ret) {
        மாறிலி எழுத்து *p, *d;

        ப்ரதிஜ்ஞா(n);

        எனில் (!unit_name_is_valid(n, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        எனில் (!p) {
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        எனில் (!d)
                திரும்பு -EINVAL;

        எனில் (ret) {
                எழுத்து *i = strndup(p, d-p);
                எனில் (!i)
                        திரும்பு -ENOMEM;

                *ret = i;
        }
        திரும்பு d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

முழு unit_name_to_prefix_and_instance(மாறிலி எழுத்து *n, எழுத்து **ret) {
        மாறிலி எழுத்து *d;
        எழுத்து *s;

        ப்ரதிஜ்ஞா(n);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_name_is_valid(n, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        d = strrchr(n, '.');
        எனில் (!d)
                திரும்பு -EINVAL;

        s = strndup(n, d - n);
        எனில் (!s)
                திரும்பு -ENOMEM;

        *ret = s;
        திரும்பு 0;
}

UnitType unit_name_to_type(மாறிலி எழுத்து *n) {
        மாறிலி எழுத்து *e;

        ப்ரதிஜ்ஞா(n);

        எனில் (!unit_name_is_valid(n, UNIT_NAME_ANY))
                திரும்பு _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        திரும்பு unit_type_from_string(e + 1);
}

முழு unit_name_change_suffix(மாறிலி எழுத்து *n, மாறிலி எழுத்து *suffix, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        size_t a, b;
        மாறிலி எழுத்து *e;

        ப்ரதிஜ்ஞா(n);
        ப்ரதிஜ்ஞா(suffix);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_name_is_valid(n, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        எனில் (!unit_suffix_is_valid(suffix))
                திரும்பு -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(எழுத்து, a + b + 1);
        எனில் (!s)
                திரும்பு -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        எனில் (!unit_name_is_valid(s, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு unit_name_build(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *instance, மாறிலி எழுத்து *suffix, எழுத்து **ret) {
        UnitType type;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(suffix);
        ப்ரதிஜ்ஞா(ret);

        எனில் (suffix[0] != '.')
                திரும்பு -EINVAL;

        type = unit_type_from_string(suffix + 1);
        எனில் (type < 0)
                திரும்பு type;

        திரும்பு unit_name_build_from_type(prefix, instance, type, ret);
}

முழு unit_name_build_from_type(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *instance, UnitType type, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        மாறிலி எழுத்து *ut;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(type >= 0);
        ப்ரதிஜ்ஞா(type < _UNIT_TYPE_MAX);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_prefix_is_valid(prefix))
                திரும்பு -EINVAL;

        ut = unit_type_to_string(type);

        எனில் (instance) {
                எனில் (!unit_instance_is_valid(instance))
                        திரும்பு -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } இல்லையெனில்
                s = strjoin(prefix, ".", ut);
        எனில் (!s)
                திரும்பு -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        எனில் (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

நிலையான எழுத்து *do_escape_char(எழுத்து c, எழுத்து *t) {
        ப்ரதிஜ்ஞா(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        திரும்பு t;
}

நிலையான எழுத்து *do_escape(மாறிலி எழுத்து *f, எழுத்து *t) {
        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        எனில் (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        க்கு (; *f; f++) {
                எனில் (*f == '/')
                        *(t++) = '-';
                இல்லையெனில் எனில் (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                இல்லையெனில்
                        *(t++) = *f;
        }

        திரும்பு t;
}

எழுத்து* unit_name_escape(மாறிலி எழுத்து *f) {
        எழுத்து *r, *t;

        ப்ரதிஜ்ஞா(f);

        r = new(எழுத்து, strlen(f)*4+1);
        எனில் (!r)
                திரும்பு வெற்று;

        t = do_escape(f, r);
        *t = 0;

        திரும்பு r;
}

முழு unit_name_unescape(மாறிலி எழுத்து *f, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *r = வெற்று;
        எழுத்து *t;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret);

        r = strdup(f);
        எனில் (!r)
                திரும்பு -ENOMEM;

        க்கு (t = r; *f; f++) {
                எனில் (*f == '-')
                        *(t++) = '/';
                இல்லையெனில் எனில் (*f == '\\') {
                        முழு a, b;

                        எனில் (f[1] != 'x')
                                திரும்பு -EINVAL;

                        a = unhexchar(f[2]);
                        எனில் (a < 0)
                                திரும்பு -EINVAL;

                        b = unhexchar(f[3]);
                        எனில் (b < 0)
                                திரும்பு -EINVAL;

                        *(t++) = (எழுத்து) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } இல்லையெனில்
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        திரும்பு 0;
}

முழு unit_name_path_escape(மாறிலி எழுத்து *f, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று;
        எழுத்து *s;
        முழு r;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret);

        r = path_simplify_alloc(f, &p);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (empty_or_வேர்(p))
                s = strdup("-");
        இல்லையெனில் {
                எனில் (!path_is_normalized(p))
                        திரும்பு -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        எனில் (!s)
                திரும்பு -ENOMEM;

        *ret = s;
        திரும்பு 0;
}

முழு unit_name_path_unescape(மாறிலி எழுத்து *f, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(f);

        எனில் (isempty(f))
                திரும்பு -EINVAL;

        எனில் (streq(f, "-")) {
                s = strdup("/");
                எனில் (!s)
                        திரும்பு -ENOMEM;
        } இல்லையெனில் {
                _cleanup_free_ எழுத்து *w = வெற்று;

                r = unit_name_unescape(f, &w);
                எனில் (r < 0)
                        திரும்பு r;

                /* Don't accept trailing or leading slashes */
                எனில் (startswith(w, "/") || endswith(w, "/"))
                        திரும்பு -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                எனில் (!s)
                        திரும்பு -ENOMEM;

                எனில் (!path_is_normalized(s))
                        திரும்பு -EINVAL;
        }

        எனில் (ret)
                *ret = TAKE_PTR(s);

        திரும்பு 0;
}

முழு unit_name_replace_instance_full(
                மாறிலி எழுத்து *original,
                மாறிலி எழுத்து *instance,
                bool accept_glob,
                எழுத்து **ret) {

        _cleanup_free_ எழுத்து *s = வெற்று;
        மாறிலி எழுத்து *prefix, *suffix;
        size_t pl;

        ப்ரதிஜ்ஞா(original);
        ப்ரதிஜ்ஞா(instance);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                திரும்பு -EINVAL;
        எனில் (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                திரும்பு -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ப்ரதிஜ்ஞா(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(எழுத்து, pl + strlen(instance) + strlen(suffix) + 1);
        எனில் (!s)
                திரும்பு -ENOMEM;

#எனில் HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        எனில் (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு unit_name_template(மாறிலி எழுத்து *f, எழுத்து **ret) {
        மாறிலி எழுத்து *p, *e;
        எழுத்து *s;
        size_t a;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                திரும்பு -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(எழுத்து, a + 1 + strlen(e) + 1);
        எனில் (!s)
                திரும்பு -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        திரும்பு 0;
}

bool unit_name_is_hashed(மாறிலி எழுத்து *name) {
        மாறிலி எழுத்து *s;

        எனில் (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                திரும்பு false;

        assert_se(s = strrchr(name, '.'));

        எனில் (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                திரும்பு false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        எனில் (s[-1] != '_')
                திரும்பு false;

        க்கு (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                எனில் (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        திரும்பு false;

        திரும்பு true;
}

முழு unit_name_hash_long(மாறிலி எழுத்து *name, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *n = வெற்று, *hash = வெற்று;
        மாறிலி எழுத்து *suffix;
        le64_t h;
        size_t len;

        ப்ரதிஜ்ஞா(ret);

        எனில் (strlen(name) < UNIT_NAME_MAX)
                திரும்பு -EMSGSIZE;

        suffix = strrchr(name, '.');
        எனில் (!suffix)
                திரும்பு -EINVAL;

        எனில் (unit_type_from_string(suffix+1) < 0)
                திரும்பு -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_சாவி.bytes));

        hash = hexmem(&h, அளவு(h));
        எனில் (!hash)
                திரும்பு -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        ப்ரதிஜ்ஞா(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        எனில் (!n)
                திரும்பு -ENOMEM;

        எனில் (!strextend(&n, "_", hash, suffix))
                திரும்பு -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        திரும்பு 0;
}

முழு unit_name_from_path(மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று, *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(suffix);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_suffix_is_valid(suffix))
                திரும்பு -EINVAL;

        r = unit_name_path_escape(path, &p);
        எனில் (r < 0)
                திரும்பு r;

        s = strjoin(p, suffix);
        எனில் (!s)
                திரும்பு -ENOMEM;

        எனில் (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ எழுத்து *n = வெற்று;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                எனில் (r < 0)
                        திரும்பு r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        எனில் (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு unit_name_from_path_instance(மாறிலி எழுத்து *prefix, மாறிலி எழுத்து *path, மாறிலி எழுத்து *suffix, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *p = வெற்று, *s = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(prefix);
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(suffix);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!unit_prefix_is_valid(prefix))
                திரும்பு -EINVAL;

        எனில் (!unit_suffix_is_valid(suffix))
                திரும்பு -EINVAL;

        r = unit_name_path_escape(path, &p);
        எனில் (r < 0)
                திரும்பு r;

        s = strjoin(prefix, "@", p, suffix);
        எனில் (!s)
                திரும்பு -ENOMEM;

        எனில் (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                திரும்பு -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        எனில் (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 0;
}

முழு unit_name_to_path(மாறிலி எழுத்து *name, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *prefix = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(name);

        r = unit_name_to_prefix(name, &prefix);
        எனில் (r < 0)
                திரும்பு r;

        எனில் (unit_name_is_hashed(name))
                திரும்பு -ENAMETOOLONG;

        திரும்பு unit_name_path_unescape(prefix, ret);
}

நிலையான bool do_escape_mangle(மாறிலி எழுத்து *f, bool allow_globs, எழுத்து *t) {
        மாறிலி எழுத்து *valid_chars;
        bool mangled = false;

        ப்ரதிஜ்ஞா(f);
        ப்ரதிஜ்ஞா(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        க்கு (; *f; f++)
                எனில் (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } இல்லையெனில் எனில் (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } இல்லையெனில்
                        *(t++) = *f;
        *t = 0;

        திரும்பு mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
முழு unit_name_mangle_with_suffix(
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *operation,
                UnitNameMangle flags,
                மாறிலி எழுத்து *suffix,
                எழுத்து **ret) {

        _cleanup_free_ எழுத்து *s = வெற்று;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        முழு r;

        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(suffix);
        ப்ரதிஜ்ஞா(ret);

        எனில் (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                திரும்பு -EINVAL;

        எனில் (!unit_suffix_is_valid(suffix))
                திரும்பு -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        எனில் (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                எனில் (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        மாறிலி எழுத்து *e = ASSERT_PTR(strrchr(name, '.'));

                        திரும்பு log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                செல் good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        எனில் (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                எனில் (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        செல் good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        எனில் (path_is_absolute(name)) {
                _cleanup_free_ எழுத்து *n = வெற்று, *u = வெற்று;

                r = path_simplify_alloc(name, &n);
                எனில் (r < 0)
                        திரும்பு r;

                எனில் (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        எனில் (r >= 0) {
                                எனில் (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        திரும்பு log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                திரும்பு 1;
                        }
                        எனில் (r != -EINVAL)
                                திரும்பு r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                எனில் (r >= 0) {
                        எனில் (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                திரும்பு log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        திரும்பு 1;
                }
                எனில் (r != -EINVAL)
                        திரும்பு r;
        }

        s = new(எழுத்து, strlen(name) * 4 + strlen(suffix) + 1);
        எனில் (!s)
                திரும்பு -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        எனில் (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        எனில் ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        எனில் (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(s);
        திரும்பு 1;

good:
        திரும்பு strdup_to(ret, name);
}

முழு slice_build_parent_slice(மாறிலி எழுத்து *slice, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(slice);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!slice_name_is_valid(slice))
                திரும்பு -EINVAL;

        எனில் (streq(slice, SPECIAL_வேர்_SLICE)) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        _cleanup_free_ எழுத்து *s = strdup(slice);
        எனில் (!s)
                திரும்பு -ENOMEM;

        எழுத்து *dash = strrchr(s, '-');
        எனில் (!dash)
                திரும்பு strdup_to_full(ret, SPECIAL_வேர்_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        திரும்பு 1;
}

முழு slice_build_subslice(மாறிலி எழுத்து *slice, மாறிலி எழுத்து *name, எழுத்து **ret) {
        எழுத்து *subslice;

        ப்ரதிஜ்ஞா(slice);
        ப்ரதிஜ்ஞா(name);
        ப்ரதிஜ்ஞா(ret);

        எனில் (!slice_name_is_valid(slice))
                திரும்பு -EINVAL;

        எனில் (!unit_prefix_is_valid(name))
                திரும்பு -EINVAL;

        எனில் (streq(slice, SPECIAL_வேர்_SLICE))
                subslice = strjoin(name, ".slice");
        இல்லையெனில் {
                மாறிலி எழுத்து *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(எழுத்து, (e - slice) + 1 + strlen(name) + 6 + 1);
                எனில் (!subslice)
                        திரும்பு -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        திரும்பு 0;
}

bool slice_name_is_valid(மாறிலி எழுத்து *name) {
        மாறிலி எழுத்து *p, *e;
        bool dash = false;

        எனில் (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                திரும்பு false;

        எனில் (streq(name, SPECIAL_வேர்_SLICE))
                திரும்பு true;

        e = endswith(name, ".slice");
        எனில் (!e)
                திரும்பு false;

        க்கு (p = name; p < e; p++) {

                எனில் (*p == '-') {

                        /* Don't allow initial dash */
                        எனில் (p == name)
                                திரும்பு false;

                        /* Don't allow multiple dashes */
                        எனில் (dash)
                                திரும்பு false;

                        dash = true;
                } இல்லையெனில்
                        dash = false;
        }

        /* Don't allow trailing hash */
        எனில் (dash)
                திரும்பு false;

        திரும்பு true;
}

bool unit_name_prefix_equal(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        மாறிலி எழுத்து *p, *q;

        ப்ரதிஜ்ஞா(a);
        ப்ரதிஜ்ஞா(b);

        எனில் (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                திரும்பு false;

        p = strchr(a, '@');
        எனில் (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        எனில் (!q)
                q = strrchr(b, '.');

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(q);

        திரும்பு memcmp_nn(a, p - a, b, q - b) == 0;
}
