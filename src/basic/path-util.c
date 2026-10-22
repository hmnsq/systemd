/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ਅਨ੍ਤਰ੍ਭਾਵ <fnmatch.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ "alloc-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "chase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "errno-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "extract-word.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fd-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "fs-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "glob-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "log.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "path-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "stat-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "string-util.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strv.h"

bool is_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *p) {
        ਜੇ (!p) /* A NULL pointer is definitely not a path */
                ਵਾਪਸ false;

        ਵਾਪਸ strchr(p, '/');
}

ਪੂਰਨ ਰਸਤਾ_ਵੰਡੋ_and_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ ***ret) {
        _cleanup_strv_free_ ਅੱਖਰ **l = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        l = strv_ਵੰਡੋ(p, ":");
        ਜੇ (!l)
                ਵਾਪਸ -ENOMEM;

        r = ਰਸਤਾ_strv_make_absolute_cwd(l);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        *ret = TAKE_PTR(l);
        ਵਾਪਸ r;
}

ਅੱਖਰ* ਰਸਤਾ_make_absolute(ਅਚਲ ਅੱਖਰ *p, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ਜੇ (ਰਸਤਾ_is_absolute(p) || isempty(ਅਗੇਤਰ))
                ਵਾਪਸ strdup(p);

        ਵਾਪਸ ਰਸਤਾ_join(ਅਗੇਤਰ, p);
}

