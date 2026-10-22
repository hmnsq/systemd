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
        ако (!p) /* A NULL pointer is definitely not a path */
                врати false;

        врати strchr(p, '/');
}

цео path_split_and_make_absolute(const char *p, char ***ret) {
        _cleanup_strv_free_ char **l = NULL;
        цео r;

        assert(p);
        assert(ret);

        l = strv_split(p, ":");
        ако (!l)
                врати -ENOMEM;

        r = path_strv_make_absolute_cwd(l);
        ако (r < 0)
                врати r;

        *ret = TAKE_PTR(l);
        врати r;
}

char* path_make_absolute(const char *p, const char *prefix) {
        assert(p);

        /* Makes every item in the list an absolute path by prepending
         * the prefix, if specified and necessary */

        ако (path_is_absolute(p) || isempty(prefix))
                врати strdup(p);

        врати path_join(prefix, p);
}

цео safe_getcwd(char **ret) {
        _cleanup_free_ char *cwd = NULL;

        cwd = get_current_dir_name();
        ако (!cwd)
                врати negative_errno();

        /* Let's make sure the directory is really absolute, to protect us from the logic behind
         * CVE-2018-1000001 */
        ако (cwd[0] != '/')
                врати -ENOMEDIUM;

        ако (ret)
                *ret = TAKE_PTR(cwd);

        врати 0;
}

цео path_make_absolute_cwd(const char *p, char **ret) {
        char *c;
        цео r;

        assert(p);
        assert(ret);

        /* Similar to path_make_absolute(), but prefixes with the
         * current working directory. */

        ако (path_is_absolute(p))
                c = strdup(p);
        инако {
                _cleanup_free_ char *cwd = NULL;

                r = safe_getcwd(&cwd);
                ако (r < 0)
                        врати r;

                c = path_join(cwd, p);
        }
        ако (!c)
                врати -ENOMEM;

        *ret = c;
        врати 0;
}

цео path_make_relative(const char *from, const char *to, char **ret) {
        _cleanup_free_ char *result = NULL;
        unsigned n_parents;
        const char *f, *t;
        цео r, k;
        char *p;

        assert(from);
        assert(to);
        assert(ret);

        /* Strips the common part, and adds ".." elements as necessary. */

        ако (!path_is_absolute(from) || !path_is_absolute(to))
                врати -EINVAL;

        за (;;) {
                r = path_find_first_component(&from, true, &f);
                ако (r < 0)
                        врати r;

                k = path_find_first_component(&to, true, &t);
                ако (k < 0)
                        врати k;

                ако (r == 0) {
                        /* end of 'from' */
                        ако (k == 0) {
                                /* from and to are equivalent. */
                                result = strdup(".");
                                ако (!result)
                                        врати -ENOMEM;
                        } инако {
                                /* 'to' is inside of 'from'. */
                                r = path_simplify_alloc(t, &result);
                                ако (r < 0)
                                        врати r;

                                ако (!path_is_valid(result))
                                        врати -EINVAL;
                        }

                        *ret = TAKE_PTR(result);
                        врати 0;
                }

                ако (r != k || !strneq(f, t, r))
                        break;
        }

        /* If we're here, then "from_dir" has one or more elements that need to
         * be replaced with "..". */

        за (n_parents = 1;; n_parents++) {
                /* If this includes ".." we can't do a simple series of "..". */
                r = path_find_first_component(&from, false, &f);
                ако (r < 0)
                        врати r;
                ако (r == 0)
                        break;
        }

        ако (isempty(t) && n_parents * 3 > PATH_MAX)
                /* PATH_MAX is counted *with* the trailing NUL byte */
                врати -EINVAL;

        result = new(char, n_parents * 3 + !isempty(t) + strlen_ptr(t));
        ако (!result)
                врати -ENOMEM;

        за (p = result; n_parents > 0; n_parents--)
                p = mempcpy(p, "../", 3);

        ако (isempty(t)) {
                /* Remove trailing slash and terminate string. */
                *(--p) = '\0';
                *ret = TAKE_PTR(result);
                врати 0;
        }

        strcpy(p, t);

        path_simplify(result);

        ако (!path_is_valid(result))
                врати -EINVAL;

        *ret = TAKE_PTR(result);
        врати 0;
}

