/* SPDX-License-Identifier: LGPL-2.1-or-later */

#അന്തര്ഭാവ <stdlib.h>

#അന്തര്ഭാവ "alloc-util.h"
#അന്തര്ഭാവ "ansi-color.h"
#അന്തര്ഭാവ "chase.h"
#അന്തര്ഭാവ "dirent-util.h"
#അന്തര്ഭാവ "env-file.h"
#അന്തര്ഭാവ "errno-util.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "fd-util.h"
#അന്തര്ഭാവ "fs-util.h"
#അന്തര്ഭാവ "glyph-util.h"
#അന്തര്ഭാവ "log.h"
#അന്തര്ഭാവ "os-util.h"
#അന്തര്ഭാവ "path-util.h"
#അന്തര്ഭാവ "stat-util.h"
#അന്തര്ഭാവ "string-table.h"
#അന്തര്ഭാവ "string-util.h"
#അന്തര്ഭാവ "strv.h"
#അന്തര്ഭാവ "time-util.h"
#അന്തര്ഭാവ "utf8.h"
#അന്തര്ഭാവ "xattr-util.h"

സ്ഥിതം സ്ഥിരം അക്ഷരം* സ്ഥിരം image_class_table[_IMAGE_CLASS_MAX] = {
        [IMAGE_MACHINE]  = "machine",
        [IMAGE_PORTABLE] = "portable",
        [IMAGE_SYSEXT]   = "sysext",
        [IMAGE_CONFEXT]  = "confext",
};

DEFINE_STRING_TABLE_LOOKUP(image_class, ImageClass);

/* Helper struct for naming simplicity and reusability */
സ്ഥിതം സ്ഥിരം ഘടന {
        സ്ഥിരം അക്ഷരം *release_file_directory;
        സ്ഥിരം അക്ഷരം *release_file_path_prefix;
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

bool image_name_is_valid(സ്ഥിരം അക്ഷരം *s) {
        എങ്കിൽ (!filename_is_valid(s))
                തിരികെ false;

        എങ്കിൽ (string_has_cc(s, ശൂന്യ))
                തിരികെ false;

        എങ്കിൽ (!utf8_is_valid(s))
                തിരികെ false;

        /* Temporary files for atomically creating new files */
        എങ്കിൽ (startswith(s, ".#"))
                തിരികെ false;

        തിരികെ true;
}

പൂർണ്ണസംഖ്യ path_extract_image_name(സ്ഥിരം അക്ഷരം *path, അക്ഷരം **ret) {
        _cleanup_free_ അക്ഷരം *fn = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);
        പ്രതിജ്ഞാ(ret);

        /* Extract last component from path, without any "/" suffixes. */
        r = path_extract_filename(path, &fn);
        എങ്കിൽ (r < 0)
                തിരികെ r;
        എങ്കിൽ (r != O_DIRECTORY) {
                /* Chop off any image suffixes we recognize (unless we already know this must refer to some dir) */
                അക്ഷരം *m = ENDSWITH_SET(fn, ".sysext.raw", ".confext.raw", ".raw");
                എങ്കിൽ (m)
                        *m = 0;
        }

        /* Truncate the version/counting suffixes */
        fn[strcspn(fn, "_+")] = 0;

        എങ്കിൽ (!image_name_is_valid(fn))
                തിരികെ -EINVAL;

        *ret = TAKE_PTR(fn);
        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ path_is_extension_വൃക്ഷം(ImageClass image_class, സ്ഥിരം അക്ഷരം *path, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(path);

        /* Does the path exist at all? If not, generate an error immediately. This is useful so that a missing root dir
         * always results in -ENOENT, and we can properly distinguish the case where the whole root doesn't exist from
         * the case where just the os-release file is missing. */
        r = access_nofollow(path, F_OK);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        /* We use /usr/lib/extension-release.d/extension-release[.NAME] as flag for something being a system extension,
         * /etc/extension-release.d/extension-release[.NAME] as flag for something being a system configuration, and finally,
         * and {/etc|/usr/lib}/os-release as a flag for something being an OS (when not an extension). */
        r = open_extension_release(path, image_class, extension, relax_extension_release_check, ശൂന്യ, ശൂന്യ);
        എങ്കിൽ (r == -ENOENT) /* We got nothing */
                തിരികെ 0;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ 1;
}

പൂർണ്ണസംഖ്യ fd_is_os_വൃക്ഷം(പൂർണ്ണസംഖ്യ fd) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(fd >= 0);

        r = open_extension_release_at(
                        fd,
                        IMAGE_MACHINE,
                        /* extension= */ ശൂന്യ,
                        /* relax_extension_release_check= */ false,
                        /* ret_path= */ ശൂന്യ,
                        /* ret_fd= */ ശൂന്യ);
        എങ്കിൽ (r == -ENOENT)
                തിരികെ false;
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ true;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ extension_release_strict_xattr_മൂല്യം(പൂർണ്ണസംഖ്യ extension_release_fd, സ്ഥിരം അക്ഷരം *extension_release_dir_path, സ്ഥിരം അക്ഷരം *filename) {
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(extension_release_fd >= 0);
        പ്രതിജ്ഞാ(extension_release_dir_path);
        പ്രതിജ്ഞാ(filename);

        /* No xattr or cannot parse it? Then skip this. */
        r = getxattr_at_bool(extension_release_fd, /* path= */ ശൂന്യ, "user.extension-release.strict", /* at_flags= */ 0);
        എങ്കിൽ (ERRNO_IS_NEG_XATTR_ABSENT(r))
                തിരികെ log_debug_errno(r, "%s/%s does not have user.extension-release.strict xattr, ignoring.",
                                       extension_release_dir_path, filename);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "%s/%s: Failed to read 'user.extension-release.strict' extended attribute from file, ignoring: %m",
                                       extension_release_dir_path, filename);

        /* Explicitly set to request strict matching? Skip it. */
        എങ്കിൽ (r > 0) {
                log_debug("%s/%s: 'user.extension-release.strict' attribute is true, ignoring file.",
                          extension_release_dir_path, filename);
                തിരികെ true;
        }

        log_debug("%s/%s: 'user.extension-release.strict' attribute is false%s",
                  extension_release_dir_path, filename,
                  glyph(GLYPH_ELLIPSIS));

        തിരികെ false;
}

