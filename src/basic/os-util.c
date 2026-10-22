/* SPDX-License-Identifier: LGPL-2.1-or-later */

#अन्तर्भाव <stdlib.h>

#अन्तर्भाव "alloc-util.h"
#अन्तर्भाव "ansi-color.h"
#अन्तर्भाव "chase.h"
#अन्तर्भाव "dirent-util.h"
#अन्तर्भाव "env-file.h"
#अन्तर्भाव "errno-util.h"
#अन्तर्भाव "escape.h"
#अन्तर्भाव "fd-util.h"
#अन्तर्भाव "fs-util.h"
#अन्तर्भाव "glyph-util.h"
#अन्तर्भाव "log.h"
#अन्तर्भाव "os-util.h"
#अन्तर्भाव "path-util.h"
#अन्तर्भाव "stat-util.h"
#अन्तर्भाव "string-table.h"
#अन्तर्भाव "string-util.h"
#अन्तर्भाव "strv.h"
#अन्तर्भाव "time-util.h"
#अन्तर्भाव "utf8.h"
#अन्तर्भाव "xattr-util.h"

स्थिर ध्रुव वर्ण* ध्रुव image_class_सारणी[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_सारणी_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
स्थिर ध्रुव संरचना {
        ध्रुव वर्ण *release_file_directory;
        ध्रुव वर्ण *release_file_मार्गः_उपसर्गः;
} image_class_release_info[_IMAGE_CLASS_MAX] = {
        [IMAGE_SYSEXT] = {
                .release_file_directory = "/usr/lib/extension-release.d/",
                .release_file_मार्गः_उपसर्गः = "/usr/lib/extension-release.d/extension-release.",
        },
        [IMAGE_CONFEXT] = {
                .release_file_directory = "/etc/extension-release.d/",
                .release_file_मार्गः_उपसर्गः = "/etc/extension-release.d/extension-release.",
        }
};

bool image_name_is_valid(ध्रुव वर्ण *s) {
        यदि (!filename_is_valid(s))
                प्रत्यागम false;

        यदि (string_has_cc(s, रिक्त))
                प्रत्यागम false;

        यदि (!utf8_is_valid(s))
                प्रत्यागम false;

        /* Temporary files for atomically creating new files */
        यदि (startswith(s, ".#"))
                प्रत्यागम false;

        प्रत्यागम true;
}

पूर्णाङ्क मार्गः_extract_image_name(ध्रुव वर्ण *मार्गः, वर्ण **ret) {
        _cleanup_free_ वर्ण *fn = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);
        प्रतिज्ञा(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = मार्गः_extract_filename(मार्गः, &fn);
        यदि (r < 0)
                प्रत्यागम r;
        यदि (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                वर्ण *m = ENDSWITH_समुच्चयः(fn, ".sysext.raw", ".confext.raw", ".raw");
                यदि (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        यदि (!image_name_is_valid(fn))
                प्रत्यागम -EINVAL;

        *ret = TAKE_PTR(fn);
        प्रत्यागम 0;
}

पूर्णाङ्क मार्गः_is_extension_वृक्षः(ImageClass image_class, ध्रुव वर्ण *मार्गः, ध्रुव वर्ण *extension, bool relax_extension_release_check) {
        पूर्णाङ्क r;

        प्रतिज्ञा(मार्गः);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(मार्गः, F_OK);
        यदि (r < 0)
                प्रत्यागम r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(मार्गः, image_class, extension, relax_extension_release_check, रिक्त, रिक्त);
        यदि (r == -ENOENT) /* We got nothing */
                प्रत्यागम 0;
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम 1;
}

पूर्णाङ्क fd_is_os_वृक्षः(पूर्णाङ्क fd) {
        पूर्णाङ्क r;

        प्रतिज्ञा(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ रिक्त,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ रिक्त,
                        /* ret_fd= */ रिक्त);
        यदि (r == -ENOENT)
                प्रत्यागम false;
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम true;
}

स्थिर पूर्णाङ्क extension_release_strict_xattr_मानम्(पूर्णाङ्क extension_release_fd, ध्रुव वर्ण *extension_release_dir_मार्गः, ध्रुव वर्ण *filename) {
        पूर्णाङ्क r;

        प्रतिज्ञा(extension_release_fd >= 0);
        प्रतिज्ञा(extension_release_dir_मार्गः);
        प्रतिज्ञा(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ रिक्त, "user.extension-release.strict", /* at_flags= */ 0);
        यदि (ERRNO_IS_NEG_XATTR_ABSENT(r))
                प्रत्यागम log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_मार्गः, filename);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_मार्गः, filename);

        /* Explicitly set to request strict matching? Skip it. */
        यदि (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_मार्गः, filename);
                प्रत्यागम true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_मार्गः, filename,
                  glyph(GLYPH_ELLIPSIS));

        प्रत्यागम false;
}

