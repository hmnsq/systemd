/* SPDX-License-Identifier: LGPL-2.1-or-later */

#அந்தர்பாவ <fnmatch.h>
#அந்தர்பாவ <stdio.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <unistd.h>

#அந்தர்பாவ "alloc-util.h"
#அந்தர்பாவ "chase.h"
#அந்தர்பாவ "errno-util.h"
#அந்தர்பாவ "extract-word.h"
#அந்தர்பாவ "fd-util.h"
#அந்தர்பாவ "fs-util.h"
#அந்தர்பாவ "glob-util.h"
#அந்தர்பாவ "log.h"
#அந்தர்பாவ "path-util.h"
#அந்தர்பாவ "stat-util.h"
#அந்தர்பாவ "string-util.h"
#அந்தர்பாவ "strv.h"

bool is_path(மாறிலி எழுத்து *p) {
        எனில் (!p) /* A NULL pointer is definitely not a path */
                திரும்பு false;

        திரும்பு strchr(p, '/');
}

முழு path_split_and_make_absolute(மாறிலி எழுத்து *p, எழுத்து ***ret) {
        _cleanup_strv_free_ எழுத்து **l = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        l = strv_split(p, ":");
        எனில் (!l)
                திரும்பு -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        எனில் (r < 0)
                திரும்பு r;

        *ret = TAKE_PTR(l);
        திரும்பு r;
}