ਪੂਰਨ ਸੁਰੱਖਿਅਤ_getcwd(ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *cwd = ਨਿਰਾ;

        cwd = get_current_dir_name();
        ਜੇ (!cwd)
                ਵਾਪਸ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ਜੇ (cwd[0] != '/')
                ਵਾਪਸ -ENOMEDIUM;

        ਜੇ (ret)
                *ret = TAKE_PTR(cwd);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_make_absolute_cwd(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ **ret) {
        ਅੱਖਰ *c;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ਜੇ (ਰਸਤਾ_is_absolute(p))
                c = strdup(p);
        ਨਹੀਂਤਾਂ {
                _cleanup_free_ ਅੱਖਰ *cwd = ਨਿਰਾ;

                r = ਸੁਰੱਖਿਅਤ_getcwd(&cwd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                c = ਰਸਤਾ_join(cwd, p);
        }
        ਜੇ (!c)
                ਵਾਪਸ -ENOMEM;

        *ret = c;
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_make_relative(ਅਚਲ ਅੱਖਰ *from, ਅਚਲ ਅੱਖਰ *to, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *ਨਤੀਜਾ = ਨਿਰਾ;
        ਬਿਨਾਨਿਸ਼ਾਨੀ n_parents;
        ਅਚਲ ਅੱਖਰ *f, *t;
        ਪੂਰਨ r, k;
        ਅੱਖਰ *p;

        ਪ੍ਰਤਿਜ੍ਞਾ(from);
        ਪ੍ਰਤਿਜ੍ਞਾ(to);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ਜੇ (!ਰਸਤਾ_is_absolute(from) || !ਰਸਤਾ_is_absolute(to))
                ਵਾਪਸ -EINVAL;

        ਲਈ (;;) {
                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&from, true, &f);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                k = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&to, true, &t);
                ਜੇ (k < 0)
                        ਵਾਪਸ k;

                ਜੇ (r == 0) {
                        /* end of 'from' */
                        ਜੇ (k == 0) {
                                /* from and to are equivalent. */
                                ਨਤੀਜਾ = strdup(".");
                                ਜੇ (!ਨਤੀਜਾ)
                                        ਵਾਪਸ -ENOMEM;
                        } ਨਹੀਂਤਾਂ {
                                /* 'to' is inside of 'from'. */
                                r = ਰਸਤਾ_simplify_alloc(t, &ਨਤੀਜਾ);
                                ਜੇ (r < 0)
                                        ਵਾਪਸ r;

                                ਜੇ (!ਰਸਤਾ_is_valid(ਨਤੀਜਾ))
                                        ਵਾਪਸ -EINVAL;
                        }

                        *ret = TAKE_PTR(ਨਤੀਜਾ);
                        ਵਾਪਸ 0;
                }

                ਜੇ (r != k || !strneq(f, t, r))
                        ਰੋਕੋ;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        ਲਈ (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&from, false, &f);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;
        }

        ਜੇ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                ਵਾਪਸ -EINVAL;

        ਨਤੀਜਾ = new(ਅੱਖਰ, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ਜੇ (!ਨਤੀਜਾ)
                ਵਾਪਸ -ENOMEM;

        ਲਈ (p = ਨਤੀਜਾ; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ਜੇ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(ਨਤੀਜਾ);
                ਵਾਪਸ 0;
        }

        strcpy(p, t);

        ਰਸਤਾ_simplify(ਨਤੀਜਾ);

        ਜੇ (!ਰਸਤਾ_is_valid(ਨਤੀਜਾ))
                ਵਾਪਸ -EINVAL;

        *ret = TAKE_PTR(ਨਤੀਜਾ);
        ਵਾਪਸ 0;
}

ਪੂਰਨ ਰਸਤਾ_make_relative_ਮਾਪਾ(ਅਚਲ ਅੱਖਰ *from_ਬੱਚਾ, ਅਚਲ ਅੱਖਰ *to, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *from = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(from_ਬੱਚਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(to);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* Similar to path_make_relative(), but provides the relative path from the parent directory of
         * 'from_child'. This may be useful when creating relative symlink.
         *
         * E.g.
         * - from = "/path/to/aaa", to = "/path/to/bbb"
         *      path_make_relative(from, to) = "../bbb"
         *      path_make_relative_parent(from, to) = "bbb"
         *
         * - from = "/path/to/aaa/bbb", to = "/path/to/ccc/ddd"
         *      path_make_relative(from, to) = "../../ccc/ddd"
         *      path_make_relative_parent(from, to) = "../ccc/ddd"
         */

        r = ਰਸਤਾ_ਕੱਢੋ_directory(from_ਬੱਚਾ, &from);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ ਰਸਤਾ_make_relative(from, to, ret);
}

ਅੱਖਰ* ਰਸਤਾ_startswith_strv(ਅਚਲ ਅੱਖਰ *p, ਅੱਖਰ * ਅਚਲ *strv) {
        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        STRV_FOREACH(s, strv) {
                ਅੱਖਰ *t;

                t = ਰਸਤਾ_startswith(p, *s);
                ਜੇ (t)
                        ਵਾਪਸ t;
        }

        ਵਾਪਸ ਨਿਰਾ;
}

ਪੂਰਨ ਰਸਤਾ_strv_make_absolute_cwd(ਅੱਖਰ **l) {
        ਪੂਰਨ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                ਅੱਖਰ *t;

                r = ਰਸਤਾ_make_absolute_cwd(*s, &t);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                ਰਸਤਾ_simplify(t);
                free_and_replace(*s, t);
        }

        ਵਾਪਸ 0;
}

ਅੱਖਰ** ਰਸਤਾ_strv_resolve(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {
        ਬਿਨਾਨਿਸ਼ਾਨੀ k = 0;
        bool enomem = false;
        ਪੂਰਨ r;

        ਜੇ (strv_isempty(l))
                ਵਾਪਸ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ ਅੱਖਰ *orig = ਨਿਰਾ;
                ਅੱਖਰ *t, *u;

                ਜੇ (ਜੜ੍ਹ) {
                        ਜੇ (!ਰਸਤਾ_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                ਮੁਕਤ(*s);
                                ਜਾਰੀ;
                        }

                        orig = *s;
                        t = ਰਸਤਾ_join(ਜੜ੍ਹ, orig);
                        ਜੇ (!t) {
                                enomem = true;
                                ਜਾਰੀ;
                        }
                } ਨਹੀਂਤਾਂ
                        t = *s;

                r = chase(t, ਜੜ੍ਹ, 0, &u, ਨਿਰਾ);
                ਜੇ (r == -ENOENT) {
                        ਜੇ (ਜੜ੍ਹ) {
                                u = TAKE_PTR(orig);
                                ਮੁਕਤ(t);
                        } ਨਹੀਂਤਾਂ
                                u = t;
                } ਨਹੀਂਤਾਂ ਜੇ (r < 0) {
                        ਮੁਕਤ(t);

                        ਜੇ (r == -ENOMEM)
                                enomem = true;

                        ਜਾਰੀ;
                } ਨਹੀਂਤਾਂ ਜੇ (ਜੜ੍ਹ) {
                        ਅੱਖਰ *x;

                        ਮੁਕਤ(t);
                        x = ਰਸਤਾ_startswith(u, ਜੜ੍ਹ);
                        ਜੇ (x) {
                                /* restore the slash if it was lost */
                                ਜੇ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                ਮੁਕਤ(u);
                                ਜੇ (!t) {
                                        enomem = true;
                                        ਜਾਰੀ;
                                }
                                u = t;
                        } ਨਹੀਂਤਾਂ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } ਨਹੀਂਤਾਂ
                        ਮੁਕਤ(t);

                l[k++] = u;
        }

        l[k] = ਨਿਰਾ;

        ਜੇ (enomem)
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ l;
}

ਅੱਖਰ** ਰਸਤਾ_strv_resolve_uniq(ਅੱਖਰ **l, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ) {

        ਜੇ (strv_isempty(l))
                ਵਾਪਸ l;

        ਜੇ (!ਰਸਤਾ_strv_resolve(l, ਜੜ੍ਹ))
                ਵਾਪਸ ਨਿਰਾ;

        ਵਾਪਸ strv_uniq(l);
}

ਅੱਖਰ* skip_leading_slash(ਅਚਲ ਅੱਖਰ *p) {
        ਵਾਪਸ skip_leading_chars(p, "/");
}

ਅੱਖਰ* ਰਸਤਾ_simplify_ਭਰਿਆ(ਅੱਖਰ *ਰਸਤਾ, ਰਸਤਾ_Simplify_Flags flags) {
        bool ਜੋੜੋ_slash = false, keep_trailing_slash, absolute, beginning = true;
        ਅੱਖਰ *f = ਰਸਤਾ;
        ਪੂਰਨ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ ਰਸਤਾ;

        keep_trailing_slash = FLAGS_ਸੈੱਟ(flags, ਰਸਤਾ_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(ਰਸਤਾ, "/");

        absolute = ਰਸਤਾ_is_absolute(ਰਸਤਾ);
        f += absolute;  /* Keep leading /, if present. */

        ਲਈ (ਅਚਲ ਅੱਖਰ *p = f;;) {
                ਅਚਲ ਅੱਖਰ *e;

                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&p, true, &e);
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (r > 0 && absolute && beginning && ਰਸਤਾ_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        ਜਾਰੀ;

                beginning = false;

                ਜੇ (ਜੋੜੋ_slash)
                        *f++ = '/';

                ਜੇ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, ਲੜੀ_ਲੰਬਾਈ(p) + 1);
                        ਵਾਪਸ ਰਸਤਾ;
                }

                memmove(f, e, r);
                f += r;

                ਜੋੜੋ_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ਜੇ (f == ਰਸਤਾ)
                *f++ = '.';

        ਜੇ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        ਵਾਪਸ ਰਸਤਾ;
}

ਪੂਰਨ ਰਸਤਾ_simplify_alloc(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        ਜੇ (!ਰਸਤਾ) {
                *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਅੱਖਰ *t = strdup(ਰਸਤਾ);
        ਜੇ (!t)
                ਵਾਪਸ -ENOMEM;

        *ret = ਰਸਤਾ_simplify(t);
        ਵਾਪਸ 0;
}

ਅੱਖਰ* ਰਸਤਾ_startswith_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *original_ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਰਸਤਾ_ਸ਼ੁਰੂ_With_Flags flags) {
        ਪ੍ਰਤਿਜ੍ਞਾ(original_ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = original_ਰਸਤਾ;

        ਜੇ ((ਰਸਤਾ[0] == '/') != (ਅਗੇਤਰ[0] == '/'))
                ਵਾਪਸ ਨਿਰਾ;

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *p, *q;
                ਪੂਰਨ m, n;

                m = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&ਰਸਤਾ, !FLAGS_ਸੈੱਟ(flags, ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT), &p);
                ਜੇ (m < 0)
                        ਵਾਪਸ ਨਿਰਾ;

                n = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&ਅਗੇਤਰ, !FLAGS_ਸੈੱਟ(flags, ਰਸਤਾ_STARTSWITH_REFUSE_DOT_DOT), &q);
                ਜੇ (n < 0)
                        ਵਾਪਸ ਨਿਰਾ;

                ਜੇ (n == 0) {
                        ਜੇ (!p)
                                p = ਰਸਤਾ;

                        ਜੇ (FLAGS_ਸੈੱਟ(flags, ਰਸਤਾ_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ਜੇ (p <= original_ਰਸਤਾ)
                                        ਵਾਪਸ ਨਿਰਾ;

                                p--;

                                ਜੇ (*p != '/')
                                        ਵਾਪਸ ਨਿਰਾ;
                        }

                        ਵਾਪਸ (ਅੱਖਰ*) p;
                }

                ਜੇ (m != n)
                        ਵਾਪਸ ਨਿਰਾ;

                ਜੇ (!strneq(p, q, m))
                        ਵਾਪਸ ਨਿਰਾ;
        }
}

ਪੂਰਨ ਰਸਤਾ_compare(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਪੂਰਨ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(ਰਸਤਾ_is_absolute(a), ਰਸਤਾ_is_absolute(b));
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        ਲਈ (;;) {
                ਅਚਲ ਅੱਖਰ *aa, *bb;
                ਪੂਰਨ j, k;

                j = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&a, true, &aa);
                k = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&b, true, &bb);

                ਜੇ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ਜੇ (r != 0)
                                ਵਾਪਸ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        ਵਾਪਸ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ਜੇ (j == 0) {
                        ਜੇ (k == 0)
                                ਵਾਪਸ 0;
                        ਵਾਪਸ -1;
                }
                ਜੇ (k == 0)
                        ਵਾਪਸ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ਜੇ (r != 0)
                        ਵਾਪਸ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ਜੇ (r != 0)
                        ਵਾਪਸ r;
        }
}

