/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ଅନ୍ତର୍ଭାଵ "sd-id128.h"

#ଅନ୍ତର୍ଭାଵ "alloc-util.h"
#ଅନ୍ତର୍ଭାଵ "glob-util.h"
#ଅନ୍ତର୍ଭାଵ "hexdecoct.h"
#ଅନ୍ତର୍ଭାଵ "log.h"
#ଅନ୍ତର୍ଭାଵ "memory-util.h"
#ଅନ୍ତର୍ଭାଵ "path-util.h"
#ଅନ୍ତର୍ଭାଵ "sparse-endian.h"
#ଅନ୍ତର୍ଭାଵ "special.h"
#ଅନ୍ତର୍ଭାଵ "siphash24.h"
#ଅନ୍ତର୍ଭାଵ "string-util.h"
#ଅନ୍ତର୍ଭାଵ "unit-def.h"
#ଅନ୍ତର୍ଭାଵ "unit-name.h"

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

#define LONG_UNIT_NAME_HASH_ଚାବି SD_ID128_MAKE(ec,f2,37,fb,58,32,4a,32,84,9f,06,9b,0d,21,eb,9a)
#define UNIT_NAME_HASH_LENGTH_CHARS 16

bool unit_name_is_valid(ଅଚଳ ଅକ୍ଷର *n, UnitNameFlags flags) {
        ଅଚଳ ଅକ୍ଷର *e, *i, *at;

        ପ୍ରତିଜ୍ଞା((flags & ~(UNIT_NAME_PLAIN|UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE)) == 0);

        ଯଦି (_unlikely_(flags == 0))
                ଫେରନ୍ତୁ false;

        ଯଦି (isempty(n))
                ଫେରନ୍ତୁ false;

        ଯଦି (strlen(n) >= UNIT_NAME_MAX)
                ଫେରନ୍ତୁ false;

        e = strrchr(n, '.');
        ଯଦି (!e || e == n)
                ଫେରନ୍ତୁ false;

        ଯଦି (unit_type_from_string(e + 1) < 0)
                ଫେରନ୍ତୁ false;

        ପାଇଁ (i = n, at = ଖାଲି; i < e; i++) {

                ଯଦି (*i == '@' && !at)
                        at = i;

                ଯଦି (!strchr(VALID_CHARS_WITH_AT, *i))
                        ଫେରନ୍ତୁ false;
        }

        ଯଦି (at == n)
                ଫେରନ୍ତୁ false;

        ଯଦି (flags & UNIT_NAME_PLAIN)
                ଯଦି (!at)
                        ଫେରନ୍ତୁ true;

        ଯଦି (flags & UNIT_NAME_INSTANCE)
                ଯଦି (at && e > at + 1)
                        ଫେରନ୍ତୁ true;

        ଯଦି (flags & UNIT_NAME_TEMPLATE)
                ଯଦି (at && e == at + 1)
                        ଫେରନ୍ତୁ true;

        ଫେରନ୍ତୁ false;
}

bool unit_prefix_is_valid(ଅଚଳ ଅକ୍ଷର *p) {

        /* We don't allow additional @ in the prefix string */

        ଯଦି (isempty(p))
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ in_charset(p, VALID_CHARS);
}

bool unit_instance_is_valid(ଅଚଳ ଅକ୍ଷର *i) {

        /* The max length depends on the length of the string, so we
         * don't really check this here. */

        ଯଦି (isempty(i))
                ଫେରନ୍ତୁ false;

        /* We allow additional @ in the instance string, we do not
         * allow them in the prefix! */

        ଫେରନ୍ତୁ in_charset(i, "@" VALID_CHARS);
}

