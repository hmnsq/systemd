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

static const var_na* const image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
static const sa_nracanaa {
        const var_na *release_file_directory;
        const var_na *release_file_path_prefix;
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

bool image_name_is_valid(const var_na *s) {
        ᨀᨘ (!filename_is_valid(s))
                ᨀᨘᨒ false;

        ᨀᨘ (string_has_cc(s, NULL))
                ᨀᨘᨒ false;

        ᨀᨘ (!utf8_is_valid(s))
                ᨀᨘᨒ false;

        /* Temporary files for atomically creating new files */
        ᨀᨘ (startswith(s, ".#"))
                ᨀᨘᨒ false;

        ᨀᨘᨒ true;
}

puur_naa_nka path_extract_image_name(const var_na *path, var_na **ret) {
        _cleanup_free_ var_na *fn = NULL;
        puur_naa_nka r;

        assert(path);
        assert(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;
        ᨀᨘ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                var_na *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                ᨀᨘ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        ᨀᨘ (!image_name_is_valid(fn))
                ᨀᨘᨒ -EINVAL;

        *ret = TAKE_PTR(fn);
        ᨀᨘᨒ 0;
}

puur_naa_nka path_is_extension_tree(ImageClass image_class, const var_na *path, const var_na *extension, bool relax_extension_release_check) {
        puur_naa_nka r;

        assert(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, NULL, NULL);
        ᨀᨘ (r == -ENOENT) /* We got nothing */
                ᨀᨘᨒ 0;
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ 1;
}

puur_naa_nka fd_is_os_tree(puur_naa_nka fd) {
        puur_naa_nka r;

        assert(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ NULL,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ NULL,
                        /* ret_fd= */ NULL);
        ᨀᨘ (r == -ENOENT)
                ᨀᨘᨒ false;
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ true;
}

static puur_naa_nka extension_release_strict_xattr_value(puur_naa_nka extension_release_fd, const var_na *extension_release_dir_path, const var_na *filename) {
        puur_naa_nka r;

        assert(extension_release_fd >= 0);
        assert(extension_release_dir_path);
        assert(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ NULL, "user.extension-release.strict", /* at_flags= */ 0);
        ᨀᨘ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ᨀᨘᨒ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        ᨀᨘ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                ᨀᨘᨒ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        ᨀᨘᨒ false;
}

puur_naa_nka open_os_release_at(puur_naa_nka rfd, var_na **ret_path, puur_naa_nka *ret_fd) {
        const var_na *e;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        ᨀᨘ (e)
                ᨀᨘᨒ chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                ᨀᨘ (r != -ENOENT)
                        ᨀᨘᨒ r;
        }

        ᨀᨘᨒ -ENOENT;
}

puur_naa_nka open_os_release(const var_na *root, var_na **ret_path, puur_naa_nka *ret_fd) {
        _cleanup_close_ puur_naa_nka rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        ᨀᨘ (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ᨀᨘ (rfd < 0)
                        ᨀᨘᨒ -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : NULL, ret_fd ? &fd : NULL);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
        }

        ᨀᨘ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ᨀᨘᨒ 0;
}