цео path_make_relative_parent(const char *from_child, const char *to, char **ret) {
        _cleanup_free_ char *from = NULL;
        цео r;

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
        ако (r < 0)
                врати r;

        врати path_make_relative(from, to, ret);
}

char* path_startswith_strv(const char *p, char * const *strv) {
        assert(p);

        STRV_FOREACH(s, strv) {
                char *t;

                t = path_startswith(p, *s);
                ако (t)
                        врати t;
        }

        врати NULL;
}

цео path_strv_make_absolute_cwd(char **l) {
        цео r;

        /* Goes through every item in the string list and makes it
         * absolute. This works in place and won't rollback any
         * changes on failure. */

        STRV_FOREACH(s, l) {
                char *t;

                r = path_make_absolute_cwd(*s, &t);
                ако (r < 0)
                        врати r;

                path_simplify(t);
                free_and_replace(*s, t);
        }

        врати 0;
}

char** path_strv_resolve(char **l, const char *root) {
        unsigned k = 0;
        bool enomem = false;
        цео r;

        ако (strv_isempty(l))
                врати l;

        /* Go through every item in the string list and canonicalize the path.
         * This works in place and doesn't roll back any changes on failure. */

        STRV_FOREACH(s, l) {
                _cleanup_free_ char *orig = NULL;
                char *t, *u;

                ако (root) {
                        ако (!path_is_absolute(*s)) {
                                log_debug("Ignoring non-absolute search path '%s'.", *s);
                                free(*s);
                                continue;
                        }

                        orig = *s;
                        t = path_join(root, orig);
                        ако (!t) {
                                enomem = true;
                                continue;
                        }
                } инако
                        t = *s;

                r = chase(t, root, 0, &u, NULL);
                ако (r == -ENOENT) {
                        ако (root) {
                                u = TAKE_PTR(orig);
                                free(t);
                        } инако
                                u = t;
                } инако ако (r < 0) {
                        free(t);

                        ако (r == -ENOMEM)
                                enomem = true;

                        continue;
                } инако ако (root) {
                        char *x;

                        free(t);
                        x = path_startswith(u, root);
                        ако (x) {
                                /* restore the slash if it was lost */
                                ако (!startswith(x, "/"))
                                        *(--x) = '/';

                                t = strdup(x);
                                free(u);
                                ако (!t) {
                                        enomem = true;
                                        continue;
                                }
                                u = t;
                        } инако {
                                /* canonicalized path goes outside of
                                 * prefix, keep the original path instead */
                                free_and_replace(u, orig);
                        }
                } инако
                        free(t);

                l[k++] = u;
        }

        l[k] = NULL;

        ако (enomem)
                врати NULL;

        врати l;
}

char** path_strv_resolve_uniq(char **l, const char *root) {

        ако (strv_isempty(l))
                врати l;

        ако (!path_strv_resolve(l, root))
                врати NULL;

        врати strv_uniq(l);
}

char* skip_leading_slash(const char *p) {
        врати skip_leading_chars(p, "/");
}

