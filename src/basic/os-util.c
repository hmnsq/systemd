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

ساکن ثابت حرف* ثابت image_class_جدول[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_جدول_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
ساکن ثابت ساخت {
        ثابت حرف *release_file_directory;
        ثابت حرف *release_file_راستہ_سابقہ;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_راستہ_سابقہ = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_راستہ_سابقہ = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(ثابت حرف *s) {
        اگر (!filename_is_valid(s))
                واپس false;

        اگر (string_has_cc(s, تہی))
                واپس false;

        اگر (!utf8_is_valid(s))
                واپس false;

        /* Temporary files for atomically creating new files */
        اگر (startswith(s, ".#"))
                واپس false;

        واپس true;
}

صحیح راستہ_نکالو_image_name(ثابت حرف *راستہ, حرف **ret) {
        _cleanup_free_ حرف *fn = تہی;
        صحیح r;

        أكد(راستہ);
        أكد(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = راستہ_نکالو_filename(راستہ, &fn);
        اگر (r < 0)
                واپس r;
        اگر (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                حرف *m = ENDSWITH_سیٹ(fn, ".sysext.raw", ".confext.raw", ".raw");
                اگر (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        اگر (!image_name_is_valid(fn))
                واپس -EINVAL;

        *ret = TAKE_PTR(fn);
        واپس 0;
}

صحیح راستہ_is_extension_درخت(ImageClass image_class, ثابت حرف *راستہ, ثابت حرف *extension, bool relax_extension_release_check) {
        صحیح r;

        أكد(راستہ);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(راستہ, F_OK);
        اگر (r < 0)
                واپس r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(راستہ, image_class, extension, relax_extension_release_check, تہی, تہی);
        اگر (r == -ENOENT) /* We got nothing */
                واپس 0;
        اگر (r < 0)
                واپس r;

        واپس 1;
}

صحیح fd_is_os_درخت(صحیح fd) {
        صحیح r;

        أكد(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ تہی,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ تہی,
                        /* ret_fd= */ تہی);
        اگر (r == -ENOENT)
                واپس false;
        اگر (r < 0)
                واپس r;

        واپس true;
}

ساکن صحیح extension_release_strict_xattr_قدر(صحیح extension_release_fd, ثابت حرف *extension_release_dir_راستہ, ثابت حرف *filename) {
        صحیح r;

        أكد(extension_release_fd >= 0);
        أكد(extension_release_dir_راستہ);
        أكد(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ تہی, "user.extension-release.strict", /* at_flags= */ 0);
        اگر (ERRNO_IS_NEG_XATTR_ABSENT(r))
                واپس log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_راستہ, filename);
        اگر (r < 0)
                واپس log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_راستہ, filename);

        /* Explicitly set to request strict matching? Skip it. */
        اگر (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_راستہ, filename);
                واپس true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_راستہ, filename,
                  glyph(GLYPH_ELLIPSIS));

        واپس false;
}

صحیح open_os_release_at(صحیح rfd, حرف **ret_راستہ, صحیح *ret_fd) {
        ثابت حرف *e;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        اگر (e)
                واپس chaseat(rfd, rfd, e, /* flags= */ 0, ret_راستہ, ret_fd);

        FOREACH_STRING(راستہ, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, راستہ, /* flags= */ 0, ret_راستہ, ret_fd);
                اگر (r != -ENOENT)
                        واپس r;
        }

        واپس -ENOENT;
}

صحیح open_os_release(ثابت حرف *جڑ, حرف **ret_راستہ, صحیح *ret_fd) {
        _cleanup_close_ صحیح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        اگر (!خالی_خانے_or_جڑ(جڑ)) {
                rfd = open(جڑ, O_CLOEXEC | O_DIRECTORY | O_راستہ);
                اگر (rfd < 0)
                        واپس -errno;
        }

        r = open_os_release_at(rfd, ret_راستہ ? &p : تہی, ret_fd ? &fd : تہی);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ) {
                r = chaseat_سابقہ_جڑ(p, جڑ, ret_راستہ);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        واپس 0;
}

صحیح open_extension_release_at(
                صحیح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_راستہ,
                صحیح *ret_fd) {

        _cleanup_free_ حرف *dir_راستہ = تہی, *راستہ_ملا = تہی;
        _cleanup_close_ صحیح fd_ملا = -EBADF;
        _cleanup_closedir_ DIR *dir = تہی;
        bool ملا = false;
        ثابت حرف *p;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));
        أكد(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        اگر (!extension)
                واپس open_os_release_at(rfd, ret_راستہ, ret_fd);

        اگر (!اندر_سیٹ(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                واپس -EINVAL;

        اگر (!image_name_is_valid(extension))
                واپس log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_راستہ_سابقہ, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_راستہ, ret_fd);
        log_بھرا_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        اگر (r != -ENOENT)
                واپس r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_راستہ, &dir);
        اگر (r < 0)
                واپس log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, واپس -errno) {
                _cleanup_close_ صحیح fd = -EBADF;
                ثابت حرف *image_name;

                اگر (!اندر_سیٹ(de->d_type, DT_REG, DT_UNKNOWN))
                        جاری;

                image_name = startswith(de->d_name, "extension-release.");
                اگر (!image_name)
                        جاری;

                اگر (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_راستہ, de->d_name);
                        جاری;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_راستہ|O_CLOEXEC|O_NOFOLLOW);
                اگر (fd < 0)
                        واپس log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_راستہ, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                اگر (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_راستہ, de->d_name);
                        جاری;
                }

                اگر (!relax_extension_release_check) {
                        _cleanup_free_ حرف *بنیاد_extension = تہی;

                        r = راستہ_نکالو_image_name(extension, &بنیاد_extension);
                        اگر (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                جاری;
                        }

                        اگر (!streq(image_name, بنیاد_extension) &&
                            extension_release_strict_xattr_قدر(fd, dir_راستہ, image_name) != 0)
                                جاری;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                اگر (ملا)
                        واپس -ENOTUNIQ;

                ملا = true;

                اگر (ret_fd)
                        fd_ملا = TAKE_FD(fd);

                اگر (ret_راستہ) {
                        راستہ_ملا = راستہ_join(dir_راستہ, de->d_name);
                        اگر (!راستہ_ملا)
                                واپس -ENOMEM;
                }
        }
        اگر (!ملا)
                واپس -ENOENT;

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd_ملا);
        اگر (ret_راستہ)
                *ret_راستہ = TAKE_PTR(راستہ_ملا);

        واپس 0;
}