പൂർണ്ണസംഖ്യ open_os_release_at(പൂർണ്ണസംഖ്യ rfd, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd) {
        സ്ഥിരം അക്ഷരം *e;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));

        e = secure_getenv("SYSTEMD_OS_RELEASE");
        എങ്കിൽ (e)
                തിരികെ chaseat(rfd, rfd, e, /* flags= */ 0, ret_path, ret_fd);

        FOREACH_STRING(path, "/etc/os-release", "/usr/lib/os-release") {
                r = chaseat(rfd, rfd, path, /* flags= */ 0, ret_path, ret_fd);
                എങ്കിൽ (r != -ENOENT)
                        തിരികെ r;
        }

        തിരികെ -ENOENT;
}

പൂർണ്ണസംഖ്യ open_os_release(സ്ഥിരം അക്ഷരം *വേര്, അക്ഷരം **ret_path, പൂർണ്ണസംഖ്യ *ret_fd) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!empty_or_വേര്(വേര്)) {
                rfd = open(വേര്, O_CLOEXEC | O_DIRECTORY | O_PATH);
                എങ്കിൽ (rfd < 0)
                        തിരികെ -errno;
        }

        r = open_os_release_at(rfd, ret_path ? &p : ശൂന്യ, ret_fd ? &fd : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path) {
                r = chaseat_prefix_വേര്(p, വേര്, ret_path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ open_extension_release_at(
                പൂർണ്ണസംഖ്യ rfd,
                ImageClass image_class,
                സ്ഥിരം അക്ഷരം *extension,
                bool relax_extension_release_check,
                അക്ഷരം **ret_path,
                പൂർണ്ണസംഖ്യ *ret_fd) {

        _cleanup_free_ അക്ഷരം *dir_path = ശൂന്യ, *path_found = ശൂന്യ;
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd_found = -EBADF;
        _cleanup_closedir_ DIR *dir = ശൂന്യ;
        bool found = false;
        സ്ഥിരം അക്ഷരം *p;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));
        പ്രതിജ്ഞാ(!extension || (image_class >= 0 && image_class < _IMAGE_CLASS_MAX));

        എങ്കിൽ (!extension)
                തിരികെ open_os_release_at(rfd, ret_path, ret_fd);

        എങ്കിൽ (!IN_SET(image_class, IMAGE_SYSEXT, IMAGE_CONFEXT))
                തിരികെ -EINVAL;

        എങ്കിൽ (!image_name_is_valid(extension))
                തിരികെ log_debug_errno(SYNTHETIC_ERRNO(EINVAL), "The extension name %s is invalid.", extension);

        p = strjoina(image_class_release_info[image_class].release_file_path_prefix, extension);
        r = chaseat(rfd, rfd, p, /* flags= */ 0, ret_path, ret_fd);
        log_full_errno_zerook(LOG_DEBUG, MIN(r, 0), "Checking for %s: %m", p);
        എങ്കിൽ (r != -ENOENT)
                തിരികെ r;

        /* Cannot find the expected extension-release file? The image filename might have been mangled on
         * deployment, so fallback to checking for any file in the extension-release.d directory, and return
         * the first one with a user.extension-release xattr instead. The user.extension-release.strict
         * xattr is checked to ensure the author of the image considers it OK if names do not match. */

        p = image_class_release_info[image_class].release_file_directory;
        r = chase_and_opendirat(rfd, rfd, p, /* chase_flags= */ 0, &dir_path, &dir);
        എങ്കിൽ (r < 0)
                തിരികെ log_debug_errno(r, "Cannot open %s, ignoring: %m", p);

        FOREACH_DIRENT(de, dir, തിരികെ -errno) {
                _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
                സ്ഥിരം അക്ഷരം *image_name;

                എങ്കിൽ (!IN_SET(de->d_type, DT_REG, DT_UNKNOWN))
                        തുടരുക;

                image_name = startswith(de->d_name, "extension-release.");
                എങ്കിൽ (!image_name)
                        തുടരുക;

                എങ്കിൽ (!image_name_is_valid(image_name)) {
                        log_debug("%s/%s is not a valid release file name, ignoring.", dir_path, de->d_name);
                        തുടരുക;
                }

                /* We already chased the directory, and checked that this is a real file, so we shouldn't
                 * fail to open it. */
                fd = openat(dirfd(dir), de->d_name, O_PATH|O_CLOEXEC|O_NOFOLLOW);
                എങ്കിൽ (fd < 0)
                        തിരികെ log_debug_errno(errno, "Failed to open release file %s/%s: %m", dir_path, de->d_name);

                /* Really ensure it is a regular file after we open it. */
                r = fd_verify_regular(fd);
                എങ്കിൽ (r < 0) {
                        log_debug_errno(r, "%s/%s is not a regular file, ignoring: %m", dir_path, de->d_name);
                        തുടരുക;
                }

                എങ്കിൽ (!relax_extension_release_check) {
                        _cleanup_free_ അക്ഷരം *base_extension = ശൂന്യ;

                        r = path_extract_image_name(extension, &base_extension);
                        എങ്കിൽ (r < 0) {
                                log_debug_errno(r, "Failed to extract image name from %s, ignoring: %m", extension);
                                തുടരുക;
                        }

                        എങ്കിൽ (!streq(image_name, base_extension) &&
                            extension_release_strict_xattr_മൂല്യം(fd, dir_path, image_name) != 0)
                                തുടരുക;
                }

                /* We already found what we were looking for, but there's another candidate? We treat this as
                 * an error, as we want to enforce that there are no ambiguities in case we are in the
                 * fallback path. */
                എങ്കിൽ (found)
                        തിരികെ -ENOTUNIQ;

                found = true;

                എങ്കിൽ (ret_fd)
                        fd_found = TAKE_FD(fd);

                എങ്കിൽ (ret_path) {
                        path_found = path_join(dir_path, de->d_name);
                        എങ്കിൽ (!path_found)
                                തിരികെ -ENOMEM;
                }
        }
        എങ്കിൽ (!found)
                തിരികെ -ENOENT;

        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(fd_found);
        എങ്കിൽ (ret_path)
                *ret_path = TAKE_PTR(path_found);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ open_extension_release(
                സ്ഥിരം അക്ഷരം *വേര്,
                ImageClass image_class,
                സ്ഥിരം അക്ഷരം *extension,
                bool relax_extension_release_check,
                അക്ഷരം **ret_path,
                പൂർണ്ണസംഖ്യ *ret_fd) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = XAT_FDROOT, fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!empty_or_വേര്(വേര്)) {
                rfd = open(വേര്, O_CLOEXEC | O_DIRECTORY | O_PATH);
                എങ്കിൽ (rfd < 0)
                        തിരികെ -errno;
        }

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check,
                                      ret_path ? &p : ശൂന്യ, ret_fd ? &fd : ശൂന്യ);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        എങ്കിൽ (ret_path) {
                r = chaseat_prefix_വേര്(p, വേര്, ret_path);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        എങ്കിൽ (ret_fd)
                *ret_fd = TAKE_FD(fd);

        തിരികെ 0;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parse_extension_release_atv(
                പൂർണ്ണസംഖ്യ rfd,
                ImageClass image_class,
                സ്ഥിരം അക്ഷരം *extension,
                bool relax_extension_release_check,
                va_list ap) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));

        r = open_extension_release_at(rfd, image_class, extension, relax_extension_release_check, &p, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ parse_env_file_fdv(fd, p, ap);
}

