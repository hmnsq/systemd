/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव "sd-id128.h"

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "hexdecoct.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "memory-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "sparse-endian.h"
#अन्तर्भाव "special.h"
#अन्तर्भाव "siphash24.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "unit-def.h"
#अन्तर्भाव "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_कुञ्चिका SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ध्रुव वर्ण *n, UnitNameFlags flags) {
        ध्रुव वर्ण *e, *i, *at;

        प्रतिज्ञा((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        सचे (_unlikely_(flags == 0))
                पटिनिवत्त false;

        सचे (isempty(n))
                पटिनिवत्त false;

        सचे (strlen(n) >= UNIT_NAME_MAX)
                पटिनिवत्त false;

        e = strrchr(n, '.');
        सचे (!e || e == n)
                पटिनिवत्त false;

        सचे (unit_type_from_string(e + 1) < 0)
                पटिनिवत्त false;

        अत्थाय (i = n, at = रिक्त; i < e; i++) {

                सचे (*i == '@' && !at)
                        at = i;

                सचे (!strchr(VALID_CHARS_WITH_AT, *i))
                        पटिनिवत्त false;
        }

        सचे (at == n)
                पटिनिवत्त false;

        सचे (flags & UNIT_NAME_PLAIN)
                सचे (!at)
                        पटिनिवत्त true;

        सचे (flags & UNIT_NAME_INSTANCE)
                सचे (at && e > at + 1)
                        पटिनिवत्त true;

        सचे (flags & UNIT_NAME_TEMPLATE)
                सचे (at && e == at + 1)
                        पटिनिवत्त true;

        पटिनिवत्त false;
}

bool unit_prefix_is_valid(ध्रुव वर्ण *p) {

        /* We don't allow additional @ in the prefix string */

        सचे (isempty(p))
                पटिनिवत्त false;

        पटिनिवत्त in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ध्रुव वर्ण *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        सचे (isempty(i))
                पटिनिवत्त false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        पटिनिवत्त in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ध्रुव वर्ण *s) {
        सचे (isempty(s))
                पटिनिवत्त false;

        सचे (s[0] != '.')
                पटिनिवत्त false;

        सचे (unit_type_from_string(s + 1) < 0)
                पटिनिवत्त false;

        पटिनिवत्त true;
}

पूर्णाङ्क unit_name_to_prefix(ध्रुव वर्ण *n, वर्ण **ret) {
        ध्रुव वर्ण *p;
        वर्ण *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        सचे (!unit_name_is_valid(n, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        p = strchr(n, '@');
        सचे (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        *ret = s;
        पटिनिवत्त 0;
}

UnitNameFlags unit_name_to_instance(ध्रुव वर्ण *n, वर्ण **ret) {
        ध्रुव वर्ण *p, *d;

        प्रतिज्ञा(n);

        सचे (!unit_name_is_valid(n, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        सचे (!p) {
                सचे (ret)
                        *ret = रिक्त;
                पटिनिवत्त UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        सचे (!d)
                पटिनिवत्त -EINVAL;

        सचे (ret) {
                वर्ण *i = strndup(p, d-p);
                सचे (!i)
                        पटिनिवत्त -ENOMEM;

                *ret = i;
        }
        पटिनिवत्त d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

पूर्णाङ्क unit_name_to_prefix_and_instance(ध्रुव वर्ण *n, वर्ण **ret) {
        ध्रुव वर्ण *d;
        वर्ण *s;

        प्रतिज्ञा(n);
        प्रतिज्ञा(ret);

        सचे (!unit_name_is_valid(n, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        d = strrchr(n, '.');
        सचे (!d)
                पटिनिवत्त -EINVAL;

        s = strndup(n, d - n);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        *ret = s;
        पटिनिवत्त 0;
}

UnitType unit_name_to_type(ध्रुव वर्ण *n) {
        ध्रुव वर्ण *e;

        प्रतिज्ञा(n);

        सचे (!unit_name_is_valid(n, UNIT_NAME_ANY))
                पटिनिवत्त _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        पटिनिवत्त unit_type_from_string(e + 1);
}

पूर्णाङ्क unit_name_change_suffix(ध्रुव वर्ण *n, ध्रुव वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        size_t a, b;
        ध्रुव वर्ण *e;

        प्रतिज्ञा(n);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        सचे (!unit_name_is_valid(n, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        सचे (!unit_suffix_is_valid(suffix))
                पटिनिवत्त -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(वर्ण, a + b + 1);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        सचे (!unit_name_is_valid(s, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_build(ध्रुव वर्ण *prefix, ध्रुव वर्ण *instance, ध्रुव वर्ण *suffix, वर्ण **ret) {
        UnitType type;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        सचे (suffix[0] != '.')
                पटिनिवत्त -EINVAL;

        type = unit_type_from_string(suffix + 1);
        सचे (type < 0)
                पटिनिवत्त type;

        पटिनिवत्त unit_name_build_from_type(prefix, instance, type, ret);
}

पूर्णाङ्क unit_name_build_from_type(ध्रुव वर्ण *prefix, ध्रुव वर्ण *instance, UnitType type, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        ध्रुव वर्ण *ut;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(type >= 0);
        प्रतिज्ञा(type < _UNIT_TYPE_MAX);
        प्रतिज्ञा(ret);

        सचे (!unit_prefix_is_valid(prefix))
                पटिनिवत्त -EINVAL;

        ut = unit_type_to_string(type);

        सचे (instance) {
                सचे (!unit_instance_is_valid(instance))
                        पटिनिवत्त -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } अञ्ञथा
                s = strjoin(prefix, ".", ut);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        सचे (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

स्थिर वर्ण *do_escape_char(वर्ण c, वर्ण *t) {
        प्रतिज्ञा(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        पटिनिवत्त t;
}

स्थिर वर्ण *do_escape(ध्रुव वर्ण *f, वर्ण *t) {
        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        सचे (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        अत्थाय (; *f; f++) {
                सचे (*f == '/')
                        *(t++) = '-';
                अञ्ञथा सचे (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                अञ्ञथा
                        *(t++) = *f;
        }

        पटिनिवत्त t;
}

वर्ण* unit_name_escape(ध्रुव वर्ण *f) {
        वर्ण *r, *t;

        प्रतिज्ञा(f);

        r = new(वर्ण, strlen(f)*4+1);
        सचे (!r)
                पटिनिवत्त रिक्त;

        t = do_escape(f, r);
        *t = 0;

        पटिनिवत्त r;
}

पूर्णाङ्क unit_name_unescape(ध्रुव वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *r = रिक्त;
        वर्ण *t;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = strdup(f);
        सचे (!r)
                पटिनिवत्त -ENOMEM;

        अत्थाय (t = r; *f; f++) {
                सचे (*f == '-')
                        *(t++) = '/';
                अञ्ञथा सचे (*f == '\\') {
                        पूर्णाङ्क a, b;

                        सचे (f[1] != 'x')
                                पटिनिवत्त -EINVAL;

                        a = unhexchar(f[2]);
                        सचे (a < 0)
                                पटिनिवत्त -EINVAL;

                        b = unhexchar(f[3]);
                        सचे (b < 0)
                                पटिनिवत्त -EINVAL;

                        *(t++) = (वर्ण) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } अञ्ञथा
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_path_escape(ध्रुव वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त;
        वर्ण *s;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        r = path_simplify_alloc(f, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (empty_or_मूलम्(p))
                s = strdup("-");
        अञ्ञथा {
                सचे (!path_is_normalized(p))
                        पटिनिवत्त -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        *ret = s;
        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_path_unescape(ध्रुव वर्ण *f, वर्ण **ret) {
        _cleanup_free_ वर्ण *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(f);

        सचे (isempty(f))
                पटिनिवत्त -EINVAL;

        सचे (streq(f, "-")) {
                s = strdup("/");
                सचे (!s)
                        पटिनिवत्त -ENOMEM;
        } अञ्ञथा {
                _cleanup_free_ वर्ण *w = रिक्त;

                r = unit_name_unescape(f, &w);
                सचे (r < 0)
                        पटिनिवत्त r;

                /* Don't accept trailing or leading slashes */
                सचे (startswith(w, "/") || endswith(w, "/"))
                        पटिनिवत्त -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                सचे (!s)
                        पटिनिवत्त -ENOMEM;

                सचे (!path_is_normalized(s))
                        पटिनिवत्त -EINVAL;
        }

        सचे (ret)
                *ret = TAKE_PTR(s);

        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_replace_instance_full(
                ध्रुव वर्ण *original,
                ध्रुव वर्ण *instance,
                bool accept_glob,
                वर्ण **ret) {

        _cleanup_free_ वर्ण *s = रिक्त;
        ध्रुव वर्ण *prefix, *suffix;
        size_t pl;

        प्रतिज्ञा(original);
        प्रतिज्ञा(instance);
        प्रतिज्ञा(ret);

        सचे (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                पटिनिवत्त -EINVAL;
        सचे (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                पटिनिवत्त -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        प्रतिज्ञा(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(वर्ण, pl + strlen(instance) + strlen(suffix) + 1);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

#सचे HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        सचे (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_template(ध्रुव वर्ण *f, वर्ण **ret) {
        ध्रुव वर्ण *p, *e;
        वर्ण *s;
        size_t a;

        प्रतिज्ञा(f);
        प्रतिज्ञा(ret);

        सचे (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                पटिनिवत्त -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(वर्ण, a + 1 + strlen(e) + 1);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        पटिनिवत्त 0;
}

bool unit_name_is_hashed(ध्रुव वर्ण *name) {
        ध्रुव वर्ण *s;

        सचे (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                पटिनिवत्त false;

        assert_se(s = strrchr(name, '.'));

        सचे (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                पटिनिवत्त false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        सचे (s[-1] != '_')
                पटिनिवत्त false;

        अत्थाय (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                सचे (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        पटिनिवत्त false;

        पटिनिवत्त true;
}

पूर्णाङ्क unit_name_hash_long(ध्रुव वर्ण *name, वर्ण **ret) {
        _cleanup_free_ वर्ण *n = रिक्त, *hash = रिक्त;
        ध्रुव वर्ण *suffix;
        le64_t h;
        size_t len;

        प्रतिज्ञा(ret);

        सचे (strlen(name) < UNIT_NAME_MAX)
                पटिनिवत्त -EMSGSIZE;

        suffix = strrchr(name, '.');
        सचे (!suffix)
                पटिनिवत्त -EINVAL;

        सचे (unit_type_from_string(suffix+1) < 0)
                पटिनिवत्त -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_कुञ्चिका.bytes));

        hash = hexmem(&h, परिमाण(h));
        सचे (!hash)
                पटिनिवत्त -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        प्रतिज्ञा(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        सचे (!n)
                पटिनिवत्त -ENOMEM;

        सचे (!strextend(&n, "_", hash, suffix))
                पटिनिवत्त -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_from_path(ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त, *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        सचे (!unit_suffix_is_valid(suffix))
                पटिनिवत्त -EINVAL;

        r = unit_name_path_escape(path, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        s = strjoin(p, suffix);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        सचे (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ वर्ण *n = रिक्त;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                सचे (r < 0)
                        पटिनिवत्त r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        सचे (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_from_path_instance(ध्रुव वर्ण *prefix, ध्रुव वर्ण *path, ध्रुव वर्ण *suffix, वर्ण **ret) {
        _cleanup_free_ वर्ण *p = रिक्त, *s = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(prefix);
        प्रतिज्ञा(path);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        सचे (!unit_prefix_is_valid(prefix))
                पटिनिवत्त -EINVAL;

        सचे (!unit_suffix_is_valid(suffix))
                पटिनिवत्त -EINVAL;

        r = unit_name_path_escape(path, &p);
        सचे (r < 0)
                पटिनिवत्त r;

        s = strjoin(prefix, "@", p, suffix);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        सचे (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                पटिनिवत्त -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        सचे (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 0;
}

पूर्णाङ्क unit_name_to_path(ध्रुव वर्ण *name, वर्ण **ret) {
        _cleanup_free_ वर्ण *prefix = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        r = unit_name_to_prefix(name, &prefix);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (unit_name_is_hashed(name))
                पटिनिवत्त -ENAMETOOLONG;

        पटिनिवत्त unit_name_path_unescape(prefix, ret);
}

स्थिर bool do_escape_mangle(ध्रुव वर्ण *f, bool allow_globs, वर्ण *t) {
        ध्रुव वर्ण *valid_chars;
        bool mangled = false;

        प्रतिज्ञा(f);
        प्रतिज्ञा(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        अत्थाय (; *f; f++)
                सचे (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } अञ्ञथा सचे (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } अञ्ञथा
                        *(t++) = *f;
        *t = 0;

        पटिनिवत्त mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
पूर्णाङ्क unit_name_mangle_with_suffix(
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *operation,
                UnitNameMangle flags,
                ध्रुव वर्ण *suffix,
                वर्ण **ret) {

        _cleanup_free_ वर्ण *s = रिक्त;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        पूर्णाङ्क r;

        प्रतिज्ञा(name);
        प्रतिज्ञा(suffix);
        प्रतिज्ञा(ret);

        सचे (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                पटिनिवत्त -EINVAL;

        सचे (!unit_suffix_is_valid(suffix))
                पटिनिवत्त -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        सचे (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                सचे (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ध्रुव वर्ण *e = ASSERT_PTR(strrchr(name, '.'));

                        पटिनिवत्त log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                गच्छ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        सचे (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                सचे (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        गच्छ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        सचे (path_is_absolute(name)) {
                _cleanup_free_ वर्ण *n = रिक्त, *u = रिक्त;

                r = path_simplify_alloc(name, &n);
                सचे (r < 0)
                        पटिनिवत्त r;

                सचे (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        सचे (r >= 0) {
                                सचे (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        पटिनिवत्त log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                पटिनिवत्त 1;
                        }
                        सचे (r != -EINVAL)
                                पटिनिवत्त r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                सचे (r >= 0) {
                        सचे (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                पटिनिवत्त log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        पटिनिवत्त 1;
                }
                सचे (r != -EINVAL)
                        पटिनिवत्त r;
        }

        s = new(वर्ण, strlen(name) * 4 + strlen(suffix) + 1);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        सचे (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        सचे ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        सचे (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(s);
        पटिनिवत्त 1;

good:
        पटिनिवत्त strdup_to(ret, name);
}

पूर्णाङ्क slice_build_parent_slice(ध्रुव वर्ण *slice, वर्ण **ret) {
        प्रतिज्ञा(slice);
        प्रतिज्ञा(ret);

        सचे (!slice_name_is_valid(slice))
                पटिनिवत्त -EINVAL;

        सचे (streq(slice, SPECIAL_मूलम्_SLICE)) {
                *ret = रिक्त;
                पटिनिवत्त 0;
        }

        _cleanup_free_ वर्ण *s = strdup(slice);
        सचे (!s)
                पटिनिवत्त -ENOMEM;

        वर्ण *dash = strrchr(s, '-');
        सचे (!dash)
                पटिनिवत्त strdup_to_full(ret, SPECIAL_मूलम्_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        पटिनिवत्त 1;
}

पूर्णाङ्क slice_build_subslice(ध्रुव वर्ण *slice, ध्रुव वर्ण *name, वर्ण **ret) {
        वर्ण *subslice;

        प्रतिज्ञा(slice);
        प्रतिज्ञा(name);
        प्रतिज्ञा(ret);

        सचे (!slice_name_is_valid(slice))
                पटिनिवत्त -EINVAL;

        सचे (!unit_prefix_is_valid(name))
                पटिनिवत्त -EINVAL;

        सचे (streq(slice, SPECIAL_मूलम्_SLICE))
                subslice = strjoin(name, ".slice");
        अञ्ञथा {
                ध्रुव वर्ण *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(वर्ण, (e - slice) + 1 + strlen(name) + 6 + 1);
                सचे (!subslice)
                        पटिनिवत्त -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        पटिनिवत्त 0;
}

bool slice_name_is_valid(ध्रुव वर्ण *name) {
        ध्रुव वर्ण *p, *e;
        bool dash = false;

        सचे (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                पटिनिवत्त false;

        सचे (streq(name, SPECIAL_मूलम्_SLICE))
                पटिनिवत्त true;

        e = endswith(name, ".slice");
        सचे (!e)
                पटिनिवत्त false;

        अत्थाय (p = name; p < e; p++) {

                सचे (*p == '-') {

                        /* Don't allow initial dash */
                        सचे (p == name)
                                पटिनिवत्त false;

                        /* Don't allow multiple dashes */
                        सचे (dash)
                                पटिनिवत्त false;

                        dash = true;
                } अञ्ञथा
                        dash = false;
        }

        /* Don't allow trailing hash */
        सचे (dash)
                पटिनिवत्त false;

        पटिनिवत्त true;
}

bool unit_name_prefix_equal(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        ध्रुव वर्ण *p, *q;

        प्रतिज्ञा(a);
        प्रतिज्ञा(b);

        सचे (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                पटिनिवत्त false;

        p = strchr(a, '@');
        सचे (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        सचे (!q)
                q = strrchr(b, '.');

        प्रतिज्ञा(p);
        प्रतिज्ञा(q);

        पटिनिवत्त memcmp_nn(a, p - a, b, q - b) == 0;
}
