/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <fnmatch.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "chase.h"
#include "errno-util.h"
#include "extract-word.h"
#include "fd-util.h"
#include "fs-util.h"
#include "glob-util.h"
#include "log.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-util.h"
#include "strv.h"

bool is_path(const char *p) {
        nếu (!p) /* A NULL pointer is definitely not a path */
                trả_về false;

        trả_về strchr(p, '/');
}

số_nguyên path_split_and_make_absolute(const char *p, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        số_nguyên r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        nếu (!l)
                trả_về -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        nếu (r < 0)
                trả_về r;

        *ret = TAKE_PTR(l);
        trả_về r;
}

char* path_make_absolute(const char *p, const char *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        nếu (path_is_absolute(p) || isempty(prefix))
                trả_về strdup(p);

        trả_về path_join(prefix, p);
}

số_nguyên safe_getcwd(char **ret) {
        _cleanup_free_ char *cwd = NULL;

        cwd = get_current_dir_name();
        nếu (!cwd)
                trả_về negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        nếu (cwd[0] != '/')
                trả_về -ENOMEDIUM;

        nếu (ret)
                *ret = TAKE_PTR(cwd);

        trả_về 0;
}

số_nguyên path_make_absolute_cwd(const char *p, char **ret) {
        char *c;
        số_nguyên r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        nếu (path_is_absolute(p))
                c = strdup(p);
        không_thì {
                _cleanup_free_ char *cwd = NULL;

                r = safe_getcwd(&cwd);
                nếu (r < 0)
                        trả_về r;

                c = path_join(cwd, p);
        }
        nếu (!c)
                trả_về -ENOMEM;

        *ret = c;
        trả_về 0;
}