صحیح open_extension_release(
                ثابت حرف *جڑ,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                حرف **ret_راستہ,
                صحیح *ret_fd) {

        _cleanup_close_ صحیح rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        اگر (!خالی_خانے_or_جڑ(جڑ)) {
                rfd = open(جڑ, O_CLOEXEC | O_DIRECTORY | O_راستہ);
                اگر (rfd < 0)
                        واپس -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_راستہ ? &p : تہی, ret_fd ? &fd : تہی);
        اگر (r < 0)
                واپس r;

        اگر (ret_راستہ) {
                r = chaseat_سابقہ_جڑ(p, جڑ, ret_راستہ);
                اگر (r < 0)
                        واپس r;
        }

        اگر (ret_fd)
                *ret_fd = TAKE_FD(fd);

        واپس 0;
}

ساکن صحیح parse_extension_release_atv(
                صحیح rfd,
                ImageClass image_class,
                ثابت حرف *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        اگر (r < 0)
                واپس r;

        واپس parse_env_file_fdv(fd, p, ap);
}

صحیح parse_extension_release_at_sentinel(
                صحیح rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        va_list ap;
        صحیح r;

        أكد(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        واپس r;
}

صحیح parse_extension_release_sentinel(
                ثابت حرف *جڑ,
                ImageClass image_class,
                bool relax_extension_release_check,
                ثابت حرف *extension,
                ...) {

        _cleanup_close_ صحیح rfd = XAT_FDROOT;
        va_list ap;
        صحیح r;

        اگر (!خالی_خانے_or_جڑ(جڑ)) {
                rfd = open(جڑ, O_CLOEXEC | O_DIRECTORY | O_راستہ);
                اگر (rfd < 0)
                        واپس -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        واپس r;
}

صحیح بھرو_os_release_جوڑے_with_سابقہ(ثابت حرف *جڑ, ثابت حرف *سابقہ, حرف ***ret) {
        _cleanup_strv_free_ حرف **os_release_جوڑے = تہی, **os_release_جوڑے_prefixed = تہی;
        صحیح r;

        أكد(ret);

        r = بھرو_os_release_جوڑے(جڑ, &os_release_جوڑے);
        اگر (r < 0)
                واپس r;

        STRV_FOREACH_PAIR(p, q, os_release_جوڑے) {
                حرف *سطر;

                /* We strictly return only the four main ID fields and ignore the rest */
                اگر (!STR_اندر_سیٹ(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        جاری;

                ascii_strlower(*p);
                سطر = strjoin(سابقہ, *p, "=", *q);
                اگر (!سطر)
                        واپس -ENOMEM;
                r = strv_consume(&os_release_جوڑے_prefixed, سطر);
                اگر (r < 0)
                        واپس r;
        }

        *ret = TAKE_PTR(os_release_جوڑے_prefixed);

        واپس 0;
}

صحیح بھرو_extension_release_جوڑے(ثابت حرف *جڑ, ImageClass image_class, ثابت حرف *extension, bool relax_extension_release_check, حرف ***ret) {
        _cleanup_close_ صحیح fd = -EBADF;
        _cleanup_free_ حرف *p = تہی;
        صحیح r;

        r = open_extension_release(جڑ, image_class, extension, relax_extension_release_check, &p, &fd);
        اگر (r < 0)
                واپس r;

        واپس بھرو_env_file_جوڑے_fd(fd, p, ret);
}

صحیح os_release_support_ended(ثابت حرف *support_انتہا, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ حرف *support_انتہا_alloc = تہی;
        صحیح r;

        اگر (!support_انتہا) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(تہی,
                                     "SUPPORT_END", &support_انتہا_alloc);
                اگر (r < 0 && r != -ENOENT)
                        واپس log_بھرا_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_انتہا = support_انتہا_alloc;
        }

        اگر (isempty(support_انتہا)) { /* An empty string is a explicit way to say "no EOL exists" */
                اگر (ret_eol)
                        *ret_eol = USEC_INFINITY;

                واپس false;  /* no end date defined */
        }

        ساخت tm tm = {};
        ثابت حرف *k = strptime(support_انتہا, "%Y-%m-%d", &tm);
        اگر (!k || *k)
                واپس log_بھرا_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_انتہا);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        اگر (r < 0)
                واپس log_بھرا_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        اگر (ret_eol)
                *ret_eol = eol;

        واپس now(CLOCK_REALTIME) > eol;
}

ثابت حرف* os_release_pretty_name(ثابت حرف *pretty_name, ثابت حرف *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        واپس خالی_خانے_مقصد_null(pretty_name) ?:
                خالی_خانے_مقصد_null(name) ?: "Linux";
}

حرف *unescape_fancy_name(حرف **fancy_name) {
        أكد(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ حرف *unescaped_fancy_name = تہی;

        اگر (isempty(*fancy_name))
                جاؤ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        اگر (n < 0) {
                log_debug_errno((صحیح) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                جاؤ clear;
        }

        اگر (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                جاؤ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        واپس *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        واپس تہی;
}

bool use_fancy_name(ثابت حرف *fancy_name) {

        /* Decides whether to show the specified fancy name */

        اگر (isempty(fancy_name))
                واپس false;

        اگر (!colors_enabled())
                واپس false;

        واپس emoji_enabled() || ascii_is_valid(fancy_name);
}