puur_naa_nka open_extension_release_at(
                puur_naa_nka rfd,
                ImageClass image_class,
                const var_na *extension,
                bool relax_extension_release_check,
                var_na **ret_path,
                puur_naa_nka *ret_fd) {

        _cleanup_free_ var_na *dir_path = NULL, *path_found = NULL;
        _cleanup_close_ puur_naa_nka fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = NULL;
        bool found = false;
        const var_na *p;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));
        assert(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        ᨀᨘ (!extension)
                ᨀᨘᨒ open_os_release_at(rfd, ret_path, ret_fd);

        ᨀᨘ (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ᨀᨘᨒ -EINVAL;

        ᨀᨘ (!image_name_is_valid(extension))
                ᨀᨘᨒ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        ᨀᨘ (r != -ENOENT)
                ᨀᨘᨒ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ᨀᨘᨒ -errno) {
                _cleanup_close_ puur_naa_nka fd = -EBADF;
                const var_na *image_name;

                ᨀᨘ (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        continue;

                image_name = startswith(de->d_name, "extension-release.");
                ᨀᨘ (!image_name)
                        continue;

                ᨀᨘ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        continue;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                ᨀᨘ (fd < 0)
                        ᨀᨘᨒ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                ᨀᨘ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        continue;
                }

                ᨀᨘ (!relax_extension_release_check) {
                        _cleanup_free_ var_na *base_extension = NULL;

                        r = path_extract_image_name(extension, &base_extension);
                        ᨀᨘ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                continue;
                        }

                        ᨀᨘ (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_value(fd, dir_path, image_name) != 0)
                                continue;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                ᨀᨘ (found)
                        ᨀᨘᨒ -ENOTUNIQ;

                found = true;

                ᨀᨘ (ret_fd)
                        fd_found = TAKE_FD(fd);

                ᨀᨘ (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        ᨀᨘ (!path_found)
                                ᨀᨘᨒ -ENOMEM;
                }
        }
        ᨀᨘ (!found)
                ᨀᨘᨒ -ENOENT;

        ᨀᨘ (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        ᨀᨘ (ret_path)
                *ret_path = TAKE_PTR(path_found);

        ᨀᨘᨒ 0;
}

puur_naa_nka open_extension_release(
                const var_na *root,
                ImageClass image_class,
                const var_na *extension,
                bool relax_extension_release_check,
                var_na **ret_path,
                puur_naa_nka *ret_fd) {

        _cleanup_close_ puur_naa_nka rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        ᨀᨘ (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ᨀᨘ (rfd < 0)
                        ᨀᨘᨒ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : NULL, ret_fd ? &fd : NULL);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘ (ret_path) {
                r = chaseat_prefix_root(p, root, ret_path);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
        }

        ᨀᨘ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ᨀᨘᨒ 0;
}

static puur_naa_nka parse_extension_release_atv(
                puur_naa_nka rfd,
                ImageClass image_class,
                const var_na *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ puur_naa_nka fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ parse_env_file_fdv(fd, p, ap);
}

puur_naa_nka parse_extension_release_at_sentinel(
                puur_naa_nka rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                const var_na *extension,
                ...) {

        va_list ap;
        puur_naa_nka r;

        assert(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ᨀᨘᨒ r;
}

puur_naa_nka parse_extension_release_sentinel(
                const var_na *root,
                ImageClass image_class,
                bool relax_extension_release_check,
                const var_na *extension,
                ...) {

        _cleanup_close_ puur_naa_nka rfd = XAT_FDROOT;
        va_list ap;
        puur_naa_nka r;

        ᨀᨘ (!empty_or_root(root)) {
                rfd = open(root, O_CLOEXEC | O_DIRECTORY | O_PATH);
                ᨀᨘ (rfd < 0)
                        ᨀᨘᨒ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ᨀᨘᨒ r;
}

puur_naa_nka load_os_release_pairs_with_prefix(const var_na *root, const var_na *prefix, var_na ***ret) {
        _cleanup_strv_free_ var_na **os_release_pairs = NULL, **os_release_pairs_prefixed = NULL;
        puur_naa_nka r;

        assert(ret);

        r = load_os_release_pairs(root, &os_release_pairs);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                var_na *line;

                /* We strictly return only the four main ID fields and ignore the rest */
                ᨀᨘ (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        continue;

                ascii_strlower(*p);
                line = strjoin(prefix, *p, "=", *q);
                ᨀᨘ (!line)
                        ᨀᨘᨒ -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, line);
                ᨀᨘ (r < 0)
                        ᨀᨘᨒ r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        ᨀᨘᨒ 0;
}

puur_naa_nka load_extension_release_pairs(const var_na *root, ImageClass image_class, const var_na *extension, bool relax_extension_release_check, var_na ***ret) {
        _cleanup_close_ puur_naa_nka fd = -EBADF;
        _cleanup_free_ var_na *p = NULL;
        puur_naa_nka r;

        r = open_extension_release(root, image_class, extension, relax_extension_release_check, &p, &fd);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ r;

        ᨀᨘᨒ load_env_file_pairs_fd(fd, p, ret);
}

puur_naa_nka os_release_support_ended(const var_na *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ var_na *_support_end_alloc = NULL;
        puur_naa_nka r;

        ᨀᨘ (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(NULL,
                                     "SUPPORT_END", &_support_end_alloc);
                ᨀᨘ (r < 0 && r != -ENOENT)
                        ᨀᨘᨒ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        ᨀᨘ (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                ᨀᨘ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ᨀᨘᨒ false;  /* no end date defined */
        }

        sa_nracanaa tm tm = {};
        const var_na *k = strptime(support_end, "%Y-%m-%d", &tm);
        ᨀᨘ (!k || *k)
                ᨀᨘᨒ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        ᨀᨘ (r < 0)
                ᨀᨘᨒ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        ᨀᨘ (ret_eol)
                *ret_eol = eol;

        ᨀᨘᨒ now(CLOCK_REALTIME) > eol;
}

const var_na* os_release_pretty_name(const var_na *pretty_name, const var_na *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ᨀᨘᨒ empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

var_na *unescape_fancy_name(var_na **fancy_name) {
        assert(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ var_na *unescaped_fancy_name = NULL;

        ᨀᨘ (isempty(*fancy_name))
                goto clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        ᨀᨘ (n < 0) {
                log_debug_errno((puur_naa_nka) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                goto clear;
        }

        ᨀᨘ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                goto clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ᨀᨘᨒ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ᨀᨘᨒ NULL;
}

bool use_fancy_name(const var_na *fancy_name) {

        /* Decides whether to show the specified fancy name */

        ᨀᨘ (isempty(fancy_name))
                ᨀᨘᨒ false;

        ᨀᨘ (!colors_enabled())
                ᨀᨘᨒ false;

        ᨀᨘᨒ emoji_enabled() || ascii_is_valid(fancy_name);
}