പൂർണ്ണസംഖ്യ parse_extension_release_at_sentinel(
                പൂർണ്ണസംഖ്യ rfd,
                ImageClass image_class,
                bool relax_extension_release_check,
                സ്ഥിരം അക്ഷരം *extension,
                ...) {

        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(wildcard_fd_is_valid(rfd));

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);
        തിരികെ r;
}

പൂർണ്ണസംഖ്യ parse_extension_release_sentinel(
                സ്ഥിരം അക്ഷരം *വേര്,
                ImageClass image_class,
                bool relax_extension_release_check,
                സ്ഥിരം അക്ഷരം *extension,
                ...) {

        _cleanup_close_ പൂർണ്ണസംഖ്യ rfd = XAT_FDROOT;
        va_list ap;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!empty_or_വേര്(വേര്)) {
                rfd = open(വേര്, O_CLOEXEC | O_DIRECTORY | O_PATH);
                എങ്കിൽ (rfd < 0)
                        തിരികെ -errno;
        }

        va_start(ap, extension);
        r = parse_extension_release_atv(rfd, image_class, extension, relax_extension_release_check, ap);
        va_end(ap);

        തിരികെ r;
}

പൂർണ്ണസംഖ്യ load_os_release_pairs_with_prefix(സ്ഥിരം അക്ഷരം *വേര്, സ്ഥിരം അക്ഷരം *prefix, അക്ഷരം ***ret) {
        _cleanup_strv_free_ അക്ഷരം **os_release_pairs = ശൂന്യ, **os_release_pairs_prefixed = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        പ്രതിജ്ഞാ(ret);

        r = load_os_release_pairs(വേര്, &os_release_pairs);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        STRV_FOREACH_PAIR(p, q, os_release_pairs) {
                അക്ഷരം *വരി;

                /* We strictly return only the four main ID fields and ignore the rest */
                എങ്കിൽ (!STR_IN_SET(*p, "ID", "VERSION_ID", "BUILD_ID", "VARIANT_ID"))
                        തുടരുക;

                ascii_strlower(*p);
                വരി = strjoin(prefix, *p, "=", *q);
                എങ്കിൽ (!വരി)
                        തിരികെ -ENOMEM;
                r = strv_consume(&os_release_pairs_prefixed, വരി);
                എങ്കിൽ (r < 0)
                        തിരികെ r;
        }

        *ret = TAKE_PTR(os_release_pairs_prefixed);

        തിരികെ 0;
}