ਪੂਰਨ ਰਸਤਾ_compare_filename(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        _cleanup_free_ ਅੱਖਰ *fa = ਨਿਰਾ, *fb = ਨਿਰਾ;
        ਪੂਰਨ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        j = ਰਸਤਾ_ਕੱਢੋ_filename(a, &fa);
        k = ਰਸਤਾ_ਕੱਢੋ_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ਜੇ (r != 0)
                ਵਾਪਸ r;

        /* fallback to use strcmp() if both paths are invalid. */
        ਜੇ (j < 0)
                ਵਾਪਸ strcmp(a, b);

        ਵਾਪਸ strcmp(fa, fb);
}

ਪੂਰਨ ਰਸਤਾ_equal_or_inode_same_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b, ਪੂਰਨ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ਜੇ (ਰਸਤਾ_equal(a, b))
                ਵਾਪਸ 1;

        ਜੇ (!a || !b)
                ਵਾਪਸ 0;

        ਵਾਪਸ inode_same(a, b, flags);
}

ਅੱਖਰ* ਰਸਤਾ_extend_ਅੰਦਰੂਨੀ(ਅੱਖਰ **x, ...) {
        size_t sz, old_sz;
        ਅੱਖਰ *q, *nx;
        ਅਚਲ ਅੱਖਰ *p;
        va_list ap;
        bool slash;

        ਸੂਚਕ_MAY_BE_NULL(x);

        /* Joins all listed strings until the sentinel and places a "/" between them unless the strings
         * end/begin already with one so that it is unnecessary. Note that slashes which are already
         * duplicate won't be removed. The string returned is hence always equal to or longer than the sum of
         * the lengths of the individual strings.
         *
         * The first argument may be an already allocated string that is extended via realloc() if
         * non-NULL. path_extend() and path_join() are macro wrappers around this function, making use of the
         * first parameter to distinguish the two operations.
         *
         * Note: any listed empty string is simply skipped. This can be useful for concatenating strings of
         * which some are optional.
         *
         * Examples:
         *
         * path_join("foo", "bar") → "foo/bar"
         * path_join("foo/", "bar") → "foo/bar"
         * path_join("", "foo", "", "bar", "") → "foo/bar" */

        sz = old_sz = x ? strlen_ptr(*x) : 0;
        va_start(ap, x);
        ਜਦੋਂ ((p = va_arg(ap, ਅੱਖਰ*)) != ਸੂਚਕ_MAX) {
                size_t ਜੋੜੋ;

                ਜੇ (isempty(p))
                        ਜਾਰੀ;

                ਜੋੜੋ = 1 + ਲੜੀ_ਲੰਬਾਈ(p);
                ਜੇ (sz > SIZE_MAX - ਜੋੜੋ) { /* overflow check */
                        va_end(ap);
                        ਵਾਪਸ ਨਿਰਾ;
                }

                sz += ਜੋੜੋ;
        }
        va_end(ap);

        nx = realloc(x ? *x : ਨਿਰਾ, GREEDY_ALLOC_ਗੇੜਾ_ਉੱਪਰ(sz+1));
        ਜੇ (!nx)
                ਵਾਪਸ ਨਿਰਾ;
        ਜੇ (x)
                *x = nx;

        ਜੇ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        ਨਹੀਂਤਾਂ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ਜਦੋਂ ((p = va_arg(ap, ਅੱਖਰ*)) != ਸੂਚਕ_MAX) {
                ਜੇ (isempty(p))
                        ਜਾਰੀ;

                ਜੇ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        ਵਾਪਸ nx;
}

ਪੂਰਨ open_and_check_executable(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ, ਅੱਖਰ **ret_ਰਸਤਾ, ਪੂਰਨ *ret_fd) {
        _cleanup_close_ ਪੂਰਨ fd = -EBADF;
        _cleanup_free_ ਅੱਖਰ *resolved = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ਜੇ (ਜੜ੍ਹ) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, ਜੜ੍ਹ, CHASE_ਅਗੇਤਰ_ਜੜ੍ਹ, &resolved, &fd);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                name = resolved;
        } ਨਹੀਂਤਾਂ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_ਰਸਤਾ|O_CLOEXEC);
                ਜੇ (fd < 0)
                        ਵਾਪਸ -errno;
        }

        r = fd_verify_regular(fd);
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        r = access_fd(fd, X_OK);
        ਜੇ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਜੇ (ret_ਰਸਤਾ) {
                ਜੇ (resolved)
                        *ret_ਰਸਤਾ = TAKE_PTR(resolved);
                ਨਹੀਂਤਾਂ {
                        r = ਰਸਤਾ_make_absolute_cwd(name, ret_ਰਸਤਾ);
                        ਜੇ (r < 0)
                                ਵਾਪਸ r;

                        ਰਸਤਾ_simplify(*ret_ਰਸਤਾ);
                }
        }

        ਜੇ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ਵਾਪਸ 0;
}

