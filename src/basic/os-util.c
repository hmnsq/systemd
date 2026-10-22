/* SPDX-License-Identifier: LGPL-2.1-or-later */

#شامل <stdlib.h>

#شامل "alloc-util.h"
#شامل "ansi-color.h"
#شامل "chase.h"
#شامل "dirent-util.h"
#شامل "env-file.h"
#شامل "errno-util.h"
#شامل "escape.h"
#شامل "fd-util.h"
#شامل "fs-util.h"
#شامل "glyph-util.h"
#شامل "log.h"
#شامل "os-util.h"
#شامل "path-util.h"
#شامل "stat-util.h"
#شامل "string-table.h"
#شامل "string-util.h"
#شامل "strv.h"
#شامل "time-util.h"
#شامل "utf8.h"
#شامل "xattr-util.h"

ایستا ثابت نویسه* ثابت image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ایستا ثابت ساختار {
        ثابت نویسه *release_file_directory;
        ثابت نویسه *release_file_path_prefix;
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

bool image_name_is_valid(ثابت نویسه *s) {
        اگر (!filename_is_valid(s))
                بازگشت false;

        اگر (string_has_cc(s, پوچ))
                بازگشت false;

        اگر (!utf8_is_valid(s))
                بازگشت false;

        /* Temporary files for atomically creating new files */
        اگر (startswith(s, ".#"))
                بازگشت false;

        بازگشت true;
}

صحیح path_extract_image_name(ثابت نویسه *path, نویسه **ret) {
        _cleanup_free_ نویسه *fn = پوچ;
        صحیح r;

        أكد(path);
        أكد(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        اگر (r < 0)
                بازگشت r;
        اگر (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                نویسه *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                اگر (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        اگر (!image_name_is_valid(fn))
                بازگشت -EINVAL;

        *ret = TAKE_PTR(fn);
        بازگشت 0;
}

صحیح path_is_extension_درخت(ImageClass image_class, ثابت نویسه *path, ثابت نویسه *extension, bool relax_extension_release_check) {
        صحیح r;

        أكد(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        اگر (r < 0)
                بازگشت r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, پوچ, پوچ);
        اگر (r == -ENOENT) /* We got nothing */
                بازگشت 0;
        اگر (r < 0)
                بازگشت r;

        بازگشت 1;
}

صحیح fd_is_os_درخت(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ پوچ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ پوچ,
                        /* ret_fd= */ پوچ);
        اگر (r == -ENOENT)
                بازگشت false;
        اگر (r < 0)
                بازگشت r;

        بازگشت true;
}

ایستا صحیح extension_release_strict_xattr_مقدار(صحیح extension_release_fd, ثابت نویسه *extension_release_dir_path, ثابت نویسه *filename) {
        صحیح r;

        أكد(extension_release_fd >= 0);
        أكد(extension_release_dir_path);
        أكد(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ پوچ, "user.extension-release.strict", /* at_flags= */ 0);
        اگر (ERRNO_IS_NEG_XATTR_ABSENT(r))
                بازگشت log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        اگر (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                بازگشت true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        بازگشت false;
}

صحیح open_os_release_at(صحیح rfd, نویسه **ret_path, صحیح *ret_fd) {
        ثابت نویسه *e;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        اگر (e)
                بازگشت chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                اگر (r != -ENOENT)
                        بازگشت r;
        }

        بازگشت -ENOENT;
}

صحیح open_os_release(ثابت نویسه *ریشه, نویسه **ret_path, صحیح *ret_fd) {
        _cleanup_close_ صحیح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        اگر (!empty_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_PATH);
                اگر (rfd < 0)
                        بازگشت -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : پوچ, ret_fd ? &fd : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path) {
                r = chaseat_prefix_ریشه(p, ریشه, ret_path);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        بازگشت 0;
}

صحیح open_extension_release_at(
                صحیح rfd,
                ImageClass image_class,
                ثابت نویسه *extension,
                bool relax_extension_release_check,
                نویسه **ret_path,
                صحیح *ret_fd) {

        _cleanup_free_ نویسه *dir_path = پوچ, *path_found = پوچ;
        _cleanup_close_ صحیح fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = پوچ;
        bool found = false;
        ثابت نویسه *p;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        اگر (!extension)
                بازگشت open_os_release_at(rfd, ret_path, ret_fd);

        اگر (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                بازگشت -EINVAL;

        اگر (!image_name_is_valid(extension))
                بازگشت log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        اگر (r != -ENOENT)
                بازگشت r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        اگر (r < 0)
                بازگشت log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, بازگشت -errno) {
                _cleanup_close_ صحیح fd = -EBADF;
                ثابت نویسه *image_name;

                اگر (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        ادامه;

                image_name = startswith(de->d_name, "extension-release.");
                اگر (!image_name)
                        ادامه;

                اگر (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        ادامه;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                اگر (fd < 0)
                        بازگشت log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                اگر (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        ادامه;
                }

                اگر (!relax_extension_release_check) {
                        _cleanup_free_ نویسه *base_extension = پوچ;

                        r = path_extract_image_name(extension, &base_extension);
                        اگر (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                ادامه;
                        }

                        اگر (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_مقدار(fd, dir_path, image_name) != 0)
                                ادامه;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                اگر (found)
                        بازگشت -ENOTUNIQ;

                found = true;

                اگر (ret_fd)
                        fd_found = TAKE_FD(fd);

                اگر (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        اگر (!path_found)
                                بازگشت -ENOMEM;
                }
        }
        اگر (!found)
                بازگشت -ENOENT;

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        اگر (ret_path)
                *ret_path = TAKE_PTR(path_found);

        بازگشت 0;
}

صحیح open_extension_release(
                ثابت نویسه *ریشه,
                ImageClass image_class,
                ثابت نویسه *extension,
                bool relax_extension_release_check,
                نویسه **ret_path,
                صحیح *ret_fd) {

        _cleanup_close_ صحیح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        اگر (!empty_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_PATH);
                اگر (rfd < 0)
                        بازگشت -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : پوچ, ret_fd ? &fd : پوچ);
        اگر (r < 0)
                بازگشت r;

        اگر (ret_path) {
                r = chaseat_prefix_ریشه(p, ریشه, ret_path);
                اگر (r < 0)
                        بازگشت r;
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        بازگشت 0;
}

ایستا صحیح parse_extension_release_atv(
                صحیح rfd,
                ImageClass image_class,
                ثابت نویسه *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت parse_env_file_fdv(fd, p, ap);
}

صحیح parse_extension_release_at_sentinel(
                صحیح rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت نویسه *extension,
                ...) {

        va_list ap;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        بازگشت r;
}

صحیح parse_extension_release_sentinel(
                ثابت نویسه *ریشه,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت نویسه *extension,
                ...) {

        _cleanup_close_ صحیح rfd = XAT_FDROOT;
        va_list ap;
        صحیح r;

        اگر (!empty_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_PATH);
                اگر (rfd < 0)
                        بازگشت -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        بازگشت r;
}

صحیح load_os_release_pairs_with_prefix(ثابت نویسه *ریشه, ثابت نویسه *prefix, نویسه ***ret) {
        _cleanup_strv_free_ نویسه **os_release_pairs = پوچ, **os_release_pairs_prefixed = پوچ;
        صحیح r;

        أكد(ret);

        r = load_os_release_pairs(ریشه, &os_release_pairs);
        اگر (r < 0)
                بازگشت r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                نویسه *سطر;

                /* We strictly return only the four main ID fields and ignore the rest */
                اگر (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        ادامه;

                ascii_strlower(*p);
                سطر = strjoin(prefix, *p, "=", *q);
                اگر (!سطر)
                        بازگشت -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, سطر);
                اگر (r < 0)
                        بازگشت r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        بازگشت 0;
}

صحیح load_extension_release_pairs(ثابت نویسه *ریشه, ImageClass image_class, ثابت نویسه *extension, bool relax_extension_release_check, نویسه ***ret) {
        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ نویسه *p = پوچ;
        صحیح r;

        r = open_extension_release(ریشه, image_class, extension, relax_extension_release_check, &p, &fd);
        اگر (r < 0)
                بازگشت r;

        بازگشت load_env_file_pairs_fd(fd, p, ret);
}

صحیح os_release_support_ended(ثابت نویسه *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ نویسه *_support_end_alloc = پوچ;
        صحیح r;

        اگر (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(پوچ,
                                     "SUPPORT_END", &_support_end_alloc);
                اگر (r < 0 && r != -ENOENT)
                        بازگشت log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        اگر (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                اگر (ret_eol)
                        *ret_eol = USEC_INFINITY;

                بازگشت false;  /* no end date defined */
        }

        ساختار tm tm = {};
        ثابت نویسه *k = strptime(support_end, "%Y-%m-%d", &tm);
        اگر (!k || *k)
                بازگشت log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        اگر (r < 0)
                بازگشت log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        اگر (ret_eol)
                *ret_eol = eol;

        بازگشت now(CLOCK_REALTIME) > eol;
}

ثابت نویسه* os_release_pretty_name(ثابت نویسه *pretty_name, ثابت نویسه *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        بازگشت empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

نویسه *unescape_fancy_name(نویسه **fancy_name) {
        أكد(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ نویسه *unescaped_fancy_name = پوچ;

        اگر (isempty(*fancy_name))
                برو clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        اگر (n < 0) {
                log_debug_errno((صحیح) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                برو clear;
        }

        اگر (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                برو clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        بازگشت *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        بازگشت پوچ;
}

bool use_fancy_name(ثابت نویسه *fancy_name) {

        /* Decides whether to show the specified fancy name */

        اگر (isempty(fancy_name))
                بازگشت false;

        اگر (!colors_enabled())
                بازگشت false;

        بازگشت emoji_enabled() || ascii_is_valid(fancy_name);
}
