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

স্থির ধ্রুবক অক্ষর* ধ্রুবক image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
স্থির ধ্রুবক কাঠামো {
        ধ্রুবক অক্ষর *release_file_directory;
        ধ্রুবক অক্ষর *release_file_path_prefix;
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

bool image_name_is_valid(ধ্রুবক অক্ষর *s) {
        যদি (!filename_is_valid(s))
                ফেরত false;

        যদি (string_has_cc(s, ফাঁকা))
                ফেরত false;

        যদি (!utf8_is_valid(s))
                ফেরত false;

        /* Temporary files for atomically creating new files */
        যদি (startswith(s, ".#"))
                ফেরত false;

        ফেরত true;
}

পূর্ণসংখ্যা path_extract_image_name(ধ্রুবক অক্ষর *path, অক্ষর **ret) {
        _cleanup_free_ অক্ষর *fn = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);
        প্রতিজ্ঞা(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        যদি (r < 0)
                ফেরত r;
        যদি (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                অক্ষর *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                যদি (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        যদি (!image_name_is_valid(fn))
                ফেরত -EINVAL;

        *ret = TAKE_PTR(fn);
        ফেরত 0;
}

পূর্ণসংখ্যা path_is_extension_বৃক্ষ(ImageClass image_class, ধ্রুবক অক্ষর *path, ধ্রুবক অক্ষর *extension, bool relax_extension_release_check) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        যদি (r < 0)
                ফেরত r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, ফাঁকা, ফাঁকা);
        যদি (r == -ENOENT) /* We got nothing */
                ফেরত 0;
        যদি (r < 0)
                ফেরত r;

        ফেরত 1;
}

পূর্ণসংখ্যা fd_is_os_বৃক্ষ(পূর্ণসংখ্যা fd) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ফাঁকা,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ফাঁকা,
                        /* ret_fd= */ ফাঁকা);
        যদি (r == -ENOENT)
                ফেরত false;
        যদি (r < 0)
                ফেরত r;

        ফেরত true;
}

স্থির পূর্ণসংখ্যা extension_release_strict_xattr_মান(পূর্ণসংখ্যা extension_release_fd, ধ্রুবক অক্ষর *extension_release_dir_path, ধ্রুবক অক্ষর *filename) {
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(extension_release_fd >= 0);
        প্রতিজ্ঞা(extension_release_dir_path);
        প্রতিজ্ঞা(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ফাঁকা, "user.extension-release.strict", /* at_flags= */ 0);
        যদি (ERRNO_IS_NEG_XATTR_ABSENT(r))
                ফেরত log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        যদি (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                ফেরত true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        ফেরত false;
}

পূর্ণসংখ্যা open_os_release_at(পূর্ণসংখ্যা rfd, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd) {
        ধ্রুবক অক্ষর *e;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        যদি (e)
                ফেরত chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                যদি (r != -ENOENT)
                        ফেরত r;
        }

        ফেরত -ENOENT;
}

পূর্ণসংখ্যা open_os_release(ধ্রুবক অক্ষর *মূল, অক্ষর **ret_path, পূর্ণসংখ্যা *ret_fd) {
        _cleanup_close_ পূর্ণসংখ্যা rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        ফেরত -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : ফাঁকা, ret_fd ? &fd : ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path) {
                r = chaseat_prefix_মূল(p, মূল, ret_path);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ফেরত 0;
}