പൂർണ്ണസംഖ്യ load_extension_release_pairs(സ്ഥിരം അക്ഷരം *വേര്, ImageClass image_class, സ്ഥിരം അക്ഷരം *extension, bool relax_extension_release_check, അക്ഷരം ***ret) {
        _cleanup_close_ പൂർണ്ണസംഖ്യ fd = -EBADF;
        _cleanup_free_ അക്ഷരം *p = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        r = open_extension_release(വേര്, image_class, extension, relax_extension_release_check, &p, &fd);
        എങ്കിൽ (r < 0)
                തിരികെ r;

        തിരികെ load_env_file_pairs_fd(fd, p, ret);
}

പൂർണ്ണസംഖ്യ os_release_support_ended(സ്ഥിരം അക്ഷരം *support_end, bool quiet, usec_t *ret_eol) {
        _cleanup_free_ അക്ഷരം *_support_end_alloc = ശൂന്യ;
        പൂർണ്ണസംഖ്യ r;

        എങ്കിൽ (!support_end) {
                /* If the caller has the variably handy, they can pass it in. If not, we'll read it
                 * ourselves. */

                r = parse_os_release(ശൂന്യ,
                                     "SUPPORT_END", &_support_end_alloc);
                എങ്കിൽ (r < 0 && r != -ENOENT)
                        തിരികെ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                              "Failed to read os-release file, ignoring: %m");

                support_end = _support_end_alloc;
        }

        എങ്കിൽ (isempty(support_end)) { /* An empty string is a explicit way to say "no EOL exists" */
                എങ്കിൽ (ret_eol)
                        *ret_eol = USEC_INFINITY;

                തിരികെ false;  /* no end date defined */
        }

        ഘടന tm tm = {};
        സ്ഥിരം അക്ഷരം *k = strptime(support_end, "%Y-%m-%d", &tm);
        എങ്കിൽ (!k || *k)
                തിരികെ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, SYNTHETIC_ERRNO(EINVAL),
                                      "Failed to parse SUPPORT_END= from os-release file, ignoring: %s", support_end);

        usec_t eol;
        r = mktime_or_timegm_usec(&tm, /* utc= */ true, &eol);
        എങ്കിൽ (r < 0)
                തിരികെ log_full_errno(quiet ? LOG_DEBUG : LOG_WARNING, r,
                                      "Failed to convert SUPPORT_END= time from os-release file, ignoring: %m");

        എങ്കിൽ (ret_eol)
                *ret_eol = eol;

        തിരികെ now(CLOCK_REALTIME) > eol;
}

