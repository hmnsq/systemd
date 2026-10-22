/* SPDX-License-Identifier: LGPL-2.1-or-later */

#অন্তর্ভাব <stdlib.h>

#অন্তর্ভাব "alloc-util.h"
#অন্তর্ভাব "ansi-color.h"
#অন্তর্ভাব "chase.h"
#অন্তর্ভাব "dirent-util.h"
#অন্তর্ভাব "env-file.h"
#অন্তর্ভাব "errno-util.h"
#অন্তর্ভাব "escape.h"
#অন্তর্ভাব "fd-util.h"
#অন্তর্ভাব "fs-util.h"
#অন্তর্ভাব "glyph-util.h"
#অন্তর্ভাব "log.h"
#অন্তর্ভাব "os-util.h"
#অন্তর্ভাব "path-util.h"
#অন্তর্ভাব "stat-util.h"
#অন্তর্ভাব "string-table.h"
#অন্তর্ভাব "string-util.h"
#অন্তর্ভাব "strv.h"
#অন্তর্ভাব "time-util.h"
#অন্তর্ভাব "utf8.h"
#অন্তর্ভাব "xattr-util.h"

স্থির ধ্রুব বর্ণ* ধ্রুব image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
স্থির ধ্রুব সংরচনা {
        ধ্রুব বর্ণ *release_file_directory;
        ধ্রুব বর্ণ *release_file_path_prefix;
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

bool image_name_is_valid(ধ্রুব বর্ণ *s) {
        যদি (!filename_is_valid(s))
                প্রত্যাগম false;

        যদি (string_has_cc(s, রিক্ত))
                প্রত্যাগম false;

        যদি (!utf8_is_valid(s))
                প্রত্যাগম false;

        /* Temporary files for atomically creating new files */
        যদি (startswith(s, ".#"))
                প্রত্যাগম false;

        প্রত্যাগম true;
}

পূর্ণাঙ্ক path_extract_image_name(ধ্রুব বর্ণ *path, বর্ণ **ret) {
        _cleanup_free_ বর্ণ *fn = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                প্রত্যাগম r;
        যদি (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                বর্ণ *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                যদি (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        যদি (!image_name_is_valid(fn))
                প্রত্যাগম -EINVAL;

        *ret = TAKE_PTR(fn);
        প্রত্যাগম 0;
}

পূর্ণাঙ্ক path_is_extension_বৃক্ষ(ImageClass image_class, ধ্রুব বর্ণ *path, ধ্রুব বর্ণ *extension, bool relax_extension_release_check) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        যদি (r < 0)
                প্রত্যাগম r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, রিক্ত, রিক্ত);
        যদি (r == -ENOENT) /* We got nothing */
                প্রত্যাগম 0;
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম 1;
}

পূর্ণাঙ্ক fd_is_os_বৃক্ষ(পূর্ণাঙ্ক fd) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ রিক্ত,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ রিক্ত,
                        /* ret_fd= */ রিক্ত);
        যদি (r == -ENOENT)
                প্রত্যাগম false;
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম true;
}

স্থির পূর্ণাঙ্ক extension_release_strict_xattr_মান(পূর্ণাঙ্ক extension_release_fd, ধ্রুব বর্ণ *extension_release_dir_path, ধ্রুব বর্ণ *filename) {
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(extension_release_fd >= 0);
        প্রতিজ্ঞা(extension_release_dir_path);
        প্রতিজ্ঞা(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ রিক্ত, "user.extension-release.strict", /* at_flags= */ 0);
        যদি (ERRNO_IS_NEG_XATTR_ABSENT(r))
                প্রত্যাগম log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        যদি (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                প্রত্যাগম true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        প্রত্যাগম false;
}

পূর্ণাঙ্ক open_os_release_at(পূর্ণাঙ্ক rfd, বর্ণ **ret_path, পূর্ণাঙ্ক *ret_fd) {
        ধ্রুব বর্ণ *e;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        যদি (e)
                প্রত্যাগম chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                যদি (r != -ENOENT)
                        প্রত্যাগম r;
        }

        প্রত্যাগম -ENOENT;
}

