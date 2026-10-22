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

स्थिर ध्रुव वर्ण* ध्रुव image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
स्थिर ध्रुव संरचना {
        ध्रुव वर्ण *release_file_directory;
        ध्रुव वर्ण *release_file_path_prefix;
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

bool image_name_is_valid(ध्रुव वर्ण *s) {
        सचे (!filename_is_valid(s))
                पटिनिवत्त false;

        सचे (string_has_cc(s, रिक्त))
                पटिनिवत्त false;

        सचे (!utf8_is_valid(s))
                पटिनिवत्त false;

        /* Temporary files for atomically creating new files */
        सचे (startswith(s, ".#"))
                पटिनिवत्त false;

        पटिनिवत्त true;
}

पूर्णाङ्क path_extract_image_name(ध्रुव वर्ण *path, वर्ण **ret) {
        _cleanup_free_ वर्ण *fn = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(path);
        प्रतिज्ञा(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        सचे (r < 0)
                पटिनिवत्त r;
        सचे (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                वर्ण *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                सचे (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        सचे (!image_name_is_valid(fn))
                पटिनिवत्त -EINVAL;

        *ret = TAKE_PTR(fn);
        पटिनिवत्त 0;
}

पूर्णाङ्क path_is_extension_वृक्षः(ImageClass image_class, ध्रुव वर्ण *path, ध्रुव वर्ण *extension, bool relax_extension_release_check) {
        पूर्णाङ्क r;

        प्रतिज्ञा(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        सचे (r < 0)
                पटिनिवत्त r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, रिक्त, रिक्त);
        सचे (r == -ENOENT) /* We got nothing */
                पटिनिवत्त 0;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त 1;
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
        सचे (r == -ENOENT)
                पटिनिवत्त false;
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त true;
}

स्थिर पूर्णाङ्क extension_release_strict_xattr_मानम्(पूर्णाङ्क extension_release_fd, ध्रुव वर्ण *extension_release_dir_path, ध्रुव वर्ण *filename) {
        पूर्णाङ्क r;

        प्रतिज्ञा(extension_release_fd >= 0);
        प्रतिज्ञा(extension_release_dir_path);
        प्रतिज्ञा(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ रिक्त, "user.extension-release.strict", /* at_flags= */ 0);
        सचे (ERRNO_IS_NEG_XATTR_ABSENT(r))
                पटिनिवत्त log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        सचे (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                पटिनिवत्त true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        पटिनिवत्त false;
}

पूर्णाङ्क open_os_release_at(पूर्णाङ्क rfd, वर्ण **ret_path, पूर्णाङ्क *ret_fd) {
        ध्रुव वर्ण *e;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        सचे (e)
                पटिनिवत्त chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                सचे (r != -ENOENT)
                        पटिनिवत्त r;
        }

        पटिनिवत्त -ENOENT;
}

पूर्णाङ्क open_os_release(ध्रुव वर्ण *मूलम्, वर्ण **ret_path, पूर्णाङ्क *ret_fd) {
        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        सचे (!empty_or_मूलम्(मूलम्)) {
                rfd = open(मूलम्, O_CLOEXEC | O_DIRECTORY | O_PATH);
                सचे (rfd < 0)
                        पटिनिवत्त -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : रिक्त, ret_fd ? &fd : रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_path) {
                r = chaseat_prefix_मूलम्(p, मूलम्, ret_path);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (ret_fd)
                *ret_fd = TAKE_FD(fd);

        पटिनिवत्त 0;
}

पूर्णाङ्क open_extension_release_at(
                पूर्णाङ्क rfd,
                ImageClass image_class,
                ध्रुव वर्ण *extension,
                bool relax_extension_release_check,
                वर्ण **ret_path,
                पूर्णाङ्क *ret_fd) {

        _cleanup_free_ वर्ण *dir_path = रिक्त, *path_found = रिक्त;
        _cleanup_close_ पूर्णाङ्क fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = रिक्त;
        bool found = false;
        ध्रुव वर्ण *p;
        पूर्णाङ्क r;

        प्रतिज्ञा(wildcard_fd_is_valid(rfd));
        प्रतिज्ञा(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        सचे (!extension)
                पटिनिवत्त open_os_release_at(rfd, ret_path, ret_fd);

        सचे (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                पटिनिवत्त -EINVAL;

        सचे (!image_name_is_valid(extension))
                पटिनिवत्त log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        सचे (r != -ENOENT)
                पटिनिवत्त r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        सचे (r < 0)
                पटिनिवत्त log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, पटिनिवत्त -errno) {
                _cleanup_close_ पूर्णाङ्क fd = -EBADF;
                ध्रुव वर्ण *image_name;

                सचे (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        अनुवर्त;

                image_name = startswith(de->d_name, "extension-release.");
                सचे (!image_name)
                        अनुवर्त;

                सचे (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        अनुवर्त;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                सचे (fd < 0)
                        पटिनिवत्त log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                सचे (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        अनुवर्त;
                }

                सचे (!relax_extension_release_check) {
                        _cleanup_free_ वर्ण *base_extension = रिक्त;

                        r = path_extract_image_name(extension, &base_extension);
                        सचे (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                अनुवर्त;
                        }

                        सचे (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_मानम्(fd, dir_path, image_name) != 0)
                                अनुवर्त;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                सचे (found)
                        पटिनिवत्त -ENOTUNIQ;

                found = true;

                सचे (ret_fd)
                        fd_found = TAKE_FD(fd);

                सचे (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        सचे (!path_found)
                                पटिनिवत्त -ENOMEM;
                }
        }
        सचे (!found)
                पटिनिवत्त -ENOENT;

        सचे (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        सचे (ret_path)
                *ret_path = TAKE_PTR(path_found);

        पटिनिवत्त 0;
}

पूर्णाङ्क open_extension_release(
                ध्रुव वर्ण *मूलम्,
                ImageClass image_class,
                ध्रुव वर्ण *extension,
                bool relax_extension_release_check,
                वर्ण **ret_path,
                पूर्णाङ्क *ret_fd) {

        _cleanup_close_ पूर्णाङ्क rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        सचे (!empty_or_मूलम्(मूलम्)) {
                rfd = open(मूलम्, O_CLOEXEC | O_DIRECTORY | O_PATH);
                सचे (rfd < 0)
                        पटिनिवत्त -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : रिक्त, ret_fd ? &fd : रिक्त);
        सचे (r < 0)
                पटिनिवत्त r;

        सचे (ret_path) {
                r = chaseat_prefix_मूलम्(p, मूलम्, ret_path);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        सचे (ret_fd)
                *ret_fd = TAKE_FD(fd);

        पटिनिवत्त 0;
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
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त parse_env_file_fdv(fd, p, ap);
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
        पटिनिवत्त r;
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

        सचे (!empty_or_मूलम्(मूलम्)) {
                rfd = open(मूलम्, O_CLOEXEC | O_DIRECTORY | O_PATH);
                सचे (rfd < 0)
                        पटिनिवत्त -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        पटिनिवत्त r;
}

पूर्णाङ्क load_os_release_pairs_with_prefix(ध्रुव वर्ण *मूलम्, ध्रुव वर्ण *prefix, वर्ण ***ret) {
        _cleanup_strv_free_ वर्ण **os_release_pairs = रिक्त, **os_release_pairs_prefixed = रिक्त;
        पूर्णाङ्क r;

        प्रतिज्ञा(ret);

        r = load_os_release_pairs(मूलम्, &os_release_pairs);
        सचे (r < 0)
                पटिनिवत्त r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                वर्ण *पङ्क्तिः;

                /* We strictly return only the four main ID fields and ignore the rest */
                सचे (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        अनुवर्त;

                ascii_strlower(*p);
                पङ्क्तिः = strjoin(prefix, *p, "=", *q);
                सचे (!पङ्क्तिः)
                        पटिनिवत्त -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, पङ्क्तिः);
                सचे (r < 0)
                        पटिनिवत्त r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        पटिनिवत्त 0;
}

पूर्णाङ्क load_extension_release_pairs(ध्रुव वर्ण *मूलम्, ImageClass image_class, ध्रुव वर्ण *extension, bool relax_extension_release_check, वर्ण ***ret) {
        _cleanup_close_ पूर्णाङ्क fd = -EBADF;
        _cleanup_free_ वर्ण *p = रिक्त;
        पूर्णाङ्क r;

        r = open_extension_release(मूलम्, image_class, extension, relax_extension_release_check, &p, &fd);
        सचे (r < 0)
                पटिनिवत्त r;

        पटिनिवत्त load_env_file_pairs_fd(fd, p, ret);
}

पूर्णाङ्क os_release_support_ended(ध्रुव वर्ण *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ वर्ण *_support_end_alloc = रिक्त;
        पूर्णाङ्क r;

        सचे (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(रिक्त,
                                     "SUPPORT_END", &_support_end_alloc);
                सचे (r < 0 && r != -ENOENT)
                        पटिनिवत्त log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        सचे (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                सचे (ret_eol)
                        *ret_eol = USEC_INFINITY;

                पटिनिवत्त false;  /* no end date defined */
        }

        संरचना tm tm = {};
        ध्रुव वर्ण *k = strptime(support_end, "%Y-%m-%d", &tm);
        सचे (!k || *k)
                पटिनिवत्त log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        सचे (r < 0)
                पटिनिवत्त log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        सचे (ret_eol)
                *ret_eol = eol;

        पटिनिवत्त now(CLOCK_REALTIME) > eol;
}

ध्रुव वर्ण* os_release_pretty_name(ध्रुव वर्ण *pretty_name, ध्रुव वर्ण *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        पटिनिवत्त empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

वर्ण *unescape_fancy_name(वर्ण **fancy_name) {
        प्रतिज्ञा(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ वर्ण *unescaped_fancy_name = रिक्त;

        सचे (isempty(*fancy_name))
                गच्छ clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        सचे (n < 0) {
                log_debug_errno((पूर्णाङ्क) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                गच्छ clear;
        }

        सचे (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                गच्छ clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        पटिनिवत्त *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        पटिनिवत्त रिक्त;
}

bool use_fancy_name(ध्रुव वर्ण *fancy_name) {

        /* Decides whether to show the specified fancy name */

        सचे (isempty(fancy_name))
                पटिनिवत्त false;

        सचे (!colors_enabled())
                पटिनिवत्त false;

        पटिनिवत्त emoji_enabled() || ascii_is_valid(fancy_name);
}