char* path_simplify_full(char *path, PathSimplifyFlags flags) {
        bool add_slash = false, keep_trailing_slash, absolute, beginning = true;
        char *f = path;
        цео r;

        /* Removes redundant inner and trailing slashes. Also removes unnecessary dots.
         * Modifies the passed string in-place.
         *
         * ///foo//./bar/.   becomes /foo/bar
         * .//./foo//./bar/. becomes foo/bar
         * /../foo/bar       becomes /foo/bar
         * /../foo/bar/..    becomes /foo/bar/..
         */

        ако (isempty(path))
                врати path;

        keep_trailing_slash = FLAGS_SET(flags, PATH_SIMPLIFY_KEEP_TRAILING_SLASH) && endswith(path, "/");

        absolute = path_is_absolute(path);
        f += absolute;  /* Keep leading /, if present. */

        за (const char *p = f;;) {
                const char *e;

                r = path_find_first_component(&p, true, &e);
                ако (r == 0)
                        break;

                ако (r > 0 && absolute && beginning && path_startswith(e, ".."))
                        /* If we're at the beginning of an absolute path, we can safely skip ".." */
                        continue;

                beginning = false;

                ако (add_slash)
                        *f++ = '/';

                ако (r < 0) {
                        /* if path is invalid, then refuse to simplify the remaining part. */
                        memmove(f, p, strlen(p) + 1);
                        врати path;
                }

                memmove(f, e, r);
                f += r;

                add_slash = true;
        }

        /* Special rule, if we stripped everything, we need a "." for the current directory. */
        ако (f == path)
                *f++ = '.';

        ако (*(f-1) != '/' && keep_trailing_slash)
                *f++ = '/';

        *f = '\0';
        врати path;
}

цео path_simplify_alloc(const char *path, char **ret) {
        assert(ret);

        ако (!path) {
                *ret = NULL;
                врати 0;
        }

        char *t = strdup(path);
        ако (!t)
                врати -ENOMEM;

        *ret = path_simplify(t);
        врати 0;
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

        ако ((path[0] == '/') != (prefix[0] == '/'))
                врати NULL;

        за (;;) {
                const char *p, *q;
                цео m, n;

                m = path_find_first_component(&path, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &p);
                ако (m < 0)
                        врати NULL;

                n = path_find_first_component(&prefix, !FLAGS_SET(flags, PATH_STARTSWITH_REFUSE_DOT_DOT), &q);
                ако (n < 0)
                        врати NULL;

                ако (n == 0) {
                        ако (!p)
                                p = path;

                        ако (FLAGS_SET(flags, PATH_STARTSWITH_RETURN_LEADING_SLASH)) {

                                ако (p <= original_path)
                                        врати NULL;

                                p--;

                                ако (*p != '/')
                                        врати NULL;
                        }

                        врати (char*) p;
                }

                ако (m != n)
                        врати NULL;

                ако (!strneq(p, q, m))
                        врати NULL;
        }
}

цео path_compare(const char *a, const char *b) {
        цео r;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ако (r != 0)
                врати r;

        /* A relative path and an absolute path must not compare as equal.
         * Which one is sorted before the other does not really matter.
         * Here a relative path is ordered before an absolute path. */
        r = CMP(path_is_absolute(a), path_is_absolute(b));
        ако (r != 0)
                врати r;

        за (;;) {
                const char *aa, *bb;
                цео j, k;

                j = path_find_first_component(&a, true, &aa);
                k = path_find_first_component(&b, true, &bb);

                ако (j < 0 || k < 0) {
                        /* When one of paths is invalid, order invalid path after valid one. */
                        r = CMP(j < 0, k < 0);
                        ако (r != 0)
                                врати r;

                        /* fallback to use strcmp() if both paths are invalid. */
                        врати strcmp(a, b);
                }

                /* Order prefixes first: "/foo" before "/foo/bar" */
                ако (j == 0) {
                        ако (k == 0)
                                врати 0;
                        врати -1;
                }
                ако (k == 0)
                        врати 1;

                /* Alphabetical sort: "/foo/aaa" before "/foo/b" */
                r = memcmp(aa, bb, MIN(j, k));
                ако (r != 0)
                        врати r;

                /* Sort "/foo/a" before "/foo/aaa" */
                r = CMP(j, k);
                ако (r != 0)
                        врати r;
        }
}