পূর্ণাঙ্ক open_os_release(ধ্রুব বর্ণ *মূল, বর্ণ **ret_path, পূর্ণাঙ্ক *ret_fd) {
        _cleanup_close_ পূর্ণাঙ্ক rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        প্রত্যাগম -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : রিক্ত, ret_fd ? &fd : রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_path) {
                r = chaseat_prefix_মূল(p, মূল, ret_path);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক open_extension_release_at(
                পূর্ণাঙ্ক rfd,
                ImageClass image_class,
                ধ্রুব বর্ণ *extension,
                bool relax_extension_release_check,
                বর্ণ **ret_path,
                পূর্ণাঙ্ক *ret_fd) {

        _cleanup_free_ বর্ণ *dir_path = রিক্ত, *path_found = রিক্ত;
        _cleanup_close_ পূর্ণাঙ্ক fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = রিক্ত;
        bool found = false;
        ধ্রুব বর্ণ *p;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        যদি (!extension)
                প্রত্যাগম open_os_release_at(rfd, ret_path, ret_fd);

        যদি (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                প্রত্যাগম -EINVAL;

        যদি (!image_name_is_valid(extension))
                প্রত্যাগম log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        যদি (r != -ENOENT)
                প্রত্যাগম r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        যদি (r < 0)
                প্রত্যাগম log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, প্রত্যাগম -errno) {
                _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
                ধ্রুব বর্ণ *image_name;

                যদি (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        অনুবর্ত;

                image_name = startswith(de->d_name, "extension-release.");
                যদি (!image_name)
                        অনুবর্ত;

                যদি (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        অনুবর্ত;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                যদি (fd < 0)
                        প্রত্যাগম log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                যদি (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        অনুবর্ত;
                }

                যদি (!relax_extension_release_check) {
                        _cleanup_free_ বর্ণ *base_extension = রিক্ত;

                        r = path_extract_image_name(extension, &base_extension);
                        যদি (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                অনুবর্ত;
                        }

                        যদি (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_মান(fd, dir_path, image_name) != 0)
                                অনুবর্ত;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                যদি (found)
                        প্রত্যাগম -ENOTUNIQ;

                found = true;

                যদি (ret_fd)
                        fd_found = TAKE_FD(fd);

                যদি (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        যদি (!path_found)
                                প্রত্যাগম -ENOMEM;
                }
        }
        যদি (!found)
                প্রত্যাগম -ENOENT;

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        যদি (ret_path)
                *ret_path = TAKE_PTR(path_found);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক open_extension_release(
                ধ্রুব বর্ণ *মূল,
                ImageClass image_class,
                ধ্রুব বর্ণ *extension,
                bool relax_extension_release_check,
                বর্ণ **ret_path,
                পূর্ণাঙ্ক *ret_fd) {

        _cleanup_close_ পূর্ণাঙ্ক rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        প্রত্যাগম -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : রিক্ত, ret_fd ? &fd : রিক্ত);
        যদি (r < 0)
                প্রত্যাগম r;

        যদি (ret_path) {
                r = chaseat_prefix_মূল(p, মূল, ret_path);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        প্রত্যাগম 0;
}

স্থির পূর্ণাঙ্ক parse_extension_release_atv(
                পূর্ণাঙ্ক rfd,
                ImageClass image_class,
                ধ্রুব বর্ণ *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম parse_env_file_fdv(fd, p, ap);
}

পূর্ণাঙ্ক parse_extension_release_at_sentinel(
                পূর্ণাঙ্ক rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ধ্রুব বর্ণ *extension,
                ...) {

        va_list ap;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        প্রত্যাগম r;
}

পূর্ণাঙ্ক parse_extension_release_sentinel(
                ধ্রুব বর্ণ *মূল,
                ImageClass image_class,
                bool relax_extension_release_check,
                ধ্রুব বর্ণ *extension,
                ...) {

        _cleanup_close_ পূর্ণাঙ্ক rfd = XAT_FDROOT;
        va_list ap;
        পূর্ণাঙ্ক r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        প্রত্যাগম -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        প্রত্যাগম r;
}

পূর্ণাঙ্ক load_os_release_pairs_with_prefix(ধ্রুব বর্ণ *মূল, ধ্রুব বর্ণ *prefix, বর্ণ ***ret) {
        _cleanup_strv_free_ বর্ণ **os_release_pairs = রিক্ত, **os_release_pairs_prefixed = রিক্ত;
        পূর্ণাঙ্ক r;

        প্রতিজ্ঞা(ret);

        r = load_os_release_pairs(মূল, &os_release_pairs);
        যদি (r < 0)
                প্রত্যাগম r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                বর্ণ *সারি;

                /* We strictly return only the four main ID fields and ignore the rest */
                যদি (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        অনুবর্ত;

                ascii_strlower(*p);
                সারি = strjoin(prefix, *p, "=", *q);
                যদি (!সারি)
                        প্রত্যাগম -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, সারি);
                যদি (r < 0)
                        প্রত্যাগম r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        প্রত্যাগম 0;
}

পূর্ণাঙ্ক load_extension_release_pairs(ধ্রুব বর্ণ *মূল, ImageClass image_class, ধ্রুব বর্ণ *extension, bool relax_extension_release_check, বর্ণ ***ret) {
        _cleanup_close_ পূর্ণাঙ্ক fd = -EBADF;
        _cleanup_free_ বর্ণ *p = রিক্ত;
        পূর্ণাঙ্ক r;

        r = open_extension_release(মূল, image_class, extension, relax_extension_release_check, &p, &fd);
        যদি (r < 0)
                প্রত্যাগম r;

        প্রত্যাগম load_env_file_pairs_fd(fd, p, ret);
}

পূর্ণাঙ্ক os_release_support_ended(ধ্রুব বর্ণ *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ বর্ণ *_support_end_alloc = রিক্ত;
        পূর্ণাঙ্ক r;

        যদি (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(রিক্ত,
                                     "SUPPORT_END", &_support_end_alloc);
                যদি (r < 0 && r != -ENOENT)
                        প্রত্যাগম log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        যদি (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                যদি (ret_eol)
                        *ret_eol = USEC_INFINITY;

                প্রত্যাগম false;  /* no end date defined */
        }

        সংরচনা tm tm = {};
        ধ্রুব বর্ণ *k = strptime(support_end, "%Y-%m-%d", &tm);
        যদি (!k || *k)
                প্রত্যাগম log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        যদি (r < 0)
                প্রত্যাগম log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        যদি (ret_eol)
                *ret_eol = eol;

        প্রত্যাগম now(CLOCK_REALTIME) > eol;
}

ধ্রুব বর্ণ* os_release_pretty_name(ধ্রুব বর্ণ *pretty_name, ধ্রুব বর্ণ *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        প্রত্যাগম empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

বর্ণ *unescape_fancy_name(বর্ণ **fancy_name) {
        প্রতিজ্ঞা(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ বর্ণ *unescaped_fancy_name = রিক্ত;

        যদি (isempty(*fancy_name))
                গচ্ছ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        যদি (n < 0) {
                log_debug_errno((পূর্ণাঙ্ক) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                গচ্ছ clear;
        }

        যদি (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                গচ্ছ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        প্রত্যাগম *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        প্রত্যাগম রিক্ত;
}

bool use_fancy_name(ধ্রুব বর্ণ *fancy_name) {

        /* Decides whether to show the specified fancy name */

        যদি (isempty(fancy_name))
                প্রত্যাগম false;

        যদি (!colors_enabled())
                প্রত্যাগম false;

        প্রত্যাগম emoji_enabled() || ascii_is_valid(fancy_name);
}