സ്ഥിരം അക്ഷരം* os_release_pretty_name(സ്ഥിരം അക്ഷരം *pretty_name, സ്ഥിരം അക്ഷരം *name) {
        /* Distills a "pretty" name to show from os-release data. First argument is supposed to be the
         * PRETTY_NAME= field, the second one the NAME= field. This function is trivial, of course, and
         * exists mostly to ensure we use the same logic wherever possible. */

        തിരികെ empty_to_null(pretty_name) ?:
                empty_to_null(name) ?: "Linux";
}

അക്ഷരം *unescape_fancy_name(അക്ഷരം **fancy_name) {
        പ്രതിജ്ഞാ(fancy_name);

        /* Checks if the fancy name is valid, unescapes if it is, nullifies it if not */

        _cleanup_free_ അക്ഷരം *unescaped_fancy_name = ശൂന്യ;

        എങ്കിൽ (isempty(*fancy_name))
                പോകുക clear;

        /* We undo one level of C escapes on this */
        ssize_t n = cunescape(*fancy_name, /* flags= */ 0, &unescaped_fancy_name);
        എങ്കിൽ (n < 0) {
                log_debug_errno((പൂർണ്ണസംഖ്യ) n, "Failed to unescape FANCY_NAME= string, suppressing: %m");
                പോകുക clear;
        }

        എങ്കിൽ (!utf8_is_valid(unescaped_fancy_name)) {
                log_debug("Unescaped FANCY_NAME= string is not valid UTF-8, suppressing.");
                പോകുക clear;
        }

        free_and_replace(*fancy_name, unescaped_fancy_name);
        തിരികെ *fancy_name;

clear:
        *fancy_name = mfree(*fancy_name);
        തിരികെ ശൂന്യ;
}

bool use_fancy_name(സ്ഥിരം അക്ഷരം *fancy_name) {

        /* Decides whether to show the specified fancy name */

        എങ്കിൽ (isempty(fancy_name))
                തിരികെ false;

        എങ്കിൽ (!colors_enabled())
                തിരികെ false;

        തിരികെ emoji_enabled() || ascii_is_valid(fancy_name);
}
