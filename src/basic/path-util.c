/* SPDX-License-Identifier: LGPL-2.1-or-later */

#అన్తర్భావ <fnmatch.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <unistd.h>

#అన్తర్భావ "alloc-util.h"
#అన్తర్భావ "chase.h"
#అన్తర్భావ "errno-util.h"
#అన్తర్భావ "extract-word.h"
#అన్తర్భావ "fd-util.h"
#అన్తర్భావ "fs-util.h"
#అన్తర్భావ "glob-util.h"
#అన్తర్భావ "log.h"
#అన్తర్భావ "path-util.h"
#అన్తర్భావ "stat-util.h"
#అన్తర్భావ "string-util.h"
#అన్తర్భావ "strv.h"

bool is_మార్గం(స్థిరరాశి అక్షరం *p) {
        ఒకవేళ (!p) /* A NULL pointer is definitely not a path */
                తిరిగి false;

        తిరిగి strchr(p, '/');
}

పూర్ణాంకం మార్గం_విభజించు_and_make_absolute(స్థిరరాశి అక్షరం *p, అక్షరం ***ret) {
        _cleanup_strv_free_ అక్షరం **l = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        l = strv_విభజించు(p, ":");
        ఒకవేళ (!l)
                తిరిగి -ENOMEM;

        r = మార్గం_strv_make_absolute_cwd(l);
        ఒకవేళ (r < 0)
                తిరిగి r;

        *ret = TAKE_PTR(l);
        తిరిగి r;
}

