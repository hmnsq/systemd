/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>

#include "alloc-util.h"
#include "ansi-color.h"
#include "chase.h"
#include "dirent-util.h"
#include "env-file.h"
#include "errno-util.h"
#include "escape.h"
#include "fd-util.h"
#include "fs-util.h"
#include "glyph-util.h"
#include "log.h"
#include "os-util.h"
#include "path-util.h"
#include "stat-util.h"
#include "string-table.h"
#include "string-util.h"
#include "strv.h"
#include "time-util.h"
#include "utf8.h"
#include "xattr-util.h"

static const char* const image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
static const struct {
        const char *release_file_directory;
        const char *release_file_path_prefix;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_path_prefix = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_path_prefix = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(const char *s) {
        如果 (!filename_is_valid(s))
                返回 false;

        如果 (string_has_cc(s, NULL))
                返回 false;

        如果 (!utf8_is_valid(s))
                返回 false;

        /* Temporary files for atomically creating new files */
        如果 (startswith(s, ".#"))
                返回 false;

        返回 true;
}

整数 path_extract_image_name(const char *path, char **ret) {
        _cleanup_free_ char *fn = NULL;
        整数 r;

        assert(path);
        assert(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        如果 (r < 0)
                返回 r;
        如果 (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                char *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                如果 (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        如果 (!image_name_is_valid(fn))
                返回 -EINVAL;

        *ret = TAKE_PTR(fn);
        返回 0;
}

整数 path_is_extension_tree(ImageClass image_class, const char *path, const char *extension, bool relax_extension_release_check) {
        整数 r;

        assert(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        如果 (r < 0)
                返回 r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, NULL, NULL);
        如果 (r == -ENOENT) /* We got nothing */
                返回 0;
        如果 (r < 0)
                返回 r;

        返回 1;
}

整数 fd_is_os_tree(整数 fd) {
        整数 r;

        assert(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ NULL,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ NULL,
                        /* ret_fd= */ NULL);
        如果 (r == -ENOENT)
                返回 false;
        如果 (r < 0)
                返回 r;

        返回 true;
}

static 整数 extension_release_strict_xattr_value(整数 extension_release_fd, const char *extension_release_dir_path, const char *filename) {
        整数 r;

        assert(extension_release_fd >= 0);
        assert(extension_release_dir_path);
        assert(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ NULL, "user.extension-release.strict", /* at_flags= */ 0);
        如果 (ERRNO_IS_NEG_XATTR_ABSENT(r))
                返回 log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        如果 (r < 0)
                返回 log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        如果 (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                返回 true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        返回 false;
}

整数 open_os_release_at(整数 rfd, char **ret_path, 整数 *ret_fd) {
        const char *e;
        整数 r;

        assert(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        如果 (e)
                返回 chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                如果 (r != -ENOENT)
                        返回 r;
        }

        返回 -ENOENT;
}

整数 open_os_release(const char *root, char **ret_path, 整数 *ret_fd) {
        _cleanup_close_ 整数 rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        整数 r;

        如果 (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                如果 (rfd < 0)
                        返回 -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : NULL, ret_fd ? &fd : NULL);
        如果 (r < 0)
                返回 r;

        如果 (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                如果 (r < 0)
                        返回 r;
        }

        如果 (ret_fd)
                *ret_fd = TAKE_FD(fd);

        返回 0;
}

整数 open_extension_release_at(
                整数 rfd,
                ImageClass image_class,
                const char *extension,
                bool relax_extension_release_check,
                char **ret_path,
                整数 *ret_fd) {

        _cleanup_free_ char *dir_path = NULL, *path_found = NULL;
        _cleanup_close_ 整数 fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = NULL;
        bool found = false;
        const char *p;
        整数 r;

        assert(wildcard_fd_is_valid(rfd));
        assert(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        如果 (!extension)
                返回 open_os_release_at(rfd, ret_path, ret_fd);

        如果 (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                返回 -EINVAL;

        如果 (!image_name_is_valid(extension))
                返回 log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        如果 (r != -ENOENT)
                返回 r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        如果 (r < 0)
                返回 log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, 返回 -errno) {
                _cleanup_close_ 整数 fd = -EBADF;
                const char *image_name;

                如果 (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        continue;

                image_name = startswith(de->d_name, "extension-release.");
                如果 (!image_name)
                        continue;

                如果 (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        continue;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                如果 (fd < 0)
                        返回 log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                如果 (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        continue;
                }

                如果 (!relax_extension_release_check) {
                        _cleanup_free_ char *base_extension = NULL;

                        r = path_extract_image_name(extension, &base_extension);
                        如果 (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                continue;
                        }

                        如果 (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_value(fd, dir_path, image_name) != 0)
                                continue;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                如果 (found)
                        返回 -ENOTUNIQ;

                found = true;

                如果 (ret_fd)
                        fd_found = TAKE_FD(fd);

                如果 (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        如果 (!path_found)
                                返回 -ENOMEM;
                }
        }
        如果 (!found)
                返回 -ENOENT;

        如果 (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        如果 (ret_path)
                *ret_path = TAKE_PTR(path_found);

        返回 0;
}

整数 open_extension_release(
                const char *root,
                ImageClass image_class,
                const char *extension,
                bool relax_extension_release_check,
                char **ret_path,
                整数 *ret_fd) {

        _cleanup_close_ 整数 rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        整数 r;

        如果 (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                如果 (rfd < 0)
                        返回 -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : NULL, ret_fd ? &fd : NULL);
        如果 (r < 0)
                返回 r;

        如果 (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                如果 (r < 0)
                        返回 r;
        }

        如果 (ret_fd)
                *ret_fd = TAKE_FD(fd);

        返回 0;
}

static 整数 parse_extension_release_atv(
                整数 rfd,
                ImageClass image_class,
                const char *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ 整数 fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        整数 r;

        assert(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        如果 (r < 0)
                返回 r;

        返回 parse_env_file_fdv(fd, p, ap);
}

整数 parse_extension_release_at_sentinel(
                整数 rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                const char *extension,
                ...) {

        va_list ap;
        整数 r;

        assert(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        返回 r;
}

整数 parse_extension_release_sentinel(
                const char *root,
                ImageClass image_class,
                bool relax_extension_release_check,
                const char *extension,
                ...) {

        _cleanup_close_ 整数 rfd = XAT_FDROOT;
        va_list ap;
        整数 r;

        如果 (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                如果 (rfd < 0)
                        返回 -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        返回 r;
}

整数 load_os_release_pairs_with_prefix(const char *root, const char *prefix, char ***ret) {
        _cleanup_strv_free_ char **os_release_pairs = NULL, **os_release_pairs_prefixed = NULL;
        整数 r;

        assert(ret);

        r = load_os_release_pairs(root, &os_release_pairs);
        如果 (r < 0)
                返回 r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                char *line;

                /* We strictly return only the four main ID fields and ignore the rest */
                如果 (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        continue;

                ascii_strlower(*p);
                line = strjoin(prefix, *p, "=", *q);
                如果 (!line)
                        返回 -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, line);
                如果 (r < 0)
                        返回 r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        返回 0;
}

整数 load_extension_release_pairs(const char *root, ImageClass image_class, const char *extension, bool relax_extension_release_check, char ***ret) {
        _cleanup_close_ 整数 fd = -EBADF;
        _cleanup_free_ char *p = NULL;
        整数 r;

        r = open_extension_release(root, image_class, extension, relax_extension_release_check, &p, &fd);
        如果 (r < 0)
                返回 r;

        返回 load_env_file_pairs_fd(fd, p, ret);
}

整数 os_release_support_ended(const char *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ char *_support_end_alloc = NULL;
        整数 r;

        如果 (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(NULL,
                                     "SUPPORT_END", &_support_end_alloc);
                如果 (r < 0 && r != -ENOENT)
                        返回 log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        如果 (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                如果 (ret_eol)
                        *ret_eol = USEC_INFINITY;

                返回 false;  /* no end date defined */
        }

        struct tm tm = {};
        const char *k = strptime(support_end, "%Y-%m-%d", &tm);
        如果 (!k || *k)
                返回 log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        如果 (r < 0)
                返回 log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        如果 (ret_eol)
                *ret_eol = eol;

        返回 now(CLOCK_REALTIME) > eol;
}

const char* os_release_pretty_name(const char *pretty_name, const char *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        返回 empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

char *unescape_fancy_name(char **fancy_name) {
        assert(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ char *unescaped_fancy_name = NULL;

        如果 (isempty(*fancy_name))
                goto clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        如果 (n < 0) {
                log_debug_errno((整数) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                goto clear;
        }

        如果 (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                goto clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        返回 *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        返回 NULL;
}

bool use_fancy_name(const char *fancy_name) {

        /* Decides whether to show the specified fancy name */

        如果 (isempty(fancy_name))
                返回 false;

        如果 (!colors_enabled())
                返回 false;

        返回 emoji_enabled() || ascii_is_valid(fancy_name);
}