ਪੂਰਨ ਲੱਭੋ_executable_ਭਰਿਆ(
                ਅਚਲ ਅੱਖਰ *name,
                ਅਚਲ ਅੱਖਰ *ਜੜ੍ਹ,
                ਅੱਖਰ * ਅਚਲ *exec_ਖੋਜ_ਰਸਤਾ,
                bool use_ਰਸਤਾ_envvar,
                ਅੱਖਰ **ret_filename,
                ਪੂਰਨ *ret_fd) {

        ਪੂਰਨ ਆਖਰੀ_error = -ENOENT, r = 0;

        ਪ੍ਰਤਿਜ੍ਞਾ(name);

        ਜੇ (is_ਰਸਤਾ(name))
                ਵਾਪਸ open_and_check_executable(name, ਜੜ੍ਹ, ret_filename, ret_fd);

        ਜੇ (exec_ਖੋਜ_ਰਸਤਾ) {
                STRV_FOREACH(element, exec_ਖੋਜ_ਰਸਤਾ) {
                        _cleanup_free_ ਅੱਖਰ *ਭਰਿਆ_ਰਸਤਾ = ਨਿਰਾ;

                        ਜੇ (!ਰਸਤਾ_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                ਜਾਰੀ;
                        }

                        ਭਰਿਆ_ਰਸਤਾ = ਰਸਤਾ_join(*element, name);
                        ਜੇ (!ਭਰਿਆ_ਰਸਤਾ)
                                ਵਾਪਸ -ENOMEM;

                        r = open_and_check_executable(ਭਰਿਆ_ਰਸਤਾ, ਜੜ੍ਹ, ret_filename, ret_fd);
                        ਜੇ (r >= 0)
                                ਵਾਪਸ 0;
                        ਜੇ (r != -EACCES)
                                ਆਖਰੀ_error = r;
                }
                ਵਾਪਸ ਆਖਰੀ_error;
        }

        ਅਚਲ ਅੱਖਰ *p = ਨਿਰਾ;

        ਜੇ (use_ਰਸਤਾ_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ਜੇ (!p)
                p = default_ਰਸਤਾ();

        /* Resolve a single-component name to a full path */
        ਲਈ (;;) {
                _cleanup_free_ ਅੱਖਰ *element = ਨਿਰਾ;

                r = ਕੱਢੋ_ਪਹਿਲਾ_ਸ਼ਬਦ(&p, &element, ":", ਕੱਢੋ_RELAX|ਕੱਢੋ_DONT_COALESCE_SEPARATORS);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (!ਰਸਤਾ_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        ਜਾਰੀ;
                }

                ਜੇ (!ਰਸਤਾ_extend(&element, name))
                        ਵਾਪਸ -ENOMEM;

                r = open_and_check_executable(element, ਜੜ੍ਹ, ret_filename, ret_fd);
                ਜੇ (r >= 0) /* Found it! */
                        ਵਾਪਸ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ਜੇ (r != -EACCES)
                        ਆਖਰੀ_error = r;
        }

        ਵਾਪਸ ਆਖਰੀ_error;
}

ਸਥਿਰ ਪੂਰਨ executable_is_good(ਅਚਲ ਅੱਖਰ *executable) {
        _cleanup_free_ ਅੱਖਰ *p = ਨਿਰਾ, *d = ਨਿਰਾ;
        ਪੂਰਨ r;

        r = ਲੱਭੋ_executable(executable, &p);
        ਜੇ (r == -ENOENT)
                ਵਾਪਸ 0;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ਜੇ (r == -EINVAL) /* not a symlink */
                ਵਾਪਸ 1;
        ਜੇ (r < 0)
                ਵਾਪਸ r;

        ਵਾਪਸ !ਰਸਤਾ_ਵਿੱਚ_ਸੈੱਟ(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

ਪੂਰਨ fsck_exists(ਖਾਲੀ) {
        ਵਾਪਸ executable_is_good("fsck");
}

ਪੂਰਨ fsck_exists_for_fstype(ਅਚਲ ਅੱਖਰ *fstype) {
        ਅਚਲ ਅੱਖਰ *checker;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(fstype);

        ਜੇ (streq(fstype, "auto"))
                ਵਾਪਸ -EINVAL;

        r = fsck_exists();
        ਜੇ (r <= 0)
                ਵਾਪਸ r;

        checker = strjoina("fsck.", fstype);
        ਵਾਪਸ executable_is_good(checker);
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_slash_or_dot(ਅਚਲ ਅੱਖਰ *p) {
        ਸੂਚਕ_MAY_BE_NULL(p);

        ਲਈ (; !isempty(p); p++) {
                ਜੇ (*p == '/')
                        ਜਾਰੀ;
                ਜੇ (startswith(p, "./")) {
                        p++;
                        ਜਾਰੀ;
                }
                ਰੋਕੋ;
        }
        ਵਾਪਸ p;
}

ਪੂਰਨ ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(ਅਚਲ ਅੱਖਰ **p, bool accept_dot_dot, ਅਚਲ ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *q, *ਪਹਿਲਾ, *ਸਿਰਾ_ਪਹਿਲਾ, *ਅਗਲਾ_2;
        size_t len;

        ਪ੍ਰਤਿਜ੍ਞਾ(p);

        /* When a path is input, then returns the pointer to the first component and its length, and
         * move the input pointer to the next component or nul. This skips both over any '/'
         * immediately *before* and *after* the first component before returning.
         *
         * Examples
         *   Input:  p: "//.//aaa///bbbbb/cc"
         *   Output: p: "bbbbb///cc"
         *           ret: "aaa///bbbbb/cc"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "aaa//"
         *   Output: p: (pointer to NUL)
         *           ret: "aaa//"
         *           return value: 3 (== strlen("aaa"))
         *
         *   Input:  p: "/", ".", ""
         *   Output: p: (pointer to NUL)
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: NULL
         *   Output: p: NULL
         *           ret: NULL
         *           return value: 0
         *
         *   Input:  p: "(too long component)"
         *   Output: return value: -EINVAL
         *
         *   (when accept_dot_dot is false)
         *   Input:  p: "//..//aaa///bbbbb/cc"
         *   Output: return value: -EINVAL
         */

        q = *p;

        ਪਹਿਲਾ = skip_slash_or_dot(q);
        ਜੇ (isempty(ਪਹਿਲਾ)) {
                *p = ਪਹਿਲਾ;
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }
        ਜੇ (streq(ਪਹਿਲਾ, ".")) {
                *p = ਪਹਿਲਾ + 1;
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਸਿਰਾ_ਪਹਿਲਾ = strchrnul(ਪਹਿਲਾ, '/');
        len = ਸਿਰਾ_ਪਹਿਲਾ - ਪਹਿਲਾ;

        ਜੇ (len > NAME_MAX)
                ਵਾਪਸ -EINVAL;
        ਜੇ (!accept_dot_dot && len == 2 && ਪਹਿਲਾ[0] == '.' && ਪਹਿਲਾ[1] == '.')
                ਵਾਪਸ -EINVAL;

        ਅਗਲਾ_2 = skip_slash_or_dot(ਸਿਰਾ_ਪਹਿਲਾ);

        *p = ਅਗਲਾ_2 + streq(ਅਗਲਾ_2, ".");
        ਜੇ (ret)
                *ret = ਪਹਿਲਾ;
        ਵਾਪਸ len;
}

ਸਥਿਰ ਅਚਲ ਅੱਖਰ* skip_slash_or_dot_backward(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *q) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(!q || q >= ਰਸਤਾ);

        ਲਈ (; q; q = PTR_SUB1(q, ਰਸਤਾ)) {
                ਜੇ (*q == '/')
                        ਜਾਰੀ;
                ਜੇ (q > ਰਸਤਾ && strneq(q - 1, "/.", 2))
                        ਜਾਰੀ;
                ਜੇ (q == ਰਸਤਾ && *q == '.')
                        ਜਾਰੀ;
                ਰੋਕੋ;
        }
        ਵਾਪਸ q;
}

ਪੂਰਨ ਰਸਤਾ_ਲੱਭੋ_ਆਖਰੀ_component(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, bool accept_dot_dot, ਅਚਲ ਅੱਖਰ **ਅਗਲਾ_2, ਅਚਲ ਅੱਖਰ **ret) {
        ਅਚਲ ਅੱਖਰ *q, *ਆਖਰੀ_ਸਿਰਾ, *ਆਖਰੀ_begin;
        size_t len;

        ਸੂਚਕ_MAY_BE_NULL(ਅਗਲਾ_2);
        ਸੂਚਕ_MAY_BE_NULL(ret);

        /* Similar to path_find_first_component(), but search components from the end.
        *
        * Examples
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: NULL
        *   Output: next: "/cc//././"
        *           ret: "cc//././"
        *           return value: 2 (== strlen("cc"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "/cc//././"
        *   Output: next: "///bbbbb/cc//././"
        *           ret: "bbbbb/cc//././"
        *           return value: 5 (== strlen("bbbbb"))
        *
        *   Input:  path: "//.//aaa///bbbbb/cc//././"
        *           next: "///bbbbb/cc//././"
        *   Output: next: "//.//aaa///bbbbb/cc//././" (next == path)
        *           ret: "aaa///bbbbb/cc//././"
        *           return value: 3 (== strlen("aaa"))
        *
        *   Input:  path: "/", ".", "", or NULL
        *   Output: next: equivalent to path
        *           ret: NULL
        *           return value: 0
        *
        *   Input:  path: "(too long component)"
        *   Output: return value: -EINVAL
        *
        *   (when accept_dot_dot is false)
        *   Input:  path: "//..//aaa///bbbbb/cc/..//"
        *   Output: return value: -EINVAL
        */

        ਜੇ (isempty(ਰਸਤਾ)) {
                ਜੇ (ਅਗਲਾ_2)
                        *ਅਗਲਾ_2 = ਰਸਤਾ;
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਜੇ (ਅਗਲਾ_2 && *ਅਗਲਾ_2) {
                ਜੇ (*ਅਗਲਾ_2 < ਰਸਤਾ || *ਅਗਲਾ_2 > ਰਸਤਾ + ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ))
                        ਵਾਪਸ -EINVAL;
                ਜੇ (*ਅਗਲਾ_2 == ਰਸਤਾ) {
                        ਜੇ (ret)
                                *ret = ਨਿਰਾ;
                        ਵਾਪਸ 0;
                }
                ਜੇ (!ਵਿੱਚ_ਸੈੱਟ(**ਅਗਲਾ_2, '\0', '/'))
                        ਵਾਪਸ -EINVAL;
                q = *ਅਗਲਾ_2 - 1;
        } ਨਹੀਂਤਾਂ
                q = ਰਸਤਾ + ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ) - 1;

        q = skip_slash_or_dot_backward(ਰਸਤਾ, q);
        ਜੇ (!q) { /* the root directory, "." or "./" */
                ਜੇ (ਅਗਲਾ_2)
                        *ਅਗਲਾ_2 = ਰਸਤਾ;
                ਜੇ (ret)
                        *ret = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        ਆਖਰੀ_ਸਿਰਾ = q + 1;

        ਜਦੋਂ (q && *q != '/')
                q = PTR_SUB1(q, ਰਸਤਾ);

        ਆਖਰੀ_begin = q ? q + 1 : ਰਸਤਾ;
        len = ਆਖਰੀ_ਸਿਰਾ - ਆਖਰੀ_begin;

        ਜੇ (len > NAME_MAX)
                ਵਾਪਸ -EINVAL;
        ਜੇ (!accept_dot_dot && len == 2 && strneq(ਆਖਰੀ_begin, "..", 2))
                ਵਾਪਸ -EINVAL;

        ਜੇ (ਅਗਲਾ_2) {
                q = skip_slash_or_dot_backward(ਰਸਤਾ, q);
                *ਅਗਲਾ_2 = q ? q + 1 : ਰਸਤਾ;
        }

        ਜੇ (ret)
                *ret = ਆਖਰੀ_begin;
        ਵਾਪਸ len;
}

ਅਚਲ ਅੱਖਰ* ਆਖਰੀ_ਰਸਤਾ_component(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {

        /* Finds the last component of the path, preserving the optional trailing slash that signifies a directory.
         *
         *    a/b/c → c
         *    a/b/c/ → c/
         *    x → x
         *    x/ → x/
         *    /y → y
         *    /y/ → y/
         *    / → /
         *    // → /
         *    /foo/a → a
         *    /foo/a/ → a/
         *
         *    Also, the empty string is mapped to itself.
         *
         * This is different than basename(), which returns "" when a trailing slash is present.
         *
         * This always succeeds (except if you pass NULL in which case it returns NULL, too).
         */

        ਬਿਨਾਨਿਸ਼ਾਨੀ l, k;

        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ ਨਿਰਾ;

        l = k = ਲੜੀ_ਲੰਬਾਈ(ਰਸਤਾ);
        ਜੇ (l == 0) /* special case — an empty string */
                ਵਾਪਸ ਰਸਤਾ;

        ਜਦੋਂ (k > 0 && ਰਸਤਾ[k-1] == '/')
                k--;

        ਜੇ (k == 0) /* the root directory */
                ਵਾਪਸ ਰਸਤਾ + l - 1;

        ਜਦੋਂ (k > 0 && ਰਸਤਾ[k-1] != '/')
                k--;

        ਵਾਪਸ ਰਸਤਾ + k;
}

ਪੂਰਨ ਰਸਤਾ_ਵੰਡੋ_ਅਗੇਤਰ_filename(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅੱਖਰ **ret_dir, ਅੱਖਰ **ret_filename) {
        _cleanup_free_ ਅੱਖਰ *d = ਨਿਰਾ;
        ਅਚਲ ਅੱਖਰ *c, *ਅਗਲਾ_2 = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਸੂਚਕ_MAY_BE_NULL(ਰਸਤਾ);

        /* Split the path into dir prefix/filename pair. Returns:
         *
         * -EINVAL        → if the path is not valid
         * -EADDRNOTAVAIL → if the path refers to the uppermost directory in hierarchy (i.e. has neither
         *                  dir prefix nor filename - the root dir itself or ".")
         * -EDESTADDRREQ  → if only a filename was passed, and caller only specifies ret_dir
         * -ENOMEM        → no memory
         *
         * Returns >= 0 on success. If the input path has a trailing slash, returns O_DIRECTORY, to
         * indicate the referenced file must be a directory.
         *
         * This function guarantees to return a fully valid filename, i.e. one that passes
         * filename_is_valid() – this means "." and ".." are not accepted. */

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ -EINVAL;

        r = ਰਸਤਾ_ਲੱਭੋ_ਆਖਰੀ_component(ਰਸਤਾ, /* accept_dot_dot = */ false, &ਅਗਲਾ_2, &c);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r == 0) /* root directory or "." */
                ਵਾਪਸ -EADDRNOTAVAIL;

        ਜੇ (ret_dir) {
                ਜੇ (ਅਗਲਾ_2 == ਰਸਤਾ) {
                        ਜੇ (*ਰਸਤਾ != '/') { /* filename only */
                                ਜੇ (!ret_filename)
                                        ਵਾਪਸ -EDESTADDRREQ;
                        } ਨਹੀਂਤਾਂ {
                                d = strdup("/");
                                ਜੇ (!d)
                                        ਵਾਪਸ -ENOMEM;
                        }
                } ਨਹੀਂਤਾਂ {
                        d = strndup(ਰਸਤਾ, ਅਗਲਾ_2 - ਰਸਤਾ);
                        ਜੇ (!d)
                                ਵਾਪਸ -ENOMEM;

                        ਰਸਤਾ_simplify(d);

                        ਜੇ (!ਰਸਤਾ_is_valid(d))
                                ਵਾਪਸ -EINVAL;
                }

        } ਨਹੀਂਤਾਂ ਜੇ (!ਰਸਤਾ_is_valid(ਰਸਤਾ))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                ਵਾਪਸ -EINVAL;

        ਜੇ (ret_filename) {
                ਅੱਖਰ *fn = strndup(c, r);
                ਜੇ (!fn)
                        ਵਾਪਸ -ENOMEM;

                *ret_filename = fn;
        }

        ਜੇ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        ਵਾਪਸ ਲੜੀ_ਲੰਬਾਈ(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ਅਚਲ ਅੱਖਰ *p) {
        ਅਚਲ ਅੱਖਰ *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ਜੇ (!p)
                ਵਾਪਸ false;

        e = strchrnul(p, '/');
        ਜੇ (*e != 0)
                ਵਾਪਸ false;

        ਜੇ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

bool filename_is_valid(ਅਚਲ ਅੱਖਰ *p) {

        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਜੇ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                ਵਾਪਸ false;

        ਵਾਪਸ filename_part_is_valid(p);
}

bool ਰਸਤਾ_is_valid_ਭਰਿਆ(ਅਚਲ ਅੱਖਰ *p, bool accept_dot_dot) {
        ਜੇ (isempty(p))
                ਵਾਪਸ false;

        ਲਈ (ਅਚਲ ਅੱਖਰ *e = p;;) {
                ਪੂਰਨ r;

                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&e, accept_dot_dot, ਨਿਰਾ);
                ਜੇ (r < 0)
                        ਵਾਪਸ false;

                ਜੇ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        ਵਾਪਸ false;
                ਜੇ (*e == 0)           /* End of string? Yay! */
                        ਵਾਪਸ true;
        }
}

bool ਰਸਤਾ_is_normalized(ਅਚਲ ਅੱਖਰ *p) {
        ਜੇ (!ਰਸਤਾ_is_ਸੁਰੱਖਿਅਤ(p))
                ਵਾਪਸ false;

        ਜੇ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || ਲੜੀ_ਖੋਜ(p, "/./"))
                ਵਾਪਸ false;

        ਜੇ (ਲੜੀ_ਖੋਜ(p, "//"))
                ਵਾਪਸ false;

        ਵਾਪਸ true;
}

ਪੂਰਨ file_ਵਿੱਚ_same_dir(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਅਚਲ ਅੱਖਰ *filename, ਅੱਖਰ **ret) {
        _cleanup_free_ ਅੱਖਰ *b = ਨਿਰਾ;
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);
        ਪ੍ਰਤਿਜ੍ਞਾ(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ਜੇ (ਰਸਤਾ_is_absolute(filename))
                b = strdup(filename);
        ਨਹੀਂਤਾਂ {
                _cleanup_free_ ਅੱਖਰ *dn = ਨਿਰਾ;

                r = ਰਸਤਾ_ਕੱਢੋ_directory(ਰਸਤਾ, &dn);
                ਜੇ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                ਨਹੀਂਤਾਂ ਜੇ (r < 0)
                        ਵਾਪਸ r;
                ਨਹੀਂਤਾਂ
                        b = ਰਸਤਾ_join(dn, filename);
        }
        ਜੇ (!b)
                ਵਾਪਸ -ENOMEM;

        *ret = TAKE_PTR(b);
        ਵਾਪਸ 0;
}

bool hidden_or_backup_file(ਅਚਲ ਅੱਖਰ *filename) {
        ਪ੍ਰਤਿਜ੍ਞਾ(filename);

        ਜੇ (filename[0] == '.' ||
            STR_ਵਿੱਚ_ਸੈੱਟ(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                ਵਾਪਸ true;

        ਅਚਲ ਅੱਖਰ *dot = strrchr(filename, '.');
        ਜੇ (!dot)
                ਵਾਪਸ false;

        /* Please, let's not add more entries to the list below. If external projects think it's a good idea
         * to come up with always new suffixes and that everybody else should just adjust to that, then it
         * really should be on them. Hence, in future, let's not add any more entries. Instead, let's ask
         * those packages to instead adopt one of the generic suffixes/prefixes for hidden files or backups,
         * possibly augmented with an additional string. Specifically: there's now:
         *
         *    The generic suffixes "~" and ".bak" for backup files
         *    The generic prefix "." for hidden files
         *
         * Thus, if a new package manager "foopkg" wants its own set of ".foopkg-new", ".foopkg-old",
         * ".foopkg-dist" or so registered, let's refuse that and ask them to use ".foopkg.new",
         * ".foopkg.old" or ".foopkg~" instead.
         */

        ਵਾਪਸ STR_ਵਿੱਚ_ਸੈੱਟ(dot + 1,
                          "ignore",
                          "rpmnew",
                          "rpmsave",
                          "rpmorig",
                          "dpkg-old",
                          "dpkg-new",
                          "dpkg-tmp",
                          "dpkg-dist",
                          "dpkg-bak",
                          "dpkg-backup",
                          "dpkg-remove",
                          "ucf-new",
                          "ucf-old",
                          "ucf-dist",
                          "swp",
                          "bak",
                          "old",
                          "new");
}

bool is_device_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ਅਚਲ ਅੱਖਰ *p = ਰਸਤਾ_STARTSWITH_ਸੈੱਟ(ASSERT_PTR(ਰਸਤਾ), "/dev/", "/sys/");
        ਵਾਪਸ !isempty(p);
}

bool valid_device_ਗੰਢ_ਰਸਤਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ਜੇ (!ਰਸਤਾ_STARTSWITH_ਸੈੱਟ(ਰਸਤਾ, "/dev/", "/run/systemd/inaccessible/"))
                ਵਾਪਸ false;

        ਜੇ (endswith(ਰਸਤਾ, "/")) /* can't be a device node if it ends in a slash */
                ਵਾਪਸ false;

        ਵਾਪਸ ਰਸਤਾ_is_normalized(ਰਸਤਾ);
}

bool valid_device_allow_ਨਮੂਨਾ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ਜੇ (STARTSWITH_ਸੈੱਟ(ਰਸਤਾ, "block-", "char-"))
                ਵਾਪਸ true;

        ਵਾਪਸ valid_device_ਗੰਢ_ਰਸਤਾ(ਰਸਤਾ);
}

bool dot_or_dot_dot(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ false;
        ਜੇ (ਰਸਤਾ[0] != '.')
                ਵਾਪਸ false;
        ਜੇ (ਰਸਤਾ[1] == 0)
                ਵਾਪਸ true;
        ਜੇ (ਰਸਤਾ[1] != '.')
                ਵਾਪਸ false;

        ਵਾਪਸ ਰਸਤਾ[2] == 0;
}

bool ਰਸਤਾ_ਸਿੱਟਾ_ਦਿੰਦਾ_directory(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ਜੇ (!ਰਸਤਾ)
                ਵਾਪਸ false;

        ਜੇ (dot_or_dot_dot(ਰਸਤਾ))
                ਵਾਪਸ true;

        ਵਾਪਸ ENDSWITH_ਸੈੱਟ(ਰਸਤਾ, "/", "/.", "/..");
}

bool ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ਜੇ (isempty(ਰਸਤਾ))
                ਵਾਪਸ true;

        ਵਾਪਸ ਰਸਤਾ_equal(ਰਸਤਾ, "/");
}

ਅਚਲ ਅੱਖਰ* ਖਾਲੀ_ਥਾਵਾਂ_ਮੰਜ਼ਿਲ_ਜੜ੍ਹ(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਵਾਪਸ isempty(ਰਸਤਾ) ? "/" : ਰਸਤਾ;
}

ਪੂਰਨ ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ_harder_ਮੰਜ਼ਿਲ_null(ਅਚਲ ਅੱਖਰ **ਰਸਤਾ) {
        ਪੂਰਨ r;

        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        /* This nullifies the input path when the path is empty or points to "/". */

        ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_or_ਜੜ੍ਹ(*ਰਸਤਾ)) {
                *ਰਸਤਾ = ਨਿਰਾ;
                ਵਾਪਸ 0;
        }

        r = ਰਸਤਾ_is_ਜੜ੍ਹ(*ਰਸਤਾ);
        ਜੇ (r < 0)
                ਵਾਪਸ r;
        ਜੇ (r > 0)
                *ਰਸਤਾ = ਨਿਰਾ;

        ਵਾਪਸ 0;
}

bool ਰਸਤਾ_strv_ਸ਼ਾਮਲ_ਹੈ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        STRV_FOREACH(i, l)
                ਜੇ (ਰਸਤਾ_equal(*i, ਰਸਤਾ))
                        ਵਾਪਸ true;

        ਵਾਪਸ false;
}