số_nguyên path_make_relative(const char *from, const char *to, char **ret) {
        _cleanup_free_ char *result = NULL;
        unsigned n_parents;
        const char *f, *t;
        số_nguyên r, k;
        char *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        nếu (!path_is_absolute(from) || !path_is_absolute(to))
                trả_về -EINVAL;

        cho (;;) {
                r = path_find_first_component(&from, true, &f);
                nếu (r < 0)
                        trả_về r;

                k = path_find_first_component(&to, true, &t);
                nếu (k < 0)
                        trả_về k;

                nếu (r == 0) {
                        /* end of 'from' */
                        nếu (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                nếu (!result)
                                        trả_về -ENOMEM;
                        } không_thì {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                nếu (r < 0)
                                        trả_về r;

                                nếu (!path_is_valid(result))
                                        trả_về -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        trả_về 0;
                }

                nếu (r != k || !strneq(f, t, r))
                        break;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        cho (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;
        }

        nếu (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                trả_về -EINVAL;

        result = new(char, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        nếu (!result)
                trả_về -ENOMEM;

        cho (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        nếu (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                trả_về 0;
        }

        strcpy(p, t);

        path_simplify(result);

        nếu (!path_is_valid(result))
                trả_về -EINVAL;

        *ret = TAKE_PTR(result);
        trả_về 0;
}

số_nguyên path_make_relative_parent(const char *from_child, const char *to, char **ret) {
        _cleanup_free_ char *from = NULL;
        số_nguyên r;

        assert(from_child);
        assert(to);
        assert(ret);

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
        nếu (r < 0)
                trả_về r;

        trả_về path_make_relative(from, to, ret);
}

char* path_startswith_strv(const char *p, char * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                char *t;

                t = path_startswith(p, *s);
                nếu (t)
                        trả_về t;
        }

        trả_về NULL;
}

số_nguyên path_strv_make_absolute_cwd(char **l) {
        số_nguyên r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                char *t;

                r = path_make_absolute_cwd(*s, &t);
                nếu (r < 0)
                        trả_về r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        trả_về 0;
}

char** path_strv_resolve(char **l, const char *root) {
        unsigned k = 0;
        bool enomem = false;
        số_nguyên r;

        nếu (strv_isempty(l))
                trả_về l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ char *orig = NULL;
                char *t, *u;

                nếu (root) {
                        nếu (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        nếu (!t) {
                                enomem = true;
                                continue;
                        }
                } không_thì
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                nếu (r == -ENOENT) {
                        nếu (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } không_thì
                                u = t;
                } không_thì nếu (r < 0) {
                        free(t);

                        nếu (r == -ENOMEM)
                                enomem = true;

                        continue;
                } không_thì nếu (root) {
                        char *x;

                        free(t);
                        x = path_startswith(u, root);
                        nếu (x) {
                                /* restore the slash if it was lost */
                                nếu (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                nếu (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } không_thì {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } không_thì
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        nếu (enomem)
                trả_về NULL;

        trả_về l;
}

char** path_strv_resolve_uniq(char **l, const char *root) {

        nếu (strv_isempty(l))
                trả_về l;

        nếu (!path_strv_resolve(l, root))
                trả_về NULL;

        trả_về strv_uniq(l);
}

char* skip_leading_slash(const char *p) {
        trả_về skip_leading_chars(p, "/");
}

char* path_simplify_full(char *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        char *f = path;
        số_nguyên r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        nếu (isempty(path))
                trả_về path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        cho (const char *p = f;;) {
                const char *e;

                r = path_find_first_component(&p, true, &e);
                nếu (r == 0)
                        break;

                nếu (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                nếu (add_slash)
                        *f++ = '/';

                nếu (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        trả_về path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        nếu (f == path)
                *f++ = '.';

        nếu (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        trả_về path;
}

số_nguyên path_simplify_alloc(const char *path, char **ret) {
        assert(ret);

        nếu (!path) {
                *ret = NULL;
                trả_về 0;
        }

        char *t = strdup(path);
        nếu (!t)
                trả_về -ENOMEM;

        *ret = path_simplify(t);
        trả_về 0;
}

char* path_startswith_full(const char *original_path, const char *prefix, PathStartWithFlags flags) {
        assert(original_path);
        assert(prefix);

        /* Returns a pointer to the start of the first component after the parts matched by
         * the prefix, iff
         * - both paths are absolute or both paths are relative,
         * and
         * - each component in prefix in turn matches a component in path at the same position.
         * An empty string will be returned when the prefix and path are equivalent.
         *
         * Returns NULL otherwise.
         */

        const char *path = original_path;

        nếu ((path[0] == '/') != (prefix[0] == '/'))
                trả_về NULL;

        cho (;;) {
                const char *p, *q;
                số_nguyên m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                nếu (m < 0)
                        trả_về NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                nếu (n < 0)
                        trả_về NULL;

                nếu (n == 0) {
                        nếu (!p)
                                p = path;

                        nếu (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                nếu (p <= original_path)
                                        trả_về NULL;

                                p--;

                                nếu (*p != '/')
                                        trả_về NULL;
                        }

                        trả_về (char*) p;
                }

                nếu (m != n)
                        trả_về NULL;

                nếu (!strneq(p, q, m))
                        trả_về NULL;
        }
}

số_nguyên path_compare(const char *a, const char *b) {
        số_nguyên r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        nếu (r != 0)
                trả_về r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        nếu (r != 0)
                trả_về r;

        cho (;;) {
                const char *aa, *bb;
                số_nguyên j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                nếu (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        nếu (r != 0)
                                trả_về r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        trả_về strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                nếu (j == 0) {
                        nếu (k == 0)
                                trả_về 0;
                        trả_về -1;
                }
                nếu (k == 0)
                        trả_về 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                nếu (r != 0)
                        trả_về r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                nếu (r != 0)
                        trả_về r;
        }
}

số_nguyên path_compare_filename(const char *a, const char *b) {
        _cleanup_free_ char *fa = NULL, *fb = NULL;
        số_nguyên r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        nếu (r != 0)
                trả_về r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        nếu (r != 0)
                trả_về r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        nếu (r != 0)
                trả_về r;

        /* fallback to use strcmp() if both paths are invalid. */
        nếu (j < 0)
                trả_về strcmp(a, b);

        trả_về strcmp(fa, fb);
}

số_nguyên path_equal_or_inode_same_full(const char *a, const char *b, số_nguyên flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        nếu (path_equal(a, b))
                trả_về 1;

        nếu (!a || !b)
                trả_về 0;

        trả_về inode_same(a, b, flags);
}

char* path_extend_internal(char **x, ...) {
        size_t sz, old_sz;
        char *q, *nx;
        const char *p;
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
        trong_khi ((p = va_arg(ap, char*)) != POINTER_MAX) {
                size_t add;

                nếu (isempty(p))
                        continue;

                add = 1 + strlen(p);
                nếu (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        trả_về NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        nếu (!nx)
                trả_về NULL;
        nếu (x)
                *x = nx;

        nếu (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        không_thì {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        trong_khi ((p = va_arg(ap, char*)) != POINTER_MAX) {
                nếu (isempty(p))
                        continue;

                nếu (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        trả_về nx;
}

số_nguyên open_and_check_executable(const char *name, const char *root, char **ret_path, số_nguyên *ret_fd) {
        _cleanup_close_ số_nguyên fd = -EBADF;
        _cleanup_free_ char *resolved = NULL;
        số_nguyên r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        nếu (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                nếu (r < 0)
                        trả_về r;

                name = resolved;
        } không_thì {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                nếu (fd < 0)
                        trả_về -errno;
        }

        r = fd_verify_regular(fd);
        nếu (r < 0)
                trả_về r;

        r = access_fd(fd, X_OK);
        nếu (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        nếu (r < 0)
                trả_về r;

        nếu (ret_path) {
                nếu (resolved)
                        *ret_path = TAKE_PTR(resolved);
                không_thì {
                        r = path_make_absolute_cwd(name, ret_path);
                        nếu (r < 0)
                                trả_về r;

                        path_simplify(*ret_path);
                }
        }

        nếu (ret_fd)
                *ret_fd = TAKE_FD(fd);

        trả_về 0;
}

số_nguyên find_executable_full(
                const char *name,
                const char *root,
                char * const *exec_search_path,
                bool use_path_envvar,
                char **ret_filename,
                số_nguyên *ret_fd) {

        số_nguyên last_error = -ENOENT, r = 0;

        assert(name);

        nếu (is_path(name))
                trả_về open_and_check_executable(name, root, ret_filename, ret_fd);

        nếu (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ char *full_path = NULL;

                        nếu (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        nếu (!full_path)
                                trả_về -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        nếu (r >= 0)
                                trả_về 0;
                        nếu (r != -EACCES)
                                last_error = r;
                }
                trả_về last_error;
        }

        const char *p = NULL;

        nếu (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        nếu (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        cho (;;) {
                _cleanup_free_ char *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                nếu (r < 0)
                        trả_về r;
                nếu (r == 0)
                        break;

                nếu (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                nếu (!path_extend(&element, name))
                        trả_về -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                nếu (r >= 0) /* Found it! */
                        trả_về 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                nếu (r != -EACCES)
                        last_error = r;
        }

        trả_về last_error;
}

static số_nguyên executable_is_good(const char *executable) {
        _cleanup_free_ char *p = NULL, *d = NULL;
        số_nguyên r;

        r = find_executable(executable, &p);
        nếu (r == -ENOENT)
                trả_về 0;
        nếu (r < 0)
                trả_về r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        nếu (r == -EINVAL) /* not a symlink */
                trả_về 1;
        nếu (r < 0)
                trả_về r;

        trả_về !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

số_nguyên fsck_exists(void) {
        trả_về executable_is_good("fsck");
}

số_nguyên fsck_exists_for_fstype(const char *fstype) {
        const char *checker;
        số_nguyên r;

        assert(fstype);

        nếu (streq(fstype, "auto"))
                trả_về -EINVAL;

        r = fsck_exists();
        nếu (r <= 0)
                trả_về r;

        checker = strjoina("fsck.", fstype);
        trả_về executable_is_good(checker);
}

static const char* skip_slash_or_dot(const char *p) {
        POINTER_MAY_BE_NULL(p);

        cho (; !isempty(p); p++) {
                nếu (*p == '/')
                        continue;
                nếu (startswith(p, "./")) {
                        p++;
                        continue;
                }
                break;
        }
        trả_về p;
}

số_nguyên path_find_first_component(const char **p, bool accept_dot_dot, const char **ret) {
        const char *q, *first, *end_first, *next;
        size_t len;

        assert(p);

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
        nếu (isempty(first)) {
                *p = first;
                nếu (ret)
                        *ret = NULL;
                trả_về 0;
        }
        nếu (streq(first, ".")) {
                *p = first + 1;
                nếu (ret)
                        *ret = NULL;
                trả_về 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        nếu (len > NAME_MAX)
                trả_về -EINVAL;
        nếu (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                trả_về -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        nếu (ret)
                *ret = first;
        trả_về len;
}

static const char* skip_slash_or_dot_backward(const char *path, const char *q) {
        assert(path);
        assert(!q || q >= path);

        cho (; q; q = PTR_SUB1(q, path)) {
                nếu (*q == '/')
                        continue;
                nếu (q > path && strneq(q - 1, "/.", 2))
                        continue;
                nếu (q == path && *q == '.')
                        continue;
                break;
        }
        trả_về q;
}

số_nguyên path_find_last_component(const char *path, bool accept_dot_dot, const char **next, const char **ret) {
        const char *q, *last_end, *last_begin;
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

        nếu (isempty(path)) {
                nếu (next)
                        *next = path;
                nếu (ret)
                        *ret = NULL;
                trả_về 0;
        }

        nếu (next && *next) {
                nếu (*next < path || *next > path + strlen(path))
                        trả_về -EINVAL;
                nếu (*next == path) {
                        nếu (ret)
                                *ret = NULL;
                        trả_về 0;
                }
                nếu (!IN_SET(**next, '\0', '/'))
                        trả_về -EINVAL;
                q = *next - 1;
        } không_thì
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        nếu (!q) { /* the root directory, "." or "./" */
                nếu (next)
                        *next = path;
                nếu (ret)
                        *ret = NULL;
                trả_về 0;
        }

        last_end = q + 1;

        trong_khi (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        nếu (len > NAME_MAX)
                trả_về -EINVAL;
        nếu (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                trả_về -EINVAL;

        nếu (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        nếu (ret)
                *ret = last_begin;
        trả_về len;
}

const char* last_path_component(const char *path) {

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

        nếu (!path)
                trả_về NULL;

        l = k = strlen(path);
        nếu (l == 0) /* special case — an empty string */
                trả_về path;

        trong_khi (k > 0 && path[k-1] == '/')
                k--;

        nếu (k == 0) /* the root directory */
                trả_về path + l - 1;

        trong_khi (k > 0 && path[k-1] != '/')
                k--;

        trả_về path + k;
}

số_nguyên path_split_prefix_filename(const char *path, char **ret_dir, char **ret_filename) {
        _cleanup_free_ char *d = NULL;
        const char *c, *next = NULL;
        số_nguyên r;

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

        nếu (isempty(path))
                trả_về -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        nếu (r < 0)
                trả_về r;
        nếu (r == 0) /* root directory or "." */
                trả_về -EADDRNOTAVAIL;

        nếu (ret_dir) {
                nếu (next == path) {
                        nếu (*path != '/') { /* filename only */
                                nếu (!ret_filename)
                                        trả_về -EDESTADDRREQ;
                        } không_thì {
                                d = strdup("/");
                                nếu (!d)
                                        trả_về -ENOMEM;
                        }
                } không_thì {
                        d = strndup(path, next - path);
                        nếu (!d)
                                trả_về -ENOMEM;

                        path_simplify(d);

                        nếu (!path_is_valid(d))
                                trả_về -EINVAL;
                }

        } không_thì nếu (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                trả_về -EINVAL;

        nếu (ret_filename) {
                char *fn = strndup(c, r);
                nếu (!fn)
                        trả_về -ENOMEM;

                *ret_filename = fn;
        }

        nếu (ret_dir)
                *ret_dir = TAKE_PTR(d);

        trả_về strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const char *p) {
        const char *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        nếu (!p)
                trả_về false;

        e = strchrnul(p, '/');
        nếu (*e != 0)
                trả_về false;

        nếu (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                trả_về false;

        trả_về true;
}

bool filename_is_valid(const char *p) {

        nếu (isempty(p))
                trả_về false;

        nếu (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                trả_về false;

        trả_về filename_part_is_valid(p);
}

bool path_is_valid_full(const char *p, bool accept_dot_dot) {
        nếu (isempty(p))
                trả_về false;

        cho (const char *e = p;;) {
                số_nguyên r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                nếu (r < 0)
                        trả_về false;

                nếu (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        trả_về false;
                nếu (*e == 0)           /* End of string? Yay! */
                        trả_về true;
        }
}

bool path_is_normalized(const char *p) {
        nếu (!path_is_safe(p))
                trả_về false;

        nếu (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                trả_về false;

        nếu (strstr(p, "//"))
                trả_về false;

        trả_về true;
}

số_nguyên file_in_same_dir(const char *path, const char *filename, char **ret) {
        _cleanup_free_ char *b = NULL;
        số_nguyên r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        nếu (path_is_absolute(filename))
                b = strdup(filename);
        không_thì {
                _cleanup_free_ char *dn = NULL;

                r = path_extract_directory(path, &dn);
                nếu (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                không_thì nếu (r < 0)
                        trả_về r;
                không_thì
                        b = path_join(dn, filename);
        }
        nếu (!b)
                trả_về -ENOMEM;

        *ret = TAKE_PTR(b);
        trả_về 0;
}

bool hidden_or_backup_file(const char *filename) {
        assert(filename);

        nếu (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                trả_về true;

        const char *dot = strrchr(filename, '.');
        nếu (!dot)
                trả_về false;

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

        trả_về STR_IN_SET(dot + 1,
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

bool is_device_path(const char *path) {

        /* Returns true for paths that likely refer to a device, either by path in sysfs or to something in
         * /dev. This accepts any path that starts with /dev/ or /sys/ and has something after that prefix.
         * It does not actually resolve the path.
         *
         * Examples:
         * /dev/sda, /dev/sda/foo, /sys/class, /dev/.., /sys/.., /./dev/foo → yes.
         * /../dev/sda, /dev, /sys, /usr/path, /usr/../dev/sda → no.
         */

        const char *p = PATH_STARTSWITH_SET(ASSERT_PTR(path), "/dev/", "/sys/");
        trả_về !isempty(p);
}

bool valid_device_node_path(const char *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        nếu (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                trả_về false;

        nếu (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                trả_về false;

        trả_về path_is_normalized(path);
}

bool valid_device_allow_pattern(const char *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        nếu (STARTSWITH_SET(path, "block-", "char-"))
                trả_về true;

        trả_về valid_device_node_path(path);
}

bool dot_or_dot_dot(const char *path) {
        nếu (!path)
                trả_về false;
        nếu (path[0] != '.')
                trả_về false;
        nếu (path[1] == 0)
                trả_về true;
        nếu (path[1] != '.')
                trả_về false;

        trả_về path[2] == 0;
}

bool path_implies_directory(const char *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        nếu (!path)
                trả_về false;

        nếu (dot_or_dot_dot(path))
                trả_về true;

        trả_về ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const char *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        nếu (isempty(path))
                trả_về true;

        trả_về path_equal(path, "/");
}

const char* empty_to_root(const char *path) {
        trả_về isempty(path) ? "/" : path;
}

số_nguyên empty_or_root_harder_to_null(const char **path) {
        số_nguyên r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        nếu (empty_or_root(*path)) {
                *path = NULL;
                trả_về 0;
        }

        r = path_is_root(*path);
        nếu (r < 0)
                trả_về r;
        nếu (r > 0)
                *path = NULL;

        trả_về 0;
}

bool path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l)
                nếu (path_equal(*i, path))
                        trả_về true;

        trả_về false;
}

bool prefixed_path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const char *j = *i;

                nếu (*j == '-')
                        j++;
                nếu (*j == '+')
                        j++;

                nếu (path_equal(j, path))
                        trả_về true;
        }

        trả_về false;
}

số_nguyên path_glob_can_match(const char *pattern, const char *prefix, char **ret) {
        assert(pattern);
        assert(prefix);

        cho (const char *a = pattern, *b = prefix;;) {
                _cleanup_free_ char *g = NULL, *h = NULL;
                const char *p, *q;
                số_nguyên r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                nếu (r < 0)
                        trả_về r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                nếu (s < 0)
                        trả_về s;

                nếu (s == 0) {
                        /* The pattern matches the prefix. */
                        nếu (ret) {
                                char *t;

                                t = path_join(prefix, p);
                                nếu (!t)
                                        trả_về -ENOMEM;

                                *ret = t;
                        }
                        trả_về true;
                }

                nếu (r == 0)
                        break;

                nếu (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                nếu (!g)
                        trả_về -ENOMEM;

                nếu (!string_is_glob(g))
                        break;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                nếu (!h)
                        trả_về -ENOMEM;

                r = fnmatch(g, h, 0);
                nếu (r == FNM_NOMATCH)
                        break;
                nếu (r != 0) /* Failure to process pattern? */
                        trả_về -EINVAL;
        }

        /* The pattern does not match the prefix. */
        nếu (ret)
                *ret = NULL;
        trả_về false;
}

#nếu HAVE_SPLIT_BIN
static bool dir_is_split(const char *a, const char *b) {
        số_nguyên r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        nếu (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                trả_về true;
        }
        trả_về r == 0;
}
#endif

const char* default_PATH(void) {
#nếu HAVE_SPLIT_BIN
        static const char *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        nếu (default_path)
                trả_về default_path;

        nếu (dir_is_split("/usr/sbin", "/usr/bin"))
                trả_về (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        nếu (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                trả_về (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        trả_về (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#không_thì
        trả_về DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