цео path_compare_filename(const char *a, const char *b) {
        _cleanup_free_ char *fa = NULL, *fb = NULL;
        цео r, j, k;

        /* Order NULL before non-NULL */
        r = CMP(!!a, !!b);
        ако (r != 0)
                врати r;

        j = path_extract_filename(a, &fa);
        k = path_extract_filename(b, &fb);

        /* When one of paths is "." or root, then order it earlier. */
        r = CMP(j != -EADDRNOTAVAIL, k != -EADDRNOTAVAIL);
        ако (r != 0)
                врати r;

        /* When one of paths is invalid (or we get OOM), order invalid path after valid one. */
        r = CMP(j < 0, k < 0);
        ако (r != 0)
                врати r;

        /* fallback to use strcmp() if both paths are invalid. */
        ако (j < 0)
                врати strcmp(a, b);

        врати strcmp(fa, fb);
}

цео path_equal_or_inode_same_full(const char *a, const char *b, цео flags) {
        /* Returns true if paths are of the same entry, false if not, <0 on error. */

        ако (path_equal(a, b))
                врати 1;

        ако (!a || !b)
                врати 0;

        врати inode_same(a, b, flags);
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
        док ((p = va_arg(ap, char*)) != POINTER_MAX) {
                size_t add;

                ако (isempty(p))
                        continue;

                add = 1 + strlen(p);
                ако (sz > SIZE_MAX - add) { /* overflow check */
                        va_end(ap);
                        врати NULL;
                }

                sz += add;
        }
        va_end(ap);

        nx = realloc(x ? *x : NULL, GREEDY_ALLOC_ROUND_UP(sz+1));
        ако (!nx)
                врати NULL;
        ако (x)
                *x = nx;

        ако (old_sz > 0)
                slash = nx[old_sz-1] == '/';
        инако {
                nx[old_sz] = 0;
                slash = true; /* no need to generate a slash anymore */
        }

        q = nx + old_sz;

        va_start(ap, x);
        док ((p = va_arg(ap, char*)) != POINTER_MAX) {
                ако (isempty(p))
                        continue;

                ако (!slash && p[0] != '/')
                        *(q++) = '/';

                q = stpcpy(q, p);
                slash = endswith(p, "/");
        }
        va_end(ap);

        врати nx;
}

цео open_and_check_executable(const char *name, const char *root, char **ret_path, цео *ret_fd) {
        _cleanup_close_ цео fd = -EBADF;
        _cleanup_free_ char *resolved = NULL;
        цео r;

        assert(name);

        /* Function chase() is invoked only when root is not NULL, as using it regardless of
         * root value would alter the behavior of existing callers for example: /bin/sleep would become
         * /usr/bin/sleep when find_executables is called. Hence, this function should be invoked when
         * needed to avoid unforeseen regression or other complicated changes. */
        ако (root) {
                /* prefix root to name in case full paths are not specified */
                r = chase(name, root, CHASE_PREFIX_ROOT, &resolved, &fd);
                ако (r < 0)
                        врати r;

                name = resolved;
        } инако {
                /* We need to use O_PATH because there may be executables for which we have only exec permissions,
                 * but not read (usually suid executables). */
                fd = open(name, O_PATH|O_CLOEXEC);
                ако (fd < 0)
                        врати -errno;
        }

        r = fd_verify_regular(fd);
        ако (r < 0)
                врати r;

        r = access_fd(fd, X_OK);
        ако (r == -ENOSYS)
                /* /proc/ is not mounted. Fall back to access(). */
                r = RET_NERRNO(access(name, X_OK));
        ако (r < 0)
                врати r;

        ако (ret_path) {
                ако (resolved)
                        *ret_path = TAKE_PTR(resolved);
                инако {
                        r = path_make_absolute_cwd(name, ret_path);
                        ако (r < 0)
                                врати r;

                        path_simplify(*ret_path);
                }
        }

        ако (ret_fd)
                *ret_fd = TAKE_FD(fd);

        врати 0;
}