bool prefixed_ਰਸਤਾ_strv_ਸ਼ਾਮਲ_ਹੈ(ਅੱਖਰ * ਅਚਲ *l, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਰਸਤਾ);

        STRV_FOREACH(i, l) {
                ਅਚਲ ਅੱਖਰ *j = *i;

                ਜੇ (*j == '-')
                        j++;
                ਜੇ (*j == '+')
                        j++;

                ਜੇ (ਰਸਤਾ_equal(j, ਰਸਤਾ))
                        ਵਾਪਸ true;
        }

        ਵਾਪਸ false;
}

ਪੂਰਨ ਰਸਤਾ_glob_can_ਮੇਲ(ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, ਅਚਲ ਅੱਖਰ *ਅਗੇਤਰ, ਅੱਖਰ **ret) {
        ਪ੍ਰਤਿਜ੍ਞਾ(ਨਮੂਨਾ);
        ਪ੍ਰਤਿਜ੍ਞਾ(ਅਗੇਤਰ);

        ਲਈ (ਅਚਲ ਅੱਖਰ *a = ਨਮੂਨਾ, *b = ਅਗੇਤਰ;;) {
                _cleanup_free_ ਅੱਖਰ *g = ਨਿਰਾ, *h = ਨਿਰਾ;
                ਅਚਲ ਅੱਖਰ *p, *q;
                ਪੂਰਨ r, s;

                r = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&a, /* accept_dot_dot= */ false, &p);
                ਜੇ (r < 0)
                        ਵਾਪਸ r;

                s = ਰਸਤਾ_ਲੱਭੋ_ਪਹਿਲਾ_component(&b, /* accept_dot_dot= */ false, &q);
                ਜੇ (s < 0)
                        ਵਾਪਸ s;

                ਜੇ (s == 0) {
                        /* The pattern matches the prefix. */
                        ਜੇ (ret) {
                                ਅੱਖਰ *t;

                                t = ਰਸਤਾ_join(ਅਗੇਤਰ, p);
                                ਜੇ (!t)
                                        ਵਾਪਸ -ENOMEM;

                                *ret = t;
                        }
                        ਵਾਪਸ true;
                }

                ਜੇ (r == 0)
                        ਰੋਕੋ;

                ਜੇ (r == s && strneq(p, q, r))
                        ਜਾਰੀ; /* common component. Check next. */

                g = strndup(p, r);
                ਜੇ (!g)
                        ਵਾਪਸ -ENOMEM;

                ਜੇ (!string_is_glob(g))
                        ਰੋਕੋ;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ਜੇ (!h)
                        ਵਾਪਸ -ENOMEM;

                r = fnmatch(g, h, 0);
                ਜੇ (r == FNM_NOMATCH)
                        ਰੋਕੋ;
                ਜੇ (r != 0) /* Failure to process pattern? */
                        ਵਾਪਸ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ਜੇ (ret)
                *ret = ਨਿਰਾ;
        ਵਾਪਸ false;
}