पूर्णाङ्क open_os_release_at(पूर्णाङ्क rfd, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        यदि (e)
                प्रत्यागम chaseat(rfd, rfd, e, /* flags= */ 0, ret_मार्गः, ret_fd);

        FOREACH_STRING(मार्गः, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, मार्गः, /* flags= */ 0, ret_मार्गः, ret_fd);
                यदि (r != -ENOENT)
                        प्रत्यागम r;
        }

        प्रत्यागम -ENOENT;
}

पूर्णाङ्क open_os_release(ध्रुव वर्ण *मूलम्, वर्ण **ret_मार्गः, पूर्णाङ्क *ret_fd) {
        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        यदि (!रिक्तानि_or_मूलम्(मूलम्)) {
                rfd = उद्घाटय(मूलम्, O_CLOEXEC | O_DIRECTORY | O_मार्गः);
                यदि (rfd < 0)
                        प्रत्यागम -errno;
        }

        r = open_os_release_at(rfd, ret_मार्गः ? &p : रिक्त, ret_fd ? &fd : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः) {
                r = chaseat_उपसर्गः_मूलम्(p, मूलम्, ret_मार्गः);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd);

        प्रत्यागम 0;
}

पूर्णाङ्क open_extension_release_at(
                पूर्णाङ्क rfd,
                ImageClass image_class,
                ध्रुव वर्ण *extension,
                bool relax_extension_release_check,
                वर्ण **ret_मार्गः,
                पूर्णाङ्क *ret_fd) {

        _cleanup_free_ वर्ण *dir_मार्गः = रिक्त, *मार्गः_प्राप्तम् = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd_प्राप्तम् = -EBADF;
        _cleanup_closedir_ DIR *dir = रिक्त;
        bool प्राप्तम् = false;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        यदि (!extension)
                प्रत्यागम open_os_release_at(rfd, ret_मार्गः, ret_fd);

        यदि (!अन्तर्_समुच्चयः(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                प्रत्यागम -EINVAL;

        यदि (!image_name_is_valid(extension))
                प्रत्यागम log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_मार्गः_उपसर्गः, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_मार्गः, ret_fd);
        log_पूर्णम्_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        यदि (r != -ENOENT)
                प्रत्यागम r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_मार्गः, &dir);
        यदि (r < 0)
                प्रत्यागम log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, प्रत्यागम -errno) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                ध्रुव वर्ण *image_name;

                यदि (!अन्तर्_समुच्चयः(de->d_type, DT_REG, DT_UNKNOWN))
                        अनुवर्त;

                image_name = startswith(de->d_name, "extension-release.");
                यदि (!image_name)
                        अनुवर्त;

                यदि (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_मार्गः, de->d_name);
                        अनुवर्त;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_मार्गः|O_CLOEXEC|O_NOFOLLOW);
                यदि (fd < 0)
                        प्रत्यागम log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_मार्गः, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                यदि (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_मार्गः, de->d_name);
                        अनुवर्त;
                }

                यदि (!relax_extension_release_check) {
                        _cleanup_free_ वर्ण *आधारः_extension = रिक्त;

                        r = मार्गः_extract_image_name(extension, &आधारः_extension);
                        यदि (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                अनुवर्त;
                        }

                        यदि (!streq(image_name, आधारः_extension) &&
                            extension_release_strict_xattr_मानम्(fd, dir_मार्गः, image_name) != 0)
                                अनुवर्त;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                यदि (प्राप्तम्)
                        प्रत्यागम -ENOTUNIQ;

                प्राप्तम् = true;

                यदि (ret_fd)
                        fd_प्राप्तम् = TAKE_FD(fd);

                यदि (ret_मार्गः) {
                        मार्गः_प्राप्तम् = मार्गः_join(dir_मार्गः, de->d_name);
                        यदि (!मार्गः_प्राप्तम्)
                                प्रत्यागम -ENOMEM;
                }
        }
        यदि (!प्राप्तम्)
                प्रत्यागम -ENOENT;

        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd_प्राप्तम्);
        यदि (ret_मार्गः)
                *ret_मार्गः = TAKE_PTR(मार्गः_प्राप्तम्);

        प्रत्यागम 0;
}