цео find_executable_full(
                const char *name,
                const char *root,
                char * const *exec_search_path,
                bool use_path_envvar,
                char **ret_filename,
                цео *ret_fd) {

        цео last_error = -ENOENT, r = 0;

        assert(name);

        ако (is_path(name))
                врати open_and_check_executable(name, root, ret_filename, ret_fd);

        ако (exec_search_path) {
                STRV_FOREACH(element, exec_search_path) {
                        _cleanup_free_ char *full_path = NULL;

                        ако (!path_is_absolute(*element)) {
                                log_debug("Exec search path '%s' isn't absolute, ignoring.", *element);
                                continue;
                        }

                        full_path = path_join(*element, name);
                        ако (!full_path)
                                врати -ENOMEM;

                        r = open_and_check_executable(full_path, root, ret_filename, ret_fd);
                        ако (r >= 0)
                                врати 0;
                        ако (r != -EACCES)
                                last_error = r;
                }
                врати last_error;
        }

        const char *p = NULL;

        ако (use_path_envvar)
                /* Plain getenv, not secure_getenv, because we want to actually allow the user to pick the
                 * binary. */
                p = getenv("PATH");
        ако (!p)
                p = default_PATH();

        /* Resolve a single-component name to a full path */
        за (;;) {
                _cleanup_free_ char *element = NULL;

                r = extract_first_word(&p, &element, ":", EXTRACT_RELAX|EXTRACT_DONT_COALESCE_SEPARATORS);
                ако (r < 0)
                        врати r;
                ако (r == 0)
                        break;

                ако (!path_is_absolute(element)) {
                        log_debug("Exec search path '%s' isn't absolute, ignoring.", element);
                        continue;
                }

                ако (!path_extend(&element, name))
                        врати -ENOMEM;

                r = open_and_check_executable(element, root, ret_filename, ret_fd);
                ако (r >= 0) /* Found it! */
                        врати 0;
                /* PATH entries which we don't have access to are ignored, as per tradition. */
                ако (r != -EACCES)
                        last_error = r;
        }

        врати last_error;
}

static цео executable_is_good(const char *executable) {
        _cleanup_free_ char *p = NULL, *d = NULL;
        цео r;

        r = find_executable(executable, &p);
        ако (r == -ENOENT)
                врати 0;
        ако (r < 0)
                врати r;

        /* An fsck that is linked to /bin/true is a non-existent fsck */

        r = readlink_malloc(p, &d);
        ако (r == -EINVAL) /* not a symlink */
                врати 1;
        ако (r < 0)
                врати r;

        врати !PATH_IN_SET(d, "true",
                               "/bin/true",
                               "/usr/bin/true",
                               "/dev/null");
}

цео fsck_exists(void) {
        врати executable_is_good("fsck");
}

цео fsck_exists_for_fstype(const char *fstype) {
        const char *checker;
        цео r;

        assert(fstype);

        ако (streq(fstype, "auto"))
                врати -EINVAL;

        r = fsck_exists();
        ако (r <= 0)
                врати r;

        checker = strjoina("fsck.", fstype);
        врати executable_is_good(checker);
}

static const char* skip_slash_or_dot(const char *p) {
        POINTER_MAY_BE_NULL(p);

        за (; !isempty(p); p++) {
                ако (*p == '/')
                        continue;
                ако (startswith(p, "./")) {
                        p++;
                        continue;
                }
                break;
        }
        врати p;
}

цео path_find_first_component(const char **p, bool accept_dot_dot, const char **ret) {
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
        ако (isempty(first)) {
                *p = first;
                ако (ret)
                        *ret = NULL;
                врати 0;
        }
        ако (streq(first, ".")) {
                *p = first + 1;
                ако (ret)
                        *ret = NULL;
                врати 0;
        }

        end_first = strchrnul(first, '/');
        len = end_first - first;

        ако (len > NAME_MAX)
                врати -EINVAL;
        ако (!accept_dot_dot && len == 2 && first[0] == '.' && first[1] == '.')
                врати -EINVAL;

        next = skip_slash_or_dot(end_first);

        *p = next + streq(next, ".");
        ако (ret)
                *ret = first;
        врати len;
}