#ਜੇ HAVE_ਵੰਡੋ_BIN
ਸਥਿਰ bool dir_is_ਵੰਡੋ(ਅਚਲ ਅੱਖਰ *a, ਅਚਲ ਅੱਖਰ *b) {
        ਪੂਰਨ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ਜੇ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                ਵਾਪਸ true;
        }
        ਵਾਪਸ r == 0;
}
#ਜੇ_ਅੰਤ

ਅਚਲ ਅੱਖਰ* default_ਰਸਤਾ(ਖਾਲੀ) {
#ਜੇ HAVE_ਵੰਡੋ_BIN
        ਸਥਿਰ ਅਚਲ ਅੱਖਰ *default_ਰਸਤਾ_2 = ਨਿਰਾ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ਜੇ (default_ਰਸਤਾ_2)
                ਵਾਪਸ default_ਰਸਤਾ_2;

        ਜੇ (dir_is_ਵੰਡੋ("/usr/sbin", "/usr/bin"))
                ਵਾਪਸ (default_ਰਸਤਾ_2 = DEFAULT_ਰਸਤਾ_WITH_ਭਰਿਆ_SBIN);  /* a */
        ਜੇ (dir_is_ਵੰਡੋ("/usr/local/sbin", "/usr/local/bin"))
                ਵਾਪਸ (default_ਰਸਤਾ_2 = DEFAULT_ਰਸਤਾ_WITH_LOCAL_SBIN); /* b */
        ਵਾਪਸ (default_ਰਸਤਾ_2 = DEFAULT_ਰਸਤਾ_WITHOUT_SBIN);            /* c */
#ਨਹੀਂਤਾਂ
        ਵਾਪਸ DEFAULT_ਰਸਤਾ_WITHOUT_SBIN;
#ਜੇ_ਅੰਤ
}
