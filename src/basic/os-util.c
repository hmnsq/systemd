/* SPDX-License-Identifier: LGPL-2.1-or-later */

#ضمن <stdlib.h>

#ضمن "alloc-util.h"
#ضمن "ansi-color.h"
#ضمن "chase.h"
#ضمن "dirent-util.h"
#ضمن "env-file.h"
#ضمن "errno-util.h"
#ضمن "escape.h"
#ضمن "fd-util.h"
#ضمن "fs-util.h"
#ضمن "glyph-util.h"
#ضمن "log.h"
#ضمن "os-util.h"
#ضمن "path-util.h"
#ضمن "stat-util.h"
#ضمن "string-table.h"
#ضمن "string-util.h"
#ضمن "strv.h"
#ضمن "time-util.h"
#ضمن "utf8.h"
#ضمن "xattr-util.h"

ساكن ثابت حرف* ثابت image_class_جدول[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ساكن ثابت بنية {
        ثابت حرف *release_file_directory;
        ثابت حرف *release_file_مسیر_پیشوند;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_مسیر_پیشوند = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_مسیر_پیشوند = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(ثابت حرف *s) {
        إذا (!filename_is_valid(s))
                أرجع false;

        إذا (string_has_cc(s, عدم))
                أرجع false;

        إذا (!utf8_is_valid(s))
                أرجع false;

        /* Temporary files for atomically creating new files */
        إذا (startswith(s, ".#"))
                أرجع false;

        أرجع true;
}

صحيح مسیر_استخراج_image_name(ثابت حرف *مسیر, حرف **ret) {
        _cleanup_free_ حرف *fn = عدم;
        صحيح r;

        أكد(مسیر);
        أكد(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = مسیر_استخراج_filename(مسیر, &fn);
        إذا (r < 0)
                أرجع r;
        إذا (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                حرف *m = ENDSWITH_مجموعه(fn, ".sysext.raw", ".confext.raw", ".raw");
                إذا (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        إذا (!image_name_is_valid(fn))
                أرجع -EINVAL;

        *ret = TAKE_PTR(fn);
        أرجع 0;
}

صحيح مسیر_is_extension_درخت(ImageClass image_class, ثابت حرف *مسیر, ثابت حرف *extension, bool relax_extension_release_check) {
        صحيح r;

        أكد(مسیر);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(مسیر, F_OK);
        إذا (r < 0)
                أرجع r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(مسیر, image_class, extension, relax_extension_release_check, عدم, عدم);
        إذا (r == -ENOENT) /* We got nothing */
                أرجع 0;
        إذا (r < 0)
                أرجع r;

        أرجع 1;
}

صحيح fd_is_os_درخت(صحيح fd) {
        صحيح r;

        أكد(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ عدم,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ عدم,
                        /* ret_fd= */ عدم);
        إذا (r == -ENOENT)
                أرجع false;
        إذا (r < 0)
                أرجع r;

        أرجع true;
}

ساكن صحيح extension_release_strict_xattr_مقدار(صحيح extension_release_fd, ثابت حرف *extension_release_dir_مسیر, ثابت حرف *filename) {
        صحيح r;

        أكد(extension_release_fd >= 0);
        أكد(extension_release_dir_مسیر);
        أكد(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ عدم, "user.extension-release.strict", /* at_flags= */ 0);
        إذا (ERRNO_IS_NEG_XATTR_ABSENT(r))
                أرجع log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_مسیر, filename);
        إذا (r < 0)
                أرجع log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_مسیر, filename);

        /* Explicitly set to request strict matching? Skip it. */
        إذا (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_مسیر, filename);
                أرجع true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_مسیر, filename,
                  glyph(GLYPH_ELLIPSIS));

        أرجع false;
}

صحيح open_os_release_at(صحيح rfd, حرف **ret_مسیر, صحيح *ret_fd) {
        ثابت حرف *e;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        إذا (e)
                أرجع chaseat(rfd, rfd, e, /* flags= */ 0, ret_مسیر, ret_fd);

        FOREACH_STRING(مسیر, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, مسیر, /* flags= */ 0, ret_مسیر, ret_fd);
                إذا (r != -ENOENT)
                        أرجع r;
        }

        أرجع -ENOENT;
}

صحيح open_os_release(ثابت حرف *ریشه, حرف **ret_مسیر, صحيح *ret_fd) {
        _cleanup_close_ صحيح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        إذا (!خالی_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_مسیر);
                إذا (rfd < 0)
                        أرجع -errno;
        }

        r = open_os_release_at(rfd, ret_مسیر ? &p : عدم, ret_fd ? &fd : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret_مسیر) {
                r = chaseat_پیشوند_ریشه(p, ریشه, ret_مسیر);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);

        أرجع 0;
}