bool unit_suffix_is_valid(ଅଚଳ ଅକ୍ଷର *s) {
        ଯଦି (isempty(s))
                ଫେରନ୍ତୁ false;

        ଯଦି (s[0] != '.')
                ଫେରନ୍ତୁ false;

        ଯଦି (unit_type_from_string(s + 1) < 0)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_prefix(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p;
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(n);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        p = strchr(n, '@');
        ଯଦି (!p)
                p = strrchr(n, '.');

        assert_se(p);

        s = strndup(n, p - n);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

UnitNameFlags unit_name_to_instance(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p, *d;

        ପ୍ରତିଜ୍ଞା(n);

        ଯଦି (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        /* Everything past the first @ and before the last . is the instance */
        p = strchr(n, '@');
        ଯଦି (!p) {
                ଯଦି (ret)
                        *ret = ଖାଲି;
                ଫେରନ୍ତୁ UNIT_NAME_PLAIN;
        }

        p++;

        d = strrchr(p, '.');
        ଯଦି (!d)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (ret) {
                ଅକ୍ଷର *i = strndup(p, d-p);
                ଯଦି (!i)
                        ଫେରନ୍ତୁ -ENOMEM;

                *ret = i;
        }
        ଫେରନ୍ତୁ d > p ? UNIT_NAME_INSTANCE : UNIT_NAME_TEMPLATE;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_prefix_and_instance(ଅଚଳ ଅକ୍ଷର *n, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *d;
        ଅକ୍ଷର *s;

        ପ୍ରତିଜ୍ଞା(n);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        d = strrchr(n, '.');
        ଯଦି (!d)
                ଫେରନ୍ତୁ -EINVAL;

        s = strndup(n, d - n);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

UnitType unit_name_to_type(ଅଚଳ ଅକ୍ଷର *n) {
        ଅଚଳ ଅକ୍ଷର *e;

        ପ୍ରତିଜ୍ଞା(n);

        ଯଦି (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ _UNIT_TYPE_INVALID;

        assert_se(e = strrchr(n, '.'));

        ଫେରନ୍ତୁ unit_type_from_string(e + 1);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_change_suffix(ଅଚଳ ଅକ୍ଷର *n, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        size_t a, b;
        ଅଚଳ ଅକ୍ଷର *e;

        ପ୍ରତିଜ୍ଞା(n);
        ପ୍ରତିଜ୍ଞା(suffix);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_name_is_valid(n, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!unit_suffix_is_valid(suffix))
                ଫେରନ୍ତୁ -EINVAL;

        assert_se(e = strrchr(n, '.'));

        a = e - n;
        b = strlen(suffix);

        s = new(ଅକ୍ଷର, a + b + 1);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        strcpy(mempcpy(s, n, a), suffix);

        /* Make sure the name is still valid (i.e. didn't grow too large due to longer suffix) */
        ଯଦି (!unit_name_is_valid(s, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_build(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *instance, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret) {
        UnitType type;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(suffix);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (suffix[0] != '.')
                ଫେରନ୍ତୁ -EINVAL;

        type = unit_type_from_string(suffix + 1);
        ଯଦି (type < 0)
                ଫେରନ୍ତୁ type;

        ଫେରନ୍ତୁ unit_name_build_from_type(prefix, instance, type, ret);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_build_from_type(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *instance, UnitType type, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *ut;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(type >= 0);
        ପ୍ରତିଜ୍ଞା(type < _UNIT_TYPE_MAX);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_prefix_is_valid(prefix))
                ଫେରନ୍ତୁ -EINVAL;

        ut = unit_type_to_string(type);

        ଯଦି (instance) {
                ଯଦି (!unit_instance_is_valid(instance))
                        ଫେରନ୍ତୁ -EINVAL;

                s = strjoin(prefix, "@", instance, ".", ut);
        } ଅନ୍ୟଥା
                s = strjoin(prefix, ".", ut);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        /* Verify that this didn't grow too large (or otherwise is invalid) */
        ଯଦି (!unit_name_is_valid(s, instance ? UNIT_NAME_INSTANCE : UNIT_NAME_PLAIN))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ସ୍ଥିର ଅକ୍ଷର *do_escape_char(ଅକ୍ଷର c, ଅକ୍ଷର *t) {
        ପ୍ରତିଜ୍ଞା(t);

        *(t++) = '\\';
        *(t++) = 'x';
        *(t++) = hexchar(c >> 4);
        *(t++) = hexchar(c);

        ଫେରନ୍ତୁ t;
}

ସ୍ଥିର ଅକ୍ଷର *do_escape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର *t) {
        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(t);

        /* do not create units with a leading '.', like for "/.dotdir" mount points */
        ଯଦି (*f == '.') {
                t = do_escape_char(*f, t);
                f++;
        }

        ପାଇଁ (; *f; f++) {
                ଯଦି (*f == '/')
                        *(t++) = '-';
                ଅନ୍ୟଥା ଯଦି (IN_SET(*f, '-', '\\') || !strchr(VALID_CHARS, *f))
                        t = do_escape_char(*f, t);
                ଅନ୍ୟଥା
                        *(t++) = *f;
        }

        ଫେରନ୍ତୁ t;
}

ଅକ୍ଷର* unit_name_escape(ଅଚଳ ଅକ୍ଷର *f) {
        ଅକ୍ଷର *r, *t;

        ପ୍ରତିଜ୍ଞା(f);

        r = new(ଅକ୍ଷର, strlen(f)*4+1);
        ଯଦି (!r)
                ଫେରନ୍ତୁ ଖାଲି;

        t = do_escape(f, r);
        *t = 0;

        ଫେରନ୍ତୁ r;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_unescape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *r = ଖାଲି;
        ଅକ୍ଷର *t;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret);

        r = strdup(f);
        ଯଦି (!r)
                ଫେରନ୍ତୁ -ENOMEM;

        ପାଇଁ (t = r; *f; f++) {
                ଯଦି (*f == '-')
                        *(t++) = '/';
                ଅନ୍ୟଥା ଯଦି (*f == '\\') {
                        ପୂର୍ଣ୍ଣସଂଖ୍ୟା a, b;

                        ଯଦି (f[1] != 'x')
                                ଫେରନ୍ତୁ -EINVAL;

                        a = unhexchar(f[2]);
                        ଯଦି (a < 0)
                                ଫେରନ୍ତୁ -EINVAL;

                        b = unhexchar(f[3]);
                        ଯଦି (b < 0)
                                ଫେରନ୍ତୁ -EINVAL;

                        *(t++) = (ଅକ୍ଷର) (((uint8_t) a << 4U) | (uint8_t) b);
                        f += 3;
                } ଅନ୍ୟଥା
                        *(t++) = *f;
        }

        *t = 0;

        *ret = TAKE_PTR(r);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_path_escape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି;
        ଅକ୍ଷର *s;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret);

        r = path_simplify_alloc(f, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (empty_or_ମୂଳ(p))
                s = strdup("-");
        ଅନ୍ୟଥା {
                ଯଦି (!path_is_normalized(p))
                        ଫେରନ୍ତୁ -EINVAL;

                /* Truncate trailing slashes and skip leading slashes */
                delete_trailing_chars(p, "/");
                s = unit_name_escape(skip_leading_chars(p, "/"));
        }
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_path_unescape(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(f);

        ଯଦି (isempty(f))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (streq(f, "-")) {
                s = strdup("/");
                ଯଦି (!s)
                        ଫେରନ୍ତୁ -ENOMEM;
        } ଅନ୍ୟଥା {
                _cleanup_free_ ଅକ୍ଷର *w = ଖାଲି;

                r = unit_name_unescape(f, &w);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                /* Don't accept trailing or leading slashes */
                ଯଦି (startswith(w, "/") || endswith(w, "/"))
                        ଫେରନ୍ତୁ -EINVAL;

                /* Prefix a slash again */
                s = strjoin("/", w);
                ଯଦି (!s)
                        ଫେରନ୍ତୁ -ENOMEM;

                ଯଦି (!path_is_normalized(s))
                        ଫେରନ୍ତୁ -EINVAL;
        }

        ଯଦି (ret)
                *ret = TAKE_PTR(s);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_replace_instance_full(
                ଅଚଳ ଅକ୍ଷର *original,
                ଅଚଳ ଅକ୍ଷର *instance,
                bool accept_glob,
                ଅକ୍ଷର **ret) {

        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *prefix, *suffix;
        size_t pl;

        ପ୍ରତିଜ୍ଞା(original);
        ପ୍ରତିଜ୍ଞା(instance);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_name_is_valid(original, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ଫେରନ୍ତୁ -EINVAL;
        ଯଦି (!unit_instance_is_valid(instance) && !(accept_glob && in_charset(instance, VALID_CHARS_GLOB)))
                ଫେରନ୍ତୁ -EINVAL;

        prefix = ASSERT_PTR(strchr(original, '@'));
        suffix = ASSERT_PTR(strrchr(original, '.'));
        ପ୍ରତିଜ୍ଞା(prefix < suffix);

        pl = prefix - original + 1; /* include '@' */

        s = new(ଅକ୍ଷର, pl + strlen(instance) + strlen(suffix) + 1);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

#ଯଦି HAS_FEATURE_MEMORY_SANITIZER
        /* MSan doesn't like stpncpy... See also https://github.com/google/sanitizers/issues/926 */
        memzero(s, pl + strlen(instance) + strlen(suffix) + 1);
#endif

        strcpy(stpcpy(stpncpy(s, original, pl), instance), suffix);

        /* Make sure the resulting name still is valid, i.e. didn't grow too large. Globs will be expanded
         * by clients when used, so the check is pointless. */
        ଯଦି (!accept_glob && !unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_template(ଅଚଳ ଅକ୍ଷର *f, ଅକ୍ଷର **ret) {
        ଅଚଳ ଅକ୍ଷର *p, *e;
        ଅକ୍ଷର *s;
        size_t a;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_name_is_valid(f, UNIT_NAME_INSTANCE|UNIT_NAME_TEMPLATE))
                ଫେରନ୍ତୁ -EINVAL;

        assert_se(p = strchr(f, '@'));
        assert_se(e = strrchr(f, '.'));

        a = p - f;

        s = new(ଅକ୍ଷର, a + 1 + strlen(e) + 1);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        strcpy(mempcpy(s, f, a + 1), e);

        *ret = s;
        ଫେରନ୍ତୁ 0;
}

bool unit_name_is_hashed(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ଅକ୍ଷର *s;

        ଯଦି (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ଫେରନ୍ତୁ false;

        assert_se(s = strrchr(name, '.'));

        ଯଦି (s - name < UNIT_NAME_HASH_LENGTH_CHARS + 1)
                ଫେରନ୍ତୁ false;

        s -= UNIT_NAME_HASH_LENGTH_CHARS;
        ଯଦି (s[-1] != '_')
                ଫେରନ୍ତୁ false;

        ପାଇଁ (size_t i = 0; i < UNIT_NAME_HASH_LENGTH_CHARS; i++)
                ଯଦି (!strchr(LOWERCASE_HEXDIGITS, s[i]))
                        ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_hash_long(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି, *hash = ଖାଲି;
        ଅଚଳ ଅକ୍ଷର *suffix;
        le64_t h;
        size_t len;

        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (strlen(name) < UNIT_NAME_MAX)
                ଫେରନ୍ତୁ -EMSGSIZE;

        suffix = strrchr(name, '.');
        ଯଦି (!suffix)
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (unit_type_from_string(suffix+1) < 0)
                ଫେରନ୍ତୁ -EINVAL;

        h = htole64(siphash24_string(name, LONG_UNIT_NAME_HASH_ଚାବି.bytes));

        hash = hexmem(&h, ଆକାର(h));
        ଯଦି (!hash)
                ଫେରନ୍ତୁ -ENOMEM;

        assert_se(strlen(hash) == UNIT_NAME_HASH_LENGTH_CHARS);

        len = UNIT_NAME_MAX - 1 - strlen(suffix+1) - UNIT_NAME_HASH_LENGTH_CHARS - 2;
        ପ୍ରତିଜ୍ଞା(len > 0 && len < UNIT_NAME_MAX);

        n = strndup(name, len);
        ଯଦି (!n)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (!strextend(&n, "_", hash, suffix))
                ଫେରନ୍ତୁ -ENOMEM;
        assert_se(unit_name_is_valid(n, UNIT_NAME_PLAIN));

        *ret = TAKE_PTR(n);

        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_from_path(ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(suffix);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_suffix_is_valid(suffix))
                ଫେରନ୍ତୁ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        s = strjoin(p, suffix);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (strlen(s) >= UNIT_NAME_MAX) {
                _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି;

                log_debug("Unit name \"%s\" too long, falling back to hashed unit name.", s);

                r = unit_name_hash_long(s, &n);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                free_and_replace(s, n);
        }

        /* Refuse if this for some other reason didn't result in a valid name */
        ଯଦି (!unit_name_is_valid(s, UNIT_NAME_PLAIN))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_from_path_instance(ଅଚଳ ଅକ୍ଷର *prefix, ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *suffix, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *p = ଖାଲି, *s = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(prefix);
        ପ୍ରତିଜ୍ଞା(path);
        ପ୍ରତିଜ୍ଞା(suffix);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!unit_prefix_is_valid(prefix))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!unit_suffix_is_valid(suffix))
                ଫେରନ୍ତୁ -EINVAL;

        r = unit_name_path_escape(path, &p);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        s = strjoin(prefix, "@", p, suffix);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        ଯଦି (strlen(s) >= UNIT_NAME_MAX) /* Return a slightly more descriptive error for this specific condition */
                ଫେରନ୍ତୁ -ENAMETOOLONG;

        /* Refuse if this for some other reason didn't result in a valid name */
        ଯଦି (!unit_name_is_valid(s, UNIT_NAME_INSTANCE))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 0;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_to_path(ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret) {
        _cleanup_free_ ଅକ୍ଷର *prefix = ଖାଲି;
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);

        r = unit_name_to_prefix(name, &prefix);
        ଯଦି (r < 0)
                ଫେରନ୍ତୁ r;

        ଯଦି (unit_name_is_hashed(name))
                ଫେରନ୍ତୁ -ENAMETOOLONG;

        ଫେରନ୍ତୁ unit_name_path_unescape(prefix, ret);
}

ସ୍ଥିର bool do_escape_mangle(ଅଚଳ ଅକ୍ଷର *f, bool allow_globs, ଅକ୍ଷର *t) {
        ଅଚଳ ଅକ୍ଷର *valid_chars;
        bool mangled = false;

        ପ୍ରତିଜ୍ଞା(f);
        ପ୍ରତିଜ୍ଞା(t);

        /* We'll only escape the obvious characters here, to play safe.
         *
         * Returns true if any characters were mangled, false otherwise.
         */

        valid_chars = allow_globs ? VALID_CHARS_GLOB : VALID_CHARS_WITH_AT;

        ପାଇଁ (; *f; f++)
                ଯଦି (*f == '/') {
                        *(t++) = '-';
                        mangled = true;
                } ଅନ୍ୟଥା ଯଦି (!strchr(valid_chars, *f)) {
                        t = do_escape_char(*f, t);
                        mangled = true;
                } ଅନ୍ୟଥା
                        *(t++) = *f;
        *t = 0;

        ଫେରନ୍ତୁ mangled;
}

/**
 *  Convert a string to a unit name. /dev/blah is converted to dev-blah.device,
 *  /blah/blah is converted to blah-blah.mount, anything else is left alone,
 *  except that @suffix is appended if a valid unit suffix is not present.
 *
 *  If @allow_globs, globs characters are preserved. Otherwise, they are escaped.
 */
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unit_name_mangle_with_suffix(
                ଅଚଳ ଅକ୍ଷର *name,
                ଅଚଳ ଅକ୍ଷର *operation,
                UnitNameMangle flags,
                ଅଚଳ ଅକ୍ଷର *suffix,
                ଅକ୍ଷର **ret) {

        _cleanup_free_ ଅକ୍ଷର *s = ଖାଲି;
        bool mangled, suggest_escape = true, warn = FLAGS_SET(flags, UNIT_NAME_MANGLE_WARN);
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା r;

        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(suffix);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (isempty(name)) /* We cannot mangle empty unit names to become valid, sorry. */
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!unit_suffix_is_valid(suffix))
                ଫେରନ୍ତୁ -EINVAL;

        /* Already a fully valid unit name? If so, no mangling is necessary... */
        ଯଦି (unit_name_is_valid(name, UNIT_NAME_ANY)) {
                ଯଦି (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !endswith(name, suffix)) {
                        ଅଚଳ ଅକ୍ଷର *e = ASSERT_PTR(strrchr(name, '.'));

                        ଫେରନ୍ତୁ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                              SYNTHETIC_ERRNO(EINVAL),
                                              "Unit name \"%s\" has unit type \"%s\", but \"%s\" is expected%s%s.",
                                              name, e + 1, suffix + 1,
                                              operation ? " " : "", strempty(operation));
                }

                ଯାଅ good;
        }

        /* Already a fully valid globbing expression? If so, no mangling is necessary either... */
        ଯଦି (string_is_glob(name) && in_charset(name, VALID_CHARS_GLOB)) {
                ଯଦି (FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB))
                        ଯାଅ good;
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Glob pattern passed%s%s, but globs are not supported for this.",
                         operation ? " " : "", strempty(operation));
                suggest_escape = false;
        }

        ଯଦି (path_is_absolute(name)) {
                _cleanup_free_ ଅକ୍ଷର *n = ଖାଲି, *u = ଖାଲି;

                r = path_simplify_alloc(name, &n);
                ଯଦି (r < 0)
                        ଫେରନ୍ତୁ r;

                ଯଦି (is_device_path(n)) {
                        r = unit_name_from_path(n, ".device", &u);
                        ଯଦି (r >= 0) {
                                ଯଦି (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".device"))
                                        ଫେରନ୍ତୁ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                              SYNTHETIC_ERRNO(EINVAL),
                                                              "Path \"%s\" resolves to unit type \"device\", but \"%s\" is expected%s%s.",
                                                              name, suffix + 1,
                                                              operation ? " " : "", strempty(operation));

                                *ret = TAKE_PTR(u);
                                ଫେରନ୍ତୁ 1;
                        }
                        ଯଦି (r != -EINVAL)
                                ଫେରନ୍ତୁ r;
                }

                r = unit_name_from_path(n, ".mount", &u);
                ଯଦି (r >= 0) {
                        ଯଦି (FLAGS_SET(flags, UNIT_NAME_MANGLE_STRICT) && !streq(suffix, ".mount"))
                                ଫେରନ୍ତୁ log_full_errno(warn ? LOG_NOTICE : LOG_DEBUG,
                                                      SYNTHETIC_ERRNO(EINVAL),
                                                      "Path \"%s\" resolves to unit type \"mount\", but \"%s\" is expected%s%s.",
                                                      name, suffix + 1,
                                                      operation ? " " : "", strempty(operation));

                        *ret = TAKE_PTR(u);
                        ଫେରନ୍ତୁ 1;
                }
                ଯଦି (r != -EINVAL)
                        ଫେରନ୍ତୁ r;
        }

        s = new(ଅକ୍ଷର, strlen(name) * 4 + strlen(suffix) + 1);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        mangled = do_escape_mangle(name, FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB), s);
        ଯଦି (mangled)
                log_full(warn ? LOG_NOTICE : LOG_DEBUG,
                         "Invalid unit name \"%s\" escaped as \"%s\"%s.",
                         name, s,
                         suggest_escape ? " (maybe you should use systemd-escape?)" : "");

        /* Append a suffix if it doesn't have any, but only if this is not a glob, so that we can allow
         * "foo.*" as a valid glob. */
        ଯଦି ((!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) || !string_is_glob(s)) && unit_name_to_type(s) < 0)
                strcat(s, suffix);

        /* Make sure mangling didn't grow this too large (but don't do this check if globbing is allowed,
         * since globs generally do not qualify as valid unit names) */
        ଯଦି (!FLAGS_SET(flags, UNIT_NAME_MANGLE_GLOB) && !unit_name_is_valid(s, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ -EINVAL;

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 1;

good:
        ଫେରନ୍ତୁ strdup_to(ret, name);
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା slice_build_parent_slice(ଅଚଳ ଅକ୍ଷର *slice, ଅକ୍ଷର **ret) {
        ପ୍ରତିଜ୍ଞା(slice);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!slice_name_is_valid(slice))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (streq(slice, SPECIAL_ମୂଳ_SLICE)) {
                *ret = ଖାଲି;
                ଫେରନ୍ତୁ 0;
        }

        _cleanup_free_ ଅକ୍ଷର *s = strdup(slice);
        ଯଦି (!s)
                ଫେରନ୍ତୁ -ENOMEM;

        ଅକ୍ଷର *dash = strrchr(s, '-');
        ଯଦି (!dash)
                ଫେରନ୍ତୁ strdup_to_full(ret, SPECIAL_ମୂଳ_SLICE);

        /* We know that s ended with .slice before truncation, so we have enough space. */
        strcpy(dash, ".slice");

        *ret = TAKE_PTR(s);
        ଫେରନ୍ତୁ 1;
}

ପୂର୍ଣ୍ଣସଂଖ୍ୟା slice_build_subslice(ଅଚଳ ଅକ୍ଷର *slice, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ret) {
        ଅକ୍ଷର *subslice;

        ପ୍ରତିଜ୍ଞା(slice);
        ପ୍ରତିଜ୍ଞା(name);
        ପ୍ରତିଜ୍ଞା(ret);

        ଯଦି (!slice_name_is_valid(slice))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (!unit_prefix_is_valid(name))
                ଫେରନ୍ତୁ -EINVAL;

        ଯଦି (streq(slice, SPECIAL_ମୂଳ_SLICE))
                subslice = strjoin(name, ".slice");
        ଅନ୍ୟଥା {
                ଅଚଳ ଅକ୍ଷର *e;

                assert_se(e = endswith(slice, ".slice"));

                subslice = new(ଅକ୍ଷର, (e - slice) + 1 + strlen(name) + 6 + 1);
                ଯଦି (!subslice)
                        ଫେରନ୍ତୁ -ENOMEM;

                stpcpy(stpcpy(stpcpy(mempcpy(subslice, slice, e - slice), "-"), name), ".slice");
        }

        *ret = subslice;
        ଫେରନ୍ତୁ 0;
}

bool slice_name_is_valid(ଅଚଳ ଅକ୍ଷର *name) {
        ଅଚଳ ଅକ୍ଷର *p, *e;
        bool dash = false;

        ଯଦି (!unit_name_is_valid(name, UNIT_NAME_PLAIN))
                ଫେରନ୍ତୁ false;

        ଯଦି (streq(name, SPECIAL_ମୂଳ_SLICE))
                ଫେରନ୍ତୁ true;

        e = endswith(name, ".slice");
        ଯଦି (!e)
                ଫେରନ୍ତୁ false;

        ପାଇଁ (p = name; p < e; p++) {

                ଯଦି (*p == '-') {

                        /* Don't allow initial dash */
                        ଯଦି (p == name)
                                ଫେରନ୍ତୁ false;

                        /* Don't allow multiple dashes */
                        ଯଦି (dash)
                                ଫେରନ୍ତୁ false;

                        dash = true;
                } ଅନ୍ୟଥା
                        dash = false;
        }

        /* Don't allow trailing hash */
        ଯଦି (dash)
                ଫେରନ୍ତୁ false;

        ଫେରନ୍ତୁ true;
}

bool unit_name_prefix_equal(ଅଚଳ ଅକ୍ଷର *a, ଅଚଳ ଅକ୍ଷର *b) {
        ଅଚଳ ଅକ୍ଷର *p, *q;

        ପ୍ରତିଜ୍ଞା(a);
        ପ୍ରତିଜ୍ଞା(b);

        ଯଦି (!unit_name_is_valid(a, UNIT_NAME_ANY) || !unit_name_is_valid(b, UNIT_NAME_ANY))
                ଫେରନ୍ତୁ false;

        p = strchr(a, '@');
        ଯଦି (!p)
                p = strrchr(a, '.');

        q = strchr(b, '@');
        ଯଦି (!q)
                q = strrchr(b, '.');

        ପ୍ରତିଜ୍ଞା(p);
        ପ୍ରତିଜ୍ଞା(q);

        ଫେରନ୍ତୁ memcmp_nn(a, p - a, b, q - b) == 0;
}