எழுத்து* path_make_absolute(மாறிலி எழுத்து *p, மாறிலி எழுத்து *prefix) {
        ப்ரதிஜ்ஞா(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        எனில் (path_is_absolute(p) || isempty(prefix))
                திரும்பு strdup(p);

        திரும்பு path_join(prefix, p);
}

முழு safe_getcwd(எழுத்து **ret) {
        _cleanup_free_ எழுத்து *cwd = வெற்று;

        cwd = get_current_dir_name();
        எனில் (!cwd)
                திரும்பு negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        எனில் (cwd[0] != '/')
                திரும்பு -ENOMEDIUM;

        எனில் (ret)
                *ret = TAKE_PTR(cwd);

        திரும்பு 0;
}

முழு path_make_absolute_cwd(மாறிலி எழுத்து *p, எழுத்து **ret) {
        எழுத்து *c;
        முழு r;

        ப்ரதிஜ்ஞா(p);
        ப்ரதிஜ்ஞா(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        எனில் (path_is_absolute(p))
                c = strdup(p);
        இல்லையெனில் {
                _cleanup_free_ எழுத்து *cwd = வெற்று;

                r = safe_getcwd(&cwd);
                எனில் (r < 0)
                        திரும்பு r;

                c = path_join(cwd, p);
        }
        எனில் (!c)
                திரும்பு -ENOMEM;

        *ret = c;
        திரும்பு 0;
}

முழு path_make_relative(மாறிலி எழுத்து *from, மாறிலி எழுத்து *to, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *result = வெற்று;
        unsigned n_parents;
        மாறிலி எழுத்து *f, *t;
        முழு r, k;
        எழுத்து *p;

        ப்ரதிஜ்ஞா(from);
        ப்ரதிஜ்ஞா(to);
        ப்ரதிஜ்ஞா(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        எனில் (!path_is_absolute(from) || !path_is_absolute(to))
                திரும்பு -EINVAL;

        க்கு (;;) {
                r = path_find_first_component(&from, true, &f);
                எனில் (r < 0)
                        திரும்பு r;

                k = path_find_first_component(&to, true, &t);
                எனில் (k < 0)
                        திரும்பு k;

                எனில் (r == 0) {
                        /* end of 'from' */
                        எனில் (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                எனில் (!result)
                                        திரும்பு -ENOMEM;
                        } இல்லையெனில் {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                எனில் (r < 0)
                                        திரும்பு r;

                                எனில் (!path_is_valid(result))
                                        திரும்பு -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        திரும்பு 0;
                }

                எனில் (r != k || !strneq(f, t, r))
                        நிறுத்து;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        க்கு (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;
        }

        எனில் (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                திரும்பு -EINVAL;

        result = new(எழுத்து, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        எனில் (!result)
                திரும்பு -ENOMEM;

        க்கு (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        எனில் (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                திரும்பு 0;
        }

        strcpy(p, t);

        path_simplify(result);

        எனில் (!path_is_valid(result))
                திரும்பு -EINVAL;

        *ret = TAKE_PTR(result);
        திரும்பு 0;
}

முழு path_make_relative_parent(மாறிலி எழுத்து *from_child, மாறிலி எழுத்து *to, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *from = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(from_child);
        ப்ரதிஜ்ஞா(to);
        ப்ரதிஜ்ஞா(ret);

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
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு path_make_relative(from, to, ret);
}

எழுத்து* path_startswith_strv(மாறிலி எழுத்து *p, எழுத்து * மாறிலி *strv) {
        ப்ரதிஜ்ஞா(p);

        STRV_FOREACH(s, strv) {
                எழுத்து *t;

                t = path_startswith(p, *s);
                எனில் (t)
                        திரும்பு t;
        }

        திரும்பு வெற்று;
}

முழு path_strv_make_absolute_cwd(எழுத்து **l) {
        முழு r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                எழுத்து *t;

                r = path_make_absolute_cwd(*s, &t);
                எனில் (r < 0)
                        திரும்பு r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        திரும்பு 0;
}

எழுத்து** path_strv_resolve(எழுத்து **l, மாறிலி எழுத்து *வேர்) {
        unsigned k = 0;
        bool enomem = false;
        முழு r;

        எனில் (strv_isempty(l))
                திரும்பு l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ எழுத்து *orig = வெற்று;
                எழுத்து *t, *u;

                எனில் (வேர்) {
                        எனில் (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                விடுவி(*s);
                                தொடர்;
                        }

                        orig = *s;
                        t = path_join(வேர், orig);
                        எனில் (!t) {
                                enomem = true;
                                தொடர்;
                        }
                } இல்லையெனில்
                        t = *s;

                r = chase(t, வேர், 0, &u, வெற்று);
                எனில் (r == -ENOENT) {
                        எனில் (வேர்) {
                                u = TAKE_PTR(orig);
                                விடுவி(t);
                        } இல்லையெனில்
                                u = t;
                } இல்லையெனில் எனில் (r < 0) {
                        விடுவி(t);

                        எனில் (r == -ENOMEM)
                                enomem = true;

                        தொடர்;
                } இல்லையெனில் எனில் (வேர்) {
                        எழுத்து *x;

                        விடுவி(t);
                        x = path_startswith(u, வேர்);
                        எனில் (x) {
                                /* restore the slash if it was lost */
                                எனில் (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                விடுவி(u);
                                எனில் (!t) {
                                        enomem = true;
                                        தொடர்;
                                }
                                u = t;
                        } இல்லையெனில் {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } இல்லையெனில்
                        விடுவி(t);

                l[k++] = u;
        }

        l[k] = வெற்று;

        எனில் (enomem)
                திரும்பு வெற்று;

        திரும்பு l;
}

எழுத்து** path_strv_resolve_uniq(எழுத்து **l, மாறிலி எழுத்து *வேர்) {

        எனில் (strv_isempty(l))
                திரும்பு l;

        எனில் (!path_strv_resolve(l, வேர்))
                திரும்பு வெற்று;

        திரும்பு strv_uniq(l);
}

எழுத்து* skip_leading_slash(மாறிலி எழுத்து *p) {
        திரும்பு skip_leading_chars(p, "/");
}

எழுத்து* path_simplify_full(எழுத்து *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        எழுத்து *f = path;
        முழு r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        எனில் (isempty(path))
                திரும்பு path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        க்கு (மாறிலி எழுத்து *p = f;;) {
                மாறிலி எழுத்து *e;

                r = path_find_first_component(&p, true, &e);
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        தொடர்;

                beginning = false;

                எனில் (add_slash)
                        *f++ = '/';

                எனில் (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        திரும்பு path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        எனில் (f == path)
                *f++ = '.';

        எனில் (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        திரும்பு path;
}

முழு path_simplify_alloc(மாறிலி எழுத்து *path, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(ret);

        எனில் (!path) {
                *ret = வெற்று;
                திரும்பு 0;
        }

        எழுத்து *t = strdup(path);
        எனில் (!t)
                திரும்பு -ENOMEM;

        *ret = path_simplify(t);
        திரும்பு 0;
}

எழுத்து* path_startswith_full(மாறிலி எழுத்து *original_path, மாறிலி எழுத்து *prefix, PathStartWithFlags flags) {
        ப்ரதிஜ்ஞா(original_path);
        ப்ரதிஜ்ஞா(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        மாறிலி எழுத்து *path = original_path;

        எனில் ((path[0] == '/') != (prefix[0] == '/'))
                திரும்பு வெற்று;

        க்கு (;;) {
                மாறிலி எழுத்து *p, *q;
                முழு m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                எனில் (m < 0)
                        திரும்பு வெற்று;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                எனில் (n < 0)
                        திரும்பு வெற்று;

                எனில் (n == 0) {
                        எனில் (!p)
                                p = path;

                        எனில் (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                எனில் (p <= original_path)
                                        திரும்பு வெற்று;

                                p--;

                                எனில் (*p != '/')
                                        திரும்பு வெற்று;
                        }

                        திரும்பு (எழுத்து*) p;
                }

                எனில் (m != n)
                        திரும்பு வெற்று;

                எனில் (!strneq(p, q, m))
                        திரும்பு வெற்று;
        }
}

முழு path_compare(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        முழு r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        எனில் (r != 0)
                திரும்பு r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        எனில் (r != 0)
                திரும்பு r;

        க்கு (;;) {
                மாறிலி எழுத்து *aa, *bb;
                முழு j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                எனில் (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        எனில் (r != 0)
                                திரும்பு r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        திரும்பு strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                எனில் (j == 0) {
                        எனில் (k == 0)
                                திரும்பு 0;
                        திரும்பு -1;
                }
                எனில் (k == 0)
                        திரும்பு 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                எனில் (r != 0)
                        திரும்பு r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                எனில் (r != 0)
                        திரும்பு r;
        }
}

முழு path_compare_filename(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        _cleanup_free_ எழுத்து *fa = வெற்று, *fb = வெற்று;
        முழு r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        எனில் (r != 0)
                திரும்பு r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        எனில் (r != 0)
                திரும்பு r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        எனில் (r != 0)
                திரும்பு r;

        /* fallback to use strcmp() if both paths are invalid. */
        எனில் (j < 0)
                திரும்பு strcmp(a, b);

        திரும்பு strcmp(fa, fb);
}

முழு path_equal_or_inode_same_full(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b, முழு flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        எனில் (path_equal(a, b))
                திரும்பு 1;

        எனில் (!a || !b)
                திரும்பு 0;

        திரும்பு inode_same(a, b, flags);
}

எழுத்து* path_extend_internal(எழுத்து **x, ...) {
        size_t sz, old_sz;
        எழுத்து *q, *nx;
        மாறிலி எழுத்து *p;
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
        போது ((p = va_arg(ap, எழுத்து*)) != POINTER_MAX) {
                size_t add;

                எனில் (isempty(p))
                        தொடர்;

                add = 1 + strlen(p);
                எனில் (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        திரும்பு வெற்று;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : வெற்று, GREEDY_ALLOC_ROUND_UP(sz+1));
        எனில் (!nx)
                திரும்பு வெற்று;
        எனில் (x)
                *x = nx;

        எனில் (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        இல்லையெனில் {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        போது ((p = va_arg(ap, எழுத்து*)) != POINTER_MAX) {
                எனில் (isempty(p))
                        தொடர்;

                எனில் (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        திரும்பு nx;
}

முழு open_and_check_executable(மாறிலி எழுத்து *name, மாறிலி எழுத்து *வேர், எழுத்து **ret_path, முழு *ret_fd) {
        _cleanup_close_ முழு fd = -EBADF;
        _cleanup_free_ எழுத்து *resolved = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        எனில் (வேர்) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, வேர், CHASE_PREFIX_வேர், &resolved, &fd);
                எனில் (r < 0)
                        திரும்பு r;

                name = resolved;
        } இல்லையெனில் {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                எனில் (fd < 0)
                        திரும்பு -errno;
        }

        r = fd_verify_regular(fd);
        எனில் (r < 0)
                திரும்பு r;

        r = access_fd(fd, X_OK);
        எனில் (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        எனில் (r < 0)
                திரும்பு r;

        எனில் (ret_path) {
                எனில் (resolved)
                        *ret_path = TAKE_PTR(resolved);
                இல்லையெனில் {
                        r = path_make_absolute_cwd(name, ret_path);
                        எனில் (r < 0)
                                திரும்பு r;

                        path_simplify(*ret_path);
                }
        }

        எனில் (ret_fd)
                *ret_fd = TAKE_FD(fd);

        திரும்பு 0;
}

முழு find_executable_full(
                மாறிலி எழுத்து *name,
                மாறிலி எழுத்து *வேர்,
                எழுத்து * மாறிலி *exec_தேடல்_path,
                bool use_path_envvar,
                எழுத்து **ret_filename,
                முழு *ret_fd) {

        முழு last_error = -ENOENT, r = 0;

        ப்ரதிஜ்ஞா(name);

        எனில் (is_path(name))
                திரும்பு open_and_check_executable(name, வேர், ret_filename, ret_fd);

        எனில் (exec_தேடல்_path) {
                STRV_FOREACH(element, exec_தேடல்_path) {
                        _cleanup_free_ எழுத்து *full_path = வெற்று;

                        எனில் (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                தொடர்;
                        }

                        full_path = path_join(*element, name);
                        எனில் (!full_path)
                                திரும்பு -ENOMEM;

                        r = open_and_check_executable(full_path, வேர், ret_filename, ret_fd);
                        எனில் (r >= 0)
                                திரும்பு 0;
                        எனில் (r != -EACCES)
                                last_error = r;
                }
                திரும்பு last_error;
        }

        மாறிலி எழுத்து *p = வெற்று;

        எனில் (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        எனில் (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        க்கு (;;) {
                _cleanup_free_ எழுத்து *element = வெற்று;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                எனில் (r < 0)
                        திரும்பு r;
                எனில் (r == 0)
                        நிறுத்து;

                எனில் (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        தொடர்;
                }

                எனில் (!path_extend(&element, name))
                        திரும்பு -ENOMEM;

                r = open_and_check_executable(element, வேர், ret_filename, ret_fd);
                எனில் (r >= 0) /* Found it! */
                        திரும்பு 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                எனில் (r != -EACCES)
                        last_error = r;
        }

        திரும்பு last_error;
}

நிலையான முழு executable_is_good(மாறிலி எழுத்து *executable) {
        _cleanup_free_ எழுத்து *p = வெற்று, *d = வெற்று;
        முழு r;

        r = find_executable(executable, &p);
        எனில் (r == -ENOENT)
                திரும்பு 0;
        எனில் (r < 0)
                திரும்பு r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        எனில் (r == -EINVAL) /* not a symlink */
                திரும்பு 1;
        எனில் (r < 0)
                திரும்பு r;

        திரும்பு !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

முழு fsck_exists(வெறுமை) {
        திரும்பு executable_is_good("fsck");
}

முழு fsck_exists_for_fstype(மாறிலி எழுத்து *fstype) {
        மாறிலி எழுத்து *checker;
        முழு r;

        ப்ரதிஜ்ஞா(fstype);

        எனில் (streq(fstype, "auto"))
                திரும்பு -EINVAL;

        r = fsck_exists();
        எனில் (r <= 0)
                திரும்பு r;

        checker = strjoina("fsck.", fstype);
        திரும்பு executable_is_good(checker);
}

நிலையான மாறிலி எழுத்து* skip_slash_or_dot(மாறிலி எழுத்து *p) {
        POINTER_MAY_BE_NULL(p);

        க்கு (; !isempty(p); p++) {
                எனில் (*p == '/')
                        தொடர்;
                எனில் (startswith(p, "./")) {
                        p++;
                        தொடர்;
                }
                நிறுத்து;
        }
        திரும்பு p;
}

முழு path_find_first_component(மாறிலி எழுத்து **p, bool accept_dot_dot, மாறிலி எழுத்து **ret) {
        மாறிலி எழுத்து *q, *first, *end_first, *next;
        size_t len;

        ப்ரதிஜ்ஞா(p);

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
        எனில் (isempty(first)) {
                *p = first;
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு 0;
        }
        எனில் (streq(first, ".")) {
                *p = first + 1;
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        எனில் (len > NAME_MAX)
                திரும்பு -EINVAL;
        எனில் (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                திரும்பு -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        எனில் (ret)
                *ret = first;
        திரும்பு len;
}

நிலையான மாறிலி எழுத்து* skip_slash_or_dot_backward(மாறிலி எழுத்து *path, மாறிலி எழுத்து *q) {
        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(!q || q >= path);

        க்கு (; q; q = PTR_SUB1(q, path)) {
                எனில் (*q == '/')
                        தொடர்;
                எனில் (q > path && strneq(q - 1, "/.", 2))
                        தொடர்;
                எனில் (q == path && *q == '.')
                        தொடர்;
                நிறுத்து;
        }
        திரும்பு q;
}

முழு path_find_last_component(மாறிலி எழுத்து *path, bool accept_dot_dot, மாறிலி எழுத்து **next, மாறிலி எழுத்து **ret) {
        மாறிலி எழுத்து *q, *last_end, *last_begin;
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

        எனில் (isempty(path)) {
                எனில் (next)
                        *next = path;
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு 0;
        }

        எனில் (next && *next) {
                எனில் (*next < path || *next > path + strlen(path))
                        திரும்பு -EINVAL;
                எனில் (*next == path) {
                        எனில் (ret)
                                *ret = வெற்று;
                        திரும்பு 0;
                }
                எனில் (!IN_SET(**next, '\0', '/'))
                        திரும்பு -EINVAL;
                q = *next - 1;
        } இல்லையெனில்
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        எனில் (!q) { /* the root directory, "." or "./" */
                எனில் (next)
                        *next = path;
                எனில் (ret)
                        *ret = வெற்று;
                திரும்பு 0;
        }

        last_end = q + 1;

        போது (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        எனில் (len > NAME_MAX)
                திரும்பு -EINVAL;
        எனில் (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                திரும்பு -EINVAL;

        எனில் (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        எனில் (ret)
                *ret = last_begin;
        திரும்பு len;
}

மாறிலி எழுத்து* last_path_component(மாறிலி எழுத்து *path) {

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

        எனில் (!path)
                திரும்பு வெற்று;

        l = k = strlen(path);
        எனில் (l == 0) /* special case — an empty string */
                திரும்பு path;

        போது (k > 0 && path[k-1] == '/')
                k--;

        எனில் (k == 0) /* the root directory */
                திரும்பு path + l - 1;

        போது (k > 0 && path[k-1] != '/')
                k--;

        திரும்பு path + k;
}

முழு path_split_prefix_filename(மாறிலி எழுத்து *path, எழுத்து **ret_dir, எழுத்து **ret_filename) {
        _cleanup_free_ எழுத்து *d = வெற்று;
        மாறிலி எழுத்து *c, *next = வெற்று;
        முழு r;

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

        எனில் (isempty(path))
                திரும்பு -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r == 0) /* root directory or "." */
                திரும்பு -EADDRNOTAVAIL;

        எனில் (ret_dir) {
                எனில் (next == path) {
                        எனில் (*path != '/') { /* filename only */
                                எனில் (!ret_filename)
                                        திரும்பு -EDESTADDRREQ;
                        } இல்லையெனில் {
                                d = strdup("/");
                                எனில் (!d)
                                        திரும்பு -ENOMEM;
                        }
                } இல்லையெனில் {
                        d = strndup(path, next - path);
                        எனில் (!d)
                                திரும்பு -ENOMEM;

                        path_simplify(d);

                        எனில் (!path_is_valid(d))
                                திரும்பு -EINVAL;
                }

        } இல்லையெனில் எனில் (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                திரும்பு -EINVAL;

        எனில் (ret_filename) {
                எழுத்து *fn = strndup(c, r);
                எனில் (!fn)
                        திரும்பு -ENOMEM;

                *ret_filename = fn;
        }

        எனில் (ret_dir)
                *ret_dir = TAKE_PTR(d);

        திரும்பு strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(மாறிலி எழுத்து *p) {
        மாறிலி எழுத்து *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        எனில் (!p)
                திரும்பு false;

        e = strchrnul(p, '/');
        எனில் (*e != 0)
                திரும்பு false;

        எனில் (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                திரும்பு false;

        திரும்பு true;
}

bool filename_is_valid(மாறிலி எழுத்து *p) {

        எனில் (isempty(p))
                திரும்பு false;

        எனில் (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                திரும்பு false;

        திரும்பு filename_part_is_valid(p);
}

bool path_is_valid_full(மாறிலி எழுத்து *p, bool accept_dot_dot) {
        எனில் (isempty(p))
                திரும்பு false;

        க்கு (மாறிலி எழுத்து *e = p;;) {
                முழு r;

                r = path_find_first_component(&e, accept_dot_dot, வெற்று);
                எனில் (r < 0)
                        திரும்பு false;

                எனில் (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        திரும்பு false;
                எனில் (*e == 0)           /* End of string? Yay! */
                        திரும்பு true;
        }
}

bool path_is_normalized(மாறிலி எழுத்து *p) {
        எனில் (!path_is_safe(p))
                திரும்பு false;

        எனில் (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                திரும்பு false;

        எனில் (strstr(p, "//"))
                திரும்பு false;

        திரும்பு true;
}

முழு file_in_same_dir(மாறிலி எழுத்து *path, மாறிலி எழுத்து *filename, எழுத்து **ret) {
        _cleanup_free_ எழுத்து *b = வெற்று;
        முழு r;

        ப்ரதிஜ்ஞா(path);
        ப்ரதிஜ்ஞா(filename);
        ப்ரதிஜ்ஞா(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        எனில் (path_is_absolute(filename))
                b = strdup(filename);
        இல்லையெனில் {
                _cleanup_free_ எழுத்து *dn = வெற்று;

                r = path_extract_directory(path, &dn);
                எனில் (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                இல்லையெனில் எனில் (r < 0)
                        திரும்பு r;
                இல்லையெனில்
                        b = path_join(dn, filename);
        }
        எனில் (!b)
                திரும்பு -ENOMEM;

        *ret = TAKE_PTR(b);
        திரும்பு 0;
}

bool hidden_or_backup_file(மாறிலி எழுத்து *filename) {
        ப்ரதிஜ்ஞா(filename);

        எனில் (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                திரும்பு true;

        மாறிலி எழுத்து *dot = strrchr(filename, '.');
        எனில் (!dot)
                திரும்பு false;

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

        திரும்பு STR_IN_SET(dot + 1,
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

bool is_device_path(மாறிலி எழுத்து *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        மாறிலி எழுத்து *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        திரும்பு !isempty(p);
}

bool valid_device_முனை_path(மாறிலி எழுத்து *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        எனில் (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                திரும்பு false;

        எனில் (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                திரும்பு false;

        திரும்பு path_is_normalized(path);
}

bool valid_device_allow_pattern(மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        எனில் (STARTSWITH_SET(path, "block-", "char-"))
                திரும்பு true;

        திரும்பு valid_device_முனை_path(path);
}

bool dot_or_dot_dot(மாறிலி எழுத்து *path) {
        எனில் (!path)
                திரும்பு false;
        எனில் (path[0] != '.')
                திரும்பு false;
        எனில் (path[1] == 0)
                திரும்பு true;
        எனில் (path[1] != '.')
                திரும்பு false;

        திரும்பு path[2] == 0;
}

bool path_implies_directory(மாறிலி எழுத்து *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        எனில் (!path)
                திரும்பு false;

        எனில் (dot_or_dot_dot(path))
                திரும்பு true;

        திரும்பு ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_வேர்(மாறிலி எழுத்து *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        எனில் (isempty(path))
                திரும்பு true;

        திரும்பு path_equal(path, "/");
}

மாறிலி எழுத்து* empty_to_வேர்(மாறிலி எழுத்து *path) {
        திரும்பு isempty(path) ? "/" : path;
}

முழு empty_or_வேர்_harder_to_null(மாறிலி எழுத்து **path) {
        முழு r;

        ப்ரதிஜ்ஞா(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        எனில் (empty_or_வேர்(*path)) {
                *path = வெற்று;
                திரும்பு 0;
        }

        r = path_is_வேர்(*path);
        எனில் (r < 0)
                திரும்பு r;
        எனில் (r > 0)
                *path = வெற்று;

        திரும்பு 0;
}

bool path_strv_contains(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(path);

        STRV_FOREACH(i, l)
                எனில் (path_equal(*i, path))
                        திரும்பு true;

        திரும்பு false;
}

bool prefixed_path_strv_contains(எழுத்து * மாறிலி *l, மாறிலி எழுத்து *path) {
        ப்ரதிஜ்ஞா(path);

        STRV_FOREACH(i, l) {
                மாறிலி எழுத்து *j = *i;

                எனில் (*j == '-')
                        j++;
                எனில் (*j == '+')
                        j++;

                எனில் (path_equal(j, path))
                        திரும்பு true;
        }

        திரும்பு false;
}

முழு path_glob_can_match(மாறிலி எழுத்து *pattern, மாறிலி எழுத்து *prefix, எழுத்து **ret) {
        ப்ரதிஜ்ஞா(pattern);
        ப்ரதிஜ்ஞா(prefix);

        க்கு (மாறிலி எழுத்து *a = pattern, *b = prefix;;) {
                _cleanup_free_ எழுத்து *g = வெற்று, *h = வெற்று;
                மாறிலி எழுத்து *p, *q;
                முழு r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                எனில் (r < 0)
                        திரும்பு r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                எனில் (s < 0)
                        திரும்பு s;

                எனில் (s == 0) {
                        /* The pattern matches the prefix. */
                        எனில் (ret) {
                                எழுத்து *t;

                                t = path_join(prefix, p);
                                எனில் (!t)
                                        திரும்பு -ENOMEM;

                                *ret = t;
                        }
                        திரும்பு true;
                }

                எனில் (r == 0)
                        நிறுத்து;

                எனில் (r == s && strneq(p, q, r))
                        தொடர்; /* common component. Check next. */

                g = strndup(p, r);
                எனில் (!g)
                        திரும்பு -ENOMEM;

                எனில் (!string_is_glob(g))
                        நிறுத்து;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                எனில் (!h)
                        திரும்பு -ENOMEM;

                r = fnmatch(g, h, 0);
                எனில் (r == FNM_NOMATCH)
                        நிறுத்து;
                எனில் (r != 0) /* Failure to process pattern? */
                        திரும்பு -EINVAL;
        }

        /* The pattern does not match the prefix. */
        எனில் (ret)
                *ret = வெற்று;
        திரும்பு false;
}

#எனில் HAVE_SPLIT_BIN
நிலையான bool dir_is_split(மாறிலி எழுத்து *a, மாறிலி எழுத்து *b) {
        முழு r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        எனில் (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                திரும்பு true;
        }
        திரும்பு r == 0;
}
#endif

மாறிலி எழுத்து* default_PATH(வெறுமை) {
#எனில் HAVE_SPLIT_BIN
        நிலையான மாறிலி எழுத்து *default_path = வெற்று;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        எனில் (default_path)
                திரும்பு default_path;

        எனில் (dir_is_split("/usr/sbin", "/usr/bin"))
                திரும்பு (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        எனில் (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                திரும்பு (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        திரும்பு (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#இல்லையெனில்
        திரும்பு DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