صحيح open_extension_release_at(
                صحيح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_مسیر,
                صحيح *ret_fd) {

        _cleanup_free_ حرف *dir_مسیر = عدم, *مسیر_یافت = عدم;
        _cleanup_close_ صحيح fd_یافت = -EBADF;
        _cleanup_closedir_ DIR *dir = عدم;
        bool یافت = false;
        ثابت حرف *p;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        إذا (!extension)
                أرجع open_os_release_at(rfd, ret_مسیر, ret_fd);

        إذا (!درون_مجموعه(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                أرجع -EINVAL;

        إذا (!image_name_is_valid(extension))
                أرجع log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_مسیر_پیشوند, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_مسیر, ret_fd);
        log_پر_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        إذا (r != -ENOENT)
                أرجع r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_مسیر, &dir);
        إذا (r < 0)
                أرجع log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, أرجع -errno) {
                _cleanup_close_ صحيح fd = -EBADF;
                ثابت حرف *image_name;

                إذا (!درون_مجموعه(de->d_type, DT_REG, DT_UNKNOWN))
                        استمر;

                image_name = startswith(de->d_name, "extension-release.");
                إذا (!image_name)
                        استمر;

                إذا (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_مسیر, de->d_name);
                        استمر;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_مسیر|O_CLOEXEC|O_NOFOLLOW);
                إذا (fd < 0)
                        أرجع log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_مسیر, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                إذا (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_مسیر, de->d_name);
                        استمر;
                }

                إذا (!relax_extension_release_check) {
                        _cleanup_free_ حرف *پایه_extension = عدم;

                        r = مسیر_استخراج_image_name(extension, &پایه_extension);
                        إذا (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                استمر;
                        }

                        إذا (!streq(image_name, پایه_extension) &&
                            extension_release_strict_xattr_مقدار(fd, dir_مسیر, image_name) != 0)
                                استمر;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                إذا (یافت)
                        أرجع -ENOTUNIQ;

                یافت = true;

                إذا (ret_fd)
                        fd_یافت = TAKE_FD(fd);

                إذا (ret_مسیر) {
                        مسیر_یافت = مسیر_join(dir_مسیر, de->d_name);
                        إذا (!مسیر_یافت)
                                أرجع -ENOMEM;
                }
        }
        إذا (!یافت)
                أرجع -ENOENT;

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd_یافت);
        إذا (ret_مسیر)
                *ret_مسیر = TAKE_PTR(مسیر_یافت);

        أرجع 0;
}

صحيح open_extension_release(
                ثابت حرف *ریشه,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_مسیر,
                صحيح *ret_fd) {

        _cleanup_close_ صحيح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        إذا (!خالی_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_مسیر);
                إذا (rfd < 0)
                        أرجع -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_مسیر ? &p : عدم, ret_fd ? &fd : عدم);
        إذا (r < 0)
                أرجع r;

        إذا (ret_مسیر) {
                r = chaseat_پیشوند_ریشه(p, ریشه, ret_مسیر);
                إذا (r < 0)
                        أرجع r;
        }

        إذا (ret_fd)
                *ret_fd = TAKE_FD(fd);

        أرجع 0;
}

ساكن صحيح parse_extension_release_atv(
                صحيح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        إذا (r < 0)
                أرجع r;

        أرجع parse_env_file_fdv(fd, p, ap);
}