पूर्णाङ्क open_extension_release(
                ध्रुव वर्ण *मूलम्,
                ImageClass image_class,
                ध्रुव वर्ण *extension,
                bool relax_extension_release_check,
                वर्ण **ret_मार्गः,
                पूर्णाङ्क *ret_fd) {

        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        यदि (!रिक्तानि_or_मूलम्(मूलम्)) {
                rfd = उद्घाटय(मूलम्, O_CLOEXEC | O_DIRECTORY | O_मार्गः);
                यदि (rfd < 0)
                        प्रत्यागम -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_मार्गः ? &p : रिक्त, ret_fd ? &fd : रिक्त);
        यदि (r < 0)
                प्रत्यागम r;

        यदि (ret_मार्गः) {
                r = chaseat_उपसर्गः_मूलम्(p, मूलम्, ret_मार्गः);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        यदि (ret_fd)
                *ret_fd = TAKE_FD(fd);

        प्रत्यागम 0;
}

स्थिर पूर्णाङ्क parse_extension_release_atv(
                पूर्णाङ्क rfd,
                ImageClass image_class,
                ध्रुव वर्ण *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम parse_env_file_fdv(fd, p, ap);
}

पूर्णाङ्क parse_extension_release_at_sentinel(
                पूर्णाङ्क rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                ध्रुव वर्ण *extension,
                ...) {

        va_list ap;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        प्रत्यागम r;
}

पूर्णाङ्क parse_extension_release_sentinel(
                ध्रुव वर्ण *मूलम्,
                ImageClass image_class,
                bool relax_extension_release_check,
                ध्रुव वर्ण *extension,
                ...) {

        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT;
        va_list ap;
        पूर्णाङ्क r;

        यदि (!रिक्तानि_or_मूलम्(मूलम्)) {
                rfd = उद्घाटय(मूलम्, O_CLOEXEC | O_DIRECTORY | O_मार्गः);
                यदि (rfd < 0)
                        प्रत्यागम -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        प्रत्यागम r;
}

पूर्णाङ्क आरोपय_os_release_युग्मानि_with_उपसर्गः(ध्रुव वर्ण *मूलम्, ध्रुव वर्ण *उपसर्गः, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **os_release_युग्मानि = रिक्त, **os_release_युग्मानि_prefixed = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = आरोपय_os_release_युग्मानि(मूलम्, &os_release_युग्मानि);
        यदि (r < 0)
                प्रत्यागम r;

        STRV_FOREACH_PAIR(p, q, os_release_युग्मानि) {
                वर्ण *पङ्क्तिः;

                /* We strictly return only the four main ID fields and ignore the rest */
                यदि (!STR_अन्तर्_समुच्चयः(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        अनुवर्त;

                ascii_strlower(*p);
                पङ्क्तिः = strjoin(उपसर्गः, *p, "=", *q);
                यदि (!पङ्क्तिः)
                        प्रत्यागम -ENOMEM;
                r = strv_consume(&os_release_युग्मानि_prefixed, पङ्क्तिः);
                यदि (r < 0)
                        प्रत्यागम r;
        }

        *ret = TAKE_PTR(os_release_युग्मानि_prefixed);

        प्रत्यागम 0;
}

पूर्णाङ्क आरोपय_extension_release_युग्मानि(ध्रुव वर्ण *मूलम्, ImageClass image_class, ध्रुव वर्ण *extension, bool relax_extension_release_check, वर्ण ***ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        r = open_extension_release(मूलम्, image_class, extension, relax_extension_release_check, &p, &fd);
        यदि (r < 0)
                प्रत्यागम r;

        प्रत्यागम आरोपय_env_file_युग्मानि_fd(fd, p, ret);
}

पूर्णाङ्क os_release_support_ended(ध्रुव वर्ण *support_अन्तः, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ वर्ण *support_अन्तः_alloc = रिक्त;
        पूर्णाङ्क r;

        यदि (!support_अन्तः) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(रिक्त,
                                     "SUPPORT_END", &support_अन्तः_alloc);
                यदि (r < 0 && r != -ENOENT)
                        प्रत्यागम log_पूर्णम्_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_अन्तः = support_अन्तः_alloc;
        }

        यदि (isempty(support_अन्तः)) { /* An empty string is a explicit way to say "no EOL exists" */
                यदि (ret_eol)
                        *ret_eol = USEC_INFINITY;

                प्रत्यागम false;  /* no end date defined */
        }

        संरचना tm tm = {};
        ध्रुव वर्ण *k = strptime(support_अन्तः, "%Y-%m-%d", &tm);
        यदि (!k || *k)
                प्रत्यागम log_पूर्णम्_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_अन्तः);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        यदि (r < 0)
                प्रत्यागम log_पूर्णम्_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        यदि (ret_eol)
                *ret_eol = eol;

        प्रत्यागम now(CLOCK_REALTIME) > eol;
}

ध्रुव वर्ण* os_release_pretty_name(ध्रुव वर्ण *pretty_name, ध्रुव वर्ण *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        प्रत्यागम रिक्तानि_गन्तव्यम्_null(pretty_name) ?:
                रिक्तानि_गन्तव्यम्_null(name) ?: "Linux";
}

वर्ण *unescape_fancy_name(वर्ण **fancy_name) {
        प्रतिज्ञा(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ वर्ण *unescaped_fancy_name = रिक्त;

        यदि (isempty(*fancy_name))
                गच्छ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        यदि (n < 0) {
                log_debug_errno((पूर्णाङ्क) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                गच्छ clear;
        }

        यदि (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                गच्छ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        प्रत्यागम *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        प्रत्यागम रिक्त;
}

bool use_fancy_name(ध्रुव वर्ण *fancy_name) {

        /* Decides whether to show the specified fancy name */

        यदि (isempty(fancy_name))
                प्रत्यागम false;

        यदि (!colors_enabled())
                प्रत्यागम false;

        प्रत्यागम emoji_enabled() || ascii_is_valid(fancy_name);
}