static const char* skip_slash_or_dot_backward(const char *path, const char *q) {
        assert(path);
        assert(!q || q >= path);

        за (; q; q = PTR_SUB1(q, path)) {
                ако (*q == '/')
                        continue;
                ако (q > path && strneq(q - 1, "/.", 2))
                        continue;
                ако (q == path && *q == '.')
                        continue;
                break;
        }
        врати q;
}

цео path_find_last_component(const char *path, bool accept_dot_dot, const char **next, const char **ret) {
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

        ако (isempty(path)) {
                ако (next)
                        *next = path;
                ако (ret)
                        *ret = NULL;
                врати 0;
        }

        ако (next && *next) {
                ако (*next < path || *next > path + strlen(path))
                        врати -EINVAL;
                ако (*next == path) {
                        ако (ret)
                                *ret = NULL;
                        врати 0;
                }
                ако (!IN_SET(**next, '\0', '/'))
                        врати -EINVAL;
                q = *next - 1;
        } инако
                q = path + strlen(path) - 1;

        q = skip_slash_or_dot_backward(path, q);
        ако (!q) { /* the root directory, "." or "./" */
                ако (next)
                        *next = path;
                ако (ret)
                        *ret = NULL;
                врати 0;
        }

        last_end = q + 1;

        док (q && *q != '/')
                q = PTR_SUB1(q, path);

        last_begin = q ? q + 1 : path;
        len = last_end - last_begin;

        ако (len > NAME_MAX)
                врати -EINVAL;
        ако (!accept_dot_dot && len == 2 && strneq(last_begin, "..", 2))
                врати -EINVAL;

        ако (next) {
                q = skip_slash_or_dot_backward(path, q);
                *next = q ? q + 1 : path;
        }

        ако (ret)
                *ret = last_begin;
        врати len;
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

        ако (!path)
                врати NULL;

        l = k = strlen(path);
        ако (l == 0) /* special case — an empty string */
                врати path;

        док (k > 0 && path[k-1] == '/')
                k--;

        ако (k == 0) /* the root directory */
                врати path + l - 1;

        док (k > 0 && path[k-1] != '/')
                k--;

        врати path + k;
}

цео path_split_prefix_filename(const char *path, char **ret_dir, char **ret_filename) {
        _cleanup_free_ char *d = NULL;
        const char *c, *next = NULL;
        цео r;

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

        ако (isempty(path))
                врати -EINVAL;

        r = path_find_last_component(path, /* accept_dot_dot = */ false, &next, &c);
        ако (r < 0)
                врати r;
        ако (r == 0) /* root directory or "." */
                врати -EADDRNOTAVAIL;

        ако (ret_dir) {
                ако (next == path) {
                        ако (*path != '/') { /* filename only */
                                ако (!ret_filename)
                                        врати -EDESTADDRREQ;
                        } инако {
                                d = strdup("/");
                                ако (!d)
                                        врати -ENOMEM;
                        }
                } инако {
                        d = strndup(path, next - path);
                        ако (!d)
                                врати -ENOMEM;

                        path_simplify(d);

                        ако (!path_is_valid(d))
                                врати -EINVAL;
                }

        } инако ако (!path_is_valid(path))
                /* We didn't validate the dir prefix, hence check if the whole path is valid now */
                врати -EINVAL;

        ако (ret_filename) {
                char *fn = strndup(c, r);
                ако (!fn)
                        врати -ENOMEM;

                *ret_filename = fn;
        }

        ако (ret_dir)
                *ret_dir = TAKE_PTR(d);

        врати strlen(c) > (size_t) r ? O_DIRECTORY : 0;
}

bool filename_part_is_valid(const char *p) {
        const char *e;

        /* Checks f the specified string is OK to be *part* of a filename. This is different from
         * filename_is_valid() as "." and ".." and "" are OK by this call, but not by filename_is_valid(). */

        ако (!p)
                врати false;

        e = strchrnul(p, '/');
        ако (*e != 0)
                врати false;

        ако (e - p > NAME_MAX) /* NAME_MAX is counted *without* the trailing NUL byte */
                врати false;

        врати true;
}