صحيح parse_extension_release_at_sentinel(
                صحيح rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        va_list ap;
        صحيح r;

        أكد(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        أرجع r;
}

صحيح parse_extension_release_sentinel(
                ثابت حرف *ریشه,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        _cleanup_close_ صحيح rfd = XAT_FDROOT;
        va_list ap;
        صحيح r;

        إذا (!خالی_or_ریشه(ریشه)) {
                rfd = open(ریشه, O_CLOEXEC | O_DIRECTORY | O_مسیر);
                إذا (rfd < 0)
                        أرجع -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        أرجع r;
}

صحيح بارگیری_os_release_جفتها_with_پیشوند(ثابت حرف *ریشه, ثابت حرف *پیشوند, حرف ***ret) {
        _cleanup_strv_free_ حرف **os_release_جفتها = عدم, **os_release_جفتها_prefixed = عدم;
        صحيح r;

        أكد(ret);

        r = بارگیری_os_release_جفتها(ریشه, &os_release_جفتها);
        إذا (r < 0)
                أرجع r;

        STRV_FOREACH_PAIR(p, q, os_release_جفتها) {
                حرف *سطر;

                /* We strictly return only the four main ID fields and ignore the rest */
                إذا (!STR_درون_مجموعه(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        استمر;

                ascii_strlower(*p);
                سطر = strjoin(پیشوند, *p, "=", *q);
                إذا (!سطر)
                        أرجع -ENOMEM;
                r = strv_consume(&os_release_جفتها_prefixed, سطر);
                إذا (r < 0)
                        أرجع r;
        }

        *ret = TAKE_PTR(os_release_جفتها_prefixed);

        أرجع 0;
}

صحيح بارگیری_extension_release_جفتها(ثابت حرف *ریشه, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret) {
        _cleanup_close_ صحيح fd = -EBADF;
        _cleanup_free_ حرف *p = عدم;
        صحيح r;

        r = open_extension_release(ریشه, image_class, extension, relax_extension_release_check, &p, &fd);
        إذا (r < 0)
                أرجع r;

        أرجع بارگیری_env_file_جفتها_fd(fd, p, ret);
}

صحيح os_release_support_ended(ثابت حرف *support_انتها, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ حرف *support_انتها_alloc = عدم;
        صحيح r;

        إذا (!support_انتها) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(عدم,
                                     "SUPPORT_END", &support_انتها_alloc);
                إذا (r < 0 && r != -ENOENT)
                        أرجع log_پر_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_انتها = support_انتها_alloc;
        }

        إذا (isempty(support_انتها)) { /* An empty string is a explicit way to say "no EOL exists" */
                إذا (ret_eol)
                        *ret_eol = USEC_INFINITY;

                أرجع false;  /* no end date defined */
        }

        بنية tm tm = {};
        ثابت حرف *k = strptime(support_انتها, "%Y-%m-%d", &tm);
        إذا (!k || *k)
                أرجع log_پر_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_انتها);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        إذا (r < 0)
                أرجع log_پر_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        إذا (ret_eol)
                *ret_eol = eol;

        أرجع now(CLOCK_REALTIME) > eol;
}

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        أرجع خالی_مقصد_null(pretty_name) ?:
                خالی_مقصد_null(name) ?: "Linux";
}

حرف *unescape_fancy_name(حرف **fancy_name) {
        أكد(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ حرف *unescaped_fancy_name = عدم;

        إذا (isempty(*fancy_name))
                اذهب clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        إذا (n < 0) {
                log_debug_errno((صحيح) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                اذهب clear;
        }

        إذا (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                اذهب clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        أرجع *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        أرجع عدم;
}

bool use_fancy_name(ثابت حرف *fancy_name) {

        /* Decides whether to show the specified fancy name */

        إذا (isempty(fancy_name))
                أرجع false;

        إذا (!colors_enabled())
                أرجع false;

        أرجع emoji_enabled() || ascii_is_valid(fancy_name);
}
