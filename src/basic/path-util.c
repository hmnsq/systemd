/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <fnmatch.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <unistd.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "extract-word.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "glob-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"

bool is_मार्गः(ध्रुव वर्ण *p) {
        यदि (!p) /* A NULL pointer is definitely not a path */
                प्रत्यागम false;

        प्रत्यागम strchr(p, '/');
}

पूर्णाङ्क मार्गः_विभज_and_make_absolute(ध्रुव वर्ण *p, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **l = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        l = strv_विभज(p, ":");
        यदि (!l)
                प्रत्यागम -ENOMEM;

        r = मार्गः_strv_make_absolute_cwd(l);
        यदि (r < 0)
                प्रत्यागम r;

        *ret = TAKE_PTR(l);
        प्रत्यागम r;
}

वर्ण* मार्गः_make_absolute(ध्रुव वर्ण *p, ध्रुव वर्ण *उपसर्गः) {
        प्रतिज्ञा(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        यदि (मार्गः_is_absolute(p) || isempty(उपसर्गः))
                प्रत्यागम strdup(p);

        प्रत्यागम मार्गः_join(उपसर्गः, p);
}

पूर्णाङ्क safe_getcwd(वर्ण **ret) {
        _cleanup_free_ वर्ण *cwd = रिक्त;

        cwd = get_current_dir_name();
        यदि (!cwd)
                प्रत्यागम negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        यदि (cwd[0] != '/')
                प्रत्यागम -ENOMEDIUM;

        यदि (ret)
                *ret = TAKE_PTR(cwd);

        प्रत्यागम 0;
}

पूर्णाङ्क मार्गः_make_absolute_cwd(ध्रुव वर्ण *p, वर्ण **ret) {
        वर्ण *c;
        पूर्णाङ्क r;

        प्रतिज्ञा(p);
        प्रतिज्ञा(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        यदि (मार्गः_is_absolute(p))
                c = strdup(p);
        अन्यथा {
                _cleanup_free_ वर्ण *cwd = रिक्त;

                r = safe_getcwd(&cwd);
                यदि (r < 0)
                        प्रत्यागम r;

                c = मार्गः_join(cwd, p);
        }
        यदि (!c)
                प्रत्यागम -ENOMEM;

        *ret = c;
        प्रत्यागम 0;
}

पूर्णाङ्क मार्गः_make_relative(ध्रुव वर्ण *from, ध्रुव वर्ण *to, वर्ण **ret) {
        _cleanup_free_ वर्ण *परिणाम = रिक्त;
        अचिह्नित n_parents;
        ध्रुव वर्ण *f, *t;
        पूर्णाङ्क r, k;
        वर्ण *p;

        प्रतिज्ञा(from);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        यदि (!मार्गः_is_absolute(from) || !मार्गः_is_absolute(to))
                प्रत्यागम -EINVAL;

        प्रति (;;) {
                r = मार्गः_find_first_component(&from, true, &f);
                यदि (r < 0)
                        प्रत्यागम r;

                k = मार्गः_find_first_component(&to, true, &t);
                यदि (k < 0)
                        प्रत्यागम k;

                यदि (r == 0) {
                        /* end of 'from' */
                        यदि (k == 0) {
                                /* from and to are equivalent. */
                                परिणाम = strdup(".");
                                यदि (!परिणाम)
                                        प्रत्यागम -ENOMEM;
                        } अन्यथा {
                                /* 'to' is inside of 'from'. */
                                r = मार्गः_simplify_alloc(t, &परिणाम);
                                यदि (r < 0)
                                        प्रत्यागम r;

                                यदि (!मार्गः_is_valid(परिणाम))
                                        प्रत्यागम -EINVAL;
                        }

                        *ret = TAKE_PTR(परिणाम);
                        प्रत्यागम 0;
                }

                यदि (r != k || !strneq(f, t, r))
                        भङ्ग;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        प्रति (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = मार्गः_find_first_component(&from, false, &f);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;
        }

        यदि (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                प्रत्यागम -EINVAL;

        परिणाम = new(वर्ण, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        यदि (!परिणाम)
                प्रत्यागम -ENOMEM;

        प्रति (p = परिणाम; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        यदि (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(परिणाम);
                प्रत्यागम 0;
        }

        वाक्यं_प्रतिलिख(p, t);

        मार्गः_simplify(परिणाम);

        यदि (!मार्गः_is_valid(परिणाम))
                प्रत्यागम -EINVAL;

        *ret = TAKE_PTR(परिणाम);
        प्रत्यागम 0;
}

पूर्णाङ्क मार्गः_make_relative_parent(ध्रुव वर्ण *from_child, ध्रुव वर्ण *to, वर्ण **ret) {
        _cleanup_free_ वर्ण *from = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(from_child);
        प्रतिज्ञा(to);
        प्रतिज्ञा(ret);

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

        r = मार्गः_extract_directory(from_child, &from);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम मार्गः_make_relative(from, to, ret);
}

वर्ण* मार्गः_startswith_strv(ध्रुव वर्ण *p, वर्ण * ध्रुव *strv) {
        प्रतिज्ञा(p);

        STRV_FOREACH(s, strv) {
                वर्ण *t;

                t = मार्गः_startswith(p, *s);
                यदि (t)
                        प्रत्यागम t;
        }

        प्रत्यागम रिक्त;
}

पूर्णाङ्क मार्गः_strv_make_absolute_cwd(वर्ण **l) {
        पूर्णाङ्क r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                वर्ण *t;

                r = मार्गः_make_absolute_cwd(*s, &t);
                यदि (r < 0)
                        प्रत्यागम r;

                मार्गः_simplify(t);
                free_and_replace(*s, t);
        }

        प्रत्यागम 0;
}

वर्ण** मार्गः_strv_resolve(वर्ण **l, ध्रुव वर्ण *मूलम्) {
        अचिह्नित k = 0;
        bool enomem = false;
        पूर्णाङ्क r;

        यदि (strv_isempty(l))
                प्रत्यागम l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ वर्ण *orig = रिक्त;
                वर्ण *t, *u;

                यदि (मूलम्) {
                        यदि (!मार्गः_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                मुक्त(*s);
                                अनुवर्त;
                        }

                        orig = *s;
                        t = मार्गः_join(मूलम्, orig);
                        यदि (!t) {
                                enomem = true;
                                अनुवर्त;
                        }
                } अन्यथा
                        t = *s;

                r = chase(t, मूलम्, 0, &u, रिक्त);
                यदि (r == -ENOENT) {
                        यदि (मूलम्) {
                                u = TAKE_PTR(orig);
                                मुक्त(t);
                        } अन्यथा
                                u = t;
                } अन्यथा यदि (r < 0) {
                        मुक्त(t);

                        यदि (r == -ENOMEM)
                                enomem = true;

                        अनुवर्त;
                } अन्यथा यदि (मूलम्) {
                        वर्ण *x;

                        मुक्त(t);
                        x = मार्गः_startswith(u, मूलम्);
                        यदि (x) {
                                /* restore the slash if it was lost */
                                यदि (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                मुक्त(u);
                                यदि (!t) {
                                        enomem = true;
                                        अनुवर्त;
                                }
                                u = t;
                        } अन्यथा {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } अन्यथा
                        मुक्त(t);

                l[k++] = u;
        }

        l[k] = रिक्त;

        यदि (enomem)
                प्रत्यागम रिक्त;

        प्रत्यागम l;
}

वर्ण** मार्गः_strv_resolve_uniq(वर्ण **l, ध्रुव वर्ण *मूलम्) {

        यदि (strv_isempty(l))
                प्रत्यागम l;

        यदि (!मार्गः_strv_resolve(l, मूलम्))
                प्रत्यागम रिक्त;

        प्रत्यागम strv_uniq(l);
}

वर्ण* skip_leading_slash(ध्रुव वर्ण *p) {
        प्रत्यागम skip_leading_chars(p, "/");
}

वर्ण* मार्गः_simplify_पूर्णम्(वर्ण *मार्गः, मार्गः_Simplify_Flags flags) {
        bool सङ्कलय_slash = false, keep_trailing_slash, absolute, beginning = true;
        वर्ण *f = मार्गः;
        पूर्णाङ्क r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        यदि (isempty(मार्गः))
                प्रत्यागम मार्गः;

        keep_trailing_slash = FLAGS_समुच्चयः(flags, मार्गः_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(मार्गः, "/");

        absolute = मार्गः_is_absolute(मार्गः);
        f += absolute;  /* Keep leading /, if present. */

        प्रति (ध्रुव वर्ण *p = f;;) {
                ध्रुव वर्ण *e;

                r = मार्गः_find_first_component(&p, true, &e);
                यदि (r == 0)
                        भङ्ग;

                यदि (r > 0 && absolute && beginning && मार्गः_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        अनुवर्त;

                beginning = false;

                यदि (सङ्कलय_slash)
                        *f++ = '/';

                यदि (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, वाक्यदैर्घ्यम्(p) + 1);
                        प्रत्यागम मार्गः;
                }

                memmove(f, e, r);
                f += r;

                सङ्कलय_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        यदि (f == मार्गः)
                *f++ = '.';

        यदि (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        प्रत्यागम मार्गः;
}

पूर्णाङ्क मार्गः_simplify_alloc(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        प्रतिज्ञा(ret);

        यदि (!मार्गः) {
                *ret = रिक्त;
                प्रत्यागम 0;
        }

        वर्ण *t = strdup(मार्गः);
        यदि (!t)
                प्रत्यागम -ENOMEM;

        *ret = मार्गः_simplify(t);
        प्रत्यागम 0;
}

वर्ण* मार्गः_startswith_पूर्णम्(ध्रुव वर्ण *original_मार्गः, ध्रुव वर्ण *उपसर्गः, मार्गः_Start_With_Flags flags) {
        प्रतिज्ञा(original_मार्गः);
        प्रतिज्ञा(उपसर्गः);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        ध्रुव वर्ण *मार्गः = original_मार्गः;

        यदि ((मार्गः[0] == '/') != (उपसर्गः[0] == '/'))
                प्रत्यागम रिक्त;

        प्रति (;;) {
                ध्रुव वर्ण *p, *q;
                पूर्णाङ्क m, n;

                m = मार्गः_find_first_component(&मार्गः, !FLAGS_समुच्चयः(flags, मार्गः_STARTSWITH_REFUSE_DOT_DOT), &p);
                यदि (m < 0)
                        प्रत्यागम रिक्त;

                n = मार्गः_find_first_component(&उपसर्गः, !FLAGS_समुच्चयः(flags, मार्गः_STARTSWITH_REFUSE_DOT_DOT), &q);
                यदि (n < 0)
                        प्रत्यागम रिक्त;

                यदि (n == 0) {
                        यदि (!p)
                                p = मार्गः;

                        यदि (FLAGS_समुच्चयः(flags, मार्गः_STARTSWITH_RETURN_LEADING_SLASH)) {

                                यदि (p <= original_मार्गः)
                                        प्रत्यागम रिक्त;

                                p--;

                                यदि (*p != '/')
                                        प्रत्यागम रिक्त;
                        }

                        प्रत्यागम (वर्ण*) p;
                }

                यदि (m != n)
                        प्रत्यागम रिक्त;

                यदि (!strneq(p, q, m))
                        प्रत्यागम रिक्त;
        }
}

पूर्णाङ्क मार्गः_compare(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        पूर्णाङ्क r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        यदि (r != 0)
                प्रत्यागम r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(मार्गः_is_absolute(a), मार्गः_is_absolute(b));
        यदि (r != 0)
                प्रत्यागम r;

        प्रति (;;) {
                ध्रुव वर्ण *aa, *bb;
                पूर्णाङ्क j, k;

                j = मार्गः_find_first_component(&a, true, &aa);
                k = मार्गः_find_first_component(&b, true, &bb);

                यदि (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        यदि (r != 0)
                                प्रत्यागम r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        प्रत्यागम वाक्यं_तुलय(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                यदि (j == 0) {
                        यदि (k == 0)
                                प्रत्यागम 0;
                        प्रत्यागम -1;
                }
                यदि (k == 0)
                        प्रत्यागम 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                यदि (r != 0)
                        प्रत्यागम r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                यदि (r != 0)
                        प्रत्यागम r;
        }
}

पूर्णाङ्क मार्गः_compare_filename(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        _cleanup_free_ वर्ण *fa = रिक्त, *fb = रिक्त;
        पूर्णाङ्क r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        यदि (r != 0)
                प्रत्यागम r;

        j = मार्गः_extract_filename(a, &fa);
        k = मार्गः_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        यदि (r != 0)
                प्रत्यागम r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        यदि (r != 0)
                प्रत्यागम r;

        /* fallback to use strcmp() if both paths are invalid. */
        यदि (j < 0)
                प्रत्यागम वाक्यं_तुलय(a, b);

        प्रत्यागम वाक्यं_तुलय(fa, fb);
}

पूर्णाङ्क मार्गः_equal_or_inode_same_पूर्णम्(ध्रुव वर्ण *a, ध्रुव वर्ण *b, पूर्णाङ्क flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        यदि (मार्गः_equal(a, b))
                प्रत्यागम 1;

        यदि (!a || !b)
                प्रत्यागम 0;

        प्रत्यागम inode_same(a, b, flags);
}

वर्ण* मार्गः_extend_आन्तरिकम्(वर्ण **x, ...) {
        size_t sz, old_sz;
        वर्ण *q, *nx;
        ध्रुव वर्ण *p;
        va_list ap;
        bool slash;

        POINTER_MAY_BE_NULL(x);

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
        यावत् ((p = va_arg(ap, वर्ण*)) != POINTER_MAX) {
                size_t सङ्कलय;

                यदि (isempty(p))
                        अनुवर्त;

                सङ्कलय = 1 + वाक्यदैर्घ्यम्(p);
                यदि (sz > SIZE_MAX - सङ्कलय) { /* overflow check */
                        va_end(ap);
                        प्रत्यागम रिक्त;
                }

                sz += सङ्कलय;
        }
        va_end(ap);

        nx = realloc(x ? *x : रिक्त, GREEDY_ALLOC_आवर्तनम्_ऊर्ध्वम्(sz+1));
        यदि (!nx)
                प्रत्यागम रिक्त;
        यदि (x)
                *x = nx;

        यदि (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        अन्यथा {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        यावत् ((p = va_arg(ap, वर्ण*)) != POINTER_MAX) {
                यदि (isempty(p))
                        अनुवर्त;

                यदि (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        प्रत्यागम nx;
}

पूर्णाङ्क open_and_check_executable(ध्रुव वर्ण *name, ध्रुव वर्ण *मूलम्, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *resolved = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        यदि (मूलम्) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, मूलम्, CHASE_उपसर्गः_मूलम्, &resolved, &fd);
                यदि (r < 0)
                        प्रत्यागम r;

                name = resolved;
        } अन्यथा {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = उद्घाटय(name, O_मार्गः|O_CLOEXEC);
                यदि (fd < 0)
                        प्रत्यागम -errno;
        }

        r = fd_verify_regular(fd);
        यदि (r < 0)
                प्रत्यागम r;

        r = access_fd(fd, X_OK);
        यदि (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः) {
                यदि (resolved)
                        *ret_मार्गः = TAKE_PTR(resolved);
                अन्यथा {
                        r = मार्गः_make_absolute_cwd(name, ret_मार्गः);
                        यदि (r < 0)
                                प्रत्यागम r;

                        मार्गः_simplify(*ret_मार्गः);
                }
        }

        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd);

        प्रत्यागम 0;
}

पूर्णाङ्क find_executable_पूर्णम्(
                ध्रुव वर्ण *name,
                ध्रुव वर्ण *मूलम्,
                वर्ण * ध्रुव *exec_अन्वेषणम्_मार्गः,
                bool use_मार्गः_envvar,
                वर्ण **ret_filename,
                पूर्णाङ्क *ret_fd) {

        पूर्णाङ्क चरमः_error = -ENOENT, r = 0;

        प्रतिज्ञा(name);

        यदि (is_मार्गः(name))
                प्रत्यागम open_and_check_executable(name, मूलम्, ret_filename, ret_fd);

        यदि (exec_अन्वेषणम्_मार्गः) {
                STRV_FOREACH(element, exec_अन्वेषणम्_मार्गः) {
                        _cleanup_free_ वर्ण *पूर्णम्_मार्गः = रिक्त;

                        यदि (!मार्गः_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                अनुवर्त;
                        }

                        पूर्णम्_मार्गः = मार्गः_join(*element, name);
                        यदि (!पूर्णम्_मार्गः)
                                प्रत्यागम -ENOMEM;

                        r = open_and_check_executable(पूर्णम्_मार्गः, मूलम्, ret_filename, ret_fd);
                        यदि (r >= 0)
                                प्रत्यागम 0;
                        यदि (r != -EACCES)
                                चरमः_error = r;
                }
                प्रत्यागम चरमः_error;
        }

        ध्रुव वर्ण *p = रिक्त;

        यदि (use_मार्गः_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        यदि (!p)
                p = default_मार्गः();

        /* Resolve a single-component name to a full path */
        प्रति (;;) {
                _cleanup_free_ वर्ण *element = रिक्त;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                यदि (r < 0)
                        प्रत्यागम r;
                यदि (r == 0)
                        भङ्ग;

                यदि (!मार्गः_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        अनुवर्त;
                }

                यदि (!मार्गः_extend(&element, name))
                        प्रत्यागम -ENOMEM;

                r = open_and_check_executable(element, मूलम्, ret_filename, ret_fd);
                यदि (r >= 0) /* Found it! */
                        प्रत्यागम 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                यदि (r != -EACCES)
                        चरमः_error = r;
        }

        प्रत्यागम चरमः_error;
}

स्थिर पूर्णाङ्क executable_is_good(ध्रुव वर्ण *executable) {
        _cleanup_free_ वर्ण *p = रिक्त, *d = रिक्त;
        पूर्णाङ्क r;

        r = find_executable(executable, &p);
        यदि (r == -ENOENT)
                प्रत्यागम 0;
        यदि (r < 0)
                प्रत्यागम r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        यदि (r == -EINVAL) /* not a symlink */
                प्रत्यागम 1;
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम !मार्गः_अन्तर्_समुच्चयः(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

पूर्णाङ्क fsck_exists(शून्य) {
        प्रत्यागम executable_is_good("fsck");
}

पूर्णाङ्क fsck_exists_for_fstype(ध्रुव वर्ण *fstype) {
        ध्रुव वर्ण *checker;
        पूर्णाङ्क r;

        प्रतिज्ञा(fstype);

        यदि (streq(fstype, "auto"))
                प्रत्यागम -EINVAL;

        r = fsck_exists();
        यदि (r <= 0)
                प्रत्यागम r;

        checker = strjoina("fsck.", fstype);
        प्रत्यागम executable_is_good(checker);
}

स्थिर ध्रुव वर्ण* skip_slash_or_dot(ध्रुव वर्ण *p) {
        POINTER_MAY_BE_NULL(p);

        प्रति (; !isempty(p); p++) {
                यदि (*p == '/')
                        अनुवर्त;
                यदि (startswith(p, "./")) {
                        p++;
                        अनुवर्त;
                }
                भङ्ग;
        }
        प्रत्यागम p;
}

पूर्णाङ्क मार्गः_find_first_component(ध्रुव वर्ण **p, bool accept_dot_dot, ध्रुव वर्ण **ret) {
        ध्रुव वर्ण *q, *first, *अन्तः_first, *अग्र_2;
        size_t len;

        प्रतिज्ञा(p);

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

        first = skip_slash_or_dot(q);
        यदि (isempty(first)) {
                *p = first;
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }
        यदि (streq(first, ".")) {
                *p = first + 1;
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        अन्तः_first = strchrnul(first, '/');
        len = अन्तः_first - first;

        यदि (len > NAME_MAX)
                प्रत्यागम -EINVAL;
        यदि (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                प्रत्यागम -EINVAL;

        अग्र_2 = skip_slash_or_dot(अन्तः_first);

        *p = अग्र_2 + streq(अग्र_2, ".");
        यदि (ret)
                *ret = first;
        प्रत्यागम len;
}

स्थिर ध्रुव वर्ण* skip_slash_or_dot_backward(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *q) {
        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(!q || q >= मार्गः);

        प्रति (; q; q = PTR_SUB1(q, मार्गः)) {
                यदि (*q == '/')
                        अनुवर्त;
                यदि (q > मार्गः && strneq(q - 1, "/.", 2))
                        अनुवर्त;
                यदि (q == मार्गः && *q == '.')
                        अनुवर्त;
                भङ्ग;
        }
        प्रत्यागम q;
}

पूर्णाङ्क मार्गः_find_चरमः_component(ध्रुव वर्ण *मार्गः, bool accept_dot_dot, ध्रुव वर्ण **अग्र_2, ध्रुव वर्ण **ret) {
        ध्रुव वर्ण *q, *चरमः_अन्तः, *चरमः_begin;
        size_t len;

        POINTER_MAY_BE_NULL(अग्र_2);
        POINTER_MAY_BE_NULL(ret);

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

        यदि (isempty(मार्गः)) {
                यदि (अग्र_2)
                        *अग्र_2 = मार्गः;
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        यदि (अग्र_2 && *अग्र_2) {
                यदि (*अग्र_2 < मार्गः || *अग्र_2 > मार्गः + वाक्यदैर्घ्यम्(मार्गः))
                        प्रत्यागम -EINVAL;
                यदि (*अग्र_2 == मार्गः) {
                        यदि (ret)
                                *ret = रिक्त;
                        प्रत्यागम 0;
                }
                यदि (!अन्तर्_समुच्चयः(**अग्र_2, '\0', '/'))
                        प्रत्यागम -EINVAL;
                q = *अग्र_2 - 1;
        } अन्यथा
                q = मार्गः + वाक्यदैर्घ्यम्(मार्गः) - 1;

        q = skip_slash_or_dot_backward(मार्गः, q);
        यदि (!q) { /* the root directory, "." or "./" */
                यदि (अग्र_2)
                        *अग्र_2 = मार्गः;
                यदि (ret)
                        *ret = रिक्त;
                प्रत्यागम 0;
        }

        चरमः_अन्तः = q + 1;

        यावत् (q && *q != '/')
                q = PTR_SUB1(q, मार्गः);

        चरमः_begin = q ? q + 1 : मार्गः;
        len = चरमः_अन्तः - चरमः_begin;

        यदि (len > NAME_MAX)
                प्रत्यागम -EINVAL;
        यदि (!accept_dot_dot && len == 2 && strneq(चरमः_begin, "..", 2))
                प्रत्यागम -EINVAL;

        यदि (अग्र_2) {
                q = skip_slash_or_dot_backward(मार्गः, q);
                *अग्र_2 = q ? q + 1 : मार्गः;
        }

        यदि (ret)
                *ret = चरमः_begin;
        प्रत्यागम len;
}

ध्रुव वर्ण* चरमः_मार्गः_component(ध्रुव वर्ण *मार्गः) {

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

        अचिह्नित l, k;

        यदि (!मार्गः)
                प्रत्यागम रिक्त;

        l = k = वाक्यदैर्घ्यम्(मार्गः);
        यदि (l == 0) /* special case — an empty string */
                प्रत्यागम मार्गः;

        यावत् (k > 0 && मार्गः[k-1] == '/')
                k--;

        यदि (k == 0) /* the root directory */
                प्रत्यागम मार्गः + l - 1;

        यावत् (k > 0 && मार्गः[k-1] != '/')
                k--;

        प्रत्यागम मार्गः + k;
}

पूर्णाङ्क मार्गः_विभज_उपसर्गः_filename(ध्रुव वर्ण *मार्गः, वर्ण **ret_dir, वर्ण **ret_filename) {
        _cleanup_free_ वर्ण *d = रिक्त;
        ध्रुव वर्ण *c, *अग्र_2 = रिक्त;
        पूर्णाङ्क r;

        POINTER_MAY_BE_NULL(मार्गः);

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

        यदि (isempty(मार्गः))
                प्रत्यागम -EINVAL;

        r = मार्गः_find_चरमः_component(मार्गः, /* accept_dot_dot = */ false, &अग्र_2, &c);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r == 0) /* root directory or "." */
                प्रत्यागम -EADDRNOTAVAIL;

        यदि (ret_dir) {
                यदि (अग्र_2 == मार्गः) {
                        यदि (*मार्गः != '/') { /* filename only */
                                यदि (!ret_filename)
                                        प्रत्यागम -EDESTADDRREQ;
                        } अन्यथा {
                                d = strdup("/");
                                यदि (!d)
                                        प्रत्यागम -ENOMEM;
                        }
                } अन्यथा {
                        d = strndup(मार्गः, अग्र_2 - मार्गः);
                        यदि (!d)
                                प्रत्यागम -ENOMEM;

                        मार्गः_simplify(d);

                        यदि (!मार्गः_is_valid(d))
                                प्रत्यागम -EINVAL;
                }

        } अन्यथा यदि (!मार्गः_is_valid(मार्गः))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                प्रत्यागम -EINVAL;

        यदि (ret_filename) {
                वर्ण *fn = strndup(c, r);
                यदि (!fn)
                        प्रत्यागम -ENOMEM;

                *ret_filename = fn;
        }

        यदि (ret_dir)
                *ret_dir = TAKE_PTR(d);

        प्रत्यागम वाक्यदैर्घ्यम्(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(ध्रुव वर्ण *p) {
        ध्रुव वर्ण *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        यदि (!p)
                प्रत्यागम false;

        e = strchrnul(p, '/');
        यदि (*e != 0)
                प्रत्यागम false;

        यदि (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                प्रत्यागम false;

        प्रत्यागम true;
}

bool filename_is_valid(ध्रुव वर्ण *p) {

        यदि (isempty(p))
                प्रत्यागम false;

        यदि (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                प्रत्यागम false;

        प्रत्यागम filename_part_is_valid(p);
}

bool मार्गः_is_valid_पूर्णम्(ध्रुव वर्ण *p, bool accept_dot_dot) {
        यदि (isempty(p))
                प्रत्यागम false;

        प्रति (ध्रुव वर्ण *e = p;;) {
                पूर्णाङ्क r;

                r = मार्गः_find_first_component(&e, accept_dot_dot, रिक्त);
                यदि (r < 0)
                        प्रत्यागम false;

                यदि (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        प्रत्यागम false;
                यदि (*e == 0)           /* End of string? Yay! */
                        प्रत्यागम true;
        }
}

bool मार्गः_is_normalized(ध्रुव वर्ण *p) {
        यदि (!मार्गः_is_safe(p))
                प्रत्यागम false;

        यदि (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || वाक्ये_अन्विष(p, "/./"))
                प्रत्यागम false;

        यदि (वाक्ये_अन्विष(p, "//"))
                प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क file_अन्तर्_same_dir(ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *filename, वर्ण **ret) {
        _cleanup_free_ वर्ण *b = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(filename);
        प्रतिज्ञा(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        यदि (मार्गः_is_absolute(filename))
                b = strdup(filename);
        अन्यथा {
                _cleanup_free_ वर्ण *dn = रिक्त;

                r = मार्गः_extract_directory(मार्गः, &dn);
                यदि (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                अन्यथा यदि (r < 0)
                        प्रत्यागम r;
                अन्यथा
                        b = मार्गः_join(dn, filename);
        }
        यदि (!b)
                प्रत्यागम -ENOMEM;

        *ret = TAKE_PTR(b);
        प्रत्यागम 0;
}

bool hidden_or_backup_file(ध्रुव वर्ण *filename) {
        प्रतिज्ञा(filename);

        यदि (filename[0] == '.' ||
            STR_अन्तर्_समुच्चयः(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                प्रत्यागम true;

        ध्रुव वर्ण *dot = strrchr(filename, '.');
        यदि (!dot)
                प्रत्यागम false;

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

        प्रत्यागम STR_अन्तर्_समुच्चयः(dot + 1,
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

bool is_device_मार्गः(ध्रुव वर्ण *मार्गः) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        ध्रुव वर्ण *p = मार्गः_STARTSWITH_समुच्चयः(ASSERT_PTR(मार्गः), "/dev/", "/sys/");
        प्रत्यागम !isempty(p);
}

bool valid_device_ग्रन्थिः_मार्गः(ध्रुव वर्ण *मार्गः) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        यदि (!मार्गः_STARTSWITH_समुच्चयः(मार्गः, "/dev/", "/run/systemd/inaccessible/"))
                प्रत्यागम false;

        यदि (endswith(मार्गः, "/")) /* can't be a device node if it ends in a slash */
                प्रत्यागम false;

        प्रत्यागम मार्गः_is_normalized(मार्गः);
}

bool valid_device_allow_आदर्शः(ध्रुव वर्ण *मार्गः) {
        प्रतिज्ञा(मार्गः);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        यदि (STARTSWITH_समुच्चयः(मार्गः, "block-", "char-"))
                प्रत्यागम true;

        प्रत्यागम valid_device_ग्रन्थिः_मार्गः(मार्गः);
}

bool dot_or_dot_dot(ध्रुव वर्ण *मार्गः) {
        यदि (!मार्गः)
                प्रत्यागम false;
        यदि (मार्गः[0] != '.')
                प्रत्यागम false;
        यदि (मार्गः[1] == 0)
                प्रत्यागम true;
        यदि (मार्गः[1] != '.')
                प्रत्यागम false;

        प्रत्यागम मार्गः[2] == 0;
}

bool मार्गः_उपपादयति_directory(ध्रुव वर्ण *मार्गः) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        यदि (!मार्गः)
                प्रत्यागम false;

        यदि (dot_or_dot_dot(मार्गः))
                प्रत्यागम true;

        प्रत्यागम ENDSWITH_समुच्चयः(मार्गः, "/", "/.", "/..");
}

bool रिक्तानि_or_मूलम्(ध्रुव वर्ण *मार्गः) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        यदि (isempty(मार्गः))
                प्रत्यागम true;

        प्रत्यागम मार्गः_equal(मार्गः, "/");
}

ध्रुव वर्ण* रिक्तानि_गन्तव्यम्_मूलम्(ध्रुव वर्ण *मार्गः) {
        प्रत्यागम isempty(मार्गः) ? "/" : मार्गः;
}

पूर्णाङ्क रिक्तानि_or_मूलम्_harder_गन्तव्यम्_null(ध्रुव वर्ण **मार्गः) {
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        /* This nullifies the input path when the path is empty or points to "/". */

        यदि (रिक्तानि_or_मूलम्(*मार्गः)) {
                *मार्गः = रिक्त;
                प्रत्यागम 0;
        }

        r = मार्गः_is_मूलम्(*मार्गः);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r > 0)
                *मार्गः = रिक्त;

        प्रत्यागम 0;
}

bool मार्गः_strv_अन्तर्भवति(वर्ण * ध्रुव *l, ध्रुव वर्ण *मार्गः) {
        प्रतिज्ञा(मार्गः);

        STRV_FOREACH(i, l)
                यदि (मार्गः_equal(*i, मार्गः))
                        प्रत्यागम true;

        प्रत्यागम false;
}

bool prefixed_मार्गः_strv_अन्तर्भवति(वर्ण * ध्रुव *l, ध्रुव वर्ण *मार्गः) {
        प्रतिज्ञा(मार्गः);

        STRV_FOREACH(i, l) {
                ध्रुव वर्ण *j = *i;

                यदि (*j == '-')
                        j++;
                यदि (*j == '+')
                        j++;

                यदि (मार्गः_equal(j, मार्गः))
                        प्रत्यागम true;
        }

        प्रत्यागम false;
}

पूर्णाङ्क मार्गः_glob_can_मेलनम्(ध्रुव वर्ण *आदर्शः, ध्रुव वर्ण *उपसर्गः, वर्ण **ret) {
        प्रतिज्ञा(आदर्शः);
        प्रतिज्ञा(उपसर्गः);

        प्रति (ध्रुव वर्ण *a = आदर्शः, *b = उपसर्गः;;) {
                _cleanup_free_ वर्ण *g = रिक्त, *h = रिक्त;
                ध्रुव वर्ण *p, *q;
                पूर्णाङ्क r, s;

                r = मार्गः_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                यदि (r < 0)
                        प्रत्यागम r;

                s = मार्गः_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                यदि (s < 0)
                        प्रत्यागम s;

                यदि (s == 0) {
                        /* The pattern matches the prefix. */
                        यदि (ret) {
                                वर्ण *t;

                                t = मार्गः_join(उपसर्गः, p);
                                यदि (!t)
                                        प्रत्यागम -ENOMEM;

                                *ret = t;
                        }
                        प्रत्यागम true;
                }

                यदि (r == 0)
                        भङ्ग;

                यदि (r == s && strneq(p, q, r))
                        अनुवर्त; /* common component. Check next. */

                g = strndup(p, r);
                यदि (!g)
                        प्रत्यागम -ENOMEM;

                यदि (!string_is_glob(g))
                        भङ्ग;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                यदि (!h)
                        प्रत्यागम -ENOMEM;

                r = fnmatch(g, h, 0);
                यदि (r == FNM_NOMATCH)
                        भङ्ग;
                यदि (r != 0) /* Failure to process pattern? */
                        प्रत्यागम -EINVAL;
        }

        /* The pattern does not match the prefix. */
        यदि (ret)
                *ret = रिक्त;
        प्रत्यागम false;
}

#यदि HAVE_विभज_BIN
स्थिर bool dir_is_विभज(ध्रुव वर्ण *a, ध्रुव वर्ण *b) {
        पूर्णाङ्क r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        यदि (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                प्रत्यागम true;
        }
        प्रत्यागम r == 0;
}
#यदि_अन्तः

ध्रुव वर्ण* default_मार्गः(शून्य) {
#यदि HAVE_विभज_BIN
        स्थिर ध्रुव वर्ण *default_मार्गः_2 = रिक्त;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        यदि (default_मार्गः_2)
                प्रत्यागम default_मार्गः_2;

        यदि (dir_is_विभज("/usr/sbin", "/usr/bin"))
                प्रत्यागम (default_मार्गः_2 = DEFAULT_मार्गः_WITH_पूर्णम्_SBIN);  /* a */
        यदि (dir_is_विभज("/usr/local/sbin", "/usr/local/bin"))
                प्रत्यागम (default_मार्गः_2 = DEFAULT_मार्गः_WITH_LOCAL_SBIN); /* b */
        प्रत्यागम (default_मार्गः_2 = DEFAULT_मार्गः_WITHOUT_SBIN);            /* c */
#अन्यथा
        प्रत्यागम DEFAULT_मार्गः_WITHOUT_SBIN;
#यदि_अन्तः
}