bool filename_is_valid(const char *p) {

        ако (isempty(p))
                врати false;

        ако (dot_or_dot_dot(p)) /* Yes, in this context we consider "." and ".." invalid */
                врати false;

        врати filename_part_is_valid(p);
}

bool path_is_valid_full(const char *p, bool accept_dot_dot) {
        ако (isempty(p))
                врати false;

        за (const char *e = p;;) {
                цео r;

                r = path_find_first_component(&e, accept_dot_dot, NULL);
                ако (r < 0)
                        врати false;

                ако (e - p >= PATH_MAX) /* Already reached the maximum length for a path? (PATH_MAX is counted
                                        * *with* the trailing NUL byte) */
                        врати false;
                ако (*e == 0)           /* End of string? Yay! */
                        врати true;
        }
}

bool path_is_normalized(const char *p) {
        ако (!path_is_safe(p))
                врати false;

        ако (streq(p, ".") || startswith(p, "./") || endswith(p, "/.") || strstr(p, "/./"))
                врати false;

        ако (strstr(p, "//"))
                врати false;

        врати true;
}

цео file_in_same_dir(const char *path, const char *filename, char **ret) {
        _cleanup_free_ char *b = NULL;
        цео r;

        assert(path);
        assert(filename);
        assert(ret);

        /* This removes the last component of path and appends filename, unless the latter is absolute anyway
         * or the former isn't */

        ако (path_is_absolute(filename))
                b = strdup(filename);
        инако {
                _cleanup_free_ char *dn = NULL;

                r = path_extract_directory(path, &dn);
                ако (r == -EDESTADDRREQ) /* no path prefix */
                        b = strdup(filename);
                инако ако (r < 0)
                        врати r;
                инако
                        b = path_join(dn, filename);
        }
        ако (!b)
                врати -ENOMEM;

        *ret = TAKE_PTR(b);
        врати 0;
}

bool hidden_or_backup_file(const char *filename) {
        assert(filename);

        ако (filename[0] == '.' ||
            STR_IN_SET(filename,
                       "lost+found",
                       "aquota.user",
                       "aquota.group") ||
            endswith(filename, "~"))
                врати true;

        const char *dot = strrchr(filename, '.');
        ако (!dot)
                врати false;

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

        врати STR_IN_SET(dot + 1,
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
        врати !isempty(p);
}

bool valid_device_node_path(const char *path) {

        /* Some superficial checks whether the specified path is a valid device node path, all without
         * looking at the actual device node. */

        ако (!PATH_STARTSWITH_SET(path, "/dev/", "/run/systemd/inaccessible/"))
                врати false;

        ако (endswith(path, "/")) /* can't be a device node if it ends in a slash */
                врати false;

        врати path_is_normalized(path);
}

bool valid_device_allow_pattern(const char *path) {
        assert(path);

        /* Like valid_device_node_path(), but also allows full-subsystem expressions like those accepted by
         * DeviceAllow= and DeviceDeny=. */

        ако (STARTSWITH_SET(path, "block-", "char-"))
                врати true;

        врати valid_device_node_path(path);
}

bool dot_or_dot_dot(const char *path) {
        ако (!path)
                врати false;
        ако (path[0] != '.')
                врати false;
        ако (path[1] == 0)
                врати true;
        ако (path[1] != '.')
                врати false;

        врати path[2] == 0;
}

bool path_implies_directory(const char *path) {

        /* Sometimes, if we look at a path we already know it must refer to a directory, because it is
         * suffixed with a slash, or its last component is "." or ".." */

        ако (!path)
                врати false;

        ако (dot_or_dot_dot(path))
                врати true;

        врати ENDSWITH_SET(path, "/", "/.", "/..");
}

bool empty_or_root(const char *path) {

        /* For operations relative to some root directory, returns true if the specified root directory is
         * redundant, i.e. either / or NULL or the empty string or any equivalent. */

        ако (isempty(path))
                врати true;

        врати path_equal(path, "/");
}

const char* empty_to_root(const char *path) {
        врати isempty(path) ? "/" : path;
}

цео empty_or_root_harder_to_null(const char **path) {
        цео r;

        assert(path);

        /* This nullifies the input path when the path is empty or points to "/". */

        ако (empty_or_root(*path)) {
                *path = NULL;
                врати 0;
        }

        r = path_is_root(*path);
        ако (r < 0)
                врати r;
        ако (r > 0)
                *path = NULL;

        врати 0;
}

bool path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l)
                ако (path_equal(*i, path))
                        врати true;

        врати false;
}

