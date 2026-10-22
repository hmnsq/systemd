/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <fnmatch.h>
#അന്തര്ഭാവ <stdio.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <unistd.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "extract-word.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "glob-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"

bool is_path(സ്ഥിരം അക്ഷരം *p) {
        എങ്കിൽ (!p) /* A NULL pointer is definitely not a path */
                തിരികെ false;

        തിരികെ strchr(p, '/');
}

പൂർണ്ണസംഖ്യ path_split_and_make_absolute(സ്ഥിരം അക്ഷരം *p, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **l = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        l = strv_split(p, ":");
        എങ്കിൽ (!l)
                തിരികെ -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        *ret = TAKE_PTR(l);
        തിരികെ r;
}

അക്ഷരം* path_make_absolute(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം അക്ഷരം *prefix) {
        പ്രതിജ്ഞാ(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        എങ്കിൽ (path_is_absolute(p) || isempty(prefix))
                തിരികെ strdup(p);

        തിരികെ path_join(prefix, p);
}

പൂർണ്ണസംഖ്യ safe_getcwd(അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *cwd = ശൂന്യ;

        cwd = get_current_dir_name();
        എങ്കിൽ (!cwd)
                തിരികെ negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        എങ്കിൽ (cwd[0] != '/')
                തിരികെ -ENOMEDIUM;

        എങ്കിൽ (ret)
                *ret = TAKE_PTR(cwd);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ path_make_absolute_cwd(സ്ഥിരം അക്ഷരം *p, അക്ഷരം **ret) {
        അക്ഷരം *c;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(p);
        പ്രതിജ്ഞാ(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        എങ്കിൽ (path_is_absolute(p))
                c = strdup(p);
        അല്ലെങ്കിൽ {
                _cleanup_free_ അക്ഷരം *cwd = ശൂന്യ;

                r = safe_getcwd(&cwd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                c = path_join(cwd, p);
        }
        എങ്കിൽ (!c)
                തിരികെ -ENOMEM;

        *ret = c;
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ path_make_relative(സ്ഥിരം അക്ഷരം *from, സ്ഥിരം അക്ഷരം *to, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *result = ശൂന്യ;
        unsigned n_parents;
        സ്ഥിരം അക്ഷരം *f, *t;
        പൂർണ്ണസംഖ്യ r, k;
        അക്ഷരം *p;

        പ്രതിജ്ഞാ(from);
        പ്രതിജ്ഞാ(to);
        പ്രതിജ്ഞാ(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        എങ്കിൽ (!path_is_absolute(from) || !path_is_absolute(to))
                തിരികെ -EINVAL;

        വേണ്ടി (;;) {
                r = path_find_first_component(&from, true, &f);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                k = path_find_first_component(&to, true, &t);
                എങ്കിൽ (k < 0)
                        തിരികെ k;

                എങ്കിൽ (r == 0) {
                        /* end of 'from' */
                        എങ്കിൽ (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                എങ്കിൽ (!result)
                                        തിരികെ -ENOMEM;
                        } അല്ലെങ്കിൽ {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                എങ്കിൽ (r < 0)
                                        തിരികെ r;

                                എങ്കിൽ (!path_is_valid(result))
                                        തിരികെ -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        തിരികെ 0;
                }

                എങ്കിൽ (r != k || !strneq(f, t, r))
                        നിർത്തുക;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        വേണ്ടി (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;
        }

        എങ്കിൽ (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                തിരികെ -EINVAL;

        result = new(അക്ഷരം, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        എങ്കിൽ (!result)
                തിരികെ -ENOMEM;

        വേണ്ടി (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        എങ്കിൽ (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                തിരികെ 0;
        }

        strcpy(p, t);

        path_simplify(result);

        എങ്കിൽ (!path_is_valid(result))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(result);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ path_make_relative_parent(സ്ഥിരം അക്ഷരം *from_child, സ്ഥിരം അക്ഷരം *to, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *from = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(from_child);
        പ്രതിജ്ഞാ(to);
        പ്രതിജ്ഞാ(ret);

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

        r = path_extract_directory(from_child, &from);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ path_make_relative(from, to, ret);
}

അക്ഷരം* path_startswith_strv(സ്ഥിരം അക്ഷരം *p, അക്ഷരം * സ്ഥിരം *strv) {
        പ്രതിജ്ഞാ(p);

        STRV_FOREACH(s, strv) {
                അക്ഷരം *t;

                t = path_startswith(p, *s);
                എങ്കിൽ (t)
                        തിരികെ t;
        }

        തിരികെ ശൂന്യ;
}

പൂർണ്ണസംഖ്യ path_strv_make_absolute_cwd(അക്ഷരം **l) {
        പൂർണ്ണസംഖ്യ r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                അക്ഷരം *t;

                r = path_make_absolute_cwd(*s, &t);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        തിരികെ 0;
}

അക്ഷരം** path_strv_resolve(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *വേര്) {
        unsigned k = 0;
        bool enomem = false;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (strv_isempty(l))
                തിരികെ l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ അക്ഷരം *orig = ശൂന്യ;
                അക്ഷരം *t, *u;

                എങ്കിൽ (വേര്) {
                        എങ്കിൽ (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                സ്വതന്ത്രം(*s);
                                തുടരുക;
                        }

                        orig = *s;
                        t = path_join(വേര്, orig);
                        എങ്കിൽ (!t) {
                                enomem = true;
                                തുടരുക;
                        }
                } അല്ലെങ്കിൽ
                        t = *s;

                r = chase(t, വേര്, 0, &u, ശൂന്യ);
                എങ്കിൽ (r == -ENOENT) {
                        എങ്കിൽ (വേര്) {
                                u = TAKE_PTR(orig);
                                സ്വതന്ത്രം(t);
                        } അല്ലെങ്കിൽ
                                u = t;
                } അല്ലെങ്കിൽ എങ്കിൽ (r < 0) {
                        സ്വതന്ത്രം(t);

                        എങ്കിൽ (r == -ENOMEM)
                                enomem = true;

                        തുടരുക;
                } അല്ലെങ്കിൽ എങ്കിൽ (വേര്) {
                        അക്ഷരം *x;

                        സ്വതന്ത്രം(t);
                        x = path_startswith(u, വേര്);
                        എങ്കിൽ (x) {
                                /* restore the slash if it was lost */
                                എങ്കിൽ (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                സ്വതന്ത്രം(u);
                                എങ്കിൽ (!t) {
                                        enomem = true;
                                        തുടരുക;
                                }
                                u = t;
                        } അല്ലെങ്കിൽ {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } അല്ലെങ്കിൽ
                        സ്വതന്ത്രം(t);

                l[k++] = u;
        }

        l[k] = ശൂന്യ;

        എങ്കിൽ (enomem)
                തിരികെ ശൂന്യ;

        തിരികെ l;
}

അക്ഷരം** path_strv_resolve_uniq(അക്ഷരം **l, സ്ഥിരം അക്ഷരം *വേര്) {

        എങ്കിൽ (strv_isempty(l))
                തിരികെ l;

        എങ്കിൽ (!path_strv_resolve(l, വേര്))
                തിരികെ ശൂന്യ;

        തിരികെ strv_uniq(l);
}

അക്ഷരം* skip_leading_slash(സ്ഥിരം അക്ഷരം *p) {
        തിരികെ skip_leading_chars(p, "/");
}

അക്ഷരം* path_simplify_full(അക്ഷരം *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        അക്ഷരം *f = path;
        പൂർണ്ണസംഖ്യ r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        എങ്കിൽ (isempty(path))
                തിരികെ path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        വേണ്ടി (സ്ഥിരം അക്ഷരം *p = f;;) {
                സ്ഥിരം അക്ഷരം *e;

                r = path_find_first_component(&p, true, &e);
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        തുടരുക;

                beginning = false;

                എങ്കിൽ (add_slash)
                        *f++ = '/';

                എങ്കിൽ (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        തിരികെ path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        എങ്കിൽ (f == path)
                *f++ = '.';

        എങ്കിൽ (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        തിരികെ path;
}

പൂർണ്ണസംഖ്യ path_simplify_alloc(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(ret);

        എങ്കിൽ (!path) {
                *ret = ശൂന്യ;
                തിരികെ 0;
        }

        അക്ഷരം *t = strdup(path);
        എങ്കിൽ (!t)
                തിരികെ -ENOMEM;

        *ret = path_simplify(t);
        തിരികെ 0;
}

അക്ഷരം* path_startswith_full(സ്ഥിരം അക്ഷരം *original_path, സ്ഥിരം അക്ഷരം *prefix, PathStartWithFlags flags) {
        പ്രതിജ്ഞാ(original_path);
        പ്രതിജ്ഞാ(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        സ്ഥിരം അക്ഷരം *path = original_path;

        എങ്കിൽ ((path[0] == '/') != (prefix[0] == '/'))
                തിരികെ ശൂന്യ;

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *p, *q;
                പൂർണ്ണസംഖ്യ m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                എങ്കിൽ (m < 0)
                        തിരികെ ശൂന്യ;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                എങ്കിൽ (n < 0)
                        തിരികെ ശൂന്യ;

                എങ്കിൽ (n == 0) {
                        എങ്കിൽ (!p)
                                p = path;

                        എങ്കിൽ (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                എങ്കിൽ (p <= original_path)
                                        തിരികെ ശൂന്യ;

                                p--;

                                എങ്കിൽ (*p != '/')
                                        തിരികെ ശൂന്യ;
                        }

                        തിരികെ (അക്ഷരം*) p;
                }

                എങ്കിൽ (m != n)
                        തിരികെ ശൂന്യ;

                എങ്കിൽ (!strneq(p, q, m))
                        തിരികെ ശൂന്യ;
        }
}

പൂർണ്ണസംഖ്യ path_compare(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        പൂർണ്ണസംഖ്യ r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        എങ്കിൽ (r != 0)
                തിരികെ r;

        വേണ്ടി (;;) {
                സ്ഥിരം അക്ഷരം *aa, *bb;
                പൂർണ്ണസംഖ്യ j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                എങ്കിൽ (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        എങ്കിൽ (r != 0)
                                തിരികെ r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        തിരികെ strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                എങ്കിൽ (j == 0) {
                        എങ്കിൽ (k == 0)
                                തിരികെ 0;
                        തിരികെ -1;
                }
                എങ്കിൽ (k == 0)
                        തിരികെ 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                എങ്കിൽ (r != 0)
                        തിരികെ r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                എങ്കിൽ (r != 0)
                        തിരികെ r;
        }
}

പൂർണ്ണസംഖ്യ path_compare_filename(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        _cleanup_free_ അക്ഷരം *fa = ശൂന്യ, *fb = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        എങ്കിൽ (r != 0)
                തിരികെ r;

        /* fallback to use strcmp() if both paths are invalid. */
        എങ്കിൽ (j < 0)
                തിരികെ strcmp(a, b);

        തിരികെ strcmp(fa, fb);
}

പൂർണ്ണസംഖ്യ path_equal_or_inode_same_full(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b, പൂർണ്ണസംഖ്യ flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        എങ്കിൽ (path_equal(a, b))
                തിരികെ 1;

        എങ്കിൽ (!a || !b)
                തിരികെ 0;

        തിരികെ inode_same(a, b, flags);
}

അക്ഷരം* path_extend_internal(അക്ഷരം **x, ...) {
        size_t sz, old_sz;
        അക്ഷരം *q, *nx;
        സ്ഥിരം അക്ഷരം *p;
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
        ആകുമ്പോൾ ((p = va_arg(ap, അക്ഷരം*)) != POINTER_MAX) {
                size_t add;

                എങ്കിൽ (isempty(p))
                        തുടരുക;

                add = 1 + strlen(p);
                എങ്കിൽ (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        തിരികെ ശൂന്യ;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : ശൂന്യ, GREEDY_ALLOC_ROUND_UP(sz+1));
        എങ്കിൽ (!nx)
                തിരികെ ശൂന്യ;
        എങ്കിൽ (x)
                *x = nx;

        എങ്കിൽ (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        അല്ലെങ്കിൽ {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        ആകുമ്പോൾ ((p = va_arg(ap, അക്ഷരം*)) != POINTER_MAX) {
                എങ്കിൽ (isempty(p))
                        തുടരുക;

                എങ്കിൽ (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        തിരികെ nx;
}

പൂർണ്ണസംഖ്യ open_and_check_executable(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        _cleanup_free_ അക്ഷരം *resolved = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        എങ്കിൽ (വേര്) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, വേര്, CHASE_PREFIX_വേര്, &resolved, &fd);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                name = resolved;
        } അല്ലെങ്കിൽ {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                എങ്കിൽ (fd < 0)
                        തിരികെ -errno;
        }

        r = fd_verify_regular(fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        r = access_fd(fd, X_OK);
        എങ്കിൽ (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path) {
                എങ്കിൽ (resolved)
                        *ret_path = TAKE_PTR(resolved);
                അല്ലെങ്കിൽ {
                        r = path_make_absolute_cwd(name, ret_path);
                        എങ്കിൽ (r < 0)
                                തിരികെ r;

                        path_simplify(*ret_path);
                }
        }

        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ find_executable_full(
                സ്ഥിരം അക്ഷരം *name,
                സ്ഥിരം അക്ഷരം *വേര്,
                അക്ഷരം * സ്ഥിരം *exec_തിരയൽ_path,
                bool use_path_envvar,
                അക്ഷരം **ret_filename,
                പൂർണ്ണസംഖ്യ *ret_fd) {

        പൂർണ്ണസംഖ്യ last_error = -ENOENT, r = 0;

        പ്രതിജ്ഞാ(name);

        എങ്കിൽ (is_path(name))
                തിരികെ open_and_check_executable(name, വേര്, ret_filename, ret_fd);

        എങ്കിൽ (exec_തിരയൽ_path) {
                STRV_FOREACH(element, exec_തിരയൽ_path) {
                        _cleanup_free_ അക്ഷരം *full_path = ശൂന്യ;

                        എങ്കിൽ (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                തുടരുക;
                        }

                        full_path = path_join(*element, name);
                        എങ്കിൽ (!full_path)
                                തിരികെ -ENOMEM;

                        r = open_and_check_executable(full_path, വേര്, ret_filename, ret_fd);
                        എങ്കിൽ (r >= 0)
                                തിരികെ 0;
                        എങ്കിൽ (r != -EACCES)
                                last_error = r;
                }
                തിരികെ last_error;
        }

        സ്ഥിരം അക്ഷരം *p = ശൂന്യ;

        എങ്കിൽ (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        എങ്കിൽ (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        വേണ്ടി (;;) {
                _cleanup_free_ അക്ഷരം *element = ശൂന്യ;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        തുടരുക;
                }

                എങ്കിൽ (!path_extend(&element, name))
                        തിരികെ -ENOMEM;

                r = open_and_check_executable(element, വേര്, ret_filename, ret_fd);
                എങ്കിൽ (r >= 0) /* Found it! */
                        തിരികെ 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                എങ്കിൽ (r != -EACCES)
                        last_error = r;
        }

        തിരികെ last_error;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ executable_is_good(സ്ഥിരം അക്ഷരം *executable) {
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ, *d = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = find_executable(executable, &p);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ 0;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        എങ്കിൽ (r == -EINVAL) /* not a symlink */
                തിരികെ 1;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

പൂർണ്ണസംഖ്യ fsck_exists(ശൂന്യം) {
        തിരികെ executable_is_good("fsck");
}

പൂർണ്ണസംഖ്യ fsck_exists_for_fstype(സ്ഥിരം അക്ഷരം *fstype) {
        സ്ഥിരം അക്ഷരം *checker;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fstype);

        എങ്കിൽ (streq(fstype, "auto"))
                തിരികെ -EINVAL;

        r = fsck_exists();
        എങ്കിൽ (r <= 0)
                തിരികെ r;

        checker = strjoina("fsck.", fstype);
        തിരികെ executable_is_good(checker);
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_slash_or_dot(സ്ഥിരം അക്ഷരം *p) {
        POINTER_MAY_BE_NULL(p);

        വേണ്ടി (; !isempty(p); p++) {
                എങ്കിൽ (*p == '/')
                        തുടരുക;
                എങ്കിൽ (startswith(p, "./")) {
                        p++;
                        തുടരുക;
                }
                നിർത്തുക;
        }
        തിരികെ p;
}

പൂർണ്ണസംഖ്യ path_find_first_component(സ്ഥിരം അക്ഷരം **p, bool accept_dot_dot, സ്ഥിരം അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *q, *first, *end_first, *next;
        size_t len;

        പ്രതിജ്ഞാ(p);

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
        എങ്കിൽ (isempty(first)) {
                *p = first;
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ 0;
        }
        എങ്കിൽ (streq(first, ".")) {
                *p = first + 1;
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        എങ്കിൽ (len > NAME_MAX)
                തിരികെ -EINVAL;
        എങ്കിൽ (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                തിരികെ -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        എങ്കിൽ (ret)
                *ret = first;
        തിരികെ len;
}

സ്ഥിതം സ്ഥിരം അക്ഷരം* skip_slash_or_dot_backward(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *q) {
        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(!q || q >= path);

        വേണ്ടി (; q; q = PTR_SUB1(q, path)) {
                എങ്കിൽ (*q == '/')
                        തുടരുക;
                എങ്കിൽ (q > path && strneq(q - 1, "/.", 2))
                        തുടരുക;
                എങ്കിൽ (q == path && *q == '.')
                        തുടരുക;
                നിർത്തുക;
        }
        തിരികെ q;
}

പൂർണ്ണസംഖ്യ path_find_last_component(സ്ഥിരം അക്ഷരം *path, bool accept_dot_dot, സ്ഥിരം അക്ഷരം **next, സ്ഥിരം അക്ഷരം **ret) {
        സ്ഥിരം അക്ഷരം *q, *last_end, *last_begin;
        size_t len;

        POINTER_MAY_BE_NULL(next);
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

        എങ്കിൽ (isempty(path)) {
                എങ്കിൽ (next)
                        *next = path;
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ 0;
        }

        എങ്കിൽ (next && *next) {
                എങ്കിൽ (*next < path || *next > path + strlen(path))
                        തിരികെ -EINVAL;
                എങ്കിൽ (*next == path) {
                        എങ്കിൽ (ret)
                                *ret = ശൂന്യ;
                        തിരികെ 0;
                }
                എങ്കിൽ (!IN_SET(**next, '\0', '/'))
                        തിരികെ -EINVAL;
                q = *next - 1;
        } അല്ലെങ്കിൽ
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        എങ്കിൽ (!q) { /* the root directory, "." or "./" */
                എങ്കിൽ (next)
                        *next = path;
                എങ്കിൽ (ret)
                        *ret = ശൂന്യ;
                തിരികെ 0;
        }

        last_end = q + 1;

        ആകുമ്പോൾ (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        എങ്കിൽ (len > NAME_MAX)
                തിരികെ -EINVAL;
        എങ്കിൽ (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                തിരികെ -EINVAL;

        എങ്കിൽ (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        എങ്കിൽ (ret)
                *ret = last_begin;
        തിരികെ len;
}

സ്ഥിരം അക്ഷരം* last_path_component(സ്ഥിരം അക്ഷരം *path) {

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

        unsigned l, k;

        എങ്കിൽ (!path)
                തിരികെ ശൂന്യ;

        l = k = strlen(path);
        എങ്കിൽ (l == 0) /* special case — an empty string */
                തിരികെ path;

        ആകുമ്പോൾ (k > 0 && path[k-1] == '/')
                k--;

        എങ്കിൽ (k == 0) /* the root directory */
                തിരികെ path + l - 1;

        ആകുമ്പോൾ (k > 0 && path[k-1] != '/')
                k--;

        തിരികെ path + k;
}

പൂർണ്ണസംഖ്യ path_split_prefix_filename(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret_dir, അക്ഷരം **ret_filename) {
        _cleanup_free_ അക്ഷരം *d = ശൂന്യ;
        സ്ഥിരം അക്ഷരം *c, *next = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        POINTER_MAY_BE_NULL(path);

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

        എങ്കിൽ (isempty(path))
                തിരികെ -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r == 0) /* root directory or "." */
                തിരികെ -EADDRNOTAVAIL;

        എങ്കിൽ (ret_dir) {
                എങ്കിൽ (next == path) {
                        എങ്കിൽ (*path != '/') { /* filename only */
                                എങ്കിൽ (!ret_filename)
                                        തിരികെ -EDESTADDRREQ;
                        } അല്ലെങ്കിൽ {
                                d = strdup("/");
                                എങ്കിൽ (!d)
                                        തിരികെ -ENOMEM;
                        }
                } അല്ലെങ്കിൽ {
                        d = strndup(path, next - path);
                        എങ്കിൽ (!d)
                                തിരികെ -ENOMEM;

                        path_simplify(d);

                        എങ്കിൽ (!path_is_valid(d))
                                തിരികെ -EINVAL;
                }

        } അല്ലെങ്കിൽ എങ്കിൽ (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                തിരികെ -EINVAL;

        എങ്കിൽ (ret_filename) {
                അക്ഷരം *fn = strndup(c, r);
                എങ്കിൽ (!fn)
                        തിരികെ -ENOMEM;

                *ret_filename = fn;
        }

        എങ്കിൽ (ret_dir)
                *ret_dir = TAKE_PTR(d);

        തിരികെ strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(സ്ഥിരം അക്ഷരം *p) {
        സ്ഥിരം അക്ഷരം *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        എങ്കിൽ (!p)
                തിരികെ false;

        e = strchrnul(p, '/');
        എങ്കിൽ (*e != 0)
                തിരികെ false;

        എങ്കിൽ (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                തിരികെ false;

        തിരികെ true;
}

bool filename_is_valid(സ്ഥിരം അക്ഷരം *p) {

        എങ്കിൽ (isempty(p))
                തിരികെ false;

        എങ്കിൽ (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                തിരികെ false;

        തിരികെ filename_part_is_valid(p);
}

bool path_is_valid_full(സ്ഥിരം അക്ഷരം *p, bool accept_dot_dot) {
        എങ്കിൽ (isempty(p))
                തിരികെ false;

        വേണ്ടി (സ്ഥിരം അക്ഷരം *e = p;;) {
                പൂർണ്ണസംഖ്യ r;

                r = path_find_first_component(&e, accept_dot_dot, ശൂന്യ);
                എങ്കിൽ (r < 0)
                        തിരികെ false;

                എങ്കിൽ (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        തിരികെ false;
                എങ്കിൽ (*e == 0)           /* End of string? Yay! */
                        തിരികെ true;
        }
}

bool path_is_normalized(സ്ഥിരം അക്ഷരം *p) {
        എങ്കിൽ (!path_is_safe(p))
                തിരികെ false;

        എങ്കിൽ (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                തിരികെ false;

        എങ്കിൽ (strstr(p, "//"))
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ file_in_same_dir(സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *filename, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *b = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(filename);
        പ്രതിജ്ഞാ(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        എങ്കിൽ (path_is_absolute(filename))
                b = strdup(filename);
        അല്ലെങ്കിൽ {
                _cleanup_free_ അക്ഷരം *dn = ശൂന്യ;

                r = path_extract_directory(path, &dn);
                എങ്കിൽ (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                അല്ലെങ്കിൽ എങ്കിൽ (r < 0)
                        തിരികെ r;
                അല്ലെങ്കിൽ
                        b = path_join(dn, filename);
        }
        എങ്കിൽ (!b)
                തിരികെ -ENOMEM;

        *ret = TAKE_PTR(b);
        തിരികെ 0;
}

bool hidden_or_backup_file(സ്ഥിരം അക്ഷരം *filename) {
        പ്രതിജ്ഞാ(filename);

        എങ്കിൽ (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                തിരികെ true;

        സ്ഥിരം അക്ഷരം *dot = strrchr(filename, '.');
        എങ്കിൽ (!dot)
                തിരികെ false;

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

        തിരികെ STR_IN_SET(dot + 1,
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

bool is_device_path(സ്ഥിരം അക്ഷരം *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        സ്ഥിരം അക്ഷരം *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        തിരികെ !isempty(p);
}

bool valid_device_കണ്ണി_path(സ്ഥിരം അക്ഷരം *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        എങ്കിൽ (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                തിരികെ false;

        എങ്കിൽ (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                തിരികെ false;

        തിരികെ path_is_normalized(path);
}

bool valid_device_allow_pattern(സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        എങ്കിൽ (STARTSWITH_SET(path, "block-", "char-"))
                തിരികെ true;

        തിരികെ valid_device_കണ്ണി_path(path);
}

bool dot_or_dot_dot(സ്ഥിരം അക്ഷരം *path) {
        എങ്കിൽ (!path)
                തിരികെ false;
        എങ്കിൽ (path[0] != '.')
                തിരികെ false;
        എങ്കിൽ (path[1] == 0)
                തിരികെ true;
        എങ്കിൽ (path[1] != '.')
                തിരികെ false;

        തിരികെ path[2] == 0;
}

bool path_implies_directory(സ്ഥിരം അക്ഷരം *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        എങ്കിൽ (!path)
                തിരികെ false;

        എങ്കിൽ (dot_or_dot_dot(path))
                തിരികെ true;

        തിരികെ ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_വേര്(സ്ഥിരം അക്ഷരം *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        എങ്കിൽ (isempty(path))
                തിരികെ true;

        തിരികെ path_equal(path, "/");
}

സ്ഥിരം അക്ഷരം* empty_to_വേര്(സ്ഥിരം അക്ഷരം *path) {
        തിരികെ isempty(path) ? "/" : path;
}

പൂർണ്ണസംഖ്യ empty_or_വേര്_harder_to_null(സ്ഥിരം അക്ഷരം **path) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        എങ്കിൽ (empty_or_വേര്(*path)) {
                *path = ശൂന്യ;
                തിരികെ 0;
        }

        r = path_is_വേര്(*path);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r > 0)
                *path = ശൂന്യ;

        തിരികെ 0;
}

bool path_strv_contains(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(path);

        STRV_FOREACH(i, l)
                എങ്കിൽ (path_equal(*i, path))
                        തിരികെ true;

        തിരികെ false;
}

bool prefixed_path_strv_contains(അക്ഷരം * സ്ഥിരം *l, സ്ഥിരം അക്ഷരം *path) {
        പ്രതിജ്ഞാ(path);

        STRV_FOREACH(i, l) {
                സ്ഥിരം അക്ഷരം *j = *i;

                എങ്കിൽ (*j == '-')
                        j++;
                എങ്കിൽ (*j == '+')
                        j++;

                എങ്കിൽ (path_equal(j, path))
                        തിരികെ true;
        }

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ path_glob_can_match(സ്ഥിരം അക്ഷരം *pattern, സ്ഥിരം അക്ഷരം *prefix, അക്ഷരം **ret) {
        പ്രതിജ്ഞാ(pattern);
        പ്രതിജ്ഞാ(prefix);

        വേണ്ടി (സ്ഥിരം അക്ഷരം *a = pattern, *b = prefix;;) {
                _cleanup_free_ അക്ഷരം *g = ശൂന്യ, *h = ശൂന്യ;
                സ്ഥിരം അക്ഷരം *p, *q;
                പൂർണ്ണസംഖ്യ r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                എങ്കിൽ (r < 0)
                        തിരികെ r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                എങ്കിൽ (s < 0)
                        തിരികെ s;

                എങ്കിൽ (s == 0) {
                        /* The pattern matches the prefix. */
                        എങ്കിൽ (ret) {
                                അക്ഷരം *t;

                                t = path_join(prefix, p);
                                എങ്കിൽ (!t)
                                        തിരികെ -ENOMEM;

                                *ret = t;
                        }
                        തിരികെ true;
                }

                എങ്കിൽ (r == 0)
                        നിർത്തുക;

                എങ്കിൽ (r == s && strneq(p, q, r))
                        തുടരുക; /* common component. Check next. */

                g = strndup(p, r);
                എങ്കിൽ (!g)
                        തിരികെ -ENOMEM;

                എങ്കിൽ (!string_is_glob(g))
                        നിർത്തുക;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                എങ്കിൽ (!h)
                        തിരികെ -ENOMEM;

                r = fnmatch(g, h, 0);
                എങ്കിൽ (r == FNM_NOMATCH)
                        നിർത്തുക;
                എങ്കിൽ (r != 0) /* Failure to process pattern? */
                        തിരികെ -EINVAL;
        }

        /* The pattern does not match the prefix. */
        എങ്കിൽ (ret)
                *ret = ശൂന്യ;
        തിരികെ false;
}

#എങ്കിൽ HAVE_SPLIT_BIN
സ്ഥിതം bool dir_is_split(സ്ഥിരം അക്ഷരം *a, സ്ഥിരം അക്ഷരം *b) {
        പൂർണ്ണസംഖ്യ r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        എങ്കിൽ (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                തിരികെ true;
        }
        തിരികെ r == 0;
}
#endif

സ്ഥിരം അക്ഷരം* default_PATH(ശൂന്യം) {
#എങ്കിൽ HAVE_SPLIT_BIN
        സ്ഥിതം സ്ഥിരം അക്ഷരം *default_path = ശൂന്യ;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        എങ്കിൽ (default_path)
                തിരികെ default_path;

        എങ്കിൽ (dir_is_split("/usr/sbin", "/usr/bin"))
                തിരികെ (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        എങ്കിൽ (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                തിരികെ (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        തിരികെ (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#അല്ലെങ്കിൽ
        തിരികെ DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