পূর্ণসংখ্যা open_extension_release_at(
                পূর্ণসংখ্যা rfd,
                ImageClass image_class,
                ধ্রুবক অক্ষর *extension,
                bool relax_extension_release_check,
                অক্ষর **ret_path,
                পূর্ণসংখ্যা *ret_fd) {

        _cleanup_free_ অক্ষর *dir_path = ফাঁকা, *path_found = ফাঁকা;
        _cleanup_close_ পূর্ণসংখ্যা fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = ফাঁকা;
        bool found = false;
        ধ্রুবক অক্ষর *p;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));
        প্রতিজ্ঞা(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        যদি (!extension)
                ফেরত open_os_release_at(rfd, ret_path, ret_fd);

        যদি (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                ফেরত -EINVAL;

        যদি (!image_name_is_valid(extension))
                ফেরত log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        যদি (r != -ENOENT)
                ফেরত r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        যদি (r < 0)
                ফেরত log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, ফেরত -errno) {
                _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
                ধ্রুবক অক্ষর *image_name;

                যদি (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        চালু;

                image_name = startswith(de->d_name, "extension-release.");
                যদি (!image_name)
                        চালু;

                যদি (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        চালু;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                যদি (fd < 0)
                        ফেরত log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                যদি (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        চালু;
                }

                যদি (!relax_extension_release_check) {
                        _cleanup_free_ অক্ষর *base_extension = ফাঁকা;

                        r = path_extract_image_name(extension, &base_extension);
                        যদি (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                চালু;
                        }

                        যদি (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_মান(fd, dir_path, image_name) != 0)
                                চালু;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                যদি (found)
                        ফেরত -ENOTUNIQ;

                found = true;

                যদি (ret_fd)
                        fd_found = TAKE_FD(fd);

                যদি (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        যদি (!path_found)
                                ফেরত -ENOMEM;
                }
        }
        যদি (!found)
                ফেরত -ENOENT;

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        যদি (ret_path)
                *ret_path = TAKE_PTR(path_found);

        ফেরত 0;
}

পূর্ণসংখ্যা open_extension_release(
                ধ্রুবক অক্ষর *মূল,
                ImageClass image_class,
                ধ্রুবক অক্ষর *extension,
                bool relax_extension_release_check,
                অক্ষর **ret_path,
                পূর্ণসংখ্যা *ret_fd) {

        _cleanup_close_ পূর্ণসংখ্যা rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        ফেরত -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : ফাঁকা, ret_fd ? &fd : ফাঁকা);
        যদি (r < 0)
                ফেরত r;

        যদি (ret_path) {
                r = chaseat_prefix_মূল(p, মূল, ret_path);
                যদি (r < 0)
                        ফেরত r;
        }

        যদি (ret_fd)
                *ret_fd = TAKE_FD(fd);

        ফেরত 0;
}

স্থির পূর্ণসংখ্যা parse_extension_release_atv(
                পূর্ণসংখ্যা rfd,
                ImageClass image_class,
                ধ্রুবক অক্ষর *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        যদি (r < 0)
                ফেরত r;

        ফেরত parse_env_file_fdv(fd, p, ap);
}

পূর্ণসংখ্যা parse_extension_release_at_sentinel(
                পূর্ণসংখ্যা rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ধ্রুবক অক্ষর *extension,
                ...) {

        va_list ap;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        ফেরত r;
}

পূর্ণসংখ্যা parse_extension_release_sentinel(
                ধ্রুবক অক্ষর *মূল,
                ImageClass image_class,
                bool relax_extension_release_check,
                ধ্রুবক অক্ষর *extension,
                ...) {

        _cleanup_close_ পূর্ণসংখ্যা rfd = XAT_FDROOT;
        va_list ap;
        পূর্ণসংখ্যা r;

        যদি (!empty_or_মূল(মূল)) {
                rfd = open(মূল, O_CLOEXEC | O_DIRECTORY | O_PATH);
                যদি (rfd < 0)
                        ফেরত -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        ফেরত r;
}

পূর্ণসংখ্যা load_os_release_pairs_with_prefix(ধ্রুবক অক্ষর *মূল, ধ্রুবক অক্ষর *prefix, অক্ষর ***ret) {
        _cleanup_strv_free_ অক্ষর **os_release_pairs = ফাঁকা, **os_release_pairs_prefixed = ফাঁকা;
        পূর্ণসংখ্যা r;

        প্রতিজ্ঞা(ret);

        r = load_os_release_pairs(মূল, &os_release_pairs);
        যদি (r < 0)
                ফেরত r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                অক্ষর *সারি;

                /* We strictly return only the four main ID fields and ignore the rest */
                যদি (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        চালু;

                ascii_strlower(*p);
                সারি = strjoin(prefix, *p, "=", *q);
                যদি (!সারি)
                        ফেরত -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, সারি);
                যদি (r < 0)
                        ফেরত r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        ফেরত 0;
}

পূর্ণসংখ্যা load_extension_release_pairs(ধ্রুবক অক্ষর *মূল, ImageClass image_class, ধ্রুবক অক্ষর *extension, bool relax_extension_release_check, অক্ষর ***ret) {
        _cleanup_close_ পূর্ণসংখ্যা fd = -EBADF;
        _cleanup_free_ অক্ষর *p = ফাঁকা;
        পূর্ণসংখ্যা r;

        r = open_extension_release(মূল, image_class, extension, relax_extension_release_check, &p, &fd);
        যদি (r < 0)
                ফেরত r;

        ফেরত load_env_file_pairs_fd(fd, p, ret);
}

পূর্ণসংখ্যা os_release_support_ended(ধ্রুবক অক্ষর *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ অক্ষর *_support_end_alloc = ফাঁকা;
        পূর্ণসংখ্যা r;

        যদি (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ফাঁকা,
                                     "SUPPORT_END", &_support_end_alloc);
                যদি (r < 0 && r != -ENOENT)
                        ফেরত log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        যদি (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                যদি (ret_eol)
                        *ret_eol = USEC_INFINITY;

                ফেরত false;  /* no end date defined */
        }

        কাঠামো tm tm = {};
        ধ্রুবক অক্ষর *k = strptime(support_end, "%Y-%m-%d", &tm);
        যদি (!k || *k)
                ফেরত log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        যদি (r < 0)
                ফেরত log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        যদি (ret_eol)
                *ret_eol = eol;

        ফেরত now(CLOCK_REALTIME) > eol;
}

ধ্রুবক অক্ষর* os_release_pretty_name(ধ্রুবক অক্ষর *pretty_name, ধ্রুবক অক্ষর *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        ফেরত empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

অক্ষর *unescape_fancy_name(অক্ষর **fancy_name) {
        প্রতিজ্ঞা(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ অক্ষর *unescaped_fancy_name = ফাঁকা;

        যদি (isempty(*fancy_name))
                যাও clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        যদি (n < 0) {
                log_debug_errno((পূর্ণসংখ্যা) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                যাও clear;
        }

        যদি (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                যাও clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        ফেরত *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        ফেরত ফাঁকা;
}

bool use_fancy_name(ধ্রুবক অক্ষর *fancy_name) {

        /* Decides whether to show the specified fancy name */

        যদি (isempty(fancy_name))
                ফেরত false;

        যদি (!colors_enabled())
                ফেরত false;

        ফেরত emoji_enabled() || ascii_is_valid(fancy_name);
}