bool prefixed_path_strv_contains(char * const *l, const char *path) {
        assert(path);

        STRV_FOREACH(i, l) {
                const char *j = *i;

                ако (*j == '-')
                        j++;
                ако (*j == '+')
                        j++;

                ако (path_equal(j, path))
                        врати true;
        }

        врати false;
}

цео path_glob_can_match(const char *pattern, const char *prefix, char **ret) {
        assert(pattern);
        assert(prefix);

        за (const char *a = pattern, *b = prefix;;) {
                _cleanup_free_ char *g = NULL, *h = NULL;
                const char *p, *q;
                цео r, s;

                r = path_find_first_component(&a, /* accept_dot_dot= */ false, &p);
                ако (r < 0)
                        врати r;

                s = path_find_first_component(&b, /* accept_dot_dot= */ false, &q);
                ако (s < 0)
                        врати s;

                ако (s == 0) {
                        /* The pattern matches the prefix. */
                        ако (ret) {
                                char *t;

                                t = path_join(prefix, p);
                                ако (!t)
                                        врати -ENOMEM;

                                *ret = t;
                        }
                        врати true;
                }

                ако (r == 0)
                        break;

                ако (r == s && strneq(p, q, r))
                        continue; /* common component. Check next. */

                g = strndup(p, r);
                ако (!g)
                        врати -ENOMEM;

                ако (!string_is_glob(g))
                        break;

                /* We found a glob component. Check if the glob pattern matches the prefix component. */

                h = strndup(q, s);
                ако (!h)
                        врати -ENOMEM;

                r = fnmatch(g, h, 0);
                ако (r == FNM_NOMATCH)
                        break;
                ако (r != 0) /* Failure to process pattern? */
                        врати -EINVAL;
        }

        /* The pattern does not match the prefix. */
        ако (ret)
                *ret = NULL;
        врати false;
}

#ако HAVE_SPLIT_BIN
static bool dir_is_split(const char *a, const char *b) {
        цео r;

        r = inode_same(a, b, AT_NO_AUTOMOUNT);
        ако (r < 0 && r != -ENOENT) {
                log_debug_errno(r, "Failed to compare \"%s\" and \"%s\", assuming split directories: %m", a, b);
                врати true;
        }
        врати r == 0;
}
#endif

const char* default_PATH(void) {
#ако HAVE_SPLIT_BIN
        static const char *default_path = NULL;

        /* Return one of the three sets of paths:
         * a) split /usr/s?bin, /usr/local/sbin doesn't matter.
         * b) merged /usr/s?bin, /usr/sbin is a symlink, but /usr/local/sbin is not,
         * c) fully merged, neither /usr/sbin nor /usr/local/sbin are symlinks,
         *
         * On error the fallback to the safe value with both directories as configured is returned.
         */

        ако (default_path)
                врати default_path;

        ако (dir_is_split("/usr/sbin", "/usr/bin"))
                врати (default_path = DEFAULT_PATH_WITH_FULL_SBIN);  /* a */
        ако (dir_is_split("/usr/local/sbin", "/usr/local/bin"))
                врати (default_path = DEFAULT_PATH_WITH_LOCAL_SBIN); /* b */
        врати (default_path = DEFAULT_PATH_WITHOUT_SBIN);            /* c */
#инако
        врати DEFAULT_PATH_WITHOUT_SBIN;
#endif
}