అక్షరం* మార్గం_make_absolute(స్థిరరాశి అక్షరం *p, స్థిరరాశి అక్షరం *ఉపసర్గ) {
        ప్రతిజ్ఞా(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ఒకవేళ (మార్గం_is_absolute(p) || isempty(ఉపసర్గ))
                తిరిగి strdup(p);

        తిరిగి మార్గం_join(ఉపసర్గ, p);
}

పూర్ణాంకం సురక్షిత_getcwd(అక్షరం **ret) {
        _cleanup_free_ అక్షరం *cwd = ఖాళీ;

        cwd = get_current_dir_name();
        ఒకవేళ (!cwd)
                తిరిగి negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ఒకవేళ (cwd[0] != '/')
                తిరిగి -ENOMEDIUM;

        ఒకవేళ (ret)
                *ret = TAKE_PTR(cwd);

        తిరిగి 0;
}

పూర్ణాంకం మార్గం_make_absolute_cwd(స్థిరరాశి అక్షరం *p, అక్షరం **ret) {
        అక్షరం *c;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(p);
        ప్రతిజ్ఞా(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ఒకవేళ (మార్గం_is_absolute(p))
                c = strdup(p);
        లేకపోతే {
                _cleanup_free_ అక్షరం *cwd = ఖాళీ;

                r = సురక్షిత_getcwd(&cwd);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                c = మార్గం_join(cwd, p);
        }
        ఒకవేళ (!c)
                తిరిగి -ENOMEM;

        *ret = c;
        తిరిగి 0;
}

పూర్ణాంకం మార్గం_make_relative(స్థిరరాశి అక్షరం *from, స్థిరరాశి అక్షరం *to, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *ఫలితం = ఖాళీ;
        గుర్తులేని n_parents;
        స్థిరరాశి అక్షరం *f, *t;
        పూర్ణాంకం r, k;
        అక్షరం *p;

        ప్రతిజ్ఞా(from);
        ప్రతిజ్ఞా(to);
        ప్రతిజ్ఞా(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ఒకవేళ (!మార్గం_is_absolute(from) || !మార్గం_is_absolute(to))
                తిరిగి -EINVAL;

        కొరకు (;;) {
                r = మార్గం_కనుగొను_మొదటి_component(&from, true, &f);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                k = మార్గం_కనుగొను_మొదటి_component(&to, true, &t);
                ఒకవేళ (k < 0)
                        తిరిగి k;

                ఒకవేళ (r == 0) {
                        /* end of 'from' */
                        ఒకవేళ (k == 0) {
                                /* from and to are equivalent. */
                                ఫలితం = strdup(".");
                                ఒకవేళ (!ఫలితం)
                                        తిరిగి -ENOMEM;
                        } లేకపోతే {
                                /* 'to' is inside of 'from'. */
                                r = మార్గం_simplify_alloc(t, &ఫలితం);
                                ఒకవేళ (r < 0)
                                        తిరిగి r;

                                ఒకవేళ (!మార్గం_is_valid(ఫలితం))
                                        తిరిగి -EINVAL;
                        }

                        *ret = TAKE_PTR(ఫలితం);
                        తిరిగి 0;
                }

                ఒకవేళ (r != k || !strneq(f, t, r))
                        ఆపు;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        కొరకు (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = మార్గం_కనుగొను_మొదటి_component(&from, false, &f);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;
        }

        ఒకవేళ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                తిరిగి -EINVAL;

        ఫలితం = new(అక్షరం, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ఒకవేళ (!ఫలితం)
                తిరిగి -ENOMEM;

        కొరకు (p = ఫలితం; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ఒకవేళ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(ఫలితం);
                తిరిగి 0;
        }

        strcpy(p, t);

        మార్గం_simplify(ఫలితం);

        ఒకవేళ (!మార్గం_is_valid(ఫలితం))
                తిరిగి -EINVAL;

        *ret = TAKE_PTR(ఫలితం);
        తిరిగి 0;
}

పూర్ణాంకం మార్గం_make_relative_మాతృ(స్థిరరాశి అక్షరం *from_బిడ్డ, స్థిరరాశి అక్షరం *to, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *from = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(from_బిడ్డ);
        ప్రతిజ్ఞా(to);
        ప్రతిజ్ఞా(ret);

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

        r = మార్గం_వెలికితీయి_directory(from_బిడ్డ, &from);
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి మార్గం_make_relative(from, to, ret);
}

అక్షరం* మార్గం_startswith_strv(స్థిరరాశి అక్షరం *p, అక్షరం * స్థిరరాశి *strv) {
        ప్రతిజ్ఞా(p);

        STRV_FOREACH(s, strv) {
                అక్షరం *t;

                t = మార్గం_startswith(p, *s);
                ఒకవేళ (t)
                        తిరిగి t;
        }

        తిరిగి ఖాళీ;
}

పూర్ణాంకం మార్గం_strv_make_absolute_cwd(అక్షరం **l) {
        పూర్ణాంకం r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                అక్షరం *t;

                r = మార్గం_make_absolute_cwd(*s, &t);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                మార్గం_simplify(t);
                free_and_replace(*s, t);
        }

        తిరిగి 0;
}

అక్షరం** మార్గం_strv_resolve(అక్షరం **l, స్థిరరాశి అక్షరం *మూలం) {
        గుర్తులేని k = 0;
        bool enomem = false;
        పూర్ణాంకం r;

        ఒకవేళ (strv_isempty(l))
                తిరిగి l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ అక్షరం *orig = ఖాళీ;
                అక్షరం *t, *u;

                ఒకవేళ (మూలం) {
                        ఒకవేళ (!మార్గం_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                విడుదల(*s);
                                కొనసాగు;
                        }

                        orig = *s;
                        t = మార్గం_join(మూలం, orig);
                        ఒకవేళ (!t) {
                                enomem = true;
                                కొనసాగు;
                        }
                } లేకపోతే
                        t = *s;

                r = chase(t, మూలం, 0, &u, ఖాళీ);
                ఒకవేళ (r == -ENOENT) {
                        ఒకవేళ (మూలం) {
                                u = TAKE_PTR(orig);
                                విడుదల(t);
                        } లేకపోతే
                                u = t;
                } లేకపోతే ఒకవేళ (r < 0) {
                        విడుదల(t);

                        ఒకవేళ (r == -ENOMEM)
                                enomem = true;

                        కొనసాగు;
                } లేకపోతే ఒకవేళ (మూలం) {
                        అక్షరం *x;

                        విడుదల(t);
                        x = మార్గం_startswith(u, మూలం);
                        ఒకవేళ (x) {
                                /* restore the slash if it was lost */
                                ఒకవేళ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                విడుదల(u);
                                ఒకవేళ (!t) {
                                        enomem = true;
                                        కొనసాగు;
                                }
                                u = t;
                        } లేకపోతే {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } లేకపోతే
                        విడుదల(t);

                l[k++] = u;
        }

        l[k] = ఖాళీ;

        ఒకవేళ (enomem)
                తిరిగి ఖాళీ;

        తిరిగి l;
}

అక్షరం** మార్గం_strv_resolve_uniq(అక్షరం **l, స్థిరరాశి అక్షరం *మూలం) {

        ఒకవేళ (strv_isempty(l))
                తిరిగి l;

        ఒకవేళ (!మార్గం_strv_resolve(l, మూలం))
                తిరిగి ఖాళీ;

        తిరిగి strv_uniq(l);
}

అక్షరం* skip_leading_slash(స్థిరరాశి అక్షరం *p) {
        తిరిగి skip_leading_chars(p, "/");
}

అక్షరం* మార్గం_simplify_నిండిన(అక్షరం *మార్గం, మార్గం_Simplify_Flags flags) {
        bool కూడు_slash = false, keep_trailing_slash, absolute, beginning = true;
        అక్షరం *f = మార్గం;
        పూర్ణాంకం r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ఒకవేళ (isempty(మార్గం))
                తిరిగి మార్గం;

        keep_trailing_slash = FLAGS_సమితి(flags, మార్గం_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(మార్గం, "/");

        absolute = మార్గం_is_absolute(మార్గం);
        f += absolute;  /* Keep leading /, if present. */

        కొరకు (స్థిరరాశి అక్షరం *p = f;;) {
                స్థిరరాశి అక్షరం *e;

                r = మార్గం_కనుగొను_మొదటి_component(&p, true, &e);
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (r > 0 && absolute && beginning && మార్గం_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        కొనసాగు;

                beginning = false;

                ఒకవేళ (కూడు_slash)
                        *f++ = '/';

                ఒకవేళ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, అక్షరశ్రేణి_పొడవు(p) + 1);
                        తిరిగి మార్గం;
                }

                memmove(f, e, r);
                f += r;

                కూడు_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ఒకవేళ (f == మార్గం)
                *f++ = '.';

        ఒకవేళ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        తిరిగి మార్గం;
}

పూర్ణాంకం మార్గం_simplify_alloc(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret) {
        ప్రతిజ్ఞా(ret);

        ఒకవేళ (!మార్గం) {
                *ret = ఖాళీ;
                తిరిగి 0;
        }

        అక్షరం *t = strdup(మార్గం);
        ఒకవేళ (!t)
                తిరిగి -ENOMEM;

        *ret = మార్గం_simplify(t);
        తిరిగి 0;
}

అక్షరం* మార్గం_startswith_నిండిన(స్థిరరాశి అక్షరం *original_మార్గం, స్థిరరాశి అక్షరం *ఉపసర్గ, మార్గం_ప్రారంభం_With_Flags flags) {
        ప్రతిజ్ఞా(original_మార్గం);
        ప్రతిజ్ఞా(ఉపసర్గ);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        స్థిరరాశి అక్షరం *మార్గం = original_మార్గం;

        ఒకవేళ ((మార్గం[0] == '/') != (ఉపసర్గ[0] == '/'))
                తిరిగి ఖాళీ;

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *p, *q;
                పూర్ణాంకం m, n;

                m = మార్గం_కనుగొను_మొదటి_component(&మార్గం, !FLAGS_సమితి(flags, మార్గం_STARTSWITH_REFUSE_DOT_DOT), &p);
                ఒకవేళ (m < 0)
                        తిరిగి ఖాళీ;

                n = మార్గం_కనుగొను_మొదటి_component(&ఉపసర్గ, !FLAGS_సమితి(flags, మార్గం_STARTSWITH_REFUSE_DOT_DOT), &q);
                ఒకవేళ (n < 0)
                        తిరిగి ఖాళీ;

                ఒకవేళ (n == 0) {
                        ఒకవేళ (!p)
                                p = మార్గం;

                        ఒకవేళ (FLAGS_సమితి(flags, మార్గం_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ఒకవేళ (p <= original_మార్గం)
                                        తిరిగి ఖాళీ;

                                p--;

                                ఒకవేళ (*p != '/')
                                        తిరిగి ఖాళీ;
                        }

                        తిరిగి (అక్షరం*) p;
                }

                ఒకవేళ (m != n)
                        తిరిగి ఖాళీ;

                ఒకవేళ (!strneq(p, q, m))
                        తిరిగి ఖాళీ;
        }
}

పూర్ణాంకం మార్గం_compare(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        పూర్ణాంకం r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ఒకవేళ (r != 0)
                తిరిగి r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(మార్గం_is_absolute(a), మార్గం_is_absolute(b));
        ఒకవేళ (r != 0)
                తిరిగి r;

        కొరకు (;;) {
                స్థిరరాశి అక్షరం *aa, *bb;
                పూర్ణాంకం j, k;

                j = మార్గం_కనుగొను_మొదటి_component(&a, true, &aa);
                k = మార్గం_కనుగొను_మొదటి_component(&b, true, &bb);

                ఒకవేళ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ఒకవేళ (r != 0)
                                తిరిగి r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        తిరిగి strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ఒకవేళ (j == 0) {
                        ఒకవేళ (k == 0)
                                తిరిగి 0;
                        తిరిగి -1;
                }
                ఒకవేళ (k == 0)
                        తిరిగి 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ఒకవేళ (r != 0)
                        తిరిగి r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ఒకవేళ (r != 0)
                        తిరిగి r;
        }
}

పూర్ణాంకం మార్గం_compare_filename(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        _cleanup_free_ అక్షరం *fa = ఖాళీ, *fb = ఖాళీ;
        పూర్ణాంకం r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ఒకవేళ (r != 0)
                తిరిగి r;

        j = మార్గం_వెలికితీయి_filename(a, &fa);
        k = మార్గం_వెలికితీయి_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ఒకవేళ (r != 0)
                తిరిగి r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ఒకవేళ (r != 0)
                తిరిగి r;

        /* fallback to use strcmp() if both paths are invalid. */
        ఒకవేళ (j < 0)
                తిరిగి strcmp(a, b);

        తిరిగి strcmp(fa, fb);
}

పూర్ణాంకం మార్గం_equal_or_inode_same_నిండిన(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b, పూర్ణాంకం flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ఒకవేళ (మార్గం_equal(a, b))
                తిరిగి 1;

        ఒకవేళ (!a || !b)
                తిరిగి 0;

        తిరిగి inode_same(a, b, flags);
}

అక్షరం* మార్గం_extend_అంతర్గత(అక్షరం **x, ...) {
        size_t sz, old_sz;
        అక్షరం *q, *nx;
        స్థిరరాశి అక్షరం *p;
        va_list ap;
        bool slash;

        సూచిక_MAY_BE_NULL(x);

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
        అయితే ((p = va_arg(ap, అక్షరం*)) != సూచిక_MAX) {
                size_t కూడు;

                ఒకవేళ (isempty(p))
                        కొనసాగు;

                కూడు = 1 + అక్షరశ్రేణి_పొడవు(p);
                ఒకవేళ (sz > SIZE_MAX - కూడు) { /* overflow check */
                        va_end(ap);
                        తిరిగి ఖాళీ;
                }

                sz += కూడు;
        }
        va_end(ap);

        nx = realloc(x ? *x : ఖాళీ, GREEDY_ALLOC_విడత_పైకి(sz+1));
        ఒకవేళ (!nx)
                తిరిగి ఖాళీ;
        ఒకవేళ (x)
                *x = nx;

        ఒకవేళ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        లేకపోతే {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        అయితే ((p = va_arg(ap, అక్షరం*)) != సూచిక_MAX) {
                ఒకవేళ (isempty(p))
                        కొనసాగు;

                ఒకవేళ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        తిరిగి nx;
}

పూర్ణాంకం open_and_check_executable(స్థిరరాశి అక్షరం *name, స్థిరరాశి అక్షరం *మూలం, అక్షరం **ret_మార్గం, పూర్ణాంకం *ret_fd) {
        _cleanup_close_ పూర్ణాంకం fd = -EBADF;
        _cleanup_free_ అక్షరం *resolved = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ఒకవేళ (మూలం) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, మూలం, CHASE_ఉపసర్గ_మూలం, &resolved, &fd);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                name = resolved;
        } లేకపోతే {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_మార్గం|O_CLOEXEC);
                ఒకవేళ (fd < 0)
                        తిరిగి -errno;
        }

        r = fd_verify_regular(fd);
        ఒకవేళ (r < 0)
                తిరిగి r;

        r = access_fd(fd, X_OK);
        ఒకవేళ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ఒకవేళ (r < 0)
                తిరిగి r;

        ఒకవేళ (ret_మార్గం) {
                ఒకవేళ (resolved)
                        *ret_మార్గం = TAKE_PTR(resolved);
                లేకపోతే {
                        r = మార్గం_make_absolute_cwd(name, ret_మార్గం);
                        ఒకవేళ (r < 0)
                                తిరిగి r;

                        మార్గం_simplify(*ret_మార్గం);
                }
        }

        ఒకవేళ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        తిరిగి 0;
}

పూర్ణాంకం కనుగొను_executable_నిండిన(
                స్థిరరాశి అక్షరం *name,
                స్థిరరాశి అక్షరం *మూలం,
                అక్షరం * స్థిరరాశి *exec_శోధన_మార్గం,
                bool use_మార్గం_envvar,
                అక్షరం **ret_filename,
                పూర్ణాంకం *ret_fd) {

        పూర్ణాంకం చివరి_error = -ENOENT, r = 0;

        ప్రతిజ్ఞా(name);

        ఒకవేళ (is_మార్గం(name))
                తిరిగి open_and_check_executable(name, మూలం, ret_filename, ret_fd);

        ఒకవేళ (exec_శోధన_మార్గం) {
                STRV_FOREACH(element, exec_శోధన_మార్గం) {
                        _cleanup_free_ అక్షరం *నిండిన_మార్గం = ఖాళీ;

                        ఒకవేళ (!మార్గం_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                కొనసాగు;
                        }

                        నిండిన_మార్గం = మార్గం_join(*element, name);
                        ఒకవేళ (!నిండిన_మార్గం)
                                తిరిగి -ENOMEM;

                        r = open_and_check_executable(నిండిన_మార్గం, మూలం, ret_filename, ret_fd);
                        ఒకవేళ (r >= 0)
                                తిరిగి 0;
                        ఒకవేళ (r != -EACCES)
                                చివరి_error = r;
                }
                తిరిగి చివరి_error;
        }

        స్థిరరాశి అక్షరం *p = ఖాళీ;

        ఒకవేళ (use_మార్గం_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ఒకవేళ (!p)
                p = default_మార్గం();

        /* Resolve a single-component name to a full path */
        కొరకు (;;) {
                _cleanup_free_ అక్షరం *element = ఖాళీ;

                r = వెలికితీయి_మొదటి_మాట(&p, &element, ":", వెలికితీయి_RELAX|వెలికితీయి_DONT_COALESCE_SEPARATORS);
                ఒకవేళ (r < 0)
                        తిరిగి r;
                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (!మార్గం_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        కొనసాగు;
                }

                ఒకవేళ (!మార్గం_extend(&element, name))
                        తిరిగి -ENOMEM;

                r = open_and_check_executable(element, మూలం, ret_filename, ret_fd);
                ఒకవేళ (r >= 0) /* Found it! */
                        తిరిగి 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ఒకవేళ (r != -EACCES)
                        చివరి_error = r;
        }

        తిరిగి చివరి_error;
}

స్థిర పూర్ణాంకం executable_is_good(స్థిరరాశి అక్షరం *executable) {
        _cleanup_free_ అక్షరం *p = ఖాళీ, *d = ఖాళీ;
        పూర్ణాంకం r;

        r = కనుగొను_executable(executable, &p);
        ఒకవేళ (r == -ENOENT)
                తిరిగి 0;
        ఒకవేళ (r < 0)
                తిరిగి r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ఒకవేళ (r == -EINVAL) /* not a symlink */
                తిరిగి 1;
        ఒకవేళ (r < 0)
                తిరిగి r;

        తిరిగి !మార్గం_లోపల_సమితి(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

పూర్ణాంకం fsck_exists(శూన్య) {
        తిరిగి executable_is_good("fsck");
}

పూర్ణాంకం fsck_exists_for_fstype(స్థిరరాశి అక్షరం *fstype) {
        స్థిరరాశి అక్షరం *checker;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(fstype);

        ఒకవేళ (streq(fstype, "auto"))
                తిరిగి -EINVAL;

        r = fsck_exists();
        ఒకవేళ (r <= 0)
                తిరిగి r;

        checker = strjoina("fsck.", fstype);
        తిరిగి executable_is_good(checker);
}

స్థిర స్థిరరాశి అక్షరం* skip_slash_or_dot(స్థిరరాశి అక్షరం *p) {
        సూచిక_MAY_BE_NULL(p);

        కొరకు (; !isempty(p); p++) {
                ఒకవేళ (*p == '/')
                        కొనసాగు;
                ఒకవేళ (startswith(p, "./")) {
                        p++;
                        కొనసాగు;
                }
                ఆపు;
        }
        తిరిగి p;
}

పూర్ణాంకం మార్గం_కనుగొను_మొదటి_component(స్థిరరాశి అక్షరం **p, bool accept_dot_dot, స్థిరరాశి అక్షరం **ret) {
        స్థిరరాశి అక్షరం *q, *మొదటి, *చివర_మొదటి, *తదుపరి_2;
        size_t len;

        ప్రతిజ్ఞా(p);

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

        మొదటి = skip_slash_or_dot(q);
        ఒకవేళ (isempty(మొదటి)) {
                *p = మొదటి;
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి 0;
        }
        ఒకవేళ (streq(మొదటి, ".")) {
                *p = మొదటి + 1;
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి 0;
        }

        చివర_మొదటి = strchrnul(మొదటి, '/');
        len = చివర_మొదటి - మొదటి;

        ఒకవేళ (len > NAME_MAX)
                తిరిగి -EINVAL;
        ఒకవేళ (!accept_dot_dot && len == 2 && మొదటి[0] == '.' && మొదటి[1] == '.')
                తిరిగి -EINVAL;

        తదుపరి_2 = skip_slash_or_dot(చివర_మొదటి);

        *p = తదుపరి_2 + streq(తదుపరి_2, ".");
        ఒకవేళ (ret)
                *ret = మొదటి;
        తిరిగి len;
}

స్థిర స్థిరరాశి అక్షరం* skip_slash_or_dot_backward(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *q) {
        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(!q || q >= మార్గం);

        కొరకు (; q; q = PTR_SUB1(q, మార్గం)) {
                ఒకవేళ (*q == '/')
                        కొనసాగు;
                ఒకవేళ (q > మార్గం && strneq(q - 1, "/.", 2))
                        కొనసాగు;
                ఒకవేళ (q == మార్గం && *q == '.')
                        కొనసాగు;
                ఆపు;
        }
        తిరిగి q;
}

పూర్ణాంకం మార్గం_కనుగొను_చివరి_component(స్థిరరాశి అక్షరం *మార్గం, bool accept_dot_dot, స్థిరరాశి అక్షరం **తదుపరి_2, స్థిరరాశి అక్షరం **ret) {
        స్థిరరాశి అక్షరం *q, *చివరి_చివర, *చివరి_begin;
        size_t len;

        సూచిక_MAY_BE_NULL(తదుపరి_2);
        సూచిక_MAY_BE_NULL(ret);

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

        ఒకవేళ (isempty(మార్గం)) {
                ఒకవేళ (తదుపరి_2)
                        *తదుపరి_2 = మార్గం;
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి 0;
        }

        ఒకవేళ (తదుపరి_2 && *తదుపరి_2) {
                ఒకవేళ (*తదుపరి_2 < మార్గం || *తదుపరి_2 > మార్గం + అక్షరశ్రేణి_పొడవు(మార్గం))
                        తిరిగి -EINVAL;
                ఒకవేళ (*తదుపరి_2 == మార్గం) {
                        ఒకవేళ (ret)
                                *ret = ఖాళీ;
                        తిరిగి 0;
                }
                ఒకవేళ (!లోపల_సమితి(**తదుపరి_2, '\0', '/'))
                        తిరిగి -EINVAL;
                q = *తదుపరి_2 - 1;
        } లేకపోతే
                q = మార్గం + అక్షరశ్రేణి_పొడవు(మార్గం) - 1;

        q = skip_slash_or_dot_backward(మార్గం, q);
        ఒకవేళ (!q) { /* the root directory, "." or "./" */
                ఒకవేళ (తదుపరి_2)
                        *తదుపరి_2 = మార్గం;
                ఒకవేళ (ret)
                        *ret = ఖాళీ;
                తిరిగి 0;
        }

        చివరి_చివర = q + 1;

        అయితే (q && *q != '/')
                q = PTR_SUB1(q, మార్గం);

        చివరి_begin = q ? q + 1 : మార్గం;
        len = చివరి_చివర - చివరి_begin;

        ఒకవేళ (len > NAME_MAX)
                తిరిగి -EINVAL;
        ఒకవేళ (!accept_dot_dot && len == 2 && strneq(చివరి_begin, "..", 2))
                తిరిగి -EINVAL;

        ఒకవేళ (తదుపరి_2) {
                q = skip_slash_or_dot_backward(మార్గం, q);
                *తదుపరి_2 = q ? q + 1 : మార్గం;
        }

        ఒకవేళ (ret)
                *ret = చివరి_begin;
        తిరిగి len;
}

స్థిరరాశి అక్షరం* చివరి_మార్గం_component(స్థిరరాశి అక్షరం *మార్గం) {

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

        గుర్తులేని l, k;

        ఒకవేళ (!మార్గం)
                తిరిగి ఖాళీ;

        l = k = అక్షరశ్రేణి_పొడవు(మార్గం);
        ఒకవేళ (l == 0) /* special case — an empty string */
                తిరిగి మార్గం;

        అయితే (k > 0 && మార్గం[k-1] == '/')
                k--;

        ఒకవేళ (k == 0) /* the root directory */
                తిరిగి మార్గం + l - 1;

        అయితే (k > 0 && మార్గం[k-1] != '/')
                k--;

        తిరిగి మార్గం + k;
}

పూర్ణాంకం మార్గం_విభజించు_ఉపసర్గ_filename(స్థిరరాశి అక్షరం *మార్గం, అక్షరం **ret_dir, అక్షరం **ret_filename) {
        _cleanup_free_ అక్షరం *d = ఖాళీ;
        స్థిరరాశి అక్షరం *c, *తదుపరి_2 = ఖాళీ;
        పూర్ణాంకం r;

        సూచిక_MAY_BE_NULL(మార్గం);

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

        ఒకవేళ (isempty(మార్గం))
                తిరిగి -EINVAL;

        r = మార్గం_కనుగొను_చివరి_component(మార్గం, /* accept_dot_dot = */ false, &తదుపరి_2, &c);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r == 0) /* root directory or "." */
                తిరిగి -EADDRNOTAVAIL;

        ఒకవేళ (ret_dir) {
                ఒకవేళ (తదుపరి_2 == మార్గం) {
                        ఒకవేళ (*మార్గం != '/') { /* filename only */
                                ఒకవేళ (!ret_filename)
                                        తిరిగి -EDESTADDRREQ;
                        } లేకపోతే {
                                d = strdup("/");
                                ఒకవేళ (!d)
                                        తిరిగి -ENOMEM;
                        }
                } లేకపోతే {
                        d = strndup(మార్గం, తదుపరి_2 - మార్గం);
                        ఒకవేళ (!d)
                                తిరిగి -ENOMEM;

                        మార్గం_simplify(d);

                        ఒకవేళ (!మార్గం_is_valid(d))
                                తిరిగి -EINVAL;
                }

        } లేకపోతే ఒకవేళ (!మార్గం_is_valid(మార్గం))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                తిరిగి -EINVAL;

        ఒకవేళ (ret_filename) {
                అక్షరం *fn = strndup(c, r);
                ఒకవేళ (!fn)
                        తిరిగి -ENOMEM;

                *ret_filename = fn;
        }

        ఒకవేళ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        తిరిగి అక్షరశ్రేణి_పొడవు(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(స్థిరరాశి అక్షరం *p) {
        స్థిరరాశి అక్షరం *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ఒకవేళ (!p)
                తిరిగి false;

        e = strchrnul(p, '/');
        ఒకవేళ (*e != 0)
                తిరిగి false;

        ఒకవేళ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                తిరిగి false;

        తిరిగి true;
}

bool filename_is_valid(స్థిరరాశి అక్షరం *p) {

        ఒకవేళ (isempty(p))
                తిరిగి false;

        ఒకవేళ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                తిరిగి false;

        తిరిగి filename_part_is_valid(p);
}

bool మార్గం_is_valid_నిండిన(స్థిరరాశి అక్షరం *p, bool accept_dot_dot) {
        ఒకవేళ (isempty(p))
                తిరిగి false;

        కొరకు (స్థిరరాశి అక్షరం *e = p;;) {
                పూర్ణాంకం r;

                r = మార్గం_కనుగొను_మొదటి_component(&e, accept_dot_dot, ఖాళీ);
                ఒకవేళ (r < 0)
                        తిరిగి false;

                ఒకవేళ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        తిరిగి false;
                ఒకవేళ (*e == 0)           /* End of string? Yay! */
                        తిరిగి true;
        }
}

bool మార్గం_is_normalized(స్థిరరాశి అక్షరం *p) {
        ఒకవేళ (!మార్గం_is_సురక్షిత(p))
                తిరిగి false;

        ఒకవేళ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || అక్షరశ్రేణి_శోధన(p, "/./"))
                తిరిగి false;

        ఒకవేళ (అక్షరశ్రేణి_శోధన(p, "//"))
                తిరిగి false;

        తిరిగి true;
}

పూర్ణాంకం file_లోపల_same_dir(స్థిరరాశి అక్షరం *మార్గం, స్థిరరాశి అక్షరం *filename, అక్షరం **ret) {
        _cleanup_free_ అక్షరం *b = ఖాళీ;
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);
        ప్రతిజ్ఞా(filename);
        ప్రతిజ్ఞా(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ఒకవేళ (మార్గం_is_absolute(filename))
                b = strdup(filename);
        లేకపోతే {
                _cleanup_free_ అక్షరం *dn = ఖాళీ;

                r = మార్గం_వెలికితీయి_directory(మార్గం, &dn);
                ఒకవేళ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                లేకపోతే ఒకవేళ (r < 0)
                        తిరిగి r;
                లేకపోతే
                        b = మార్గం_join(dn, filename);
        }
        ఒకవేళ (!b)
                తిరిగి -ENOMEM;

        *ret = TAKE_PTR(b);
        తిరిగి 0;
}

bool hidden_or_backup_file(స్థిరరాశి అక్షరం *filename) {
        ప్రతిజ్ఞా(filename);

        ఒకవేళ (filename[0] == '.' ||
            STR_లోపల_సమితి(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                తిరిగి true;

        స్థిరరాశి అక్షరం *dot = strrchr(filename, '.');
        ఒకవేళ (!dot)
                తిరిగి false;

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

        తిరిగి STR_లోపల_సమితి(dot + 1,
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

bool is_device_మార్గం(స్థిరరాశి అక్షరం *మార్గం) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        స్థిరరాశి అక్షరం *p = మార్గం_STARTSWITH_సమితి(ASSERT_PTR(మార్గం), "/dev/", "/sys/");
        తిరిగి !isempty(p);
}

bool valid_device_కణుపు_మార్గం(స్థిరరాశి అక్షరం *మార్గం) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ఒకవేళ (!మార్గం_STARTSWITH_సమితి(మార్గం, "/dev/", "/run/systemd/inaccessible/"))
                తిరిగి false;

        ఒకవేళ (endswith(మార్గం, "/")) /* can't be a device node if it ends in a slash */
                తిరిగి false;

        తిరిగి మార్గం_is_normalized(మార్గం);
}

bool valid_device_allow_నమూనా(స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(మార్గం);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ఒకవేళ (STARTSWITH_సమితి(మార్గం, "block-", "char-"))
                తిరిగి true;

        తిరిగి valid_device_కణుపు_మార్గం(మార్గం);
}

bool dot_or_dot_dot(స్థిరరాశి అక్షరం *మార్గం) {
        ఒకవేళ (!మార్గం)
                తిరిగి false;
        ఒకవేళ (మార్గం[0] != '.')
                తిరిగి false;
        ఒకవేళ (మార్గం[1] == 0)
                తిరిగి true;
        ఒకవేళ (మార్గం[1] != '.')
                తిరిగి false;

        తిరిగి మార్గం[2] == 0;
}

bool మార్గం_సూచిస్తుంది_directory(స్థిరరాశి అక్షరం *మార్గం) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ఒకవేళ (!మార్గం)
                తిరిగి false;

        ఒకవేళ (dot_or_dot_dot(మార్గం))
                తిరిగి true;

        తిరిగి ENDSWITH_సమితి(మార్గం, "/", "/.", "/..");
}

bool ఖాళీలు_or_మూలం(స్థిరరాశి అక్షరం *మార్గం) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ఒకవేళ (isempty(మార్గం))
                తిరిగి true;

        తిరిగి మార్గం_equal(మార్గం, "/");
}

స్థిరరాశి అక్షరం* ఖాళీలు_గమ్యం_మూలం(స్థిరరాశి అక్షరం *మార్గం) {
        తిరిగి isempty(మార్గం) ? "/" : మార్గం;
}

పూర్ణాంకం ఖాళీలు_or_మూలం_harder_గమ్యం_null(స్థిరరాశి అక్షరం **మార్గం) {
        పూర్ణాంకం r;

        ప్రతిజ్ఞా(మార్గం);

        /* This nullifies the input path when the path is empty or points to "/". */

        ఒకవేళ (ఖాళీలు_or_మూలం(*మార్గం)) {
                *మార్గం = ఖాళీ;
                తిరిగి 0;
        }

        r = మార్గం_is_మూలం(*మార్గం);
        ఒకవేళ (r < 0)
                తిరిగి r;
        ఒకవేళ (r > 0)
                *మార్గం = ఖాళీ;

        తిరిగి 0;
}

bool మార్గం_strv_కలిగియుంది(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(మార్గం);

        STRV_FOREACH(i, l)
                ఒకవేళ (మార్గం_equal(*i, మార్గం))
                        తిరిగి true;

        తిరిగి false;
}

bool prefixed_మార్గం_strv_కలిగియుంది(అక్షరం * స్థిరరాశి *l, స్థిరరాశి అక్షరం *మార్గం) {
        ప్రతిజ్ఞా(మార్గం);

        STRV_FOREACH(i, l) {
                స్థిరరాశి అక్షరం *j = *i;

                ఒకవేళ (*j == '-')
                        j++;
                ఒకవేళ (*j == '+')
                        j++;

                ఒకవేళ (మార్గం_equal(j, మార్గం))
                        తిరిగి true;
        }

        తిరిగి false;
}

పూర్ణాంకం మార్గం_glob_can_సరిపోలిక(స్థిరరాశి అక్షరం *నమూనా, స్థిరరాశి అక్షరం *ఉపసర్గ, అక్షరం **ret) {
        ప్రతిజ్ఞా(నమూనా);
        ప్రతిజ్ఞా(ఉపసర్గ);

        కొరకు (స్థిరరాశి అక్షరం *a = నమూనా, *b = ఉపసర్గ;;) {
                _cleanup_free_ అక్షరం *g = ఖాళీ, *h = ఖాళీ;
                స్థిరరాశి అక్షరం *p, *q;
                పూర్ణాంకం r, s;

                r = మార్గం_కనుగొను_మొదటి_component(&a, /* accept_dot_dot= */ false, &p);
                ఒకవేళ (r < 0)
                        తిరిగి r;

                s = మార్గం_కనుగొను_మొదటి_component(&b, /* accept_dot_dot= */ false, &q);
                ఒకవేళ (s < 0)
                        తిరిగి s;

                ఒకవేళ (s == 0) {
                        /* The pattern matches the prefix. */
                        ఒకవేళ (ret) {
                                అక్షరం *t;

                                t = మార్గం_join(ఉపసర్గ, p);
                                ఒకవేళ (!t)
                                        తిరిగి -ENOMEM;

                                *ret = t;
                        }
                        తిరిగి true;
                }

                ఒకవేళ (r == 0)
                        ఆపు;

                ఒకవేళ (r == s && strneq(p, q, r))
                        కొనసాగు; /* common component. Check next. */

                g = strndup(p, r);
                ఒకవేళ (!g)
                        తిరిగి -ENOMEM;

                ఒకవేళ (!string_is_glob(g))
                        ఆపు;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ఒకవేళ (!h)
                        తిరిగి -ENOMEM;

                r = fnmatch(g, h, 0);
                ఒకవేళ (r == FNM_NOMATCH)
                        ఆపు;
                ఒకవేళ (r != 0) /* Failure to process pattern? */
                        తిరిగి -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ఒకవేళ (ret)
                *ret = ఖాళీ;
        తిరిగి false;
}

#ఒకవేళ HAVE_విభజించు_BIN
స్థిర bool dir_is_విభజించు(స్థిరరాశి అక్షరం *a, స్థిరరాశి అక్షరం *b) {
        పూర్ణాంకం r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ఒకవేళ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                తిరిగి true;
        }
        తిరిగి r == 0;
}
#అయితే_ముగింపు

స్థిరరాశి అక్షరం* default_మార్గం(శూన్య) {
#ఒకవేళ HAVE_విభజించు_BIN
        స్థిర స్థిరరాశి అక్షరం *default_మార్గం_2 = ఖాళీ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ఒకవేళ (default_మార్గం_2)
                తిరిగి default_మార్గం_2;

        ఒకవేళ (dir_is_విభజించు("/usr/sbin", "/usr/bin"))
                తిరిగి (default_మార్గం_2 = DEFAULT_మార్గం_WITH_నిండిన_SBIN);  /* a */
        ఒకవేళ (dir_is_విభజించు("/usr/local/sbin", "/usr/local/bin"))
                తిరిగి (default_మార్గం_2 = DEFAULT_మార్గం_WITH_LOCAL_SBIN); /* b */
        తిరిగి (default_మార్గం_2 = DEFAULT_మార్గం_WITHOUT_SBIN);            /* c */
#లేకపోతే
        తిరిగి DEFAULT_మార్గం_WITHOUT_SBIN;
#అయితే_ముగింపు
}
